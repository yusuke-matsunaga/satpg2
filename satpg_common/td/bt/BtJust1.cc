
/// @file BtJust1.cc
/// @brief BtJust1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BtJust1.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
// クラス BtJust1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BtJust1::BtJust1()
{
}

// @brief デストラクタ
BtJust1::~BtJust1()
{
}

// @brief バックトレースを行なう．
// @param[in] ffr_root 故障のあるFFRの根のノード
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[in] val_map ノードの値を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
// val_map には ffr_root のファンアウトコーン上の故障値と関係する
// 回路全体の正常値が入っている．
void
BtJust1::run(const TpgNode* ffr_root,
	     const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // assign_list の値を正当化する．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    if ( nv.time() == 0 ) {
      justify0(node, val_map, pi_assign_list);
    }
    else {
      justify(node, val_map, pi_assign_list);
    }
  }

  // 故障差の伝搬している外部出力を選ぶ．
  const TpgNode* onode = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( val_map.gval(node) != val_map.fval(node) ) {
      onode = node;
      break;
    }
  }
  ASSERT_COND( onode != nullptr );

  // 正当化を行う．
  justify(onode, val_map, pi_assign_list);

  // 1時刻前の故障ノードの値も正当化する．
  // ここでは ffr_root を正当化しているが，故障の影響が伝搬するためには
  justify0(ffr_root, val_map, pi_assign_list);

  // 一連の処理でつけたマークを消す．
  clear_justified();
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
void
BtJust1::justify(const TpgNode* node,
		 const ValMap& val_map,
		 NodeValList& assign_list)
{
  if ( justified_mark(node) ) {
    return;
  }
  set_justified(node);

  if ( node->is_primary_input() ) {
    // val を記録
    record_value(node, val_map, 1, assign_list);
    return;
  }

  if ( node->is_dff_output() ) {
    const TpgDff* dff = node->dff();
    const TpgNode* alt_node = dff->input();
    justify0(alt_node, val_map, assign_list);
    return;
  }

  Val3 gval = val_map.gval(node);
  Val3 fval = val_map.fval(node);

  if ( gval != fval ) {
    // 正常値と故障値が異なっていたら
    // すべてのファンインをたどる．
    just_sub1(node, val_map, assign_list);
    return;
  }

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    justify(node->fanin(0), val_map, assign_list);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_sub1(node, val_map, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_sub2(node, val_map, kVal0, assign_list);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_sub2(node, val_map, kVal0, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_sub1(node, val_map, assign_list);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_sub2(node, val_map, kVal1, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_sub1(node, val_map, assign_list);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_sub1(node, val_map, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_sub2(node, val_map, kVal1, assign_list);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just_sub1(node, val_map, assign_list);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[out] assign_list 値の割当リスト
void
BtJust1::just_sub1(const TpgNode* node,
		   const ValMap& val_map,
		   NodeValList& assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    justify(inode, val_map, assign_list);
  }
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[in] val 値
// @param[out] assign_list 値の割当リスト
void
BtJust1::just_sub2(const TpgNode* node,
		   const ValMap& val_map,
		   Val3 val,
		   NodeValList& assign_list)
{
  bool gfound = false;
  bool ffound = false;
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = val_map.gval(inode);
    Val3 ifval = val_map.fval(inode);
    if ( !gfound && igval == val ) {
      justify(inode, val_map, assign_list);
      gfound = true;
      if ( ifval == val ) {
	break;
      }
    }
    else if ( !ffound && ifval == val ) {
      justify(inode, val_map, assign_list);
      ffound = true;
    }
    if ( gfound && ffound ) {
      break;
    }
  }
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
void
BtJust1::justify0(const TpgNode* node,
		  const ValMap& val_map,
		  NodeValList& assign_list)
{
  if ( justified0_mark(node) ) {
    return;
  }
  set_justified0(node);

  if ( node->is_ppi() ) {
    // val を記録
    record_value(node, val_map, 0, assign_list);
    return;
  }

  Val3 gval = val_map.gval(node, 0);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    justify0(node->fanin(0), val_map, assign_list);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just0_sub1(node, val_map, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just0_sub2(node, val_map, kVal0, assign_list);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just0_sub2(node, val_map, kVal0, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just0_sub1(node, val_map, assign_list);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just0_sub2(node, val_map, kVal1, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just0_sub1(node, val_map, assign_list);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just0_sub1(node, val_map, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just0_sub2(node, val_map, kVal1, assign_list);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just0_sub1(node, val_map, assign_list);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[out] assign_list 値の割当リスト
void
BtJust1::just0_sub1(const TpgNode* node,
		    const ValMap& val_map,
		    NodeValList& assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    justify0(inode, val_map, assign_list);
  }
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[in] val 値
// @param[out] assign_list 値の割当リスト
void
BtJust1::just0_sub2(const TpgNode* node,
		    const ValMap& val_map,
		    Val3 val,
		    NodeValList& assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = val_map.gval(inode, 0);
    if ( igval == val ) {
      justify0(inode, val_map, assign_list);
      break;
    }
  }
}

END_NAMESPACE_YM_SATPG_TD
