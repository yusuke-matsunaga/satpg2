#ifndef DTPGRESULT_H
#define DTPGRESULT_H

/// @file DtpgResult.h
/// @brief DtpgResult のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/SatBool3.h"
#include "TestVector.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgResult DtpgResult.h "DtpgResult.h"
/// @brief DTPG の結果を表すクラス
///
/// 具体的には
/// - パタン生成の結果(検出，冗長，アボート)
/// - テストパタン
/// を持つ．
/// もちろん，テストパタンは検出できた時のみ意味を持つ．
//////////////////////////////////////////////////////////////////////
class DtpgResult
{
public:

  /// @brief 内容を指定したコンストラクタ
  /// @param[in] stat 結果 ( SatBool3::X か SatBool3::False )
  ///
  /// stat が SatBool3 の時はエラーとなる．
  explicit
  DtpgResult(SatBool3 stat = SatBool3::X);


  /// @brief テストベクタを指定したコンストラクタ
  /// @param[in] testvect テストベクタ
  explicit
  DtpgResult(const TestVector& testvect);


  /// @brief テストベクタの右辺参照を指定したコンストラクタ
  /// @param[in] testvect テストベクタ
  explicit
  DtpgResult(TestVector&& testvect);

  /// @brief コピーコンストラクタ
  DtpgResult(const DtpgResult& src) = default;

  /// @brief ムーブコンストラクタ
  DtpgResult(DtpgResult&& src) = default;

  /// @brief コピー代入演算子
  DtpgResult&
  operator=(const DtpgResult& src) = default;

  /// @brief ムーブ代入演算子
  DtpgResult&
  operator=(DtpgResult& src) = default;

  /// @brief デストラクタ
  ~DtpgResult();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 結果を返す．
  SatBool3
  stat() const;

  /// @brief テストベクタを返す．
  const TestVector&
  testvector() const;

  /// @brief テストベクタの右辺参照を返す．
  TestVector&&
  testvector_rvalue();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 結果
  SatBool3 mStat;

  // テストベクタ
  TestVector mTestVector;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 内容を指定したコンストラクタ
// @param[in] stat 結果 ( SatBool3::_X か SatBool3::_False )
//
// stat が SatBool3 の時はエラーとなる．
inline
DtpgResult::DtpgResult(SatBool3 stat) :
  mStat(stat)
{
  ASSERT_COND( stat != SatBool3::True );
}

// @brief テストベクタを指定したコンストラクタ
// @param[in] testvect テストベクタ
inline
DtpgResult::DtpgResult(const TestVector& testvect) :
  mStat(SatBool3::True),
  mTestVector(testvect)
{
}

// @brief テストベクタの右辺参照を指定したコンストラクタ
// @param[in] testvect テストベクタ
inline
DtpgResult::DtpgResult(TestVector&& testvect) :
  mStat(SatBool3::True),
  mTestVector(testvect)
{
}

// @brief デストラクタ
inline
DtpgResult::~DtpgResult()
{
}

// @brief 結果を返す．
inline
SatBool3
DtpgResult::stat() const
{
  return mStat;
}

// @brief テストベクタを返す．
inline
const TestVector&
DtpgResult::testvector() const
{
  return mTestVector;
}

// @brief テストベクタの右辺参照を返す．
inline
TestVector&&
DtpgResult::testvector_rvalue()
{
  return std::move(mTestVector);
}

END_NAMESPACE_YM_SATPG

#endif // DTPGRESULT_H
