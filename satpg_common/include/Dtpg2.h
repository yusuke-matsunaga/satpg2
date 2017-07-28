#ifndef DTPG2_H
#define DTPG2_H

/// @file Dtpg2.h
/// @brief Dtpg2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "DtpgStats.h"
#include "FaultStatus.h"
#include "ym/SatBool3.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

class Dtpg2Impl;

//////////////////////////////////////////////////////////////////////
/// @class Dtpg2 Dtpg2.h "Dtpg2.h"
/// @brief 遷移回数を考慮した DTPG の基本エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg2
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  Dtpg2(const string& sat_type,
	const string& sat_option,
	ostream* sat_outp,
	BackTracer& bt);

  /// @brief デストラクタ
  ~Dtpg2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim 故障シミュレーター
  /// @param[in] network 対象のネットワーク
  /// @param[in] dop 故障検出時に実行されるファンクター
  /// @param[in] uop 故障が冗長故障の時に実行されるファンクター
  /// @param[inout] stats DTPGの統計情報
  void
  run(TvMgr& tvmgr,
      TpgFaultMgr& fmgr,
      Fsim& fsim,
      const TpgNetwork& network,
      bool use_xorsampling,
      DetectOp& dop,
      UntestOp& uop,
      DtpgStats& stats);

  /// @brief テスト生成を行なう．
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] fsim 故障シミュレーター
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  ///
  /// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
  /// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
  SatBool3
  dtpg(TvMgr& tvmgr,
       Fsim& fsim,
       const TpgNetwork& network,
       const TpgFault* fault,
       bool use_xorsampling,
       ymuint wsa_limit,
       NodeValList& nodeval_list,
       DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATタイプ
  string mSatType;

  // SATオプション
  string mSatOption;

  // SATのログ出力
  ostream* mSatOutP;

  // バックトレーサー
  BackTracer& mBackTracer;

  // 実装クラス
  Dtpg2Impl* mImpl;

  // 乱数発生器
  RandGen mRandGen;

};

END_NAMESPACE_YM_SATPG

#endif // DTPG2_H
