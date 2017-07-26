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
#include "FaultType.h"
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

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief ベクタ長を返す．
  ///
  /// - fault_type() == kFtStuckAt の時は input_num() + dff_num()
  /// - fault_type() == kFtTransitionDelay の時は input_num() * 2 + dff_num()
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
  x_count() const;

  /// @brief 入力のベクタを得る．
  const InputVector&
  input_vector() const;

  /// @brief DFFのベクタを得る．
  ///
  /// nullptr の場合もある．
  const DffVector&
  dff_vector() const;

  /// @brief ２時刻目の入力のベクタを得る．
  ///
  /// nullptr の場合もある．
  const InputVector&
  aux_input_vector() const;

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
  // 特殊なアロケーションをしているのでコンストラクタ関係は
  // プライベートにしている．
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] input_vector入力のベクタ
  /// @param[in] dff_vector DFFのベクタ
  /// @param[in] aux_input_vector ２時刻目の入力のベクタ
  TestVector(InputVector* input_vector,
	     DffVector* dff_vector,
	     InputVector* aux_input_vector);

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

  // 入力用ベクタ
  InputVector* mInputVector;

  // DFF用ベクタ
  DffVector* mDffVector;

  // ２時刻目の入力用ベクタ
  InputVector* mAuxInputVector;

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
  return mInputVector->vect_len();
}

// @brief DFF数を得る．
inline
ymuint
TestVector::dff_num() const
{
  if ( mDffVector != nullptr ) {
    return mDffVector->vect_len();
  }
  return 0;
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

// @brief 故障の種類を返す．
inline
FaultType
TestVector::fault_type() const
{
  if ( mAuxInputVector != nullptr ) {
    return kFtTransitionDelay;
  }
  else {
    return kFtStuckAt;
  }
}

// @brief ベクタ長を返す．
inline
ymuint
TestVector::vect_len() const
{
  ymuint ans = input_num() + dff_num();
  if ( mAuxInputVector != nullptr ) {
    ans += mAuxInputVector->vect_len();
  }
  return ans;
}

// @brief PPIの値を得る．
// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
inline
Val3
TestVector::ppi_val(ymuint pos) const
{
  ASSERT_COND( pos < ppi_num() );

  if ( pos < input_num() ) {
    return mInputVector->val(pos);
  }
  else {
    return mDffVector->val(pos - input_num());
  }
}

// @brief 1時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::input_val(ymuint pos) const
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < input_num() );

  return mInputVector->val(pos);
}

// @brief 1時刻目のDFFの値を得る．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
inline
Val3
TestVector::dff_val(ymuint pos) const
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < dff_num() );

  return mDffVector->val(pos);
}

// @brief 2時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::aux_input_val(ymuint pos) const
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < input_num() );

  return mAuxInputVector->val(pos);
}

// @brief 入力のベクタを得る．
inline
const InputVector&
TestVector::input_vector() const
{
  return *mInputVector;
}

// @brief DFFのベクタを得る．
//
// nullptr の場合もある．
inline
const DffVector&
TestVector::dff_vector() const
{
  return *mDffVector;
}

// @brief ２時刻目の入力のベクタを得る．
//
// nullptr の場合もある．
inline
const InputVector&
TestVector::aux_input_vector() const
{
  return *mAuxInputVector;
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
  ASSERT_COND( fault_type() == kFtStuckAt );
  ASSERT_COND( pos < ppi_num() );

  if ( pos < input_num() ) {
    mInputVector->set_val(pos, val);
  }
  else {
    mDffVector->set_val(pos - input_num(), val);
  }
}

// @breif 1時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_input_val(ymuint pos,
			  Val3 val)
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < input_num() );

  mInputVector->set_val(pos, val);
}

// @breif 1時刻目のDFFの値を設定する．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
// @param[in] val 値
inline
void
TestVector::set_dff_val(ymuint pos,
			Val3 val)
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < dff_num() );

  mDffVector->set_val(pos, val);
}

// @breif 2時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_aux_input_val(ymuint pos,
			      Val3 val)
{
  ASSERT_COND( fault_type() == kFtTransitionDelay );
  ASSERT_COND( pos < input_num() );

  if ( fault_type() == kFtTransitionDelay ) {
    mAuxInputVector->set_val(pos, val);
  }
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
