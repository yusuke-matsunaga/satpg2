
/// @file GateLitMap_vect.cc
/// @brief GateLitMap_vect の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap_vect.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス GateLitMap_vect
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateLitMap_vect::GateLitMap_vect(const vector<SatVarId>& ivars,
				 SatVarId ovar) :
  mIvars(ivars),
  mOvar(ovar)
{
}

// @brief デストラクタ
GateLitMap_vect::~GateLitMap_vect()
{
}

// @brief 入力数を返す．
ymuint
GateLitMap_vect::input_size() const
{
  return mIvars.size();
}

// @brief 入力のリテラルを返す．
SatLiteral
GateLitMap_vect::input(ymuint pos) const
{
  return SatLiteral(mIvars[pos], false);
}

// @brief 出力のリテラルを返す．
SatLiteral
GateLitMap_vect::output() const
{
  return SatLiteral(mOvar, false);
}

END_NAMESPACE_YM_SATPG
