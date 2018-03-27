
/// @file Dtpg.cc
/// @brief Dtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ(ffrモード)
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] jt 正当化を行うファンクタ
// @param[in] network 対象のネットワーク
// @param[in] ffr 故障伝搬の起点となる FFR
// @param[out] stats DTPGの統計情報
Dtpg::Dtpg(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   Justifier& jt,
	   const TpgNetwork& network,
	   const TpgFFR* ffr,
	   DtpgStats& stats) :
  mStructEnc(network.node_num(), fault_type, sat_type, sat_option, sat_outp),
  mFaultType(fault_type),
  mJustifier(jt),
  mTimerEnable(true)
{
  cnf_begin();

  mStructEnc.add_simple_cone(ffr->root(), true);

  mStructEnc.make_vars();

  mStructEnc.make_cnf();

  cnf_end(stats);
}

// @brief コンストラクタ(mffcモード)
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] jt 正当化を行うファンクタ
// @param[in] network 対象のネットワーク
// @param[in] mffc 故障伝搬の起点となる MFFC
// @param[out] stats DTPGの統計情報
//
// この MFFC に含まれるすべての FFR が対象となる．
// FFR と MFFC が一致している場合は ffr モードと同じことになる．
Dtpg::Dtpg(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   Justifier& jt,
	   const TpgNetwork& network,
	   const TpgMFFC* mffc,
	   DtpgStats& stats) :
  mStructEnc(network.node_num(), fault_type, sat_type, sat_option, sat_outp),
  mFaultType(fault_type),
  mJustifier(jt),
  mTimerEnable(true)
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

// @brief デストラクタ
Dtpg::~Dtpg()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで SatBool3::X を返す．
SatBool3
Dtpg::dtpg(const TpgFault* fault,
	   NodeValList& nodeval_list,
	   DtpgStats& stats)
{
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

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // バックトレースを行う．
    NodeValList assign_list;
    mStructEnc.extract(model, fault, 0, assign_list);

    mStructEnc.justify(model, assign_list, mJustifier, nodeval_list);

    timer.stop();
    stats.mBackTraceTime += timer.time();

    stats.update_det(sat_stats, time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    stats.update_red(sat_stats, time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    stats.update_abort(sat_stats, time);
  }

  return ans;
}

// @brief タイマーをスタートする．
void
Dtpg::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
Dtpg::cnf_end(DtpgStats& stats)
{
  USTime time = timer_stop();
  stats.mCnfGenTime += time;
  ++ stats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
Dtpg::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
Dtpg::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

END_NAMESPACE_YM_SATPG
