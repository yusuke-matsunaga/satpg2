
/// @file TpgFFR.cc
/// @brief TpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgFFR
//////////////////////////////////////////////////////////////////////

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] fault_list 故障のリスト
// @param[in] alloc メモリアロケータ
void
TpgFFR::set(const TpgNode* root,
	    vector<TpgFault*>& fault_list,
	    Alloc& alloc)
{
  mRoot = root;

  mFaultNum = fault_list.size();
  mFaultList = alloc.get_array<const TpgFault*>(mFaultNum);
  for ( int i = 0; i < mFaultNum; ++ i ) {
    TpgFault* fault = fault_list[i];
    mFaultList[i] = fault;
    fault->set_ffr(this);
  }
}

END_NAMESPACE_YM_SATPG
