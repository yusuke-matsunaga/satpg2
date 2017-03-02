
/// @file BackTracer.cc
/// @brief BackTracer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/BackTracer.h"
#include "BtSimple.h"
#include "BtJust1.h"
#include "BtJust2.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
// クラス BackTracer
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ID番号の最大値
BackTracer::BackTracer(ymuint xmode,
		       ymuint max_id)
{
  switch ( xmode ) {
  case 0:  mImpl = new BtSimple(); break;
  case 1:  mImpl = new BtJust1(); break;
  case 2:  mImpl = new BtJust2(); break;
  default: mImpl = new BtJust2(); break;
  }
  mImpl->set_max_id(max_id);
}

// @brief デストラクタ
BackTracer::~BackTracer()
{
  delete mImpl;
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
BackTracer::operator()(const TpgNode* ffr_root,
		       const NodeValList& assign_list,
		       const vector<const TpgNode*>& output_list,
		       const ValMap& val_map,
		       NodeValList& pi_assign_list)
{
  mImpl->run(ffr_root, assign_list, output_list, val_map, pi_assign_list);
}

END_NAMESPACE_YM_SATPG_TD
