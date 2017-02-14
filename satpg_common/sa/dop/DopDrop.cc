﻿
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "DopDrop.h"
#include "TpgFaultMgr.h"
#include "sa/Fsim.h"


BEGIN_NAMESPACE_YM_SATPG_SA

// @brief 'drop' タイプを生成する．
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopDrop(TpgFaultMgr& fmgr,
	    Fsim& fsim)
{
  return new DopDrop(fmgr, fsim);
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレータ
DopDrop::DopDrop(TpgFaultMgr& fmgr,
		 Fsim& fsim) :
  mMgr(fmgr),
  mFsim(fsim)
{
}

// @brief デストラクタ
DopDrop::~DopDrop()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopDrop::operator()(const TpgFault* f,
		    const NodeValList& assign_list)
{
  ymuint n = mFsim.sppfp(assign_list);
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = mFsim.det_fault(i);
    mMgr.set_status(f, kFsDetected);
    mFsim.set_skip(f);
  }
}

END_NAMESPACE_YM_SATPG_SA
