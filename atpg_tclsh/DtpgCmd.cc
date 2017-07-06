
/// @file DtpgCmd.cc
/// @brief DtpgCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgCmd.h"
#include "ym/TclPopt.h"
#include "AtpgMgr.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFaultMgr.h"
#include "DtpgStats.h"
#include "Dtpg.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "BackTracer.h"
#include "DetectOp.h"
#include "DopList.h"
#include "DopVerifyResult.h"
#include "UntestOp.h"
#include "UopList.h"


BEGIN_NAMESPACE_YM_SATPG

void
run_single(Dtpg& dtpg,
	   const TpgNetwork& network,
	   TpgFaultMgr& fmgr,
	   DetectOp& dop,
	   UntestOp& uop,
	   DtpgStats& stats)
{
  ymuint nf = network.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      const TpgFFR* ffr = fault->ffr();
      dtpg.gen_ffr_cnf(network, ffr, stats);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list, stats);
      if ( ans == kB3True ) {
	dop(fault, nodeval_list);
      }
      else if ( ans == kB3False ) {
	uop(fault);
      }
    }
  }
}

void
run_mffc(Dtpg& dtpg,
	 const TpgNetwork& network,
	 TpgFaultMgr& fmgr,
	 DetectOp& dop,
	 UntestOp& uop,
	 DtpgStats& stats)
{
  ymuint n = network.mffc_num();
  for (ymuint i = 0; i < n; ++ i) {
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
	  dop(fault, nodeval_list);
	}
	else if ( ans == kB3False ) {
	  uop(fault);
	}
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////
// テストパタン生成を行うコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgCmd::DtpgCmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptVerbose = new TclPoptInt(this, "verbose",
				"specify verbose level (0, 1, 2, ...)");
  mPoptSat = new TclPoptStr(this, "sat",
			 "SAT mode");
  mPoptSatOption = new TclPoptStr(this, "sat-option",
			 "SAT option <STRING>");
  mPoptSatRec = new TclPopt(this, "satrec",
			 "SATREC mode");
  mPoptMiniSat = new TclPopt(this, "minisat",
			     "MINISAT mode");
  mPoptMiniSat2 = new TclPopt(this, "minisat2",
			     "MINISAT-2 mode");
  mPoptYmSat1 = new TclPopt(this, "ymsat1",
			    "YmSat1 mode");
  mPoptStuckAt = new TclPopt(this, "stuck-at",
			     "stuck-at fault mode");
  mPoptTransitionDelay = new TclPopt(this, "transition-delay",
				     "transition delay fault mode");
  mPoptPrintStats = new TclPopt(this, "print_stats",
				"print statistics");
  mPoptSingle0 = new TclPopt(this, "single0",
			     "original single mode");
  mPoptSingle = new TclPopt(this, "single",
			    "single mode");
  mPoptMFFC = new TclPopt(this, "mffc",
			  "MFFC mode");
  mPoptX = new TclPoptInt(this, "x",
			  "X-extract mode [0-2]");
  mPoptDrop = new TclPopt(this, "drop",
			  "with fault drop");
  mPoptKDet = new TclPoptInt(this, "k_det",
			     "detection count");
  mPoptOpt = new TclPoptStr(this, "option",
			    "specify option string <STR>");
  mPoptVerify = new TclPopt(this, "verify",
			    "verify generated pattern");
  mPoptNoPat = new TclPopt(this, "no_pat",
			   "do not generate patterns");
  mPoptTimer = new TclPopt(this, "timer",
			   "enable timer");
  mPoptNoTimer = new TclPopt(this, "notimer",
			     "disable timer");

  new_popt_group(mPoptSat, mPoptMiniSat, mPoptMiniSat2, mPoptSatRec);
  new_popt_group(mPoptStuckAt, mPoptTransitionDelay);

  TclPoptGroup* g0 = new_popt_group(mPoptSingle0, mPoptSingle, mPoptMFFC);

  new_popt_group(mPoptTimer, mPoptNoTimer);
}

// @brief デストラクタ
DtpgCmd::~DtpgCmd()
{
}

// コマンド処理関数
int
DtpgCmd::cmd_proc(TclObjVector& objv)
{
  ymuint objc = objv.size();
  if ( objc != 1 ) {
    print_usage();
    return TCL_ERROR;
  }

  // verbose level の設定
  if ( mPoptVerbose->is_specified() ) {
#if 0
    int verbose_level = mPoptVerbose->val();
    mDtpg.set_verbose_level(verbose_level);
#endif
  }

  // SAT mode の設定
  string sat_type;
  string sat_option;
  ostream* outp = nullptr;
  if ( mPoptSatOption->is_specified() ) {
    sat_option = mPoptSatOption->val();
  }
  if ( mPoptSat->is_specified() ) {
    sat_type = mPoptSat->val();
  }
  else {
    sat_type = "";
  }

  bool print_stats = mPoptPrintStats->is_specified();

  string engine_type;
  ymuint mode_val = 0;
  ymuint kdet_val = 0;
  if ( mPoptSingle->is_specified() ) {
    if ( mPoptKDet->is_specified() ) {
      engine_type = "single_kdet";
      kdet_val = mPoptKDet->val();
    }
    else {
      engine_type = "single";
    }
  }
  else if ( mPoptSingle0->is_specified() ) {
    engine_type = "single0";
  }
  else if ( mPoptMFFC->is_specified() ) {
    engine_type = "mffc";
  }

  bool sa_mode = true;
  FaultType fault_type = kFtStuckAt;
  if ( mPoptTransitionDelay->is_specified() ) {
    sa_mode = false;
    fault_type = kFtTransitionDelay;
  }

  string option_str = mPoptOpt->val();

  DopList dop_list;
  UopList uop_list;

  TvMgr& tv_mgr = sa_mode ? _sa_tv_mgr() : _td_tv_mgr();
  vector<const TestVector*>& tv_list = sa_mode ? _sa_tv_list() : _td_tv_list();
  TpgFaultMgr& fault_mgr = sa_mode ? _sa_fault_mgr() : _td_fault_mgr();

  if ( !mPoptNoPat->is_specified() ) {
    dop_list.add(new_DopTvList(tv_mgr, tv_list));
  }
  dop_list.add(new_DopBase(fault_mgr));
  uop_list.add(new_UopBase(fault_mgr));

  ymuint xmode = 0;
  if ( mPoptX->is_specified() ) {
    xmode = mPoptX->val();
  }

  BackTracer bt(xmode, fault_type, _network().node_num());

  if ( mPoptDrop->is_specified() ) {
    dop_list.add(new_DopDrop(fault_mgr, _fsim3()));
  }

  DopVerifyResult verify_result;
  if ( mPoptVerify->is_specified() ) {
    dop_list.add(new_DopVerify(_fsim3(), verify_result));
  }

  bool timer_enable = true;
  if ( mPoptNoTimer->is_specified() ) {
    timer_enable = false;
  }

  _fsim3().set_skip_all();
  for (ymuint i = 0; i < fault_mgr.max_fault_id(); ++ i) {
    const TpgFault* f = fault_mgr.fault(i);
    if ( f != nullptr && fault_mgr.status(f) == kFsUndetected ) {
      _fsim3().clear_skip(f);
    }
  }

  Dtpg dtpg(sat_type, sat_option, outp, fault_type, bt);

  DtpgStats stats;
  if ( engine_type == "single" ) {
    run_single(dtpg, _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "mffc" ) {
    run_mffc(dtpg, _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else {
    run_single(dtpg, _network(), fault_mgr, dop_list, uop_list, stats);
  }

  after_update_faults();

  // -verify オプションの処理
  if ( mPoptVerify->is_specified() ) {
    ymuint n = verify_result.error_count();
    for (ymuint i = 0; i < n; ++ i) {
      const TpgFault* f = verify_result.error_fault(i);
      const NodeValList& assign_list = verify_result.error_assign_list(i);
      cout << "Error: " << f->str() << " is not detected with "
	   << assign_list << endl;
    }
  }

  // -print_stats オプションの処理
  if ( print_stats ) {
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
  }

  // stats の結果を TCL 変数にセットする．
  TclObj base("::atpg::dtpg_stats");
  set_var(base, "cnf_count",
	  stats.mCnfGenCount,
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "cnf_time",
	  stats.mCnfGenTime.usr_time(),
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "det_count",
	  stats.mDetCount,
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "det_time",
	  stats.mDetTime.usr_time(),
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "part_red_count",
	  stats.mPartRedCount,
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "part_red_time",
	  stats.mPartRedTime.usr_time(),
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "red_count",
	  stats.mRedCount,
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "red_time",
	  stats.mRedTime.usr_time(),
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
