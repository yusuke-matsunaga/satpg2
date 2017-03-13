
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"
#include "Dtpg.h"
#include "NodeValList.h"
#include "BackTracer.h"
#include "Fsim.h"
#include "DetectOp.h"
#include "DopList.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

const char* argv0 = "";

pair<ymuint, ymuint>
single_test(Dtpg& dtpg,
	    const TpgNetwork& network,
	    TpgFaultMgr& fmgr,
	    DetectOp& dop,
	    DtpgStats& stats)
{
  ymuint detect_num = 0;
  ymuint untest_num = 0;
  ymuint nf = network.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      const TpgFFR* ffr = fault->ffr();

      dtpg.gen_ffr_cnf(network, ffr, stats);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list, stats);
      if ( ans == kB3True ) {
	++ detect_num;
	dop(fault, nodeval_list);
      }
      else if ( ans == kB3False ) {
	++ untest_num;
      }
    }
  }

  return make_pair(detect_num, untest_num);
}

pair<ymuint, ymuint>
ffr_test(Dtpg& dtpg,
	 const TpgNetwork& network,
	 TpgFaultMgr& fmgr,
	 DetectOp& dop,
	 DtpgStats& stats)
{
  ymuint detect_num = 0;
  ymuint untest_num = 0;
  for (ymuint i = 0; i < network.ffr_num(); ++ i) {
    const TpgFFR* ffr = network.ffr(i);

    dtpg.gen_ffr_cnf(network, ffr, stats);

    ymuint nf = ffr->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = ffr->fault(j);
      if ( fmgr.status(fault) == kFsUndetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list, stats);
	if ( ans == kB3True ) {
	  ++ detect_num;
	  dop(fault, nodeval_list);
	}
	else if ( ans == kB3False ) {
	  ++ untest_num;
	}
      }
    }
  }

  return make_pair(detect_num, untest_num);
}

pair<ymuint, ymuint>
mffc_test(Dtpg& dtpg,
	  const TpgNetwork& network,
	  TpgFaultMgr& fmgr,
	  DetectOp& dop,
	  DtpgStats& stats)
{
  ymuint detect_num = 0;
  ymuint untest_num = 0;
  for (ymuint i = 0; i < network.mffc_num(); ++ i) {
    const TpgMFFC* mffc = network.mffc(i);

    dtpg.gen_mffc_cnf(network, mffc, stats);

    ymuint nf = mffc->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* fault = mffc->fault(j);

      if ( fmgr.status(fault) == kFsUndetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list, stats);
	if ( ans == kB3True ) {
	  ++ detect_num;
	  dop(fault, nodeval_list);
	}
	else if ( ans == kB3False ) {
	  ++ untest_num;
	}
      }
    }
  }

  return make_pair(detect_num, untest_num);
}

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?--single|--mffc? --blif|--iscas89 <file>" << endl;
}

int
dtpg_test(int argc,
	  char** argv)
{
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;

  bool blif = false;
  bool iscas89 = false;

  bool single = false;
  bool ffr = false;
  bool mffc = false;

  bool dump = false;
  bool verify = false;

  int bt_mode = -1;

  argv0 = argv[0];

  ymuint pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--single") == 0 ) {
	if ( ffr || mffc ) {
	  cerr << "--single, --ffr and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	single = true;
      }
      else if ( strcmp(argv[pos], "--ffr") == 0 ) {
	if ( single || mffc ) {
	  cerr << "--single, --ffr  and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	ffr = true;
      }
      else if ( strcmp(argv[pos], "--mffc") == 0 ) {
	if ( single || ffr ) {
	  cerr << "--single, --ffr and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	mffc = true;
      }
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	if ( iscas89 ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	blif = true;
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	if ( blif ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	iscas89 = true;
      }
      else if ( strcmp(argv[pos], "--bt0") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 0;
      }
      else if ( strcmp(argv[pos], "--bt1") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 1;
      }
      else if ( strcmp(argv[pos], "--bt2") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 2;
      }
      else if ( strcmp(argv[pos], "--verify") == 0 ) {
	verify = true;
      }
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
      }
      else {
	cerr << argv[pos] << ": illegal option" << endl;
	usage();
	return -1;
      }
    }
    else {
      break;
    }
  }

  if ( pos != argc - 1 ) {
    usage();
    return -1;
  }

  if ( !single && !ffr && !mffc ) {
    // mffc をデフォルトにする．
    mffc = true;
  }

  if ( !blif && !iscas89 ) {
    // とりあえず blif をデフォルトにする．
    blif = true;
  }

  if ( bt_mode == -1 ) {
    // bt0 をデフォルトにする．
    bt_mode = 0;
  }

  string filename = argv[pos];
  TpgNetwork network;
  if ( blif ) {
    if ( !network.read_blif(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else if ( iscas89 ) {
    if ( !network.read_iscas89(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else {
    ASSERT_NOT_REACHED;
  }

  if ( dump ) {
    print_network(cout, network);
  }

  Fsim* fsim = nullptr;
  DopList dop_list;
  if ( verify ) {
    fsim = new_Fsim3();
    fsim->set_network(network);
    dop_list.add(new_DopSaVerify(*fsim));
  }

  TpgFaultMgr fmgr(network);

  BackTracer bt(bt_mode, network.node_num());

  bool td_mode = false;
  Dtpg dtpg(sat_type, sat_option, sat_outp, td_mode, bt);

  StopWatch timer;
  timer.start();

  DtpgStats stats;
  pair<ymuint, ymuint> num_pair;
  if ( single ) {
    num_pair = single_test(dtpg, network, fmgr, dop_list, stats);
  }
  else if ( ffr ) {
    num_pair = ffr_test(dtpg, network, fmgr, dop_list, stats);
  }
  else if ( mffc ) {
    num_pair = mffc_test(dtpg, network, fmgr, dop_list, stats);
  }
  else {
    ASSERT_NOT_REACHED;
  }

  delete fsim;

  timer.stop();
  USTime time = timer.time();

  ymuint detect_num = num_pair.first;
  ymuint untest_num = num_pair.second;

  cout << "# of inputs             = " << network.input_num() << endl
       << "# of outputs            = " << network.output_num() << endl
       << "# of DFFs               = " << network.dff_num() << endl
       << "# of logic gates        = " << network.node_num() - network.ppi_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of total faults       = " << network.rep_fault_num() << endl
       << "# of detected faults    = " << detect_num << endl
       << "# of untestable faults  = " << untest_num << endl
       << "Total CPU time          = " << time << endl;

  ios::fmtflags save = cout.flags();
  cout.setf(ios::fixed, ios::floatfield);
  if ( stats.mCnfGenCount > 0 ) {
    cout << "CNF generation" << endl
	 << "  " << setw(10) << stats.mCnfGenCount
	 << "  " << stats.mCnfGenTime
	 << "  " << setw(8) << stats.mCnfGenTime.usr_time_usec() / stats.mCnfGenCount
	 << "u usec"
	 << "  " << setw(8) << stats.mCnfGenTime.sys_time_usec() / stats.mCnfGenCount
	 << "s usec" << endl;
  }
  if ( stats.mDetCount > 0 ) {
    cout << endl
	 << "*** SAT instances (" << stats.mDetCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << stats.mDetTime.usr_time() << "u"
	 << " " << setw(8) << stats.mDetTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << stats.mDetTime.usr_time_usec() / stats.mDetCount
	 << "u"
	 << " " << setw(8) << stats.mDetTime.sys_time_usec() / stats.mDetCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) stats.mDetStats.mRestart / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) stats.mDetStats.mConflictNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) stats.mDetStats.mDecisionNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) stats.mDetStats.mPropagationNum / stats.mDetCount
	 << " / " << setw(8) << stats.mDetStatsMax.mPropagationNum << endl;
  }
  if ( stats.mRedCount > 0 ) {
    cout << endl
	 << "*** UNSAT instances (" << stats.mRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << stats.mRedTime.usr_time() << "u"
	 << " " << setw(8) << stats.mRedTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << stats.mRedTime.usr_time_usec() / stats.mRedCount
	 << "u"
	 << " " << setw(8) << stats.mRedTime.sys_time_usec() / stats.mRedCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) stats.mRedStats.mRestart / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) stats.mRedStats.mConflictNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) stats.mRedStats.mDecisionNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) stats.mRedStats.mPropagationNum / stats.mRedCount
	 << " / " << setw(8) << stats.mRedStatsMax.mPropagationNum << endl;
  }
  if ( stats.mPartRedCount > 0 ) {
    cout << endl
	 << "*** Partial UNSAT instances (" << stats.mPartRedCount << ") ***" << endl
	 << "Total CPU time  (s)            = " << setw(10) << stats.mPartRedTime.usr_time() << "u"
	 << " " << setw(8) << stats.mPartRedTime.sys_time() << "s" << endl
	 << "Ave. CPU time (usec)           = "
	 << setw(10) << stats.mPartRedTime.usr_time_usec() / stats.mPartRedCount
	 << "u"
	 << " " << setw(8) << stats.mPartRedTime.sys_time_usec() / stats.mPartRedCount
	 << "s" << endl

	 << "# of restarts (Ave./Max)       = "
	 << setw(10) << (double) stats.mPartRedStats.mRestart / stats.mPartRedCount
	 << " / " << setw(8) << stats.mPartRedStatsMax.mRestart << endl

	 << "# of conflicts (Ave./Max)      = "
	 << setw(10) << (double) stats.mPartRedStats.mConflictNum / stats.mPartRedCount
	 << " / " << setw(8) << stats.mPartRedStatsMax.mConflictNum << endl

	 << "# of decisions (Ave./Max)      = "
	 << setw(10) << (double) stats.mPartRedStats.mDecisionNum / stats.mPartRedCount
	 << " / " << setw(8) << stats.mPartRedStatsMax.mDecisionNum << endl

	 << "# of implications (Ave./Max)   = "
	 << setw(10) << (double) stats.mPartRedStats.mPropagationNum / stats.mPartRedCount
	 << " / " << setw(8) << stats.mPartRedStatsMax.mPropagationNum << endl;
  }
  if ( stats.mAbortCount > 0 ) {
    cout << endl
	 << "*** ABORT instances ***" << endl
	 << "  " << setw(10) << stats.mAbortCount
	 << "  " << stats.mAbortTime
	 << "  " << setw(8) << stats.mAbortTime.usr_time_usec() / stats.mAbortCount
	 << "u usec"
	 << "  " << setw(8) << stats.mAbortTime.sys_time_usec() / stats.mAbortCount
	 << "s usec" << endl;
  }
  cout << endl
       << "*** backtrace time ***" << endl
       << "  " << stats.mBackTraceTime
       << "  " << setw(8) << stats.mBackTraceTime.usr_time_usec() / stats.mDetCount
       << "u usec"
       << "  " << setw(8) << stats.mBackTraceTime.sys_time_usec() / stats.mDetCount
       << "s usec" << endl;
  cout.flags(save);

  return 0;
}

END_NAMESPACE_YM_SATPG


int
main(int argc,
     char** argv)
{
  return nsYm::nsSatpg::dtpg_test(argc, argv);
}
