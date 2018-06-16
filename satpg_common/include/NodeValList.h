#ifndef NODEVALLIST_H
#define NODEVALLIST_H

/// @file NodeValList.h
/// @brief NodeValList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class NodeVal NodeValList.h "td/NodeValList.h"
/// @brief ノードに対する値の割当を表すクラス
///
/// 昔の C でよく使われていたポインタの下位ビットが0であることを
/// 利用して，そこにフラグを埋め込むテクニック
/// C++ の時代では醜いことこのうえない．
///
/// なお，縮退故障モードのときは時刻は 1 となる．
//////////////////////////////////////////////////////////////////////
class NodeVal
{
public:

  /// @brief 空のコンストラクタ
  ///
  /// 内容は不定
  NodeVal();

  /// @brief 値を指定したコンストラクタ
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[in] val 値
  NodeVal(const TpgNode* node,
	  int time,
	  bool val);


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを返す．
  const TpgNode*
  node() const;

  /// @brief 時刻を返す．
  int
  time() const;

  /// @brief 値を返す．
  bool
  val() const;

  /// @brief 大小関係の比較関数
  friend
  bool
  operator<(const NodeVal& left,
	    const NodeVal& right);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パックした値
  ympuint mPackVal;

};


//////////////////////////////////////////////////////////////////////
/// @class NodeValList NodeValList.h "td/NodeValList.h"
/// @brief ノードに対する値の割当を記録するクラス
//////////////////////////////////////////////////////////////////////
class NodeValList
{
public:

  /// @brief コンストラクタ
  ///
  /// 空のリストが生成される．
  NodeValList();

  /// @brief コピーコンストラクタ
  NodeValList(const NodeValList& src);

  /// @brief ムーブコンストラクタ
  NodeValList(NodeValList&& src);

  /// @brief コピー代入演算子
  NodeValList&
  operator=(const NodeValList& src);

  /// @brief ムーブ代入演算子
  NodeValList&
  operator=(NodeValList&& src);

  /// @brief デストラクタ
  ~NodeValList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  void
  clear();

  /// @brief 値を追加する．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[in] val 値
  void
  add(const TpgNode* node,
      int time,
      bool val);

  /// @brief ソートする．
  void
  sort();

  /// @brief マージする．
  /// @param[in] src_list マージするリスト
  ///
  /// 矛盾する割当があった場合の動作は不定
  void
  merge(const NodeValList& src_list);

  /// @brief 差分を計算する．
  /// @param[in] src_list 差分の対象のリスト
  void
  diff(const NodeValList& src_list);

  /// @brief 要素数を返す．
  int
  size() const;

  /// @brief 要素を返す．
  /// @param[in] pos 位置 ( 0 <= pos < size() )
  NodeVal
  operator[](int pos) const;

  /// @brief 要素を返す．
  /// @param[in] pos 位置 ( 0 <= pos < size() )
  ///
  /// operator[] の別名
  NodeVal
  elem(int pos) const;

  /// @brief 矛盾した内容になっていないかチェックする．
  /// @return 正しければ true を返す．
  ///
  /// 具体的には同じノードで異なる値がある場合にエラーとなる．
  /// この関数はソートされている前提で動作する．
  bool
  sanity_check() const;

  /// @brief 先頭の反復子を返す．
  vector<NodeVal>::const_iterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  vector<NodeVal>::const_iterator
  end() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値割り当てのリスト
  vector<NodeVal> mAsList;

};

/// @brief 2つの割当リストが矛盾しているか調べる．
bool
check_conflict(const NodeValList& src_list1,
	       const NodeValList& src_list2);

/// @brief 包含関係を調べる．
bool
check_contain(const NodeValList& src_list1,
	      const NodeValList& src_list2);

/// @brief 割当の内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] nv 値の割り当て
/// @return s を返す．
ostream&
operator<<(ostream& s,
	   NodeVal nv);

/// @brief 割当リストの内容を出力する．
/// @param[in] s 出力先のストリーム
/// @param[in] src_list 値の割り当てリスト
/// @return s を返す．
ostream&
operator<<(ostream& s,
	   const NodeValList& src_list);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
//
// 内容は不定
inline
NodeVal::NodeVal() :
  mPackVal(0UL)
{
}

// @brief 値を指定したコンストラクタ
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
// @param[in] val 値
inline
NodeVal::NodeVal(const TpgNode* node,
		 int time,
		 bool val) :
  mPackVal(reinterpret_cast<ympuint>(node) | (time << 1) | val)
{
}

// @brief ノードを返す．
inline
const TpgNode*
NodeVal::node() const
{
  return reinterpret_cast<const TpgNode*>(mPackVal & ~3UL);
}

// @brief 時刻を返す．
inline
int
NodeVal::time() const
{
  return static_cast<int>((mPackVal >> 1) & 1U);
}

// @brief 値を返す．
inline
bool
NodeVal::val() const
{
  return static_cast<bool>(mPackVal & 1UL);
}

// @brief 大小関係の比較関数
inline
bool
operator>(const NodeVal& left,
	  const NodeVal& right)
{
  return operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator<=(const NodeVal& left,
	   const NodeVal& right)
{
  return !operator<(right, left);
}

// @brief 大小関係の比較関数
inline
bool
operator>=(const NodeVal& left,
	   const NodeVal& right)
{
  return !operator<(left, right);
}

// @brief コンストラクタ
inline
NodeValList::NodeValList()
{
}

// @brief コピーコンストラクタ
inline
NodeValList::NodeValList(const NodeValList& src) :
  mAsList(src.mAsList)
{
}

// @brief ムーブコンストラクタ
inline
NodeValList::NodeValList(NodeValList&& src) :
  mAsList(src.mAsList)
{
  // vector<> はムーブコンストラクタを持っている．
}

// @brief コピー代入演算子
inline
NodeValList&
NodeValList::operator=(const NodeValList& src)
{
  mAsList = src.mAsList;

  return *this;
}

// @brief ムーブ代入演算子
inline
NodeValList&
NodeValList::operator=(NodeValList&& src)
{
  // vector<> はムーブ代入演算子を持っている．
  mAsList = src.mAsList;

  return *this;
}

// @brief デストラクタ
inline
NodeValList::~NodeValList()
{
}

// @brief 内容を空にする．
inline
void
NodeValList::clear()
{
  mAsList.clear();
}

// @brief 要素数を返す．
inline
int
NodeValList::size() const
{
  return mAsList.size();
}

// @brief ソートする．
inline
void
NodeValList::sort()
{
  std::sort(mAsList.begin(), mAsList.end());
}

// @brief 値を追加する．
// @param[in] node ノード
// @param[in] time 時刻(0 or 1)
// @param[in] val 値
inline
void
NodeValList::add(const TpgNode* node,
		 int time,
		 bool val)
{
  mAsList.push_back(NodeVal(node, time, val));
}

// @brief 要素を返す．
// @param[in] pos 位置 ( 0 <= pos < size() )
inline
NodeVal
NodeValList::operator[](int pos) const
{
  return elem(pos);
}

// @brief 要素を返す．
// @param[in] pos 位置 ( 0 <= pos < size() )
//
// operator[] の別名
inline
NodeVal
NodeValList::elem(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < size() );

  return mAsList[pos];
}

// @brief 先頭の反復子を返す．
inline
vector<NodeVal>::const_iterator
NodeValList::begin() const
{
  return mAsList.begin();
}

// @brief 末尾の反復子を返す．
inline
vector<NodeVal>::const_iterator
NodeValList::end() const
{
  return mAsList.end();
}

END_NAMESPACE_YM_SATPG

#endif // NODEVALLIST_H
