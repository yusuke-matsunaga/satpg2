#ifndef DTPG_SE_H
#define DTPG_SE_H

/// @file Dtpg_se.h
/// @brief Dtpg_se のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "FaultType.h"
#include "DtpgStats.h"
#include "FaultStatus.h"
#include "StructEnc.h"
#include "ym/SatBool3.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Dtpg_se Dtpg_se.h "Dtpg_se.h"
/// @brief StructEnc を用いた DTPG エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg_se
{
public:

  /// @brief コンストラクタ(ffrモード)
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] jt 正当化を行うファンクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] ffr 故障伝搬の起点となる FFR
  /// @param[out] stats DTPGの統計情報
  Dtpg_se(const string& sat_type,
	  const string& sat_option,
	  ostream* sat_outp,
	  FaultType fault_type,
	  Justifier& jt,
	  const TpgNetwork& network,
	  const TpgFFR& ffr,
	  DtpgStats& stats);

  /// @brief コンストラクタ(mffcモード)
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] jt 正当化を行うファンクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] mffc 故障伝搬の起点となる MFFC
  /// @param[out] stats DTPGの統計情報
  ///
  /// この MFFC に含まれるすべての FFR が対象となる．
  /// FFR と MFFC が一致している場合は ffr モードと同じことになる．
  Dtpg_se(const string& sat_type,
	  const string& sat_option,
	  ostream* sat_outp,
	  FaultType fault_type,
	  Justifier& jt,
	  const TpgNetwork& network,
	  const TpgMFFC& mffc,
	  DtpgStats& stats);

  /// @brief デストラクタ
  ~Dtpg_se();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  SatBool3
  dtpg(const TpgFault* fault,
       NodeValList& nodeval_list,
       DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF 作成を開始する．
  void
  cnf_begin();

  /// @brief CNF 作成を終了する．
  void
  cnf_end(DtpgStats& stats);

  /// @brief 時間計測を開始する．
  void
  timer_start();

  /// @brief 時間計測を終了する．
  USTime
  timer_stop();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // StructEnc の本体
  StructEnc mStructEnc;

  // 故障の種類
  FaultType mFaultType;

  // バックトレーサー
  Justifier& mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  StopWatch mTimer;

};

END_NAMESPACE_YM_SATPG

#endif // DTPG_SE_H
