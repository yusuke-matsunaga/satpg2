﻿
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "EventQ.h"
#include "SimPrim.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
EventQ::EventQ() :
  mArraySize(0),
  mArray(nullptr),
  mNum(0)
{
  // 適当なサイズで初期化しておく
  init(128);
}

// @brief デストラクタ
EventQ::~EventQ()
{
  delete [] mArray;
}

// @brief 初期化を行う．
// @param[in] max_level 最大レベル
void
EventQ::init(ymuint max_level)
{
  if ( max_level >= mArraySize ) {
    delete [] mArray;
    mArraySize = max_level + 1;
    mArray = new SimNode*[mArraySize];
  }
  mCurLevel = 0;
  for (ymuint i = 0; i < mArraySize; i ++) {
    mArray[i] = nullptr;
  }
  mNum = 0;
}

END_NAMESPACE_YM_SATPG_FSIM
