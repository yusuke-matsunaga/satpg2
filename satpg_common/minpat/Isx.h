#ifndef ISX_H
#define ISX_H

/// @file Isx.h
/// @brief Isx のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "MpColGraph.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Isx Isx.h "Isx.h"
/// @brief independent set extraction を行うクラス
//////////////////////////////////////////////////////////////////////
class Isx
{
public:

  /// @brief コンストラクタ
  /// @param[in] graph 対象のグラフ
  Isx(MpColGraph& graph);

  /// @brief デストラクタ
  ~Isx();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief independent set extraction を用いた coloring を行う．
  /// @param[in] limit 残りのノード数がこの値を下回ったら処理をやめる．
  /// @return 彩色数を返す．
  ///
  /// ここでは部分的な彩色を行う．
  int
  coloring(int limit);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief maximal independent set を選ぶ．
  ///
  /// - 結果は indep_set に格納される．
  /// - mRandGen を用いてランダムに選ぶ．
  void
  get_indep_set(vector<int>& indep_set);

  /// @brief mCandList, mCandMark を初期化する．
  void
  init_cand_list();

  /// @brief 集合に加えるノ選ぶ．
  ///
  /// - 独立集合に隣接していないノードの内，隣接ノード数の少ないものを選ぶ．
  /// - 追加できるノードがない場合は -1 を返す．
  int
  select_node();

  /// @brief 候補リストを更新する．
  /// @param[in] node 新たに加わったノード
  void
  update_cand_list(int node_id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  MpColGraph& mGraph;

  // 候補ノードのリスト
  vector<int> mCandList;

  // 候補ノードの印
  // サイズは node_num()
  vector<bool> mCandMark;

  // 候補ノードの隣接数
  // サイズは node_num()
  vector<int> mAdjCount;

  // 乱数生成器
  RandGen mRandGen;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // ISX_H
