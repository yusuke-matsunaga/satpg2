#ifndef MINPATMGR_H
#define MINPATMGR_H

/// @file MinPatMgr.h
/// @brief MinPatMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "TestVector.h"


BEGIN_NAMESPACE_YM_SATPG

class MpColGraph;

//////////////////////////////////////////////////////////////////////
/// @class MinPatMgr MinPatMgr.h "MinPatMgr.h"
/// @brief テストセット最小化を行うクラス
//////////////////////////////////////////////////////////////////////
class MinPatMgr
{
public:

  /// @brief コンストラクタ
  MinPatMgr();

  /// @brief デストラクタ
  ~MinPatMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @param[in] fault_list 故障のリスト
  /// @param[in] tv_list テストパタンのリスト
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  void
  init(const vector<const TpgFault*>& fault_list,
       const vector<TestVector>& tv_list,
       const TpgNetwork& network,
       FaultType fault_type);

  /// @brief 故障数を得る．
  int
  fault_num() const;

  /// @brief 故障を得る．
  /// @param[in] pos 位置 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(int pos) const;

  /// @brief 故障リストを得る．
  const vector<const TpgFault*>&
  fault_list() const;

  /// @brief 初期テストパタン数を得る．
  int
  orig_tv_num() const;

  /// @brief 初期テストパタンを得る．
  /// @param[in] pos 位置 ( 0 <= pos < orig_tv_num() )
  TestVector
  orig_tv(int pos) const;

  /// @brief 初期テストパタンのリストを得る．
  const vector<TestVector>&
  orig_tv_list() const;

  /// @brief 問題を解く．
  /// @param[in] algorithm アルゴリズム名
  /// @param[out] new_tv_list 圧縮したテストパタンのリスト
  /// @return 結果のパタン数を返す．
  int
  solve(const string& algorithm,
	vector<TestVector>& new_tv_list);

  /// @brief 彩色問題でパタン圧縮を行う．
  /// @param[in] tv_list 初期テストパタンのリスト
  /// @param[out] new_tv_list 圧縮結果のテストパタンのリスト
  /// @return 結果のパタン数を返す．
  static
  int
  coloring(const vector<TestVector>& tv_list,
	   vector<TestVector>& new_tv_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行い被覆表を作る．
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  ///
  /// 結果は mElemList に格納される．
  void
  gen_covering_matrix(const TpgNetwork& network,
		      FaultType fault_type);

  /// @brief 故障シミュレーションを行う．
  void
  do_fsim(Fsim& fsim,
	  int tv_base);

  /// @brief テストパタンの衝突リストを作る．
  void
  gen_conflict_list();

  /// @brief テストパタンの衝突リストを作る．
  void
  gen_conflict_list2();

  /// @brief mincov を解いてから coloring を行う．
  /// @param[out] color_map 彩色結果
  /// @return 彩色数を返す．
  ///
  /// 彩色数を nc とすると color_map[i] は 0 - nc の値を取る．
  /// 0 のテストパタンは未彩色を表す．
  int
  mincov_coloring(vector<int>& color_map);

  /// @brief coloring を解いてから mincov を行う．
  /// @param[out] color_map 彩色結果
  /// @return 彩色数を返す．
  ///
  /// 彩色数を nc とすると color_map[i] は 0 - nc の値を取る．
  /// 0 のテストパタンは未彩色を表す．
  int
  coloring_mincov(vector<int>& color_map);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障リスト
  vector<const TpgFault*> mFaultList;

  // 故障のID番号をキーにして mFaultList 上の位置を保持する配列
  vector<int> mFidMap;

  // 元のテストパタンのリスト
  vector<TestVector> mOrigTvList;

  // covering matrix の要素
  vector<pair<int, int>> mElemList;

  // 衝突するパタンのリストの対のリスト
  // サイズはテストパタンのビット長 x 2
  vector<vector<int>> mConflictPairList;

  // テストパタンごとの衝突リストのリスト
  vector<vector<int>> mConflictListArray;

  vector<vector<int>> mConflictListArray2;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 故障数を得る．
inline
int
MinPatMgr::fault_num() const
{
  return mFaultList.size();
}

// @brief 故障を得る．
// @param[in] pos 位置 ( 0 <= pos < fault_num() )
inline
const TpgFault*
MinPatMgr::fault(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < fault_num() );

  return mFaultList[pos];
}

// @brief 故障リストを得る．
inline
const vector<const TpgFault*>&
MinPatMgr::fault_list() const
{
  return mFaultList;
}

// @brief 初期テストパタン数を得る．
inline
int
MinPatMgr::orig_tv_num() const
{
  return mOrigTvList.size();
}

// @brief 初期テストパタンを得る．
// @param[in] pos 位置 ( 0 <= pos < orig_tv_num() )
inline
TestVector
MinPatMgr::orig_tv(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < orig_tv_num() );

  return mOrigTvList[pos];
}

// @brief 初期テストパタンのリストを得る．
inline
const vector<TestVector>&
MinPatMgr::orig_tv_list() const
{
  return mOrigTvList;
}

END_NAMESPACE_YM_SATPG

#endif // MINPATMGR_H
