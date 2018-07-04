
/// @file MinPatMgr.cc
/// @brief MinPatMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "MpColGraph.h"
#include "MpColNode.h"
#include "Isx.h"
#include "Dsatur.h"
#include "ym/MinCov.h"
#include "ym/UdGraph.h"
#include "ym/Range.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// @brief 彩色結果から新しいテストパタンのリストを生成する．
void
merge_tv_list(const vector<TestVector>& tv_list,
	      int nc,
	      const vector<int>& color_map,
	      vector<TestVector>& new_tv_list)
{
  int nv = tv_list.size();
  vector<vector<int>> tvgroup_list(nc);
  for ( auto tvid: Range(nv) ) {
    int c = color_map[tvid];
    if ( c > 0 ) {
      tvgroup_list[c - 1].push_back(tvid);
    }
  }

  new_tv_list.clear();
  for ( auto new_id: Range(nc) ) {
    const vector<int>& id_list = tvgroup_list[new_id];
    ASSERT_COND( id_list.size() > 0 );
    TestVector tv = tv_list[id_list[0]];
    for ( auto i: Range(1, id_list.size()) ) {
      tv &= tv_list[id_list[i]];
    }
    new_tv_list.push_back(tv);
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス MinPatMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatMgr::MinPatMgr()
{
}

// @brief デストラクタ
MinPatMgr::~MinPatMgr()
{
}

// @brief 初期化する．
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
void
MinPatMgr::init(const vector<const TpgFault*>& fault_list,
		const vector<TestVector>& tv_list,
		const TpgNetwork& network,
		FaultType fault_type)
{
  mFaultList = fault_list;
  mFidMap.clear();
  mFidMap.resize(network.max_fault_id(), -1);
  for ( auto i: Range(mFaultList.size()) ) {
    auto fault = mFaultList[i];
    mFidMap[fault->id()] = i;
  }

  mOrigTvList = tv_list;

  gen_covering_matrix(network, fault_type);

  gen_conflict_list();
}

// @brief 問題を解く．
// @param[in] algorithm アルゴリズム名
// @param[out] new_tv_list 圧縮したテストパタンのリスト
// @return 結果のパタン数を返す．
int
MinPatMgr::solve(const string& algorithm,
		 vector<TestVector>& new_tv_list)
{
#if 0
  vector<int> color_map;
  int nc = mincov_coloring(color_map);

  merge_tv_list(mOrigTvList, nc, color_map, new_tv_list);

  return nc;
#else
  return 0;
#endif
}

// @brief mincov を解いてから coloring を行う．
// @param[out] color_map 彩色結果
// @return 彩色数を返す．
//
// 彩色数を nc とすると color_map[i] は 0 - nc の値を取る．
// 0 のテストパタンは未彩色を表す．
int
MinPatMgr::mincov_coloring(vector<int>& color_map)
{
#if 0
  int nf = mFaultList.size();
  int nv = mOrigTvList.size();
  MinCov mincov(nf, nv);
  for ( auto p: mElemList ) {
    int row = p.first;
    int col = p.second;
    mincov.insert_elem(row, col);
  }

  vector<int> solution;
  int new_nv = mincov.heuristic(solution);

  vector<int> new_map(nv, -1);
  int new_pos = 0;
  for ( auto col: solution ) {
    new_map[col] = new_pos;
    ++ new_pos;
  }

  UdGraph graph(new_nv);
  for ( auto col1: solution ) {
    int new_col1 = new_map[col1];
    ASSERT_COND( new_col1 != -1 );
    const vector<int>& idlist_list = mConflictListArray[col1];
    vector<bool> mark(new_nv, false);
    for ( auto idlist: idlist_list ) {
      for ( auto col2: mConflictPairList[idlist] ) {
	int new_col2 = new_map[col2];
	if ( new_col2 > new_col1 && !mark[new_col2] ) {
	  mark[new_col2] = true;
	  graph.connect(new_col1, new_col2);
	}
      }
    }
  }

  vector<int> tmp_map;
  int nc = coloring(graph, "isx", tmp_map);

  color_map.clear();
  color_map.resize(nv, 0);
  for ( auto new_col: Range(new_nv) ) {
    int c = tmp_map[new_col];
    int old_col = solution[new_col];
    color_map[old_col] = c;
  }

  return nc;
#else
  return 0;
#endif
}

// @brief coloring を解いてから mincov を行う．
// @param[out] color_map 彩色結果
// @return 彩色数を返す．
//
// 彩色数を nc とすると color_map[i] は 0 - nc の値を取る．
// 0 のテストパタンは未彩色を表す．
int
MinPatMgr::coloring_mincov(vector<int>& color_map)
{
#if 0
  int nv = mOrigTvList.size();
  UdGraph graph(nv);
  for ( auto col1: Range(nv) ) {
    const vector<int>& idlist_list = mConflictListArray[col1];
    vector<bool> mark(nv, false);
    for ( auto idlist: idlist_list ) {
      for ( auto col2: mConflictPairList[idlist] ) {
	if ( col2 > col1 && !mark[col2] ) {
	  mark[col2] = true;
	  graph.connect(col1, col2);
	}
      }
    }
  }
  vector<int> tmp_map;
  int nc = coloring(graph, "isx", tmp_map);

  vector<TestVector> tmp_tv_list;
  merge_tv_list(mOrigTvList, nc, tmp_map, tmp_tv_list);
#else
  return 0;
#endif
}

// @brief 故障シミュレーションを行い被覆表を作る．
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
void
MinPatMgr::gen_covering_matrix(const TpgNetwork& network,
			       FaultType fault_type)
{
  Fsim fsim;
  fsim.init_fsim3(network, fault_type);

  mElemList.clear();
  int wpos = 0;
  fsim.clear_patterns();
  int tv_base = 0;
  for ( auto tv: mOrigTvList ) {
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      do_fsim(fsim, tv_base);
      fsim.clear_patterns();
      wpos = 0;
      tv_base += kPvBitLen;
    }
  }
  if ( wpos > 0 ) {
    do_fsim(fsim, tv_base);
  }
}

// @brief 故障シミュレーションを行う．
void
MinPatMgr::do_fsim(Fsim& fsim,
		   int tv_base)
{
  int ndet = fsim.ppsfp();
  for ( auto i: Range(ndet) ) {
    const TpgFault* fault = fsim.det_fault(i);
    PackedVal dbits = fsim.det_fault_pat(i);
    int fid = mFidMap[fault->id()];
    for ( auto bit: Range(kPvBitLen) ) {
      if ( dbits & (1UL << bit) ) {
	int tvid = tv_base + bit;
	// (fid, tvid) を記録
	mElemList.push_back(make_pair(fid, tvid));
      }
    }
  }
}

// @brief テストパタンの衝突リストを作る．
void
MinPatMgr::gen_conflict_list()
{
  if ( mOrigTvList.empty() ) {
    return;
  }

  StopWatch timer;
  timer.start();

  int nv = mOrigTvList.size();
  int vs = mOrigTvList[0].vector_size();
  mConflictPairList.resize(vs * 2);
  mConflictListArray.resize(nv);
  for ( auto bit: Range(vs) ) {
    int tvid = 0;
    int pos0 = bit * 2 + 0;
    int pos1 = bit * 2 + 1;
    vector<int>& list0 = mConflictPairList[pos0];
    vector<int>& list1 = mConflictPairList[pos1];
    for ( auto tvid: Range(nv) ) {
      TestVector tv = mOrigTvList[tvid];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(tvid);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(tvid);
      }
    }
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto tvid: list0 ) {
	mConflictListArray[tvid].push_back(pos1);
      }
      for ( auto tvid: list1 ) {
	mConflictListArray[tvid].push_back(pos0);
      }
    }
  }

  timer.stop();
  {
    USTime time = timer.time();
    cout << "Phase1: " << time << endl;
  }
}

// @brief テストパタンの衝突リストを作る．
void
MinPatMgr::gen_conflict_list2()
{
  if ( mOrigTvList.empty() ) {
    return;
  }

  StopWatch timer;
  timer.start();

  int nv = mOrigTvList.size();
  mConflictListArray2.resize(nv);
  for ( auto tvid: Range(nv) ) {
    vector<int>& conflict_list = mConflictListArray2[tvid];
    const vector<int>& idlist_list = mConflictListArray[tvid];
    vector<bool> mark(nv, false);
    for ( auto idlist: idlist_list ) {
      for ( auto id: mConflictPairList[idlist] ) {
	if ( !mark[id] ) {
	  mark[id] = true;
	  conflict_list.push_back(id);
	}
      }
    }
  }

  timer.stop();
  {
    USTime time = timer.time();
    cout << "Phase2: " << time << endl;
  }
}

// @brief 彩色問題でパタン圧縮を行う．
// @param[in] tv_list 初期テストパタンのリスト
// @param[out] new_tv_list 圧縮結果のテストパタンのリスト
// @return 結果のパタン数を返す．
int
MinPatMgr::coloring(const vector<TestVector>& tv_list,
		    vector<TestVector>& new_tv_list)
{
  new_tv_list.clear();
  int nv = tv_list.size();
  if ( nv == 0 ) {
    return 0;
  }

  cout << "*** coloring ***" << endl;
  cout << "# of initial patterns: " << nv << endl;

  MpColGraph graph(tv_list);

  cout << " MpColGraph generated" << endl;

  Isx isx(graph);
  isx.coloring(500);

  vector<int> color_map;
  int nc = graph.get_color_map(color_map);
  merge_tv_list(tv_list, nc, color_map, new_tv_list);

  return nc;
}

END_NAMESPACE_YM_SATPG
