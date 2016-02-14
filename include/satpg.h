﻿#ifndef SATPG_H
#define SATPG_H

/// @file satpg.h
/// @brief SATPG 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2013 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSatpg)

#define END_NAMESPACE_YM_SATPG \
END_NAMESPACE(nsSatpg) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief タイマーのモード
//////////////////////////////////////////////////////////////////////
enum {
  /// @brief その他
  TM_MISC = 0,
  /// @brief ファイル読み込み
  TM_READ = 1,
  /// @brief DTPG
  TM_DTPG = 2,
  /// @brief 故障シミュレーション
  TM_FSIM = 3,
  /// @brief SAT
  TM_SAT  = 4,
  /// @brief タイマーのモード数
  TM_SIZE = 5
};


//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class AtpgMgr;
class TpgFault;
class FaultMgr;
class TpgNetwork;
class TpgNode;
class NodeSet;
class NodeValList;
class BackTracer;
class DetectOp;
class UntestOp;
class TestVector;
class TvMgr;
class Dtpg;
class DtpgEngine;
class DtpgStats;
class SatEngine;
class GvalCnf;
class FvalCnf;
class MvalCnf;
class ValMap;
class VidMap;
class LitMap;
class Rtpg;
class RtpgStats;
class MinPat;
class MinPatStats;
class Fsim;
class FsimOp;

END_NAMESPACE_YM_SATPG

#endif // SATPG_H
