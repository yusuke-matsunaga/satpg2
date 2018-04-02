
/// @file TpgMFFC.cc
/// @brief TpgMFFC の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgMFFC.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgMFFC
//////////////////////////////////////////////////////////////////////

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] ffr_list このMFFCに含まれるFFRのリスト
// @param[in] fault_list このMFFCに含まれる故障のリスト
// @param[in] alloc メモリアロケータ
void
TpgMFFC::set(const TpgNode* root,
	     const vector<const TpgFFR*>& ffr_list,
	     const vector<TpgFault*>& fault_list,
	     Alloc& alloc)
{
  mRoot = root;

  mElemNum = ffr_list.size();
  mElemList = alloc.get_array<const TpgFFR*>(mElemNum);
  for ( int i = 0; i < mElemNum; ++ i ) {
    const TpgFFR* ffr = ffr_list[i];
    mElemList[i] = ffr;
  }

  mFaultNum = fault_list.size();
  mFaultList = alloc.get_array<const TpgFault*>(mFaultNum);
  for ( int i = 0; i < mFaultNum; ++ i ) {
    TpgFault* fault = fault_list[i];
    mFaultList[i] = fault;
    fault->set_mffc(this);
  }
}

END_NAMESPACE_YM_SATPG
