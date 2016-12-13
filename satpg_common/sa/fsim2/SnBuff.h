﻿#ifndef SNBUFF_H
#define SNBUFF_H

/// @file SnBuff.h
/// @brief SnBuff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SnGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnBuff SimBuff.h
/// @brief BUFFノード
//////////////////////////////////////////////////////////////////////
class SnBuff :
  public SnGate1
{
public:

  /// @brief コンストラクタ
  SnBuff(ymuint id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnBuff();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_lobs(ymuint ipos);

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

};


//////////////////////////////////////////////////////////////////////
/// @class SnNot SimNode.h
/// @brief NOTノード
//////////////////////////////////////////////////////////////////////
class SnNot :
  public SnBuff
{
public:

  /// @brief コンストラクタ
  SnNot(ymuint id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNot();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const;

  /// @brief 正常値の計算を行う．
  virtual
  PackedVal
  _calc_gval();

  /// @brief 故障値の計算を行う．
  virtual
  PackedVal
  _calc_fval();

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const;

};

END_NAMESPACE_YM_SATPG_FSIM

#endif // SNBUFF_H
