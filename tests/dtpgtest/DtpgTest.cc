
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgTest.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "BackTracer.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATタイプ
// @param[in] sat_option SATオプション
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
DtpgTest::DtpgTest(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   FaultType fault_type,
		   int bt_mode,
		   const TpgNetwork& network) :
  mFaultType(fault_type),
  mNetwork(network),
  mFaultMgr(network),
  mBackTracer(bt_mode, fault_type, network.node_num()),
  mDtpg(sat_type, sat_option, sat_outp, fault_type, mBackTracer)
{
  mFsim = Fsim::new_Fsim3(network, fault_type);
  mDop.add(new_DopVerify(*mFsim, mVerifyResult));
}

// @brief デストラクタ
DtpgTest::~DtpgTest()
{
  delete mFsim;
}

// @brief シングルモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<ymuint, ymuint>
DtpgTest::single_test()
{
  mTimer.reset();
  mTimer.start();

  ymuint detect_num = 0;
  ymuint untest_num = 0;
  ymuint nf = mNetwork.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = mNetwork.rep_fault(i);
    if ( mFaultMgr.status(fault) == kFsUndetected ) {
      const TpgFFR* ffr = fault->ffr();
      mDtpg.gen_ffr_cnf(mNetwork, ffr, mStats);
      NodeValList nodeval_list;
      SatBool3 ans = mDtpg.dtpg(fault, nodeval_list, mStats);
      if ( ans == kB3True ) {
	++ detect_num;
	mDop(fault, nodeval_list);
      }
      else if ( ans == kB3False ) {
	++ untest_num;
      }
    }
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief FFRモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<ymuint, ymuint>
DtpgTest::ffr_test()
{
  mTimer.reset();
  mTimer.start();

  ymuint detect_num = 0;
  ymuint untest_num = 0;
  for (ymuint i = 0; i < mNetwork.ffr_num(); ++ i) {
    const TpgFFR* ffr = mNetwork.ffr(i);
    mDtpg.gen_ffr_cnf(mNetwork, ffr, mStats);
    ymuint nf = ffr->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = ffr->fault(j);
      if ( mFaultMgr.status(fault) == kFsUndetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = mDtpg.dtpg(fault, nodeval_list, mStats);
	if ( ans == kB3True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == kB3False ) {
	  ++ untest_num;
	}
      }
    }
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief MFFCモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<ymuint, ymuint>
DtpgTest::mffc_test()
{
  mTimer.reset();
  mTimer.start();

  ymuint detect_num = 0;
  ymuint untest_num = 0;
  for (ymuint i = 0; i < mNetwork.mffc_num(); ++ i) {
    const TpgMFFC* mffc = mNetwork.mffc(i);
    mDtpg.gen_mffc_cnf(mNetwork, mffc, mStats);
        ymuint nf = mffc->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = mffc->fault(j);
      if ( mFaultMgr.status(fault) == kFsUndetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = mDtpg.dtpg(fault, nodeval_list, mStats);
	if ( ans == kB3True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == kB3False ) {
	  ++ untest_num;
	}
      }
    }
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief 検証結果を得る．
const DopVerifyResult&
DtpgTest::verify_result() const
{
  return mVerifyResult;
}

// @brief 統計情報を出力する．
void
DtpgTest::print_stats(ymuint detect_num,
		      ymuint untest_num)
{
  USTime time = mTimer.time();

  cout << "# of inputs             = " << mNetwork.input_num() << endl
       << "# of outputs            = " << mNetwork.output_num() << endl
       << "# of DFFs               = " << mNetwork.dff_num() << endl
       << "# of logic gates        = " << mNetwork.node_num() - mNetwork.ppi_num() << endl
       << "# of MFFCs              = " << mNetwork.mffc_num() << endl
       << "# of FFRs               = " << mNetwork.ffr_num() << endl
       << "# of total faults       = " << mNetwork.rep_fault_num() << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "Total CPU time          = " << time << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( mStats.mCnfGenCount > 0 ) {
    cout << "CNF generation" << endl
	 << "  " << setw(10) << mStats.mCnfGenCount
	 << "  " << mStats.mCnfGenTime
	 << "  " << setw(8) << mStats.mCnfGenTime.usr_time_usec() / mStats.mCnfGenCount
	 << "u usec"
	 << "  " << setw(8) << mStats.mCnfGenTime.sys_time_usec() / mStats.mCnfGenCount
	 << "s usec" << endl;
  }
  if ( mStats.mDetCount > 0 ) {
    cout << endl
	 << "*** SAT instances (" << mStats.mDetCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << mStats.mDetTime.usr_time() << "u"
	 << " " << setw(8) << mStats.mDetTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << mStats.mDetTime.usr_time_usec() / mStats.mDetCount
	 << "u"
	 << " " << setw(8) << mStats.mDetTime.sys_time_usec() / mStats.mDetCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) mStats.mDetStats.mRestart / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) mStats.mDetStats.mConflictNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) mStats.mDetStats.mDecisionNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) mStats.mDetStats.mPropagationNum / mStats.mDetCount
	 << " / " << setw(8) << mStats.mDetStatsMax.mPropagationNum << endl;
  }
  if ( mStats.mRedCount > 0 ) {
    cout << endl
	 << "*** UNSAT instances (" << mStats.mRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << mStats.mRedTime.usr_time() << "u"
	 << " " << setw(8) << mStats.mRedTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << mStats.mRedTime.usr_time_usec() / mStats.mRedCount
	 << "u"
	 << " " << setw(8) << mStats.mRedTime.sys_time_usec() / mStats.mRedCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) mStats.mRedStats.mRestart / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) mStats.mRedStats.mConflictNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) mStats.mRedStats.mDecisionNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) mStats.mRedStats.mPropagationNum / mStats.mRedCount
	 << " / " << setw(8) << mStats.mRedStatsMax.mPropagationNum << endl;
  }
  if ( mStats.mPartRedCount > 0 ) {
    cout << endl
	 << "*** Partial UNSAT instances (" << mStats.mPartRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << mStats.mPartRedTime.usr_time() << "u"
	 << " " << setw(8) << mStats.mPartRedTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << mStats.mPartRedTime.usr_time_usec() / mStats.mPartRedCount
	 << "u"
	 << " " << setw(8) << mStats.mPartRedTime.sys_time_usec() / mStats.mPartRedCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) mStats.mPartRedStats.mRestart / mStats.mPartRedCount
	 << " / " << setw(8) << mStats.mPartRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) mStats.mPartRedStats.mConflictNum / mStats.mPartRedCount
	 << " / " << setw(8) << mStats.mPartRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) mStats.mPartRedStats.mDecisionNum / mStats.mPartRedCount
	 << " / " << setw(8) << mStats.mPartRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) mStats.mPartRedStats.mPropagationNum / mStats.mPartRedCount
	 << " / " << setw(8) << mStats.mPartRedStatsMax.mPropagationNum << endl;
  }
  if ( mStats.mAbortCount > 0 ) {
    cout << endl
	 << "*** ABORT instances ***" << endl
	 << "  " << setw(10) << mStats.mAbortCount
	 << "  " << mStats.mAbortTime
	 << "  " << setw(8) << mStats.mAbortTime.usr_time_usec() / mStats.mAbortCount
	 << "u usec"
	 << "  " << setw(8) << mStats.mAbortTime.sys_time_usec() / mStats.mAbortCount
	 << "s usec" << endl;
  }
  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << mStats.mBackTraceTime
       << "  " << setw(8) << mStats.mBackTraceTime.usr_time_usec() / mStats.mDetCount
       << "u usec"
       << "  " << setw(8) << mStats.mBackTraceTime.sys_time_usec() / mStats.mDetCount
       << "s usec" << endl;
  cout.flags(save);
}

END_NAMESPACE_YM_SATPG
