#ifndef BITVECTOR_H
#define BITVECTOR_H

/// @file BitVector.h
/// @brief BitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "Val3.h"
#include "PackedVal.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class BitVector BitVector.h "BitVector.h"
/// @brief ビットベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
///
/// mPat[i * 2 + 0], mPat[i * 2 + 1] の２つのワードでそれぞれ0のビット
/// と1のビットを表す．
/// X の場合は両方のビットに1を立てる．
//////////////////////////////////////////////////////////////////////
class BitVector
{
public:

  /// @brief コンストラクタ
  /// @param[in] vect_len ベクタ長
  explicit
  BitVector(int vect_len);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  BitVector(const BitVector& src);

  /// @brief ムーブコンストラクタ
  /// @param[in] src ムーブ元のソース
  BitVector(BitVector&& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  BitVector&
  operator=(const BitVector& src);

  /// @brief ムーブ代入演算子
  /// @param[in] src ムーブ元のソース
  BitVector&
  operator=(BitVector&& src);

  /// @brief デストラクタ
  ~BitVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長を返す．
  int
  vect_len() const;

  /// @brief 値を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < vect_len() )
  Val3
  val(int pos) const;

  /// @brief X の個数を得る．
  int
  x_count() const;

  /// @brief 2つのベクタが両立しないとき true を返す．
  /// @param[in] bv1, bv2 対象のビットベクタ
  ///
  /// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
  static
  bool
  is_conflict(const BitVector& bv1,
	      const BitVector& bv2);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] right オペランド
  /// @return 自分自身と right が等しいとき true を返す．
  bool
  operator==(const BitVector& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  ///
  /// - false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<(const BitVector& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<=(const BitVector& right) const;

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @brief 値を設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < vect_len() )
  /// @param[in] val 値
  void
  set_val(int pos,
	  Val3 val);

  /// @brief HEX文字列から内容を設定する．
  /// @param[in] hex_string HEX 文字列
  /// @retval true 適切に設定された．
  /// @retval false hex_string に不適切な文字が含まれていた．
  ///
  /// - hex_string が短い時には残りは0で初期化される．
  /// - hex_string が長い時には余りは捨てられる．
  bool
  set_from_hex(const string& hex_string);

  /// @brief 乱数パタンを設定する．
  /// @param[in] randgen 乱数生成器
  ///
  /// - 結果はかならず 0 か 1 になる．(Xは含まれない)
  void
  set_from_random(RandGen& randgen);

  /// @brief X の部分を乱数で 0/1 に設定する．
  /// @param[in] randgen 乱数生成器
  void
  fix_x_from_random(RandGen& randgen);

  /// @brief ビットベクタをコピーする．
  /// @param[in] src コピー元のビットベクタ
  ///
  /// src の X の部分はコピーしない．
  void
  copy(const BitVector& src);

  /// @breif ビットベクタをマージする．
  /// @note X 以外で相異なるビットがあったら false を返す．
  bool
  merge(const BitVector& src);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ブロック数を返す．
  /// @param[in] ni 入力数
  static
  int
  block_num(int ni);

  /// @brief HEX文字列の長さを返す．
  /// @param[in] ni 入力数
  static
  int
  hex_length(int ni);

  // 入力位置からブロック番号を得る．
  /// @param[in] ipos 入力の位置番号
  static
  int
  block_idx(int ipos);

  // 入力位置からシフト量を得る．
  /// @param[in] ipos 入力の位置番号
  static
  int
  shift_num(int ipos);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ベクタ長
  int mVectLen;

  // 最後のブロックのマスク
  PackedVal mMask;

  // ベクタ本体の配列
  PackedVal* mPat;


private:
  //////////////////////////////////////////////////////////////////////
  // このクラスに固有の定数
  //////////////////////////////////////////////////////////////////////

  // 1ワードあたりのHEX文字数
  static
  const int HPW = kPvBitLen / 4;

};

/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const BitVector& left,
	   const BitVector& right)
{
  return !left.operator==(right);
}

/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(const BitVector& left,
	  const BitVector& right)
{
  return right.operator<(left);
}

/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note こちらは等しい場合も含む．
/// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>=(const BitVector& left,
	   const BitVector& right)
{
  return right.operator<=(left);
}


/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] bv ビットベクタ
ostream&
operator<<(ostream& s,
	   const BitVector& bv);

/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] bvp ビットベクタへのポインタ
ostream&
operator<<(ostream& s,
	   const BitVector* bvp);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ムーブコンストラクタ
// @param[in] src ムーブ元のソース
inline
BitVector::BitVector(BitVector&& src) :
  mVectLen(src.mVectLen),
  mMask(src.mMask),
  mPat(src.mPat)
{
  src.mPat = nullptr;
}

// @brief ムーブ代入演算子
// @param[in] src ムーブ元のソース
inline
BitVector&
BitVector::operator=(BitVector&& src)
{
  mVectLen = src.mVectLen;
  mMask = src.mMask;
  mPat = src.mPat;

  src.mPat = nullptr;

  return *this;
}

// @brief デストラクタ
inline
BitVector::~BitVector()
{
  delete [] mPat;
}

// @brief ベクタ長を返す．
inline
int
BitVector::vect_len() const
{
  return mVectLen;
}

// @brief 値を得る．
// @param[in] pos 位置番号 ( 0 <= pos < vect_len() )
inline
Val3
BitVector::val(int pos) const
{
  ASSERT_COND( pos < vect_len() );

  int shift = shift_num(pos);
  int block0 = block_idx(pos);
  int block1 = block0 + 1;
  int v0 = (mPat[block0] >> shift) & 1UL;
  int v1 = (mPat[block1] >> shift) & 1UL;
  int tmp = v0 + v0 + v1;
  return static_cast<Val3>((v0 + v0 + v1) ^ 3);
}

// @breif pos 番めの値を設定する．
inline
void
BitVector::set_val(int pos,
		   Val3 val)
{
  ASSERT_COND( pos < vect_len() );

  int shift = shift_num(pos);
  int block0 = block_idx(pos);
  int block1 = block0 + 1;
  PackedVal mask = 1UL << shift;
  switch ( val ) {
  case Val3::_0:
    mPat[block0] |= mask;
    mPat[block1] &= ~mask;
    break;
  case Val3::_1:
    mPat[block0] &= ~mask;
    mPat[block1] |= mask;
    break;
  case Val3::_X:
    mPat[block0] |= mask;
    mPat[block1] |= mask;
  }
}

// @brief ブロック数を返す．
inline
int
BitVector::block_num(int ni)
{
  return ((ni + kPvBitLen - 1) / kPvBitLen) * 2;
}

// @brief HEX文字列の長さを返す．
inline
int
BitVector::hex_length(int ni)
{
  return (ni + 3) / 4;
}

// 入力位置からブロック番号を得る．
inline
int
BitVector::block_idx(int ipos)
{
  return (ipos / kPvBitLen) * 2;
}

// 入力位置からシフト量を得る．
inline
int
BitVector::shift_num(int ipos)
{
  return (kPvBitLen - 1 - ipos) % kPvBitLen;
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const BitVector& bv)
{
  return s << bv.bin_str();
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const BitVector* bvp)
{
  return s << bvp->bin_str();
}

END_NAMESPACE_YM_SATPG

#endif // BITVECTOR_H
