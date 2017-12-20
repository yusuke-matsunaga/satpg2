
/// @file JustBase.cc
/// @brief JustBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス JustBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] max_id ID番号の最大値
JustBase::JustBase(bool td_mode,
		   ymuint max_id) :
  mTdMode(td_mode),
  mMaxId(max_id),
  mMarkArray(max_id, 0U)
{
  mValMapPtr = nullptr;
}

// @brief デストラクタ
JustBase::~JustBase()
{
}

// @brief 全てのマークを消す．
void
JustBase::clear_justified_mark()
{
  for (ymuint i = 0; i < mMaxId; ++ i) {
    mMarkArray[i] = 0U;
  }
}

END_NAMESPACE_YM_SATPG
