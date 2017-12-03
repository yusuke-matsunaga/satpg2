#ifndef GATELITMAP_VECT_H
#define GATELITMAP_VECT_H

/// @file VectLitMap.h
/// @brief VectLitMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class GateLitMap_vect LitMap.h "LitMap.h"
/// @brief すべての変数を指定した LitMap
//////////////////////////////////////////////////////////////////////
class GateLitMap_vect :
  public GateLitMap
{
public:

  /// @brief コンストラクタ
  /// @param[in] ivars 入力の変数のベクタ
  /// @param[in] ovar 出力の変数
  GateLitMap_vect(const vector<SatVarId>& ivars,
		  SatVarId ovar);

  /// @brief デストラクタ
  virtual
  ~GateLitMap_vect();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  virtual
  ymuint
  input_size() const;

  /// @brief 入力のリテラルを返す．
  virtual
  SatLiteral
  input(ymuint pos) const;

  /// @brief 出力のリテラルを返す．
  virtual
  SatLiteral
  output() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力の変数
  vector<SatVarId> mIvars;

  // 出力の変数
  SatVarId mOvar;

};

END_NAMESPACE_YM_SATPG

#endif // GATELITMAP_VECT_H
