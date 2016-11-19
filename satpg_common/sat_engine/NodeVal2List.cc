
/// @file NodeVal2List.cc
/// @brief NodeVal2List の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "NodeVal2List.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief マージする．
// @param[in] src_list マージするリスト
//
// 矛盾する割当があった場合の動作は不定
void
NodeVal2List::merge(const NodeVal2List& src_list)
{
  vector<NodeVal2> tmp_list;
  ymuint n1 = mAsList.size();
  ymuint n2 = src_list.mAsList.size();
  tmp_list.reserve(n1 + n2);
  ymuint i1 = 0;
  ymuint i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal2 v1 = mAsList[i1];
    NodeVal2 v2 = src_list.mAsList[i2];
    if ( v1 < v2 ) {
      tmp_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      tmp_list.push_back(v2);
      ++ i2;
    }
    else {
      tmp_list.push_back(v1);
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1) {
    NodeVal2 v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }
  for ( ; i2 < n2; ++ i2) {
    NodeVal2 v2 = src_list.mAsList[i2];
    tmp_list.push_back(v2);
  }

  mAsList = tmp_list;
}

// @brief 差分を計算する．
// @param[in] src_list 差分の対象のリスト
void
NodeVal2List::diff(const NodeVal2List& src_list)
{
  vector<NodeVal2> tmp_list;
  ymuint n1 = mAsList.size();
  ymuint n2 = src_list.mAsList.size();
  tmp_list.reserve(n1);
  ymuint i1 = 0;
  ymuint i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal2 v1 = mAsList[i1];
    NodeVal2 v2 = src_list.mAsList[i2];
    if ( v1 < v2 ) {
      tmp_list.push_back(v1);
      ++ i1;
    }
    else if ( v1 > v2 ) {
      ++ i2;
    }
    else {
      ++ i1;
      ++ i2;
    }
  }
  for ( ; i1 < n1; ++ i1) {
    NodeVal2 v1 = mAsList[i1];
    tmp_list.push_back(v1);
  }

  mAsList = tmp_list;
}

// @brief 矛盾した内容になっていないかチェックする．
// @return 正しければ true を返す．
//
// 具体的には同じノードで異なる値がある場合にエラーとなる．
// この関数はソートされている前提で動作する．
bool
NodeVal2List::sanity_check() const
{
  NodeVal2 prev(nullptr, false);
  for (ymuint i = 0; i < mAsList.size(); ++ i) {
    NodeVal2 nv = mAsList[i];
    if ( prev.node() == nv.node() && prev.val() != nv.val() ) {
      return false;
    }
    prev = nv;
  }
  return true;
}

// @brief 2つの割当リストが矛盾しているか調べる．
bool
check_conflict(const NodeVal2List& src_list1,
	       const NodeVal2List& src_list2)
{
  ymuint n1 = src_list1.size();
  ymuint n2 = src_list2.size();
  ymuint i1 = 0;
  ymuint i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal2 nv1 = src_list1[i1];
    NodeVal2 nv2 = src_list2[i2];
    if ( nv1.node() == nv2.node() ) {
      if ( nv1.val() != nv2.val() ) {
	return true;
      }
      ++ i1;
      ++ i2;
    }
    else if ( nv1 < nv2 ) {
      ++ i1;
    }
    else {
      ++ i2;
    }
  }
  return false;
}

// @brief 包含関係を調べる．
bool
check_contain(const NodeVal2List& src_list1,
	      const NodeVal2List& src_list2)
{
  ymuint n1 = src_list1.size();
  ymuint n2 = src_list2.size();
  ymuint i1 = 0;
  ymuint i2 = 0;
  while ( i1 < n1 && i2 < n2 ) {
    NodeVal2 nv1 = src_list1[i1];
    NodeVal2 nv2 = src_list2[i2];
    if ( nv1 < nv2 ) {
      ++ i1;
    }
    else if ( nv1 > nv2 ) {
      return false;
    }
    else {
      ++ i1;
      ++ i2;
    }
  }
  if ( i2 < n2 ) {
    return false;
  }
  else {
    return true;
  }
}

// @brief 割当リストの内容を出力する．
ostream&
operator<<(ostream& s,
	   const NodeVal2List& src_list)
{
  ymuint n = src_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal2 nv = src_list[i];
    s << " Node#" << nv.node()->id()
      << ":" << nv.val();
  }
  return s;
}

END_NAMESPACE_YM_SATPG
