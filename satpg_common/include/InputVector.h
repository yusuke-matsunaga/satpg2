#ifndef INPUTVECTOR_H
#define INPUTVECTOR_H

/// @file InputVector.h
/// @brief InputVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BitVector.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class InputVector InputVector.h "InputVector.h"
/// @brief 入力用のビットベクタ
///
/// 実は BitVector の別名だが他のクラスと区別するために typedef は使わない．
//////////////////////////////////////////////////////////////////////
class InputVector :
  public BitVector
{
  friend class TvMgr;

private:

  /// @brief コンストラクタ
  /// @param[in] vect_len ベクタ長
  explicit
  InputVector(ymuint vect_len);

  /// @brief デストラクタ
  ~InputVector();

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  ///
  /// この関数は実装しない．
  InputVector(const InputVector& src);

  /// @brief 代入演算子
  /// @param[in] src コピー元のソース
  ///
  /// この関数は実装しない．
  const InputVector&
  operator=(const InputVector& src);


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
InputVector::InputVector(ymuint vect_len) :
  BitVector(vect_len)
{
}

// @brief デストラクタ
inline
InputVector::~InputVector()
{
}

END_NAMESPACE_YM_SATPG

#endif // INPUTVECTOR_H
