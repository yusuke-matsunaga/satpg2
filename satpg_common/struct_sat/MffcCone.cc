
/// @file MffcCone.cc
/// @brief MffcCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "MffcCone.h"
#include "StructSat.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "ValMap_model.h"
#include "Extractor.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

bool debug_mffccone = false;

#define DEBUG_OUT cout

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] struct_sat StructSat ソルバ
// @param[in] mffc MFFC の情報
// @param[in] block_node ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
//
// ブロックノードより先のノードは含めない．
// 通常 block_node は nullptr か root_node の dominator
// となっているはず．
MffcCone::MffcCone(StructSat& struct_sat,
		   const TpgMFFC* mffc,
		   const TpgNode* block_node,
		   bool detect) :
  FoCone(struct_sat, mffc->root(), block_node, detect),
  mElemArray(mffc->elem_num()),
  mElemVarArray(mffc->elem_num())
{
  for (ymuint i = 0; i < mffc->elem_num(); ++ i ) {
    const TpgFFR* ffr = mffc->elem(i);
    mElemArray[i] = ffr->root();
    ASSERT_COND( ffr->root() != nullptr );
    ymuint nf = ffr->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* f = ffr->fault(j);
      const TpgNode* node = f->tpg_onode()->ffr_root();
      mElemPosMap.add(node->id(), i);
    }
  }
}

// @brief デストラクタ
MffcCone::~MffcCone()
{
}

// @brief 故障検出に必要な割り当てを求める．
// @param[in] model SAT のモデル
// @param[in] root 起点のノード
// @param[out] 値の割り当て結果を入れるリスト
void
MffcCone::extract(const vector<SatBool3>& model,
		  const TpgNode* root,
		  NodeValList& assign_list)
{
  // 実際の処理は Extractor が行う．
  ValMap_model val_map(gvar_map(), fvar_map(), model);
  Extractor extractor;
  extractor(root, val_map, assign_list);
}

// @brief 関係するノードの変数を作る．
void
MffcCone::make_vars()
{
  FoCone::make_vars();
}

// @brief 関係するノードの入出力の関係を表すCNFを作る．
void
MffcCone::make_cnf()
{
  FoCone::make_cnf();

  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    mElemVarArray[i] = solver().new_variable();

    if ( debug_mffccone ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  HashMap<ymuint, ymuint> elem_map;
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    elem_map.add(node->id(), i);
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    SatVarId ovar = fvar(node);
    ymuint elem_pos;
    if ( elem_map.find(node->id(), elem_pos) ) {
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[elem_pos]
      ovar = solver().new_variable();
      inject_fault(elem_pos, ovar);
      // ovar が fvar(node) ではない！
      struct_sat().make_node_cnf(node, fvar_map(), ovar);
    }
    else {
      struct_sat().make_node_cnf(node, fvar_map());
    }

    if ( debug_mffccone ) {
      DEBUG_OUT << "Node#" << node->id() << ": ofvar("
		<< ovar << ") := " << node->gate_type()
		<< "(";
      ymuint ni = node->fanin_num();
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* inode = node->fanin(i);
	DEBUG_OUT << " " << fvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 故障挿入回路のCNFを作る．
// @param[in] elem_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
MffcCone::inject_fault(ymuint elem_pos,
		       SatVarId ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[elem_pos]);
  const TpgNode* node = mElemArray[elem_pos];
  SatLiteral olit(fvar(node));

  solver().add_xorgate_rel(lit1, lit2, olit);

  if ( debug_mffccone ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[elem_pos] << endl;
  }
}

// @brief 故障の影響伝搬させる条件を作る．
// @param[in] root 起点となるノード
// @param[out] assumptions 結果の仮定を表すリテラルのリスト
void
MffcCone::make_prop_condition(const TpgNode* root,
			      vector<SatLiteral>& assumptions)
{
  // root のある FFR を活性化する条件を作る．
  ymuint ffr_id;
  bool stat = mElemPosMap.find(root->id(), ffr_id);
  if ( !stat ) {
    cerr << "Error[MffcCone::make_prop_condition()]: "
	 << root->id() << " is not within the MFFC" << endl;
    return;
  }

  ymuint ffr_num = mElemArray.size();
  if ( ffr_num > 1 ) {
    // FFR の根の出力に故障を挿入する．
    assumptions.reserve(assumptions.size() + ffr_num);
    for (ymuint i = 0; i < ffr_num; ++ i) {
      SatVarId evar = mElemVarArray[i];
      bool inv = (i != ffr_id);
      assumptions.push_back(SatLiteral(evar, inv));
    }
  }
}

END_NAMESPACE_YM_SATPG
