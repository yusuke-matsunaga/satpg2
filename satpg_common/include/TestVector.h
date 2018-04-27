#ifndef TESTVECTOR_H
#define TESTVECTOR_H

/// @file TestVector.h
/// @brief TestVector のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "FaultType.h"
#include "InputVector.h"
#include "DffVector.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TestVector TestVector.h "TestVector.h"
/// @brief テストベクタを表すクラス
///
/// 基本的には3値(0, 1, X)のベクタを表している．
///
/// スキャン方式の縮退故障用ベクタと
/// ブロードサイド方式の遷移故障用ベクタの共用となる．
/// 縮退故障用の時は
/// - 外部入力(InputVector)
/// - DFF(DffVector)
/// の値を持つ．
///
/// 遷移故障用の時は
/// - 外部入力[1時刻目](InputVector)
/// - DFF[1時刻目](DffVector)
/// - 外部入力[2時刻目](InputVector)
/// の値を持つ．
///
/// @sa InputVector, DffVector
//////////////////////////////////////////////////////////////////////
class TestVector
{
public:

  /// @brief 空のコンストラクタ
  TestVector();

  /// @brief コンストラクタ
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  TestVector(int input_num,
	     int dff_num,
	     FaultType fault_type);

  /// @brief コピーコンストラクタ
  /// @param[in] src コピー元のソース
  TestVector(const TestVector& src);

  /// @brief コピー代入演算子
  /// @param[in] src コピー元のソース
  TestVector&
  operator=(const TestVector& src);

  /// @brief ムーブコンストラクタ
  /// @param[in] src ムーブ元のソース
  TestVector(TestVector&& src);

  /// @brief ムーブ代入演算子
  /// @param[in] src ムーブ元のソース
  TestVector&
  operator=(TestVector&& src);

  /// @brief デストラクタ
  ~TestVector();


public:
  //////////////////////////////////////////////////////////////////////
  // 値を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を得る．
  int
  input_num() const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief ２時刻目の外部入力を持つ時 true を返す．
  bool
  has_aux_input() const;

  /// @brief PPI数を得る．
  ///
  /// = input_num() + dff_num()
  int
  ppi_num() const;

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief ベクタ長を返す．
  ///
  /// - fault_type() == FaultType::StuckAt の時は input_num() + dff_num()
  /// - fault_type() == FaultType::TransitionDelay の時は input_num() * 2 + dff_num()
  int
  vect_len() const;

  /// @brief PPIの値を得る．
  /// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
  Val3
  ppi_val(int pos) const;

  /// @brief 1時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos) と同じ．
  Val3
  input_val(int pos) const;

  /// @brief 1時刻目のDFFの値を得る．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  ///
  /// is_td_mode() == true の時のみ有効<br>
  /// 実は ppi_val(pos + input_num()) と同じ．
  Val3
  dff_val(int pos) const;

  /// @brief 2時刻目の外部入力の値を得る．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  ///
  /// is_td_mode() == true の時のみ有効
  Val3
  aux_input_val(int pos) const;

  /// @brief X の個数を得る．
  int
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

  /// @brief 内容を BIN 形式で表す．
  string
  bin_str() const;

  /// @brief 内容を HEX 形式で表す．
  /// @note X を含む場合の出力は不定
  string
  hex_str() const;


public:
  //////////////////////////////////////////////////////////////////////
  // TestVector の演算
  //////////////////////////////////////////////////////////////////////

  /// @brief 2つのベクタが両立しないとき true を返す．
  /// @param[in] tv1, tv2 対象のテストベクタ
  ///
  /// 同じビット位置にそれぞれ 0 と 1 を持つ場合が両立しない場合．
  static
  bool
  is_conflict(const TestVector& tv1,
	      const TestVector& tv2);

  /// @brief マージして代入する．
  TestVector&
  operator&=(const TestVector& right);

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



public:
  //////////////////////////////////////////////////////////////////////
  // 値を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを(再)設定する．
  /// @param[in] input_num 入力数
  /// @param[in] dff_numr DFF数
  /// @param[in] fault_type 故障の種類
  void
  resize(int input_num,
	 int dff_num,
	 FaultType fault_type);

  /// @brief すべて未定(X) で初期化する．
  void
  init();

  /// @brief PPIの値を設定する．
  /// @param[in] pos PPIの位置番号 ( 0 <= pos < ppi_num() )
  /// @param[in] val 値
  ///
  /// is_sa_mode() == true の時のみ有効
  void
  set_ppi_val(int pos,
	      Val3 val);

  /// @breif 1時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_input_val(int pos,
		Val3 val);

  /// @breif 1時刻目のDFFの値を設定する．
  /// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ有効
  void
  set_dff_val(int pos,
	      Val3 val);

  /// @breif 2時刻目の外部入力の値を設定する．
  /// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
  /// @param[in] val 値
  ///
  /// is_td_mode() == true の時のみ意味を持つ．
  void
  set_aux_input_val(int pos,
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

  /// @breif テストベクタをマージする．
  /// @note X 以外で相異なるビットがあったら false を返す．
  bool
  merge(const TestVector& src);


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタをコピーする．
  /// @param[in] src コピー元のテストベクタ
  /// @note X の部分はコピーしない．
  void
  _copy(const TestVector& src);

  /// @brief InputVector を作る．
  /// @param[in] input_num 入力数
  static
  std::unique_ptr<InputVector>
  new_input_vector(int input_num);

  /// @brief DffVector を作る．
  /// @param[in] dff_num DFF数
  static
  std::unique_ptr<DffVector>
  new_dff_vector(int dff_num);

  /// @brief InputVector を作る．
  /// @param[in] input_num 入力数
  /// @param[in] fault_type 故障のタイプ
  ///
  /// fault_type == StuckAt の時は nullptr を返す．
  static
  std::unique_ptr<InputVector>
  new_aux_input_vector(int input_num,
		       FaultType fault_type);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力用ベクタ
  std::unique_ptr<InputVector> mInputVector;

  // DFF用ベクタ
  std::unique_ptr<DffVector> mDffVector;

  // ２時刻目の入力用ベクタ
  std::unique_ptr<InputVector> mAuxInputVector;

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

/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
inline
TestVector
operator&(const TestVector& left,
	  const TestVector& right)
{
  return TestVector(left).operator&=(right);
}

/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
inline
TestVector
operator&(TestVector&& left,
	  const TestVector& right)
{
  return left.operator&=(right);
}

/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
inline
TestVector
operator&(const TestVector& left,
	  TestVector&& right)
{
  return right.operator&=(left);
}

/// @brief マージする．
/// @param[in] left, right オペランド
/// @return マージ結果を返す．
///
/// left と right がコンフリクトしている時の結果は不定
inline
TestVector
operator&(TestVector&& left,
	  TestVector&& right)
{
  return left.operator&=(right);
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
int
TestVector::input_num() const
{
  if ( mInputVector != nullptr ) {
    return mInputVector->vect_len();
  }
  else {
    return 0;
  }
}

// @brief DFF数を得る．
inline
int
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
int
TestVector::ppi_num() const
{
  return input_num() + dff_num();
}

// @brief ２時刻目の外部入力を持つ時 true を返す．
inline
bool
TestVector::has_aux_input() const
{
  return mAuxInputVector != nullptr;
}

// @brief 故障の種類を返す．
inline
FaultType
TestVector::fault_type() const
{
  if ( mAuxInputVector != nullptr ) {
    return FaultType::TransitionDelay;
  }
  else {
    return FaultType::StuckAt;
  }
}

// @brief ベクタ長を返す．
inline
int
TestVector::vect_len() const
{
  int ans = input_num() + dff_num();
  if ( mAuxInputVector != nullptr ) {
    ans += mAuxInputVector->vect_len();
  }
  return ans;
}

// @brief PPIの値を得る．
// @param[in] pos PPI の位置番号 ( 0 <= pos < ppi_num() )
inline
Val3
TestVector::ppi_val(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < ppi_num() );

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
TestVector::input_val(int pos) const
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos >= 0 && pos < input_num() );

  return mInputVector->val(pos);
}

// @brief 1時刻目のDFFの値を得る．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
inline
Val3
TestVector::dff_val(int pos) const
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos >= 0 && pos < dff_num() );

  return mDffVector->val(pos);
}

// @brief 2時刻目の外部入力の値を得る．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
inline
Val3
TestVector::aux_input_val(int pos) const
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos >= 0 && pos < input_num() );

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
TestVector::set_ppi_val(int pos,
			Val3 val)
{
  ASSERT_COND( fault_type() == FaultType::StuckAt );
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
TestVector::set_input_val(int pos,
			  Val3 val)
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos < input_num() );

  mInputVector->set_val(pos, val);
}

// @breif 1時刻目のDFFの値を設定する．
// @param[in] pos DFFの位置番号 ( 0 <= pos < dff_num() )
// @param[in] val 値
inline
void
TestVector::set_dff_val(int pos,
			Val3 val)
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos < dff_num() );

  mDffVector->set_val(pos, val);
}

// @breif 2時刻目の外部入力の値を設定する．
// @param[in] pos 入力の位置番号 ( 0 <= pos < input_num() )
// @param[in] val 値
inline
void
TestVector::set_aux_input_val(int pos,
			      Val3 val)
{
  ASSERT_COND( fault_type() == FaultType::TransitionDelay );
  ASSERT_COND( pos < input_num() );

  if ( fault_type() == FaultType::TransitionDelay ) {
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
