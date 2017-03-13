#ifndef SATPG_H
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
/// @brief 論理ゲートの種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum GateType {
  /// @brief 定数0
  kGateCONST0 = 0,

  /// @brief 定数1
  kGateCONST1 = 1,

  /// @brief 入力
  kGateINPUT  = 2,

  /// @brief バッファ
  kGateBUFF   = 3,

  /// @brief NOT
  kGateNOT    = 4,

  /// @brief AND
  kGateAND    = 5,

  /// @brief NAND
  kGateNAND   = 6,

  /// @brief OR
  kGateOR     = 7,

  /// @brief NOR
  kGateNOR    = 8,

  /// @brief XOR
  kGateXOR    = 9,

  /// @brief XNOR
  kGateXNOR   = 10,

  /// @brief 上記以外
  kGateCPLX   = 11
};

/// @brief GateType のストリーム演算子
ostream&
operator<<(ostream& s,
	   GateType gate_type);


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

class TpgNetwork;
class TpgDff;
class TpgGateInfo;
class TpgNode;
class TpgMFFC;
class TpgFFR;

class TpgFault;
class TpgFaultMgr;

class NodeValList;
class TestVector;
class TvMgr;

class Dtpg;
class DetectOp;
class UntestOp;

class BackTracer;

class GateLitMap;
class VidMap;
class ValMap;

class Fsim;

END_NAMESPACE_YM_SATPG

#endif // SATPG_H
