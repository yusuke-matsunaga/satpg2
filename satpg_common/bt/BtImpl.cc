
/// @file BtImpl.cc
/// @brief BtImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtImpl.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] fault_type 故障の型
BtImpl::BtImpl(ymuint max_id,
	       FaultType fault_type) :
  mFaultType(fault_type),
  mMarkArray(max_id, 0U)
{
  mValMap = nullptr;
}

// @brief デストラクタ
BtImpl::~BtImpl()
{
}

// @brief バックトレースを行なう．
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
BtImpl::run(const NodeValList& assign_list,
	    const vector<const TpgNode*>& output_list,
	    const ValMap& val_map,
	    NodeValList& pi_assign_list)
{
  // ValMap をセットする．
  // 以降 gval(), fval() が使えるようになる．
  mValMap = &val_map;

  // バックトレースの本体
  _run(assign_list, output_list, pi_assign_list);

  // マークを消す．
  for (vector<ymuint>::iterator p = mNodeIdList.begin();
       p != mNodeIdList.end(); ++ p) {
    ymuint id = *p;
    mMarkArray[id] = 0U;
    _clear_hook(id);
  }
  mNodeIdList.clear();
}

// @brief 処理の終了後に作業領域をクリアするためのフック関数
// @param[in] id ノード番号
//
// デフォルトの実装では何もしない．
void
BtImpl::_clear_hook(ymuint id)
{
}

END_NAMESPACE_YM_SATPG
