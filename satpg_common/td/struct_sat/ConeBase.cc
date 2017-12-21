
/// @file ConeBase.cc
/// @brief ConeBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/ConeBase.h"
#include "td/StructEnc.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_TD

BEGIN_NONAMESPACE

bool debug = false;

struct Lt
{
  bool
  operator()(const TpgNode* left,
	     const TpgNode* right)
  {
    return left->output_id2() < right->output_id2();
  }
};

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] struct_sat StructEnc ソルバ
ConeBase::ConeBase(StructEnc& struct_sat) :
  mStructEnc(struct_sat),
  mMaxNodeId(struct_sat.max_node_id()),
  mMarkArray(max_id()),
  mFvarMap(max_id()),
  mDvarMap(max_id())
{
}

// @brief デストラクタ
ConeBase::~ConeBase()
{
}

// @brief 指定されたノードの TFO の TFI に印をつける．
// @param[in] node_list 起点となるノードのリスト
// @param[in] use_dvar D-var を使う時 true にする．
void
ConeBase::mark_tfo_tfi(const vector<const TpgNode*>& node_list,
		       bool use_dvar)
{
  mNodeList.reserve(max_id());

  // node_list の TFO を mNodeList に入れる．
  for (ymuint i = 0; i < node_list.size(); ++ i) {
    const TpgNode* node = node_list[i];
    set_tfo_mark(node);
  }
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    if ( end_mark(node) ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* fonode = node->fanout(i);
      if ( !tfo_mark(fonode) ) {
	set_tfo_mark(fonode);
      }
    }
  }

  mTfoNum = mNodeList.size();

  // mNodeList に含まれるノードの TFI を mNodeList に追加する．
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !tfo_mark(inode) ) {
	set_tfo_mark(inode);
      }
    }
  }

  // 出力のリストを整列しておく．
  sort(mOutputList.begin(), mOutputList.end(), Lt());

  // 出力のリストに end マークをつけておく．
  for (ymuint i = 0; i < mOutputList.size(); ++ i) {
    set_end_mark(mOutputList[i]);
  }

  // TFO のノードに変数を割り当てる．
  for (ymuint i = 0; i < mTfoNum; ++ i) {
    const TpgNode* node = mNodeList[i];
    mStructEnc.make_tfi_cnf(node);
    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);
    if ( debug ) {
      cout << "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
    if ( use_dvar ) {
      SatVarId dvar = solver().new_variable();
      set_dvar(node, dvar);
    }
  }

  // TFO に含まれないノードの fvar を gvar にする．
  for (ymuint i = mTfoNum; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    set_fvar(node, gvar(node));
  }
}

// @brief 指定されたノードの TFO の TFI に印をつける．
// @param[in] node 起点となるノード
// @param[in] use_dvar D-var を使う時 true にする．
void
ConeBase::mark_tfo_tfi(const TpgNode* node,
		       bool use_dvar)
{
  mark_tfo_tfi(vector<const TpgNode*>(1, node), use_dvar);
}

// @brief node に関する故障伝搬条件を作る．
void
ConeBase::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(gvar(node), false);
  SatLiteral flit(fvar(node), false);
  SatLiteral dlit(dvar(node), false);

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに dlit が 1 の時，正常回路と故障回路で異なっていなければならない．
  solver().add_clause(~glit, ~flit, ~dlit);
  solver().add_clause( glit,  flit, ~dlit);

  if ( end_mark(node) ) {
    // 出力ノードの場合，XOR(glit, flit) -> dlit となる．
    solver().add_clause(~glit,  flit, dlit);
    solver().add_clause( glit, ~flit, dlit);
  }
  else {
    // dlit が 1 の時，ファンアウトの dlit が最低1つは 1 でなければならない．
    ymuint nfo = node->fanout_num();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(nfo + 1);
    tmp_lits.push_back(~dlit);
    for (ymuint j = 0; j < nfo; ++ j) {
      const TpgNode* onode = node->fanout(j);
      SatLiteral odlit(dvar(onode), false);
      tmp_lits.push_back(odlit);
    }
    solver().add_clause(tmp_lits);

    // immediate dominator がある場合，immediate dominator の dlit も 1 でなければならない．
    const TpgNode* idom = node->imm_dom();
    if ( idom != nullptr ) {
      SatLiteral odlit(dvar(idom));
      solver().add_clause(~dlit, odlit);
    }
  }
}

END_NAMESPACE_YM_SATPG_TD
