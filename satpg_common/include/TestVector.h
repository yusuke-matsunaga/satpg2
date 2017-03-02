#ifndef TESTVECTOR_H
#define TESTVECTOR_H

/// @file TestVector.h
/// @brief TestVector のヘッダファイル
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
/// @class TestVector TestVector.h "TestVector.h"
/// @brief テストベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
/// 生成/破壊は TvMgr のみが行う．
/// 同じ TvMgr が扱うテストベクタのサイズはすべて同じ．
///
/// スキャン方式の縮退故障用ベクタと
/// ブロードサイド方式の遷移故障用ベクタの共用となる．
/// 縮退故障用の時は
/// - 外部入力
/// - DFF
/// の値を持つ．
///
/// 遷移故障用の時は
/// - 外部入力(1時刻目)
/// - DFF(1時刻目)
/// - 外部入力(2時刻目)
/// の値を持つ．
///
/// 縮退故障用の時には２時刻目の外部入力を用いない．
//////////////////////////////////////////////////////////////////////
class TestVector
{
  friend class TvMgr;

public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を得る．
  ymuint
  input_num() const;

  /// @brief DFF数を得る．
  ymuint
  dff_num() const;

  /// @brief PPI数を得る．
  ///
  /// = input_num() + dff_num()
  ymuint
  ppi_num() const;

  /// @brief 縮退故障用のベクタの時 true を返す．
  bool
  is_sa_mode() const;

  /// @brief 遷移故障用のベクタの時 true を返す．
  bool
  is_td_mode() const;

  /// @brief ベクタ長を返す．
  ///
  /// - is_sa_mode() == true の時は input_num() + dff_num()
  /// - is_td_mode() == true の時は input_num() * 2 + dff_num()
  ymuint
  vect_len() const;

  /// @brief PPIの値を得る．
  /// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
  Val3
  ppi_val(ymuint pos) const;

  /// @brief 1時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos) と同じ．
  Val3
  input_val(ymuint pos) const;

  /// @brief 1時刻目のDFFの値を得る．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos + input_num()) と同じ．
  Val3
  dff_val(ymuint pos) const;

  /// @brief 2時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効
  Val3
  aux_input_val(ymuint pos) const;

  /// @brief X の個数を得る．
  ymuint
  x_num() const;

  /// @brief 2つのベクタが両立しないとき true を返す．
  /// @param[in] tv1, tv2 対象のテストベクタ
  ///
  /// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
  static
  bool
  is_conflict(const TestVector& tv1,
	      const TestVector& tv2);

  /// @brief 等価関係の比較を行なう．
  /// @param[in] right オペランド
  /// @return 自分自身と right が等しいとき true を返す．
  bool
  operator==(const TestVector& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  /// @note false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<(const TestVector& right) const;

  /// @brief 包含関係の比較を行なう
  /// @param[in] right オペランド
  /// @return minterm の集合として right が自分自身を含んでいたら true を返す．
  /// @note こちらは等しい場合も含む．
  /// @note false だからといって逆に自分自身が right を含むとは限らない．
  bool
  operator<=(const TestVector& right) const;

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

  /// @brief PPIの値を設定する．
  /// @param[in] pos PPIの位置番号 ( 0 <= pos < ppi_num() )
  /// @param[in] val 値
  ///
  /// is_sa_mode() == true の時のみ有効
  void
  set_ppi_val(ymuint pos,
	      Val3 val);

  /// @breif 1時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_input_val(ymuint pos,
		Val3 val);

  /// @breif 1時刻目のDFFの値を設定する．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_dff_val(ymuint pos,
	      Val3 val);

  /// @breif 2時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ意味を持つ．
  void
  set_aux_input_val(ymuint pos,
		    Val3 val);

  /// @brief 割当リストから内容を設定する．
  /// @param[in] assign_list 割当リスト
  ///
  /// assign_list に外部入力とDFF以外の割当が含まれている場合無視する．
  void
  set_from_assign_list(const NodeValList& assign_list);

  /// @brief HEX文字列から内容を設定する．
  /// @param[in] hex_string HEX 文字列
  /// @return hex_string に不適切な文字が含まれていたら false を返す．
  ///
  /// 1時刻目の外部入力，１時刻目のDFF，２時刻目の外部入力の順にならんでいると仮定する．<br>
  /// hex_string が短い時には残りは0で初期化される．<br>
  /// hex_string が長い時には余りは捨てられる．<br>
  bool
  set_from_hex(const string& hex_string);

  /// @brief 乱数パタンを設定する．
  /// @param[in] randgen 乱数生成器
  /// @note 結果はかならず 0 か 1 になる．(Xは含まれない)
  void
  set_from_random(RandGen& randgen);

  /// @brief X の部分を乱数で 0/1 に設定する．
  /// @param[in] randgen 乱数生成器
  void
  fix_x_from_random(RandGen& randgen);

  /// @brief テストベクタをコピーする．
  /// @param[in] src コピー元のテストベクタ
  /// @note X の部分はコピーしない．
  void
  copy(const TestVector& src);

  /// @breif テストベクタをマージする．
  /// @note X 以外で相異なるビットがあったら false を返す．
  bool
  merge(const TestVector& src);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めの値を得る．
  /// @param[in] pos 入力の位置番号
  Val3
  _val(ymuint pos) const;

  /// @breif pos 番めの値を設定する．
  /// @param[in] pos 入力の位置番号
  /// @param[in] val 値
  void
  _set_val(ymuint pos,
	   Val3 val);

  /// @brief ブロック数を返す．
  /// @param[in] ni 入力数
  static
  ymuint
  block_num(ymuint ni);

  /// @brief HEX文字列の長さを返す．
  /// @param[in] ni 入力数
  static
  ymuint
  hex_length(ymuint ni);

  // 入力位置からブロック番号を得る．
  /// @param[in] ipos 入力の位置番号
  static
  ymuint
  block_idx(ymuint ipos);

  // 入力位置からシフト量を得る．
  /// @param[in] ipos 入力の位置番号
  static
  ymuint
  shift_num(ymuint ipos);


private:
  //////////////////////////////////////////////////////////////////////
  // 特殊なアロケーションをしているのでコンストラクタ関係は
  // プライベートにしている．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] input_num 入力数
  /// @param[in] dff_num DFF数
  /// @param[in] td_mode 遷移故障用の時 true にするフラグ
  explicit
  TestVector(ymuint input_num,
	     ymuint dff_num,
	     bool td_mode);

  /// @brief デストラクタ
  ~TestVector();

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  TestVector(const TestVector& src);

  /// @brief 代入演算子
  /// @param[in] src コピー元のソース
  const TestVector&
  operator=(const TestVector& src);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部入力数
  // ただし最下位ビットで縮退故障用か遷移故障用かを区別する．
  // 最下位ビットが0の時縮退故障用
  ymuint mInputNum;

  // DFF数
  ymuint mDffNum;

  // 最後のブロックのマスク
  PackedVal mMask;

  // ベクタ本体(ただしサイズは可変)
  PackedVal mPat[1];


private:
  //////////////////////////////////////////////////////////////////////
  // このクラスに固有の定数
  //////////////////////////////////////////////////////////////////////

  // 1ワードあたりのHEX文字数
  static
  const ymuint HPW = kPvBitLen / 4;

};

/// @brief 等価関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return left と right が等しくないとき true を返す．
inline
bool
operator!=(const TestVector& left,
	   const TestVector& right)
{
  return !left.operator==(right);
}

/// @brief 包含関係の比較を行なう．
/// @param[in] left, right オペランド
/// @return minterm の集合として left が right を含んでいたら true を返す．
/// @note false だからといって逆に right が left を含むとは限らない．
inline
bool
operator>(const TestVector& left,
	  const TestVector& right)
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
operator>=(const TestVector& left,
	   const TestVector& right)
{
  return right.operator<=(left);
}


/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] tv テストベクタ
ostream&
operator<<(ostream& s,
	   const TestVector& tv);

/// @brief 内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] tvp テストベクタへのポインタ
ostream&
operator<<(ostream& s,
	   const TestVector* tvp);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 入力数を得る．
inline
ymuint
TestVector::input_num() const
{
  return mInputNum / 2;
}

// @brief DFF数を得る．
inline
ymuint
TestVector::dff_num() const
{
  return mDffNum;
}

// @brief PPI数を得る．
//
// = input_num() + dff_num()
inline
ymuint
TestVector::ppi_num() const
{
  return input_num() + dff_num();
}

// @brief 縮退故障用のベクタの時 true を返す．
inline
bool
TestVector::is_sa_mode() const
{
  return !is_td_mode();
}

// @brief 遷移故障用のベクタの時 true を返す．
inline
bool
TestVector::is_td_mode() const
{
  return static_cast<bool>(mInputNum & 1U);
}

// @brief ベクタ長を返す．
inline
ymuint
TestVector::vect_len() const
{
  if ( is_sa_mode() ) {
    return input_num() + dff_num();
  }
  else {
    return input_num() * 2 + dff_num();
  }
}

// @brief PPIの値を得る．
// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
inline
Val3
TestVector::ppi_val(ymuint pos) const
{
  ASSERT_COND( pos < ppi_num() );

  return _val(pos);
}

// @brief 1時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::input_val(ymuint pos) const
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < input_num() );

  return _val(pos);
}

// @brief 1時刻目のDFFの値を得る．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
inline
Val3
TestVector::dff_val(ymuint pos) const
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < dff_num() );

  return _val(pos + input_num());
}

// @brief 2時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::aux_input_val(ymuint pos) const
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < input_num() );

  return _val(pos + input_num() + dff_num());
}

// @brief PPIの値を設定する．
// @param[in] pos PPIの位置番号 ( 0 <= pos < ppi_num() )
// @param[in] val 値
//
// is_sa_mode() == true の時のみ有効
inline
void
TestVector::set_ppi_val(ymuint pos,
			Val3 val)
{
  ASSERT_COND( is_sa_mode() );
  ASSERT_COND( pos < ppi_num() );

  _set_val(pos, val);
}

// @breif 1時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_input_val(ymuint pos,
			  Val3 val)
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < input_num() );

  _set_val(pos, val);
}

// @breif 1時刻目のDFFの値を設定する．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
// @param[in] val 値
inline
void
TestVector::set_dff_val(ymuint pos,
			Val3 val)
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < dff_num() );

  _set_val(pos + input_num(), val);
}

// @breif 2時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_aux_input_val(ymuint pos,
			      Val3 val)
{
  ASSERT_COND( is_td_mode() );
  ASSERT_COND( pos < input_num() );

  if ( is_td_mode() ) {
    _set_val(pos + input_num() + dff_num(), val);
  }
}

// @brief pos 番めの値を得る．
inline
Val3
TestVector::_val(ymuint pos) const
{
  ymuint shift = shift_num(pos);
  ymuint block0 = block_idx(pos);
  ymuint block1 = block0 + 1;
  int v0 = (mPat[block0] >> shift) & 1UL;
  int v1 = (mPat[block1] >> shift) & 1UL;
  return static_cast<Val3>(v1 + v1 + v0);
}

// @breif pos 番めの値を設定する．
inline
void
TestVector::_set_val(ymuint pos,
		     Val3 val)
{
  ymuint shift = shift_num(pos);
  ymuint block0 = block_idx(pos);
  ymuint block1 = block0 + 1;
  PackedVal mask = 1UL << shift;
  if ( val == kVal0 ) {
    mPat[block0] |= mask;
    mPat[block1] &= ~mask;
  }
  else if ( val == kVal1 ) {
    mPat[block0] &= ~mask;
    mPat[block1] |= mask;
  }
  else { // val == kValX
    mPat[block0] &= ~mask;
    mPat[block1] &= ~mask;
  }
}

// @brief ブロック数を返す．
inline
ymuint
TestVector::block_num(ymuint ni)
{
  return ((ni + kPvBitLen - 1) / kPvBitLen) * 2;
}

// @brief HEX文字列の長さを返す．
inline
ymuint
TestVector::hex_length(ymuint ni)
{
  return (ni + 3) / 4;
}

// 入力位置からブロック番号を得る．
inline
ymuint
TestVector::block_idx(ymuint ipos)
{
  return (ipos / kPvBitLen) * 2;
}

// 入力位置からシフト量を得る．
inline
ymuint
TestVector::shift_num(ymuint ipos)
{
  return (kPvBitLen - 1 - ipos) % kPvBitLen;
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const TestVector& tv)
{
  return s << tv.bin_str();
}

// @brief 内容を出力する．
inline
ostream&
operator<<(ostream& s,
	   const TestVector* tvp)
{
  return s << tvp->bin_str();
}

END_NAMESPACE_YM_SATPG

#endif // TESTVECTOR_H
