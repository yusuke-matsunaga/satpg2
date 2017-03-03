#ifndef TD_DTPG_H
#define TD_DTPG_H

/// @file td/Dtpg.h
/// @brief Dtpg のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"
#include "td/DtpgStats.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class Dtpg Dtpg.h "td/Dtpg.h"
/// @brief DTPG の基本エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg
{
public:

  /// @brief デストラクタ
  virtual
  ~Dtpg() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief オプション文字列をセットする．
  virtual
  void
  set_option(const string& option_str) = 0;

  /// @brief 統計情報をクリアする．
  virtual
  void
  clear_stats() = 0;

  /// @brief 統計情報を得る．
  /// @param[in] stats 結果を格納する構造体
  virtual
  void
  get_stats(DtpgStats& stats) const = 0;

  /// @breif 時間計測を制御する．
  virtual
  void
  timer_enable(bool enable) = 0;

  /// @brief テスト生成を行なう．
  /// @param[in] tpgnetwork 対象のネットワーク
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] fault_list 対象の故障リスト
  /// @param[out] stats 結果を格納する構造体
  virtual
  void
  run(TpgNetwork& tgnetwork,
      TpgFaultMgr& fmgr,
      Fsim& fsim,
      const vector<const TpgFault*>& fault_list,
      DtpgStats& stats) = 0;

};


/// @brief Single エンジンを作る．
/// @param[in] sat_type SATソルバの種類を表す文字列
/// @param[in] sat_option SATソルバに渡すオプション文字列
/// @param[in] sat_outp SATソルバ用の出力ストリーム
/// @param[in] bt バックトレーサー
/// @param[in] dop パタンが求められた時に実行されるファンクタ
/// @param[in] uop 検出不能と判定された時に実行されるファンクタ
Dtpg*
new_DtpgSatS(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt,
	     DetectOp& dop,
	     UntestOp& uop);

/// @brief Hierachical エンジンを作る．
/// @param[in] sat_type SATソルバの種類を表す文字列
/// @param[in] sat_option SATソルバに渡すオプション文字列
/// @param[in] sat_outp SATソルバ用の出力ストリーム
/// @param[in] bt バックトレーサー
/// @param[in] dop パタンが求められた時に実行されるファンクタ
/// @param[in] uop 検出不能と判定された時に実行されるファンクタ
Dtpg*
new_DtpgSatH(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt,
	     DetectOp& dop,
	     UntestOp& uop);

END_NAMESPACE_YM_SATPG_TD

#endif // TD_DTPG_H
