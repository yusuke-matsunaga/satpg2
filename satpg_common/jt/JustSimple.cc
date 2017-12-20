
/// @file JustSimple.cc
/// @brief JustSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustSimple.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief JustSimple を生成する．
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
Justifier*
new_JustSimple(bool td_mode,
	       ymuint max_id)
{
  return new JustSimple(td_mode, max_id);
}


//////////////////////////////////////////////////////////////////////
// クラス JustSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
JustSimple::JustSimple(bool td_mode,
		       ymuint max_id) :
  JustBase(td_mode, max_id)
{
}

// @brief デストラクタ
JustSimple::~JustSimple()
{
}

// @brief 正当化に必要な割当を求める．
// @param[in] assign_list 値の割り当てリスト
// @param[in] val_map ノードの値を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::operator()(const NodeValList& assign_list,
		       const ValMap& val_map,
		       NodeValList& pi_assign_list)
{
  pi_assign_list.clear();
  clear_justified_mark();

  set_val_map(val_map);

  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    int time = nv.time();
    justify(node, time, pi_assign_list);
  }
}

// @brief 正当化に必要な割当を求める．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
JustSimple::justify(const TpgNode* node,
		    int time,
		    NodeValList& pi_assign_list)
{
  if ( justified_mark(node, time) ) {
    // 処理済みならなにもしない．
    return;
  }
  // 処理済みの印を付ける．
  set_justified(node, time);

  if ( node->is_primary_input() ) {
    // 外部入力なら値を記録する．
    record_value(node, time, pi_assign_list);
  }
  else if ( node->is_dff_output() ) {
    if ( time == 1 && td_mode() ) {
      // DFF の出力で1時刻目の場合は0時刻目に戻る．
      const TpgDff* dff = node->dff();
      const TpgNode* alt_node = dff->input();
      justify(alt_node, 0, pi_assign_list);
    }
    else {
      // DFFを擬似入力だと思って値を記録する．
      record_value(node, time, pi_assign_list);
    }
  }
  else {
    // すべてのファンインに再帰する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      justify(inode, time, pi_assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG
