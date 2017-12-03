
/// @file DtpgImpl.cc
/// @brief DtpgImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgImpl_new.h"

#include "TpgNetwork.h"
#include "TpgFault.h"

#include "FoCone.h"
#include "BackTracer.h"
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
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgImpl::DtpgImpl(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   FaultType fault_type,
		   BackTracer& bt,
		   const TpgNetwork& network,
		   const TpgNode* root) :
  mStructSat(network.node_num(), fault_type, sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mRoot(root),
  mBackTracer(bt),
  mTimerEnable(true)
{
}

// @brief デストラクタ
DtpgImpl::~DtpgImpl()
{
}

// @brief 回路の構造を表すCNF式を作る．
// @param[out] stats DTPGの統計情報
//
// このオブジェクトに対しては1回行えばよい．
// というか1回しか行えない．
void
DtpgImpl::gen_cnf(DtpgStats& stats)
{
  cnf_begin();

  gen_cnf_base();

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
  if ( fault->tpg_onode()->ffr_root() != root_node() ) {
    cerr << "Error[DtpgImpl::dtpg()]: " << fault << " is not within mFfrRoot's FFR" << endl;
    cerr << " fautl->ffr_root() = " << fault->tpg_onode()->ffr_root()->name() << endl;
    return kB3X;
  }

  SatBool3 ans = solve(fault, vector<SatLiteral>(), nodeval_list, stats);

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

// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
void
DtpgImpl::gen_cnf_base()
{
  mFoCone = mStructSat.add_focone(mRoot, true);

  mStructSat.make_vars();

  mStructSat.make_cnf();
}

// @brief 一つの SAT問題を解く．
// @param[in] fault 対象の故障
// @param[in] assumptions 値の決まっている変数のリスト
// @param[out] nodeval_list 結果の値割り当てリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgImpl::solve(const TpgFault* fault,
		const vector<SatLiteral>& assumptions,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  solver().get_stats(prev_stats);

  // FFR 内の故障伝搬条件を assign_list に入れる．
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  NodeValList assign_list;
  mStructSat.add_ffr_condition(ffr_root, fault, assign_list);

  // assign_list の内容と assumptions を足したものを assumptions1 に入れる．
  ymuint n0 = assumptions.size();
  ymuint n = assign_list.size();
  vector<SatLiteral> assumptions1;
  assumptions1.reserve(n + n0);
  mStructSat.conv_to_assumption(assign_list, assumptions1);
  for (ymuint i = 0; i < n0; ++ i) {
    assumptions1.push_back(assumptions[i]);
  }

  vector<SatBool3> model;
  SatBool3 ans = solver().solve(assumptions1, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  solver().get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == kB3True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    NodeValList assign_list;
    mFoCone->extract(model, assign_list);
#if 0
    // バックトレースを行う．
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

END_NAMESPACE_YM_SATPG
