#ifndef MPCOLGRAPH_H
#define MPCOLGRAPH_H

/// @file MpColGraph.h
/// @brief MpColGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class MpColGraph MpColGraph.h "MpColGraph.h"
/// @brief MinPatMgr の coloring 用のグラフを表すクラス
//////////////////////////////////////////////////////////////////////
class MpColGraph
{
public:

  /// @brief コンストラクタ
  /// @param[in] tv_list テストパタンのリスト
  MpColGraph(const vector<TestVector>& tv_list);

  /// @brief デストラクタ
  ~MpColGraph();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を返す．
  int
  node_num() const;

  /// @brief 隣接するノード番号のリストを得る．
  /// @param[in] node_id 対象のノード番号 ( 0 <= node_id < node_num() )
  Array<int>
  adj_list(int node_id) const;

  /// @brief 色数を返す．
  int
  color_num() const;

  /// @brief ノードの色を得る．
  /// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
  int
  color(int node_id) const;

  /// @brief 新しい色を割り当てる．
  /// @return 新しい色番号を返す．
  int
  new_color();

  /// @brief ノードを色をつける．
  /// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
  /// @param[in] color 色番号 ( 1 <= color <= color_num() )
  void
  set_color(int node_id,
	    int color);

  /// @brief ノードの集合を色をつける．
  /// @param[in] node_id_list ノード番号のリスト
  /// @param[in] color 色 ( 1 <= color <= color_num() )
  void
  set_color(const vector<int>& node_id_list,
	    int color);

  /// @brief color_map を作る．
  int
  get_color_map(vector<int>& color_map) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する
  /// @param[in] tv_list テストパタンのリスト
  void
  init(const vector<TestVector>& tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 隣接リストを表す構造体
  struct AdjList
  {
    AdjList();

    ~AdjList();

    void
    init(const vector<int>& src_list);

    // 要素数
    int mNum;

    // 実体の配列
    int* mBody;
  };

  // ノード数
  int mNodeNum;

  // ノードの隣接リストの配列
  // サイズは mNodeNum;
  AdjList* mAdjListArray;

  // 現在使用中の色数
  int mColNum;

  // 彩色結果の配列
  int* mColorMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード数を返す．
inline
int
MpColGraph::node_num() const
{
  return mNodeNum;
}

// @brief 隣接するノード番号のリストを得る．
// @param[in] node_id 対象のノード番号 ( 0 <= node_id < node_num() )
inline
Array<int>
MpColGraph::adj_list(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  const AdjList& adj_list = mAdjListArray[node_id];
  return Array<int>(adj_list.mBody, 0, adj_list.mNum);
}

// @brief 色数を返す．
inline
int
MpColGraph::color_num() const
{
  return mColNum;
}

// @brief ノードの色を得る．
// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
inline
int
MpColGraph::color(int node_id) const
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );

  return mColorMap[node_id];
}

// @brief 新しい色を割り当てる．
// @return 新しい色番号を返す．
inline
int
MpColGraph::new_color()
{
  return ++ mColNum;
}

// @brief ノードを色をつける．
// @param[in] node_id ノード番号 ( 0 <= node_id < node_num() )
// @param[in] color 色番号 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(int node_id,
		      int color)
{
  ASSERT_COND( node_id >= 0 && node_id < node_num() );
  ASSERT_COND( color >= 1 && color <= color_num() );

  mColorMap[node_id] = color;
}

// @brief ノードの集合を色をつける．
// @param[in] node_id_list ノード番号のリスト
// @param[in] color 色 ( 1 <= color <= color_num() )
inline
void
MpColGraph::set_color(const vector<int>& node_id_list,
		      int color)
{
  ASSERT_COND( color >= 1 && color <= color_num() );

  for ( auto node_id: node_id_list ) {
    set_color(node_id, color);
  }
}

END_NAMESPACE_YM_SATPG

#endif // MPCOLGRAPH_H
