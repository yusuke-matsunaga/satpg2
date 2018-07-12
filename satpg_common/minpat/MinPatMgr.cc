
/// @file MinPatMgr.cc
/// @brief MinPatMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatMgr.h"
#include "TestVector.h"
#include "MpColGraph.h"
#include "MatrixGen.h"
#include "ym/McMatrix.h"
#include "ym/HashSet.h"
#include "ym/Range.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

class MpComp :
  public McColComp
{
public:

  /// @brief コンストラクタ
  MpComp(const MpColGraph& graph);

  /// @brief 比較関数
  ///
  /// col1 の代わりに col2 を使ってもコストが上がらない時に true を返す．
  bool
  operator()(int col1,
	     int col2) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  const MpColGraph& mGraph;

};

MpComp::MpComp(const MpColGraph& graph) :
  mGraph(graph)
{
}

// @brief 比較関数
bool
MpComp::operator()(int col1,
		   int col2) const
{
  bool ans = mGraph.containment_check(col2, col1);
  return ans;
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

// @brief 彩色問題でパタン圧縮を行う．
// @param[in] tv_list 初期テストパタンのリスト
// @param[out] new_tv_list 圧縮結果のテストパタンのリスト
// @return 結果のパタン数を返す．
int
MinPatMgr::coloring(const vector<const TpgFault*>& fault_list,
		    const vector<TestVector>& tv_list,
		    const TpgNetwork& network,
		    FaultType fault_type,
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

  MatrixGen matgen(fault_list, tv_list, network, fault_type);
  std::unique_ptr<McMatrix> matrix = matgen.generate();

  cout << " McMatrix generated" << endl;

  {
    cout << matrix->active_row_num() << " x " << matrix->active_col_num() << endl;
  }

  StopWatch timer;

  MpComp comp(graph);
  vector<int> selected_cols;
  bool changed = true;
  while ( !selected_cols.empty() || matrix->active_row_num() > 0 ) {
    if ( changed && selected_cols.empty() ) {
      {
	timer.reset();
	timer.start();
	cout << "reducing matrix: "
	     << matrix->active_row_num() << " x " << matrix->active_col_num()
	     << endl;
      }

      // 被覆行列の縮約を行う．
      vector<int> deleted_cols;
      matrix->reduce(selected_cols, deleted_cols, comp);
      ASSERT_COND( !selected_cols.empty() || matrix->active_row_num() > 0 );

      {
	timer.stop();
	USTime time = timer.time();
	cout << " ==> "
	     << matrix->active_row_num() << " x " << matrix->active_col_num()
	     << ", # of selected_cols = " << selected_cols.size()
	     << ", " << time << endl;
      }

      // 今の縮約で削除された列を衝突グラフからも削除する．
      for ( auto col: deleted_cols ) {
	graph.delete_node(col);
      }
    }

    // 両立集合を1つ選ぶ．
    vector<int> node_list;
    get_compatible_nodes(graph, *matrix, selected_cols, node_list);
    ASSERT_COND( !node_list.empty() );

    // 選ばれた両立集合に彩色を行う．
    int color = graph.new_color();
    graph.set_color(node_list, color);

    // 被覆行列の更新を行う．
    changed = false;
    for ( auto col: node_list ) {
      if ( !matrix->col_deleted(col) ) {
	matrix->select_col(col);
	changed = true;
      }
    }
    {
      cout << "# of selected columns: " << node_list.size() << endl
	   << " ==> " << matrix->active_row_num()
	   << " x " << matrix->active_col_num() << endl;
    }

    // selected_cols の更新を行う．
    if ( !selected_cols.empty() ) {
      vector<bool> mark(matrix->col_size(), false);
      for ( auto col: node_list ) {
	mark[col] = true;
      }
      int rpos = 0;
      int wpos = 0;
      int n = selected_cols.size();
      for ( ; rpos < n; ++ rpos ) {
	auto col = selected_cols[rpos];
	if ( !mark[col] ) {
	  selected_cols[wpos] = col;
	  ++ wpos;
	}
      }
      selected_cols.erase(selected_cols.begin() + wpos, selected_cols.end());
    }
  }

  vector<int> color_map;
  int nc = graph.get_color_map(color_map);
  merge_tv_list(tv_list, nc, color_map, new_tv_list);

  cout << "# of reduced patterns: " << nc << endl;

  return nc;
}

// @brief 両立集合を取り出す．
// @param[in] graph 衝突グラフ
// @param[in] matrix 被覆行列
// @param[in] selected_nodes 選択済みのノードリスト
// @param[out] node_list 結果の両立集合を表すリスト
//
// * selected_nodes に含まれるノードは matrix からは削除されている．
void
MinPatMgr::get_compatible_nodes(const MpColGraph& graph,
				const McMatrix& matrix,
				const vector<int>& selected_nodes,
				vector<int>& node_list)
{
  vector<bool> col_mark(graph.node_num(), false);
  vector<bool> row_mark(matrix.row_size(), false);
  node_list.clear();
  if ( selected_nodes.empty() ) {
    // matrix のアクティブな列から要素数の最も多い列を選ぶ．
    int max_n = 0;
    int min_c = graph.node_num() + 1;
    int max_col = -1;
    for ( auto col: matrix.col_head_list() ) {
      int n = matrix.col_elem_num(col);
      if ( max_n < n ) {
	max_n = n;
	min_c = graph.conflict_num(col);
	max_col = col;
      }
      else if ( max_n == n ) {
	int c = graph.conflict_num(col);
	if ( min_c > c ) {
	  min_c = c;
	  max_col = col;
	}
      }
    }
    node_list.push_back(max_col);
    col_mark[max_col] = true;
    for ( auto row: matrix.col_list(max_col) ) {
      row_mark[row] = true;
    }
  }
  else {
    // 衝突数の最も少ないノードを選ぶ．
    int min_c = graph.node_num() + 1;
    int min_id = -1;
    for ( auto id: selected_nodes ) {
      int c = graph.conflict_num(id);
      if ( min_c > c ) {
	min_c = c;
	min_id = id;
      }
    }
    node_list.push_back(min_id);
    col_mark[min_id] = true;

    // min_id に両立するノードを cand_list に入れる．
    vector<int> cand_list;
    cand_list.reserve(selected_nodes.size());
    for ( auto id: selected_nodes ) {
      if ( !col_mark[id] && graph.compatible_check(id, min_id) ) {
	cand_list.push_back(id);
      }
    }
    while ( !cand_list.empty() ) {
      // selected_nodes の中で node_list に両立するものを
      // 衝突数の少ない順に加える．
      int min_c = graph.node_num() + 1;
      int min_id = -1;
      for ( auto id: cand_list ) {
	int c = graph.conflict_num(id);
	if ( min_c > c ) {
	  min_c = c;
	  min_id = id;
	}
      }
      node_list.push_back(min_id);
      col_mark[min_id] = true;
      // min_id と衝突するノードを cand_list から除外する．
      int rpos = 0;
      int wpos = 0;
      int n = cand_list.size();
      for ( ; rpos < n; ++ rpos ) {
	auto id = cand_list[rpos];
	if ( id != min_id && graph.compatible_check(id, min_id) ) {
	  cand_list[wpos] = id;
	  ++ wpos;
	}
      }
      // 最低でも min_id は削除される．
      cand_list.erase(cand_list.begin() + wpos, cand_list.end());
    }
  }

  // node_list に両立するノードを cand_list に入れる．
  vector<int> cand_list;
  cand_list.reserve(matrix.active_col_num());
  for ( auto col: matrix.col_head_list() ) {
    if ( !col_mark[col] && graph.compatible_check(col, node_list) ) {
      cand_list.push_back(col);
    }
  }
  for ( ; ; ) {
    // cand_list の中で新たに被覆する行の最も多いものを選ぶ．
    int max_num = 0;
    int max_col = -1;
    for ( auto col: cand_list ) {
      int num = 0;
      for ( auto row: matrix.col_list(col) ) {
	if ( !row_mark[row] ) {
	  ++ num;
	}
      }
      if ( max_num < num ) {
	max_num = num;
	max_col = col;
      }
    }
    if ( max_col == -1 ) {
      // なければ終わる．
      break;
    }

    // max_col を選ぶ．
    node_list.push_back(max_col);
    for ( auto row: matrix.col_list(max_col) ) {
      row_mark[row] = true;
    }

    // max_col と衝突するノードを cand_list から除外する．
    int rpos = 0;
    int wpos = 0;
    int n = cand_list.size();
    for ( ; rpos < n; ++ rpos ) {
      auto col = cand_list[rpos];
      if ( col != max_col && graph.compatible_check(col, max_col) ) {
	cand_list[wpos] = col;
	++ wpos;
      }
    }
    // 必ず wpos < n
    cand_list.erase(cand_list.begin() + wpos, cand_list.end());
  }
}

// @brief 彩色結果から新しいテストパタンのリストを生成する．
// @param[in] tv_list テストパタンのリスト
// @param[in] nc 彩色数
// @param[in] color_map 彩色結果
// @param[out] new_tv_list マージされたテストパタンのリスト
void
MinPatMgr::merge_tv_list(const vector<TestVector>& tv_list,
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

END_NAMESPACE_YM_SATPG
