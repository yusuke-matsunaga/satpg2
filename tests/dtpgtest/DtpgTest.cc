
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgTest.h"
#include "Dtpg_se.h"
#include "Dtpg.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "Justifier.h"
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
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mFaultType(fault_type),
  mNetwork(network),
  mFaultMgr(network)
{
  bool td_mode = (fault_type == FaultType::TransitionDelay);
  mJustifier = nullptr;
  switch ( bt_mode ) {
  case 1: mJustifier = new_Just1(network.node_num()); break;
  case 2: mJustifier = new_Just2(network.node_num()); break;
  default: mJustifier = new_Just2(network.node_num()); break;
  }
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
pair<int, int>
DtpgTest::single_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;

  for ( auto fault: mNetwork.rep_fault_list() ) {
    if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
      const TpgNode* node = fault->tpg_onode();
      Dtpg_se dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, node);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
      if ( ans == SatBool3::True ) {
	++ detect_num;
	mDop(fault, nodeval_list);
      }
      else if ( ans == SatBool3::False ) {
	++ untest_num;
      }
      mStats.merge(dtpg.stats());
    }
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief FFRモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::ffr_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    Dtpg_se dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, ffr);
    for ( auto fault: ffr.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief MFFCモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::mffc_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& mffc: mNetwork.mffc_list() ) {
    Dtpg_se dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, mffc);
    for ( auto fault: mffc.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief シングルモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::single_new_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto fault: mNetwork.rep_fault_list() ) {
    if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
      const TpgNode* node = fault->tpg_onode();
      Dtpg dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, node);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
      if ( ans == SatBool3::True ) {
	++ detect_num;
	mDop(fault, nodeval_list);
      }
      else if ( ans == SatBool3::False ) {
	++ untest_num;
      }
      mStats.merge(dtpg.stats());
    }
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief FFRモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::ffr_new_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    Dtpg dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, ffr);
    for ( auto fault: ffr.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
  }

  mTimer.stop();

  return make_pair(detect_num, untest_num);
}

// @brief MFFCモードのテストを行う．
// @return 検出故障数と冗長故障数を返す．
pair<int, int>
DtpgTest::mffc_new_test()
{
  mTimer.reset();
  mTimer.start();

  int detect_num = 0;
  int untest_num = 0;
  for ( auto& mffc: mNetwork.mffc_list() ) {
    Dtpg dtpg(mSatType, mSatOption, mSatOutP, mFaultType, *mJustifier, mNetwork, mffc);
    for ( auto fault: mffc.fault_list() ) {
      if ( mFaultMgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  ++ detect_num;
	  mDop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  ++ untest_num;
	}
      }
    }
    mStats.merge(dtpg.stats());
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
DtpgTest::print_stats(int detect_num,
		      int untest_num)
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
