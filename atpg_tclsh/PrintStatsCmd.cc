
/// @file src/atpg_tclsh/PrintStatsCmd.cc
/// @brief ランダムパタン生成を行うコマンド
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "PrintStatsCmd.h"
#include "TpgNetwork.h"
#include "TpgFaultMgr.h"
#include "ym/TclPopt.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// 情報を表示するコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PrintStatsCmd::PrintStatsCmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptSa = new TclPopt(this, "stuck-at",
			"for Stuck-At faults");

  mPoptTd = new TclPopt(this, "transition-delay",
			"for Transition Delay faults");

  new_popt_group(mPoptSa, mPoptTd);

  set_usage_string("?filename?");
  mStopWatch.start();
}

// @brief デストラクタ
PrintStatsCmd::~PrintStatsCmd()
{
}

// コマンド処理関数
int
PrintStatsCmd::cmd_proc(TclObjVector& objv)
{
  // このコマンドはファイル名のみを引数に取る．
  // 引数がなければ標準出力に出す．
  ymuint objc = objv.size();
  if ( objc > 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  // 出力先のストリームを開く
  ostream* osp = &cout;
  ofstream ofs;
  if ( objc == 2 ) {
    string filename = objv[1];
    if ( !open_ofile(ofs, filename) ) {
      // ファイルが開けなかった．
      return TCL_ERROR;
    }
    osp = &ofs;
  }

  // 参照を使いたいのでめんどくさいことをやっている．
  ostream& out = *osp;

#warning "TODO: ostream を使うようにする．"

  USTime lap = mStopWatch.time();

  USTime r_time = read_time();
  USTime d_time = dtpg_time();
  USTime f_time = fsim_time();
  USTime s_time = sat_time();
  USTime m_time = misc_time();

  TpgFaultMgr& fmgr = mPoptTd->is_specified() ? _td_fault_mgr() : _sa_fault_mgr();

  ymuint n_all = _network().max_fault_id();
  ymuint n_rep = _network().rep_fault_num();
  ymuint n_remain = 0;
  ymuint n_untest = 0;
  ymuint n_det = 0;
  for (ymuint i = 0; i < n_rep; ++ i) {
    const TpgFault* fault = _network().rep_fault(i);
    switch ( fmgr.status(fault) ) {
    case kFsDetected:   ++ n_det; break;
    case kFsUntestable: ++ n_untest; break;
    case kFsUndetected: ++ n_remain; break;
    default: break;
    }
  }

  const vector<const TestVector*>& tv_list = mPoptTd->is_specified() ? _td_tv_list() : _sa_tv_list();

  fprintf(stdout, "#A: # of total faults       = %7lu\n", n_rep);
  fprintf(stdout, "#B: # of detected faults    = %7lu\n", n_det);
  fprintf(stdout, "#C: # of redundant faults   = %7lu\n", n_untest);
  fprintf(stdout, "#D: # of undetected faults  = %7lu\n", n_remain);
  fprintf(stdout, "#E: # of generated patterns = %7lu\n", tv_list.size());
  fprintf(stdout, "#F: # of MFFCs              = %7u\n", _network().mffc_num());
  fprintf(stdout, "#G: # of FFRs               = %7u\n", _network().ffr_num());
  fprintf(stdout, "#H:   Total CPU time        = %7.2fu %7.2fs\n",
	  lap.usr_time(), lap.sys_time());
  fprintf(stdout, "#I:    (read time           = %7.2f)\n",
	  r_time.usr_time());
  fprintf(stdout, "#J:    (dtpg time           = %7.2f)\n",
	  d_time.usr_time());
  fprintf(stdout, "#K:    (fsim time           = %7.2f)\n",
	  f_time.usr_time());
  fprintf(stdout, "#L:    (sat  time           = %7.2f)\n",
	  s_time.usr_time());
  fprintf(stdout, "#M:    (misc time           = %7.2f)\n",
	  m_time.usr_time());

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
