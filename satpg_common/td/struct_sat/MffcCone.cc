
/// @file MffcCone.cc
/// @brief MffcCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/MffcCone.h"
#include "td/StructEnc.h"
#include "TpgNode.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateLitMap_vect.h"
#include "GateLitMap_vid.h"


BEGIN_NAMESPACE_YM_SATPG_TD

// @brief コンストラクタ
// @param[in] struct_sat StructEnc ソルバ
// @param[in] fnode 故障位置のノード
MffcCone::MffcCone(StructEnc& struct_sat,
		   const TpgNode* fnode) :
  ConeBase(struct_sat),
  mElemList(fnode->mffc()->elem_num()),
  mElemVarList(fnode->mffc()->elem_num())
{
  const TpgMFFC* mffc = fnode->mffc();
  ASSERT_COND( mffc != nullptr );

  vector<int> elem_map(max_id(), -1);
  for (ymuint i = 0; i < mffc->elem_num(); ++ i) {
    const TpgNode* node = mffc->elem(i)->root();
    mElemList[i] = node;
    mElemVarList[i] = solver().new_variable();
    elem_map[node->id()] = i;
  }

  // mElemList に含まれるノードの TFO を mNodeList に加える．
  mark_tfo_tfi(mElemList);

  for (ymuint i = 0; i < tfo_num(); ++ i) {
    const TpgNode* node = tfo_node(i);
    int fpos = elem_map[node->id()];
    if ( fpos >= 0 ) {
      // 出力に故障挿入変数との XOR ゲートを挿入する．
      SatVarId tmp_var = solver().new_variable();
      ymuint ni = node->fanin_num();
      vector<SatVarId> tmp_ivars(ni);
      for (ymuint j = 0; j < ni; ++ j) {
	tmp_ivars[j] = fvar(node->fanin(j));
      }
      GateLitMap_vect lit_map(tmp_ivars, tmp_var);
      node->make_cnf(solver(), lit_map);

      SatLiteral ilit(tmp_var);
      SatLiteral olit(fvar(node));
      SatLiteral dlit(mElemVarList[fpos]);
      solver().add_clause( ilit,  dlit, ~olit);
      solver().add_clause( ilit, ~dlit,  olit);
      solver().add_clause(~ilit,  dlit,  olit);
      solver().add_clause(~ilit, ~dlit, ~olit);
    }
    else {
      // 故障回路のゲートの入出力関係を表すCNFを作る．
      node->make_cnf(solver(), GateLitMap_vid(node, fvar_map()));
    }

    // D-Chain 制約を作る．
    make_dchain_cnf(node);
  }

  ymuint npo = output_num();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(npo);
  for (ymuint i = 0; i < npo; ++ i) {
    const TpgNode* node = output_node(i);
    SatLiteral dlit(dvar(node));
    tmp_lits.push_back(dlit);
  }
  solver().add_clause(tmp_lits);

  SatLiteral dlit(dvar(fnode));
  solver().add_clause(dlit);
}

// @brief デストラクタ
MffcCone::~MffcCone()
{
}

// @brief 故障挿入位置を選ぶ．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
// @param[out] assumptions 結果の割り当てを追加するベクタ
void
MffcCone::select_fault_node(ymuint pos,
			    vector<SatLiteral>& assumptions) const
{
  ASSERT_COND( pos < mElemList.size() );

  // mElemVar の設定を行う．
  for (ymuint i = 0; i < mElemList.size(); ++ i) {
    SatVarId fdvar = mElemVarList[i];
    if ( i == pos ) {
      assumptions.push_back(SatLiteral(fdvar, false));
    }
    else {
      assumptions.push_back(SatLiteral(fdvar, true));
    }
  }

  // node1 の TFO でない部分の dvar を false にする．
  const TpgNode* node0 = mElemList[0];
  const TpgNode* node1 = mElemList[pos];
  vector<const TpgNode*> node_list;
  vector<bool> mark(max_id(), false);
  node_list.push_back(node1);
  mark[node1->id()] = true;
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( mark[onode->id()] ) {
	continue;
      }
      mark[onode->id()] = true;
      node_list.push_back(onode);
    }
  }
  for (ymuint i = 0; i < tfo_num(); ++ i) {
    const TpgNode* node = tfo_node(i);
    if ( !mark[node->id()] ) {
      SatLiteral dlit(dvar(node), true);
      assumptions.push_back(dlit);
    }
  }
}

END_NAMESPACE_YM_SATPG_TD
