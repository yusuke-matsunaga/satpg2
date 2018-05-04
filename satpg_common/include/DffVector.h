#ifndef DFFVECTOR_H
#define DFFVECTOR_H

/// @file DffVector.h
/// @brief DffVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BitVector.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DffVector DffVector.h "DffVector.h"
/// @brief FF用のビットベクタ
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class DffVector :
  public BitVector
{
public:

  /// @brief コンストラクタ
  /// @param[in] vect_len ベクタ長
  explicit
  DffVector(int vect_len);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  DffVector(const DffVector& src);

  /// @brief ムーブコンストラクタ
  /// @param[in] src ムーブ元のソース
  DffVector(DffVector&& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  DffVector&
  operator=(const DffVector& src);

  /// @brief ムーブ代入演算子
  /// @param[in] src ムーブ元のソース
  DffVector&
  operator=(DffVector&& src);

  /// @brief デストラクタ
  ~DffVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] vect_len ベクタ長
inline
DffVector::DffVector(int vect_len) :
  BitVector(vect_len)
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
inline
DffVector::DffVector(const DffVector& src) :
  BitVector(src)
{
}

// @brief ムーブコンストラクタ
// @param[in] src ムーブ元のソース
inline
DffVector::DffVector(DffVector&& src) :
  BitVector(src)
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
inline
DffVector&
DffVector::operator=(const DffVector& src)
{
  BitVector::operator=(src);

  return *this;
}

// @brief ムーブ代入演算子
// @param[in] src ムーブ元のソース
inline
DffVector&
DffVector::operator=(DffVector&& src)
{
  BitVector::operator=(src);

  return *this;
}

// @brief デストラクタ
inline
DffVector::~DffVector()
{
}

END_NAMESPACE_YM_SATPG

#endif // DFFVECTOR_H
