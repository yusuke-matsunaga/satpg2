
/// @file DtpgMinPowerCmd.cc
/// @brief DtpgMinPowerCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgMinPowerCmd.h"
#include "ym/TclPopt.h"
#include "AtpgMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFaultMgr.h"
#include "DtpgStats.h"
#include "Dtpg2.h"
#include "Fsim.h"
#include "NodeValList.h"
#include "BackTracer.h"
#include "DetectOp.h"
#include "DopList.h"
#include "UntestOp.h"
#include "UopList.h"

#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// テストパタン生成を行うコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DtpgMinPowerCmd::DtpgMinPowerCmd(AtpgMgr* mgr) :
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
  mPoptPrintStats = new TclPopt(this, "print_stats",
				"print statistics");
  mPoptX = new TclPoptInt(this, "x",
			  "X-extract mode [0-2]");
  mPoptR = new TclPopt(this, "xor_sampling",
		       "use XOR-smpling");
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

  TclPoptGroup* g0 = new_popt_group(mPoptSingle0, mPoptSingle, mPoptMFFC);

  new_popt_group(mPoptTimer, mPoptNoTimer);
}

// @brief デストラクタ
DtpgMinPowerCmd::~DtpgMinPowerCmd()
{
}

// コマンド処理関数
int
DtpgMinPowerCmd::cmd_proc(TclObjVector& objv)
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

  bool td_mode = true;

  string engine_type;
  ymuint mode_val = 0;
  ymuint kdet_val = 0;

  string option_str = mPoptOpt->val();

  bool xor_sampling = mPoptR->is_specified();

  DopList dop_list;
  UopList uop_list;

  if ( !mPoptNoPat->is_specified() ) {
    dop_list.add(new_DopTvListTd(_tv_mgr(), _td_tv_list()));
  }
  dop_list.add(new_DopBase(_fault_mgr()));
  uop_list.add(new_UopBase(_fault_mgr()));

  ymuint xmode = 0;
  if ( mPoptX->is_specified() ) {
    xmode = mPoptX->val();
  }

  BackTracer bt(xmode, _network().node_num());

  if ( mPoptDrop->is_specified() ) {
    dop_list.add(new_DopDrop(_fault_mgr(), _fsim3()));
  }
  if ( mPoptVerify->is_specified() ) {
    dop_list.add(new_DopTdVerify(_fsim3()));
  }

  bool timer_enable = true;
  if ( mPoptNoTimer->is_specified() ) {
    timer_enable = false;
  }

  _fsim3().set_skip_all();
  for (ymuint i = 0; i < _fault_mgr().max_fault_id(); ++ i) {
    const TpgFault* f = _fault_mgr().fault(i);
    if ( f != nullptr && _fault_mgr().status(f) == kFsUndetected ) {
      _fsim3().clear_skip(f);
    }
  }

#if 0
  { // ランダムパタンシミュレーションを行い，平均の遷移回数を求める．
    RandGen randgen;
    ymuint max_pat = 2000;
    ymuint wsa_sum = 0;
    TestVector* tv = _tv_mgr().new_td_vector();
    for (ymuint i = 0; i < max_pat; ++ i) {
      tv->set_from_random(randgen);

      ymuint wsa = _fsim2().td_calc_wsa(tv, false);
      wsa_sum += wsa;
    }
    double wsa_ave = static_cast<double>(wsa_sum) / static_cast<double>(max_pat);
    _tv_mgr().delete_vector(tv);
    cout << "Ave. wsa = " << wsa_ave << endl;
  }
#endif

  Dtpg2 dtpg(sat_type, sat_option, outp, bt);

  DtpgStats stats;
  dtpg.run(_tv_mgr(), _fault_mgr(), _fsim2(), _network(), xor_sampling,
	   dop_list, uop_list, stats);

  after_update_faults();

  { // 求まったパタンの平均の遷移回数を求める．
    ymuint wsa_sum = 0;
    ymuint np = _td_tv_list().size();
    vector<ymuint> wsa_list;
    wsa_list.reserve(np);
    for (ymuint i = 0; i < np; ++ i) {
      const TestVector* tv = _td_tv_list()[i];
      //cout << tv->hex_str() << endl;
      ymuint wsa = _fsim2().td_calc_wsa(tv, false);
      wsa_sum += wsa;
      wsa_list.push_back(wsa);
    }
    double wsa_ave = static_cast<double>(wsa_sum) / static_cast<double>(np);
    cout << "Ave. wsa = " << wsa_ave << endl;

    double div_sum = 0.0;
    for (ymuint i = 0; i < np; ++ i) {
      ymuint wsa = wsa_list[i];
      double div = static_cast<double>(wsa) - wsa_ave;
      div_sum += div * div;
    }
    double std_div = sqrt(div_sum / static_cast<double>(np));
    cout << "Std. div = " << std_div << endl;
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
