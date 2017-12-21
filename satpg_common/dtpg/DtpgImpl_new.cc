
/// @file DtpgImpl.cc
/// @brief DtpgImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgImpl_new.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgMFFC.h"

#include "Justifier.h"
#include "ValMap.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] jt 正当化を行うファンクタ
// @param[in] max_node_id ノード番号の最大値(+1)
DtpgImpl::DtpgImpl(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   FaultType fault_type,
		   Justifier& jt,
		   ymuint max_node_id) :
  mStructEnc(max_node_id, fault_type, sat_type, sat_option, sat_outp),
  mFaultType(fault_type),
  mJustifier(jt),
  mTimerEnable(true)
{
}

// @brief デストラクタ
DtpgImpl::~DtpgImpl()
{
}

// @brief 回路の構造を表すCNF式を作る．
// @param[in] root FFR(MFFC)の根のノード
// @param[out] stats DTPGの統計情報
//
// このオブジェクトに対しては1回行えばよい．
// というか1回しか行えない．
void
DtpgImpl::gen_cnf(const TpgNode* root,
		  DtpgStats& stats)
{
  cnf_begin();

  mStructEnc.add_simple_cone(root, true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end(stats);
}

// @brief 回路の構造を表すCNF式を作る．
// @param[out] stats DTPGの統計情報
//
// このオブジェクトに対しては1回行えばよい．
// というか1回しか行えない．
void
DtpgImpl::gen_cnf(const TpgMFFC* mffc,
		  DtpgStats& stats)
{
  cnf_begin();

  if ( mffc->elem_num() > 1 ) {
    mStructEnc.add_mffc_cone(mffc, true);
  }
  else {
    mStructEnc.add_simple_cone(mffc->root(), true);
  }

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end(stats);
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgImpl::dtpg(const TpgFault* fault,
	       NodeValList& nodeval_list,
	       DtpgStats& stats)
{
#if 0
  if ( fault->tpg_onode()->ffr_root() != root_node() ) {
    cerr << "Error[DtpgImpl::dtpg()]: " << fault << " is not within mFfrRoot's FFR" << endl;
    cerr << " fautl->ffr_root() = " << fault->tpg_onode()->ffr_root()->name() << endl;
    return kB3X;
  }
#endif

  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  mStructEnc.solver().get_stats(prev_stats);

  vector<SatLiteral> assumptions;
  mStructEnc.make_fault_condition(fault, 0, assumptions);

  vector<SatBool3> model;
  SatBool3 ans = mStructEnc.solver().solve(assumptions, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mStructEnc.solver().get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == kB3True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // バックトレースを行う．
    NodeValList assign_list;
    mStructEnc.extract(model, fault, 0, assign_list);

    mStructEnc.justify(model, assign_list, mJustifier, nodeval_list);
#if 0
    //mFoCone->extract(model, assign_list);

    ValMap* val_map = mFoCone->val_map(model);
    mBackTracer(assign_list, mFoCone->output_list(), *val_map, nodeval_list);
    // ここで val_map を削除しなければならない仕様はちょっと醜い．
    delete val_map;
#endif
    timer.stop();
    stats.mBackTraceTime += timer.time();

    stats.update_det(sat_stats, time);
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    stats.update_red(sat_stats, time);
  }
  else {
    // ans == kB3X つまりアボート
    stats.update_abort(sat_stats, time);
  }

  return ans;
}

// @brief タイマーをスタートする．
void
DtpgImpl::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DtpgImpl::cnf_end(DtpgStats& stats)
{
  USTime time = timer_stop();
  stats.mCnfGenTime += time;
  ++ stats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
DtpgImpl::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
DtpgImpl::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

END_NAMESPACE_YM_SATPG
