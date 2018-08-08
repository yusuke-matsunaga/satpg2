#ifndef FAULTREDUCER_H
#define FAULTREDUCER_H

/// @file FaultReducer.h
/// @brief FaultReducer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/McMatrix.h"
#include "ym/SatSolverType.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class FaultReducer FaultReducer.h "FaultReducer.h"
/// @brief 支配故障を求めるクラスn
//////////////////////////////////////////////////////////////////////
class FaultReducer
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  FaultReducer(const TpgNetwork& network,
	       FaultType fault_type);

  /// @brief デストラクタ
  ~FaultReducer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif 内部で用いる SAT ソルバのタイプの設定を行う．
  /// @param[in] solver_type SATソルバのタイプ
  void
  set_solver_type(const SatSolverType& solver_type);

  /// @brief デバッグフラグをセットする．
  /// @param[in] debug 設定する値 (true/false)
  void
  set_debug(bool debug);

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  /// @param[inout] fault_list 対象の故障リスト
  /// @param[in] algorithm アルゴリズム
  void
  fault_reduction(vector<const TpgFault*>& fault_list,
		  const string& algorithm);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部のデータ構造を初期化する．
  /// @param[in] fi_list 故障情報のリスト
  ///
  /// * この時点で FFR 内でわかる支配故障の縮約は行う．
  /// * テストパタンの生成を行い，被覆行列を作る．
  void
  init(const vector<const TpgFault*>& fault_list);

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction1();

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction2();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // SATソルバのタイプ
  SatSolverType mSolverType;

  // デバッグフラグ
  bool mDebug;

  // 故障リスト
  vector<const TpgFault*> mFaultList;

  // 削除マーク
  // 故障のID番号をキーにする．
  vector<bool> mDelMark;

  // 故障番号をキーにして mFaultList 上の位置を格納する配列
  vector<int> mRowIdMap;

  // 被覆行列
  McMatrix mMatrix;

  // 計時を行うオブジェクト
  StopWatch mTimer;

};

END_NAMESPACE_YM_SATPG

#endif // FAULTREDUCER_H
