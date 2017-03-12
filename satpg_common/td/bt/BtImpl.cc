
/// @file BtImpl.cc
/// @brief BtImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BtImpl.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
// クラス BtImpl
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] val_map ノードの値を保持するクラス
BtImpl::BtImpl(const ValMap& val_map) :
  mValMap(val_map)
{
}

// @brief デストラクタ
BtImpl::~BtImpl()
{
}

END_NAMESPACE_YM_SATPG_TD
