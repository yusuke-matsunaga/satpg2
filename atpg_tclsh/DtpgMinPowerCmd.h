#ifndef DTPGMINPOWERCMD_H
#define DTPGMINPOWERCMD_H

/// @file DtpgMinPowerCmd.h
/// @brief DtpgMinPowerCmd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgCmd.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// テストパタン生成を行うコマンド
//////////////////////////////////////////////////////////////////////
class DtpgMinPowerCmd :
  public AtpgCmd
{
public:

  /// @brief コンストラクタ
  DtpgMinPowerCmd(AtpgMgr* mgr);

  /// @brief デストラクタ
  virtual
  ~DtpgMinPowerCmd();


protected:

  /// @brief コマンド処理関数
  virtual
  int
  cmd_proc(TclObjVector& objv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // verbose オプションの解析用オブジェクト
  TclPoptInt* mPoptVerbose;

  // sat モードオプションの解析用オブジェクト
  TclPoptStr* mPoptSat;

  // sat-option オプションの解析用オブジェクト
  TclPoptStr* mPoptSatOption;

  // satrec モードオプションの解析用オブジェクト
  TclPopt* mPoptSatRec;

  // minisat モードオプションの解析用オブジェクト
  TclPopt* mPoptMiniSat;

  // minisat2 モードオプションの解析用オブジェクト
  TclPopt* mPoptMiniSat2;

  // ymsat1 モードオプションの解析用オブジェクト
  TclPopt* mPoptYmSat1;

  // print_stats オプションの解析用オブジェクト
  TclPopt* mPoptPrintStats;

  // single0 オプションの解析用オブジェクト
  TclPopt* mPoptSingle0;

  // single オプションの解析用オブジェクト
  TclPopt* mPoptSingle;

  // mffc オプションの解析用オブジェクト
  TclPopt* mPoptMFFC;

  // X抽出オプションの解析用オブジェクト
  TclPoptInt* mPoptX;

  // ランダムサンプリングオプションの解析用オブジェクト
  TclPopt* mPoptR;

  // drop オプションの解析用オブジェクト
  TclPopt* mPoptDrop;

  // k_det オプションの解析用オブジェクト
  TclPoptInt* mPoptKDet;

  // no_pat オプションの解析用オブジェクト
  TclPopt* mPoptNoPat;

  // オプション文字列オプションの解析用オブジェクト
  TclPoptStr* mPoptOpt;

  // verify オプションの解析用オブジェクト
  TclPopt* mPoptVerify;

  // timer オプションの解析用オブジェクト
  TclPopt* mPoptTimer;

  // no-timer オプションの解析用オブジェクト
  TclPopt* mPoptNoTimer;

};

END_NAMESPACE_YM_SATPG

#endif // DTPGMINPOWERCMD_H
