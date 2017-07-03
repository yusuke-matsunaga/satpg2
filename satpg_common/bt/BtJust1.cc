﻿
/// @file BtJust1.cc
/// @brief BtJust1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtJust1.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtJust1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] val_map ノードの値を保持するクラス
BtJust1::BtJust1(ymuint max_id,
		 bool td_mode,
		 const ValMap& val_map) :
  BtImpl(max_id, td_mode, val_map)
{
}

// @brief デストラクタ
BtJust1::~BtJust1()
{
}

// @brief バックトレースを行なう．
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
// val_map には ffr_root のファンアウトコーン上の故障値と関係する
// 回路全体の正常値が入っている．
void
BtJust1::run(const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // assign_list の値を正当化する．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    justify(node, nv.time(), pi_assign_list);
  }

  // 故障差の伝搬している外部出力を選ぶ．
  const TpgNode* onode = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node, 1) != fval(node, 1) ) {
      onode = node;
      break;
    }
  }
  ASSERT_COND( onode != nullptr );

  // 正当化を行う．
  justify(onode, 1, pi_assign_list);
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
void
BtJust1::justify(const TpgNode* node,
		 int time,
		 NodeValList& assign_list)
{
  if ( justified_mark(node, time) ) {
    return;
  }
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // val を記録
    record_value(node, time, assign_list);
    return;
  }
  if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // 1時刻前のタイムフレームに戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, assign_list);
    }
    else {
      // val を記録
      record_value(node, time, assign_list);
    }
    return;
  }

  Val3 gval = this->gval(node, time);
  Val3 fval = this->fval(node, time);

  if ( gval != fval ) {
    // 正常値と故障値が異なっていたら
    // すべてのファンインをたどる．
    just_all(node, time, assign_list);
    return;
  }

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    justify(node->fanin(0), time, assign_list);
    break;

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, time, kVal0, assign_list);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      just_one(node, time, kVal0, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, time, kVal1, assign_list);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      just_all(node, time, assign_list);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      just_one(node, time, kVal1, assign_list);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    just_all(node, time, assign_list);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] assign_list 値の割当リスト
void
BtJust1::just_all(const TpgNode* node,
		  int time,
		  NodeValList& assign_list)
{
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    justify(inode, time, assign_list);
  }
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[in] val 値
// @param[out] assign_list 値の割当リスト
void
BtJust1::just_one(const TpgNode* node,
		  int time,
		  Val3 val,
		  NodeValList& assign_list)
{
  bool gfound = false;
  bool ffound = false;
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode, time);
    Val3 ifval = fval(inode, time);
    if ( !gfound && igval == val ) {
      justify(inode, time, assign_list);
      gfound = true;
      if ( ifval == val ) {
	break;
      }
    }
    else if ( !ffound && ifval == val ) {
      justify(inode, time, assign_list);
      ffound = true;
    }
    if ( gfound && ffound ) {
      break;
    }
  }
}

END_NAMESPACE_YM_SATPG