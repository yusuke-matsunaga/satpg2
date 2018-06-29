
/// @file AtpgMgr.cc
/// @brief AtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgMgr.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultStatusMgr.h"
#include "TestVector.h"
#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// AtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AtpgMgr::AtpgMgr() :
  mTimer(TM_SIZE, TM_MISC)
{
  mSaFsim2 = nullptr;
  mSaFsim3 = nullptr;
  mSaFaultMgr = nullptr;
  mTdFsim2 = nullptr;
  mTdFsim3 = nullptr;
  mTdFaultMgr = nullptr;
}

// @brief デストラクタ
AtpgMgr::~AtpgMgr()
{
  delete mSaFaultMgr;
  delete mTdFaultMgr;
}

// @brief ファイル読み込みに関わる時間を得る．
USTime
AtpgMgr::read_time() const
{
  return mTimer.time(TM_READ);
}

// @brief DTPG に関わる時間を得る．
USTime
AtpgMgr::dtpg_time() const
{
  return mTimer.time(TM_DTPG);
}

// @brief 故障シミュレーションに関わる時間を得る．
USTime
AtpgMgr::fsim_time() const
{
  return mTimer.time(TM_FSIM);
}

// @brief SAT に関わる時間を得る．
USTime
AtpgMgr::sat_time() const
{
  return mTimer.time(TM_SAT);
}

// @brief その他の時間を得る．
USTime
AtpgMgr::misc_time() const
{
  return mTimer.time(TM_MISC);
}

// @brief ネットワークをセットした後に呼ぶ関数
void
AtpgMgr::after_set_network()
{
  delete mSaFaultMgr;
  delete mTdFaultMgr;

  mSaTvList.clear();
  mTdTvList.clear();

  mSaFsim2 = Fsim::new_Fsim2(_network(), FaultType::StuckAt);
  mSaFsim3 = Fsim::new_Fsim3(_network(), FaultType::StuckAt);
  mSaFaultMgr = new FaultStatusMgr(_network());
  mTdFsim2 = Fsim::new_Fsim2(_network(), FaultType::TransitionDelay);
  mTdFsim3 = Fsim::new_Fsim3(_network(), FaultType::TransitionDelay);
  mTdFaultMgr = new FaultStatusMgr(_network());
}

END_NAMESPACE_YM_SATPG
