﻿
/// @file BtSimple.cc
/// @brief BtSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtSimple.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] val_map ノードの値を保持するクラス
BtSimple::BtSimple(ymuint max_id,
		   bool td_mode,
		   const ValMap& val_map) :
  BtImpl(max_id, td_mode, val_map)
{
}

// @brief デストラクタ
BtSimple::~BtSimple()
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
BtSimple::run(const NodeValList& assign_list,
	      const vector<const TpgNode*>& output_list,
	      NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // output_list のファンインに含まれる入力ノードに印をつける．
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node, 1) != fval(node, 1) ) {
      tfi_recur(node, pi_assign_list);
    }
  }

  // 念のため assign_list に含まれるノードの正当化を行っておく．
  // たぶんすでに処理済みのマークがついているので実質オーバーヘッドはない．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    if ( td_mode() ) {
      if ( nv.time() == 0 ) {
	tfi_recur0(node, pi_assign_list);
      }
      else {
	tfi_recur(node, pi_assign_list);
      }
    }
    else {
      tfi_recur(node, pi_assign_list);
    }
  }
}

// @brief node のファンインのうち外部入力を記録する．
// @param[in] node ノード
// @param[out] assign_list 値割当の結果を入れるリスト
void
BtSimple::tfi_recur(const TpgNode* node,
		    NodeValList& assign_list)
{
  if ( justified_mark(node, 1) ) {
    return;
  }
  set_justified(node, 1);

  if ( node->is_primary_input() ) {
    record_value(node, 1, assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( td_mode() ) {
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      tfi_recur0(alt_node, assign_list);
    }
    else {
      record_value(node, 1, assign_list);
    }
  }
  else {
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      tfi_recur(inode, assign_list);
    }
  }
}

// @brief node のファンインのうち外部入力を記録する．
// @param[in] node ノード
// @param[out] assign_list 値割当の結果を入れるリスト
void
BtSimple::tfi_recur0(const TpgNode* node,
		     NodeValList& assign_list)
{
  if ( justified_mark(node, 0) ) {
    return;
  }
  set_justified(node, 0);

  if ( node->is_ppi() ) {
    record_value(node, 0, assign_list);
  }
  else {
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      tfi_recur0(inode, assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG
