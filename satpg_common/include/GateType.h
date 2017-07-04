#ifndef GATETYPE_H
#define GATETYPE_H

/// @file GateType.h
/// @brief GateType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


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

END_NAMESPACE_YM_SATPG

#endif // GATETYPE_H
