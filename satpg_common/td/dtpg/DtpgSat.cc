
/// @file DtpgSat.cc
/// @brief DtpgSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgSat.h"

#include "TpgFault.h"

#include "DetectOp.h"
#include "UntestOp.h"
#include "td/DtpgStats.h"

#include "td/BackTracer.h"
#include "NodeValList.h"
#include "ValMap.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_TD

// @brief コンストラクタ
DtpgSat::DtpgSat(const string& sat_type,
		 const string& sat_option,
		 ostream* sat_outp,
		 BackTracer& bt,
		 DetectOp& dop,
		 UntestOp& uop) :
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mBackTracer(bt),
  mDetectOp(dop),
  mUntestOp(uop)
{
  mTimerEnable = false;
}

// @brief デストラクタ
DtpgSat::~DtpgSat()
{
}

// @brief オプションを設定する．
void
DtpgSat::set_option(const string& option_str)
{
  for (string::size_type next = 0; ; ++ next) {
    string::size_type pos = option_str.find(':', next);
    if ( pos == next ) {
      continue;
    }
    string option = option_str.substr(next, pos - next);
    if ( pos == string::npos ) {
      break;
    }
    next = pos;
  }
}

// @brief 統計情報をクリアする．
void
DtpgSat::clear_stats()
{
  mStats.mCnfGenCount = 0;
  mStats.mCnfGenTime.set(0.0, 0.0, 0.0);

  mStats.mDetCount = 0;
  mStats.mDetTime.set(0.0, 0.0, 0.0);
  mStats.mDetStats.clear();
  mStats.mDetStatsMax.clear();

  mStats.mRedCount = 0;
  mStats.mRedTime.set(0.0, 0.0, 0.0);
  mStats.mRedStats.clear();
  mStats.mRedStatsMax.clear();

  mStats.mPartRedCount = 0;
  mStats.mPartRedTime.set(0.0, 0.0, 0.0);
  mStats.mPartRedStats.clear();
  mStats.mPartRedStatsMax.clear();

  mStats.mAbortCount = 0;
  mStats.mAbortTime.set(0.0, 0.0, 0.0);
}

// @brief 統計情報を得る．
// @param[in] stats 結果を格納する構造体
void
DtpgSat::get_stats(DtpgStats& stats) const
{
  stats = mStats;
}

// @breif 時間計測を制御する．
void
DtpgSat::timer_enable(bool enable)
{
  mTimerEnable = enable;
}

// @brief タイマーをスタートする．
void
DtpgSat::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DtpgSat::cnf_end()
{
  USTime time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
DtpgSat::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
DtpgSat::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

// @brief 一つの SAT問題を解く．
SatBool3
DtpgSat::solve(SatSolver& solver,
	       const vector<SatLiteral>& assumptions,
	       const TpgFault* fault,
	       const TpgNode* root,
	       const vector<const TpgNode*>& output_list,
	       const VidMap& hvar_map,
	       const VidMap& gvar_map,
	       const VidMap& fvar_map)
{
  StopWatch timer;

  SatStats prev_stats;
  solver.get_stats(prev_stats);

  timer.reset();
  timer.start();

  vector<SatBool3> model;
  SatBool3 ans = solver.solve(assumptions, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  solver.get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == kB3True ) {
    timer.reset();
    timer.start();

    // パタンが求まった．
    ValMap val_map(hvar_map, gvar_map, fvar_map, model);

    // バックトレースを行う．
    NodeValList assign_list;
    mBackTracer(root, assign_list, output_list, val_map, mLastAssign);

    // パタンの登録などを行う．
    mDetectOp(fault, mLastAssign);

    timer.stop();
    mStats.mBackTraceTime += timer.time();

    mStats.update_det(sat_stats, time);
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    mUntestOp(fault);

    mStats.update_red(sat_stats, time);
  }
  else { // ans == kB3X つまりアボート
    mStats.update_abort(sat_stats, time);
  }
  return ans;
}

// @brief 最後に生成された値割当リストを得る．
const NodeValList&
DtpgSat::last_assign()
{
  return mLastAssign;
}

END_NAMESPACE_YM_SATPG_TD
