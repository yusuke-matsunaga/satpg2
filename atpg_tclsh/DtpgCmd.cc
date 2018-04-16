
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
#include "TpgFFR.h"
#include "TpgMFFC.h"
#include "TpgFault.h"
#include "FaultStatusMgr.h"
#include "DtpgStats.h"
#include "Dtpg_se.h"
#include "Dtpg.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "Justifier.h"
#include "DetectOp.h"
#include "DopList.h"
#include "DopVerifyResult.h"
#include "UntestOp.h"
#include "UopList.h"


BEGIN_NAMESPACE_YM_SATPG

void
run_single_new(const string& sat_type,
	       const string& sat_option,
	       ostream* sat_outp,
	       FaultType fault_type,
	       Justifier& jt,
	       const TpgNetwork& network,
	       FaultStatusMgr& fmgr,
	       DetectOp& dop,
	       UntestOp& uop,
	       DtpgStats& stats)
{
  for ( auto fault: network.rep_fault_list() ) {
    if ( fmgr.get(fault) == FaultStatus::Undetected ) {
      const TpgNode* node = fault->tpg_onode();
      Dtpg dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, node);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
      if ( ans == SatBool3::True ) {
	dop(fault, nodeval_list);
      }
      else if ( ans == SatBool3::False ) {
	uop(fault);
      }
      stats.merge(dtpg.stats());
    }
  }
}

void
run_ffr_new(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    FaultType fault_type,
	    Justifier& jt,
	    const TpgNetwork& network,
	    FaultStatusMgr& fmgr,
	    DetectOp& dop,
	    UntestOp& uop,
	    DtpgStats& stats)
{
  int nffr = network.ffr_num();
  for ( auto& ffr: network.ffr_list() ) {
    Dtpg dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, ffr);
    for ( auto fault: ffr.fault_list() ) {
      if ( fmgr.get(fault) == FaultStatus::Undetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  dop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  uop(fault);
	}
      }
    }
    stats.merge(dtpg.stats());
  }
}

void
run_mffc_new(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     FaultType fault_type,
	     Justifier& jt,
	     const TpgNetwork& network,
	     FaultStatusMgr& fmgr,
	     DetectOp& dop,
	     UntestOp& uop,
	     DtpgStats& stats)
{
  int n = network.mffc_num();
  for ( auto& mffc: network.mffc_list() ) {
    Dtpg dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, mffc);
    for ( auto fault: mffc.fault_list() ) {
      if ( fmgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  dop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  uop(fault);
	}
      }
    }
    stats.merge(dtpg.stats());
  }
}

void
run_single(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   Justifier& jt,
	   const TpgNetwork& network,
	   FaultStatusMgr& fmgr,
	   DetectOp& dop,
	   UntestOp& uop,
	   DtpgStats& stats)
{
  for ( auto fault: network.rep_fault_list() ) {
    if ( fmgr.get(fault) == FaultStatus::Undetected ) {
      const TpgNode* node = fault->tpg_onode();
      Dtpg_se dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, node);
      NodeValList nodeval_list;
      SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
      if ( ans == SatBool3::True ) {
	dop(fault, nodeval_list);
      }
      else if ( ans == SatBool3::False ) {
	uop(fault);
      }
      stats.merge(dtpg.stats());
    }
  }
}

void
run_ffr(const string& sat_type,
	const string& sat_option,
	ostream* sat_outp,
	FaultType fault_type,
	Justifier& jt,
	const TpgNetwork& network,
	FaultStatusMgr& fmgr,
	DetectOp& dop,
	UntestOp& uop,
	DtpgStats& stats)
{
  for ( auto& ffr: network.ffr_list() ) {
    Dtpg_se dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, ffr);
    for ( auto fault: ffr.fault_list() ) {
      if ( fmgr.get(fault) == FaultStatus::Undetected ) {
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	if ( ans == SatBool3::True ) {
	  dop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  uop(fault);
	}
      }
    }
    stats.merge(dtpg.stats());
  }
}

void
run_mffc(const string& sat_type,
	 const string& sat_option,
	 ostream* sat_outp,
	 FaultType fault_type,
	 Justifier& jt,
	 const TpgNetwork& network,
	 FaultStatusMgr& fmgr,
	 DetectOp& dop,
	 UntestOp& uop,
	 DtpgStats& stats)
{
  for ( auto& mffc: network.mffc_list() ) {
    Dtpg_se dtpg(sat_type, sat_option, sat_outp, fault_type, jt, network, mffc);
    for ( auto fault: mffc.fault_list() ) {
      if ( fmgr.get(fault) == FaultStatus::Undetected ) {
	// 故障に対するテスト生成を行なう．
	NodeValList nodeval_list;
	SatBool3 ans = dtpg.dtpg(fault, nodeval_list);
	stats.merge(dtpg.stats());
	if ( ans == SatBool3::True ) {
	  dop(fault, nodeval_list);
	}
	else if ( ans == SatBool3::False ) {
	  uop(fault);
	}
      }
    }
    stats.merge(dtpg.stats());
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
  mPoptStuckAt = new TclPopt(this, "stuck-at",
			     "stuck-at fault mode");
  mPoptTransitionDelay = new TclPopt(this, "transition-delay",
				     "transition delay fault mode");
  mPoptPrintStats = new TclPopt(this, "print_stats",
				"print statistics");
  mPoptNew = new TclPopt(this, "new",
			 "use 'new' engine");
  mPoptSingle = new TclPopt(this, "single",
			    "single mode");
  mPoptFFR = new TclPopt(this, "ffr",
			 "FFR mode");
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

  new_popt_group(mPoptStuckAt, mPoptTransitionDelay);

  TclPoptGroup* g0 = new_popt_group(mPoptSingle, mPoptFFR, mPoptMFFC);

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
  int objc = objv.size();
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
  if ( mPoptSatRec->is_specified() ) {
    outp = &cout;
  }

  bool print_stats = mPoptPrintStats->is_specified();

  string engine_type;
  int mode_val = 0;
  int kdet_val = 0;
  if ( mPoptSingle->is_specified() ) {
    if ( mPoptKDet->is_specified() ) {
      engine_type = "single_kdet";
      kdet_val = mPoptKDet->val();
    }
    else {
      if ( mPoptNew->is_specified() ) {
	engine_type = "single_new";
      }
      else {
	engine_type = "single";
      }
    }
  }
  else if ( mPoptFFR->is_specified() ) {
    if ( mPoptNew->is_specified() ) {
      engine_type = "ffr_new";
    }
    else {
      engine_type = "ffr";
    }
  }
  else if ( mPoptMFFC->is_specified() ) {
    if ( mPoptNew->is_specified() ) {
      engine_type = "mffc_new";
    }
    else {
      engine_type = "mffc";
    }
  }

  bool sa_mode = true;
  FaultType fault_type = FaultType::StuckAt;
  if ( mPoptTransitionDelay->is_specified() ) {
    sa_mode = false;
    fault_type = FaultType::TransitionDelay;
  }

  string option_str = mPoptOpt->val();

  DopList dop_list;
  UopList uop_list;

  TvMgr& tv_mgr = sa_mode ? _sa_tv_mgr() : _td_tv_mgr();
  vector<const TestVector*>& tv_list = sa_mode ? _sa_tv_list() : _td_tv_list();
  Fsim& fsim3 = sa_mode ? _sa_fsim3() : _td_fsim3();
  FaultStatusMgr& fault_mgr = sa_mode ? _sa_fault_mgr() : _td_fault_mgr();

  if ( !mPoptNoPat->is_specified() ) {
    dop_list.add(new_DopTvList(tv_mgr, tv_list));
  }
  dop_list.add(new_DopBase(fault_mgr));
  uop_list.add(new_UopBase(fault_mgr));

  int xmode = 0;
  if ( mPoptX->is_specified() ) {
    xmode = mPoptX->val();
  }

  bool td_mode = (fault_type == FaultType::TransitionDelay);
  Justifier* jt = nullptr;
  switch ( xmode ) {
  case 1: jt = new_Just1(_network().node_num()); break;
  case 2: jt = new_Just2(_network().node_num()); break;
  default: jt = new_Just2(_network().node_num()); break;
  }

  if ( mPoptDrop->is_specified() ) {
    dop_list.add(new_DopDrop(fault_mgr, fsim3));
  }

  DopVerifyResult verify_result;
  if ( mPoptVerify->is_specified() ) {
    dop_list.add(new_DopVerify(fsim3, verify_result));
  }

  bool timer_enable = true;
  if ( mPoptNoTimer->is_specified() ) {
    timer_enable = false;
  }

  fsim3.set_skip_all();
  for ( auto f: _network().rep_fault_list() ) {
    if ( fault_mgr.get(f) == FaultStatus::Undetected ) {
      fsim3.clear_skip(f);
    }
  }

  DtpgStats stats;
  if ( engine_type == "single" ) {
    run_single(sat_type, sat_option, outp, fault_type, *jt,
	       _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "ffr" ) {
    run_ffr(sat_type, sat_option, outp, fault_type, *jt,
	    _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "mffc" ) {
    run_mffc(sat_type, sat_option, outp, fault_type, *jt,
	     _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "single_new" ) {
    run_single_new(sat_type, sat_option, outp, fault_type, *jt,
		   _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "ffr_new" ) {
    run_ffr_new(sat_type, sat_option, outp, fault_type, *jt,
		_network(), fault_mgr, dop_list, uop_list, stats);
  }
  else if ( engine_type == "mffc_new" ) {
    run_mffc_new(sat_type, sat_option, outp, fault_type, *jt,
		 _network(), fault_mgr, dop_list, uop_list, stats);
  }
  else {
    run_single(sat_type, sat_option, outp, fault_type, *jt,
	       _network(), fault_mgr, dop_list, uop_list, stats);
  }

  after_update_faults();

  // -verify オプションの処理
  if ( mPoptVerify->is_specified() ) {
    int n = verify_result.error_count();
    for (int i = 0; i < n; ++ i) {
      const TpgFault* f = verify_result.error_fault(i);
      const NodeValList& assign_list = verify_result.error_assign_list(i);
      cout << "Error: " << f->str() << " is not detected with "
	   << assign_list << endl;
    }
  }

  delete jt;

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
  set_var(base, "red_count",
	  stats.mRedCount,
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);
  set_var(base, "red_time",
	  stats.mRedTime.usr_time(),
	  TCL_NAMESPACE_ONLY | TCL_LEAVE_ERR_MSG);

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
