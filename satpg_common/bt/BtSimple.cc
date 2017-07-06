
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
// @param[in] fault_type 故障の型
BtSimple::BtSimple(ymuint max_id,
		   FaultType fault_type) :
  BtImpl(max_id, fault_type)
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
BtSimple::_run(const NodeValList& assign_list,
	       const vector<const TpgNode*>& output_list,
	       NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // output_list のファンインに含まれる入力ノードに印をつける．
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node, 1) != fval(node, 1) ) {
      justify(node, 1, pi_assign_list);
    }
  }

  // 念のため assign_list に含まれるノードの正当化を行っておく．
  // たぶんすでに処理済みのマークがついているので実質オーバーヘッドはない．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    justify(node, nv.time(), pi_assign_list);
  }
}

// @brief node のファンインのうち外部入力を記録する．
// @param[in] node ノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] assign_list 値割当の結果を入れるリスト
void
BtSimple::justify(const TpgNode* node,
		  int time,
		  NodeValList& assign_list)
{
  if ( justified_mark(node, time) ) {
    return;
  }
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    record_value(node, time, assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && fault_type() == kFtTransitionDelay ) {
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, assign_list);
    }
    else {
      record_value(node, time, assign_list);
    }
  }
  else {
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      justify(inode, time, assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG
