
/// @file MpColGraph.cc
/// @brief MpColGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MpColGraph.h"
#include "MpColNode.h"
#include "TestVector.h"
#include "ym/Range.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] tv_list テストパタンのリスト
MpColGraph::MpColGraph(const vector<TestVector>& tv_list)
{
  init(tv_list);
}

// @brief デストラクタ
MpColGraph::~MpColGraph()
{
  delete [] mAdjListArray;
  delete [] mColorMap;
}

// @brief 初期化する
// @param[in] tv_list テストパタンのリスト
void
MpColGraph::init(const vector<TestVector>& tv_list)
{
  mColNum = 0;
  mNodeNum = tv_list.size();
  if ( mNodeNum == 0 ) {
    mAdjListArray = nullptr;
    mColorMap = nullptr;
    return;
  }

  mAdjListArray = new AdjList[mNodeNum];
  mColorMap = new int[mNodeNum];
  for ( auto i: Range(mNodeNum) ) {
    mColorMap[i] = 0;
  }

  int vs = tv_list[0].vector_size();
  vector<vector<int>> conflict_pair_array(vs * 2);
  vector<vector<int>> conflict_pos_array(mNodeNum);
  for ( auto bit: Range(vs) ) {
    int pos0 = bit * 2 + 0;
    int pos1 = bit * 2 + 1;
    vector<int>& list0 = conflict_pair_array[pos0];
    vector<int>& list1 = conflict_pair_array[pos1];
    for ( auto id: Range(mNodeNum) ) {
      const TestVector& tv = tv_list[id];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(id);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(id);
      }
    }
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto id: list0 ) {
	conflict_pos_array[id].push_back(pos1);
      }
      for ( auto id: list1 ) {
	conflict_pos_array[id].push_back(pos0);
      }
    }
  }

  for ( auto id: Range(mNodeNum) ) {
    HashSet<int> id_set;
    vector<int> tmp_list;
    for ( auto pos: conflict_pos_array[id] ) {
      const vector<int>& conflict_list = conflict_pair_array[pos];
      for ( auto id1: conflict_list ) {
	if ( !id_set.check(id1) ) {
	  id_set.add(id1);
	  tmp_list.push_back(id1);
	}
      }
    }
    mAdjListArray[id].init(tmp_list);
  }
}

// @brief color_map を作る．
int
MpColGraph::get_color_map(vector<int>& color_map) const
{
  color_map.clear();
  color_map.resize(node_num());
  for ( auto node_id: Range(node_num()) ) {
    color_map[node_id] = mColorMap[node_id];
  }
  return color_num();
}

MpColGraph::AdjList::AdjList() :
  mBody(nullptr),
  mNum(0)
{
}

MpColGraph::AdjList::~AdjList()
{
  delete [] mBody;
}

void
MpColGraph::AdjList::init(const vector<int>& src_list)
{
  delete [] mBody;
  mNum = src_list.size();
  mBody = new int[mNum];
  for ( auto i: Range(mNum) ) {
    mBody[i] = src_list[i];
  }
}

END_NAMESPACE_YM_SATPG
