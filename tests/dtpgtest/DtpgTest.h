#ifndef DTPGTEST_H
#define DTPGTEST_H

/// @file DtpgTest.h
/// @brief DtpgTest のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "TpgNetwork.h"
#include "TpgFaultMgr.h"
#include "Fsim.h"
#include "Justifier.h"
#include "DopList.h"
#include "DopVerifyResult.h"
#include "DtpgStats.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgTest DtpgTest.h "DtpgTest.h"
/// @brief Dtpg のテスト用クラス
//////////////////////////////////////////////////////////////////////
class DtpgTest
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATタイプ
  /// @param[in] sat_option SATオプション
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  DtpgTest(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   int bt_mode,
	   const TpgNetwork& network);

  /// @brief デストラクタ
  ~DtpgTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief シングルモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  single_test();

  /// @brief FFRモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  ffr_test();

  /// @brief MFFCモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  mffc_test();

  /// @brief シングルモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  single_new_test();

  /// @brief FFRモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  ffr_new_test();

  /// @brief MFFCモードのテストを行う．
  /// @return 検出故障数と冗長故障数を返す．
  pair<int, int>
  mffc_new_test();

  /// @brief 検証結果を得る．
  const DopVerifyResult&
  verify_result() const;

  /// @brief 統計情報を出力する．
  void
  print_stats(int detect_num,
	      int untest_num);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT ソルバの種類
  string mSatType;

  // SAT ソルバのオプション
  string mSatOption;

  // SAT ソルバのログ出力
  ostream* mSatOutP;

  // 故障の種類
  FaultType mFaultType;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障マネージャ
  TpgFaultMgr mFaultMgr;

  // 故障シミュレータ
  Fsim* mFsim;

  // バックトレーサー
  Justifier* mJustifier;

  // 故障検出時に起動されるファンクタのリスト
  DopList mDop;

  // 検証結果
  DopVerifyResult mVerifyResult;

  // 統計情報
  DtpgStats mStats;

  // タイマー
  StopWatch mTimer;

};

END_NAMESPACE_YM_SATPG

#endif // DTPGTEST_H
