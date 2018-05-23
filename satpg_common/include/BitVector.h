#ifndef BITVECTOR_H
#define BITVECTOR_H

/// @file BitVector.h
/// @brief BitVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BitVectorRep.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class BitVector BitVector.h "BitVector.h"
/// @brief 入力用のビットベクタを表すクラス
///
/// 実体は BitVectorRep が持つ．
//////////////////////////////////////////////////////////////////////
class BitVector
{
public:

  /// @brief コンストラクタ
  /// @param[in] vect_len ベクタ長
  ///
  /// 内容は X で初期化される．
  explicit
  BitVector(int vect_len);

  /// @brief BitVectorRep を引数にとるコンストラクタ
  /// @param[in] rep 本体
  BitVector(BitVectorRep* rep);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  BitVector(const BitVector& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  BitVector&
  operator=(const BitVector& src);

  /// @brief デストラクタ
  ~BitVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
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

  /// @brief マージして代入する．
  BitVector&
  operator&=(const BitVector& right);

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


public:
  //////////////////////////////////////////////////////////////////////
  // friend 関数の定義(publicに意味はない)
  //////////////////////////////////////////////////////////////////////

  /// @brief 両立関係の比較を行う．
  /// @param[in] left, right オペランド
  /// @return left と right が両立する時 true を返す．
  friend
  bool
  operator&&(const BitVector& left,
	     const BitVector& right);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] left, right オペランド
  /// @return left と right が等しいとき true を返す．
  friend
  bool
  operator==(const BitVector& left,
	     const BitVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<(const BitVector& left,
	    const BitVector& right);

  /// @brief 包含関係の比較を行なう
  /// @param[in] left, right オペランド
  /// @return minterm の集合として right が left を含んでいたら true を返す．
  ///
  /// - こちらは等しい場合も含む．
  /// - false だからといって逆に left が right を含むとは限らない．
  friend
  bool
  operator<=(const BitVector& left,
	     const BitVector& right);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 多重参照の場合に複製して単一参照にする．
  ///
  /// 内容を書き換える前に呼ばれる．
  void
  uniquefy();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  std::shared_ptr<BitVectorRep> mPtr;

};


//////////////////////////////////////////////////////////////////////
// BitVector の演算
//////////////////////////////////////////////////////////////////////

/// @relates BitVector
/// @brief 両立関係の比較を行う．
/// @param[in] left, right オペランド
/// @return left と right が両立する時 true を返す．
bool
operator&&(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しいとき true を返す．
bool
operator==(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
bool
operator!=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<(const BitVector& left,
	  const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - false だからといって逆に right が left を含むとは限らない．
bool
operator>(const BitVector& left,
	  const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として right が left を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に left が right を含むとは限らない．
bool
operator<=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief 包含関係の比較を行なう
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
///
/// - こちらは等しい場合も含む．
/// - false だからといって逆に right が left を含むとは限らない．
bool
operator>=(const BitVector& left,
	   const BitVector& right);

/// @relates BitVector
/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
BitVector
operator&(const BitVector& left,
	  const BitVector& right);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] vect_len ベクタ長
inline
BitVector::BitVector(int vect_len) :
  mPtr(new BitVectorRep(vect_len))
{
}

// @brief BitVectorRep を引数にとるコンストラクタ
// @param[in] rep 本体
inline
BitVector::BitVector(BitVectorRep* rep) :
  mPtr(rep)
{
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
inline
BitVector::BitVector(const BitVector& src) :
  mPtr(src.mPtr)
{
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
inline
BitVector&
BitVector::operator=(const BitVector& src)
{
  mPtr = src.mPtr;

  return *this;
}

// @brief デストラクタ
inline
BitVector::~BitVector()
{
}

// @brief ベクタ長を返す．
inline
int
BitVector::vect_len() const
{
  return mPtr->vect_len();
}

// @brief 値を得る．
// @param[in] pos 位置番号 ( 0 <= pos < vect_len() )
inline
Val3
BitVector::val(int pos) const
{
  return mPtr->val(pos);
}

// @brief X の個数を得る．
inline
int
BitVector::x_count() const
{
  return mPtr->x_count();
}

// @brief マージして代入する．
inline
BitVector&
BitVector::operator&=(const BitVector& right)
{
  uniquefy();

  mPtr->merge(*right.mPtr);

  return *this;
}

// @brief 両立関係の比較を行う．
// @param[in] left, right オペランド
// @return left と right が両立する時 true を返す．
inline
bool
operator&&(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_compat(*left.mPtr, *right.mPtr);
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しいとき true を返す．
inline
bool
operator==(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_eq(*left.mPtr, *right.mPtr);
}

// @brief 等価関係の比較を行なう．
// @param[in] left, right オペランド
// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const BitVector& left,
	   const BitVector& right)
{
  return !operator==(left, right);
}

// @brief 包含関係の比較を行なう．
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
//
// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator<(const BitVector& left,
	  const BitVector& right)
{
  return BitVectorRep::is_lt(*left.mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator>(const BitVector& left,
	  const BitVector& right)
{
  return operator<(right, left);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として left が right を含んでいたら true を返す．
//
// - こちらは等しい場合も含む．
// - false だからといって逆に right が left を含むとは限らない．
inline
bool
operator<=(const BitVector& left,
	   const BitVector& right)
{
  return BitVectorRep::is_le(*left.mPtr, *right.mPtr);
}

// @brief 包含関係の比較を行なう
// @param[in] left, right オペランド
// @return minterm の集合として right が left を含んでいたら true を返す．
//
// - こちらは等しい場合も含む．
// - false だからといって逆に left が right を含むとは限らない．
inline
bool
operator>=(const BitVector& left,
	   const BitVector& right)
{
  return operator<=(right, left);
}

// @relates BitVector
// @brief マージする．
// @param[in] left, right オペランド
// @return マージ結果を返す．
//
// left と right がコンフリクトしている時の結果は不定
inline
BitVector
operator&(const BitVector& left,
	  const BitVector& right)
{
  return BitVector(left).operator&=(right);
}

// @brief 内容を BIN 形式で表す．
inline
string
BitVector::bin_str() const
{
  return mPtr->bin_str();
}

// @brief 内容を HEX 形式で表す．
// @note X を含む場合の出力は不定
inline
string
BitVector::hex_str() const
{
  return mPtr->hex_str();
}

// @brief すべて未定(X) で初期化する．
inline
void
BitVector::init()
{
  uniquefy();

  mPtr->init();
}

// @brief 値を設定する．
// @param[in] pos 位置番号 ( 0 <= pos < vect_len() )
// @param[in] val 値
inline
void
BitVector::set_val(int pos,
		   Val3 val)
{
  uniquefy();

  mPtr->set_val(pos, val);
}

// @brief HEX文字列から内容を設定する．
// @param[in] hex_string HEX 文字列
// @retval true 適切に設定された．
// @retval false hex_string に不適切な文字が含まれていた．
//
// - hex_string が短い時には残りは0で初期化される．
// - hex_string が長い時には余りは捨てられる．
inline
bool
BitVector::set_from_hex(const string& hex_string)
{
  uniquefy();

  return mPtr->set_from_hex(hex_string);
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
//
// - 結果はかならず 0 か 1 になる．(Xは含まれない)
inline
void
BitVector::set_from_random(RandGen& randgen)
{
  uniquefy();

  mPtr->set_from_random(randgen);
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
inline
void
BitVector::fix_x_from_random(RandGen& randgen)
{
  uniquefy();

  mPtr->fix_x_from_random(randgen);
}

// @brief 多重参照の場合に複製して単一参照にする．
//
// 内容を書き換える前に呼ばれる．
inline
void
BitVector::uniquefy()
{
  if ( !mPtr.unique() ) {
    // 内容を変更するので複製する．
    mPtr = std::shared_ptr<BitVectorRep>(new BitVectorRep(*mPtr));
  }
}

END_NAMESPACE_YM_SATPG

#endif // BITVECTOR_H
