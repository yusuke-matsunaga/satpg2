﻿#ifndef SPAND_H
#define SPAND_H

/// @file SpAnd.h
/// @brief SpAnd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SpGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SpAnd SpAnd.h
/// @brief ANDノード
//////////////////////////////////////////////////////////////////////
class SpAnd :
  public SpGate
{
public:

  /// @brief コンストラクタ
  SpAnd(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpAnd();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SpAnd2 SpAnd.h
/// @brief 2入力ANDノード
//////////////////////////////////////////////////////////////////////
class SpAnd2 :
  public SpGate2
{
public:

  /// @brief コンストラクタ
  SpAnd2(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpAnd2();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SpAnd3 SpAnd.h
/// @brief 3入力ANDノード
//////////////////////////////////////////////////////////////////////
class SpAnd3 :
  public SpGate3
{
public:

  /// @brief コンストラクタ
  SpAnd3(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpAnd3();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SpAnd4 SpAnd.h
/// @brief 4入力ANDノード
//////////////////////////////////////////////////////////////////////
class SpAnd4 :
  public SpGate4
{
public:

  /// @brief コンストラクタ
  SpAnd4(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpAnd4();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

  /// @brief ゲートの入力から出力までの可観測性を計算する．(2値版)
  virtual
  PackedVal
  _calc_gobs(ymuint ipos);

};


//////////////////////////////////////////////////////////////////////
/// @class SpNand SpAnd.h
/// @brief NANDノード
//////////////////////////////////////////////////////////////////////
class SpNand :
  public SpAnd
{
public:

  /// @brief コンストラクタ
  SpNand(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpNand();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SpNand2 SpAnd.h
/// @brief 2入力NANDノード
//////////////////////////////////////////////////////////////////////
class SpNand2 :
  public SpAnd2
{
public:

  /// @brief コンストラクタ
  SpNand2(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpNand2();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SpNand3 SpAnd.h
/// @brief 3入力NANDノード
//////////////////////////////////////////////////////////////////////
class SpNand3 :
  public SpAnd3
{
public:

  /// @brief コンストラクタ
  SpNand3(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpNand3();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

};


//////////////////////////////////////////////////////////////////////
/// @class SpNand4 SpAnd.h
/// @brief 4入力NANDノード
//////////////////////////////////////////////////////////////////////
class SpNand4 :
  public SpAnd4
{
public:

  /// @brief コンストラクタ
  SpNand4(const vector<SimPrim*>& fanins);

  /// @brief デストラクタ
  virtual
  ~SpNand4();


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  virtual
  PackedVal
  _calc_val();

};

END_NAMESPACE_YM_SATPG_FSIM

#endif // SPAND_H