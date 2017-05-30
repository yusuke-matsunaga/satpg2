#ifndef DTPG2IMPL_H
#define DTPG2IMPL_H

/// @file Dtpg2Impl.h
/// @brief Dtpg2Impl のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgStats.h"
#include "NodeValList.h"

#include "ym/RandGen.h"
#include "ym/ym_sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

#include "VidMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Dtpg2Impl Dtpg2Impl.h "Dtpg2Impl.h"
/// @brief Dtpg の実装用のクラス
//////////////////////////////////////////////////////////////////////
class Dtpg2Impl
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  /// @param[in] network 対象のネットワーク
  /// @param[in] root 故障伝搬の起点となるノード
  Dtpg2Impl(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    BackTracer& bt,
	    const TpgNetwork& network,
	    const TpgNode* root);

  /// @brief デストラクタ
  ~Dtpg2Impl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 回路の構造を表すCNF式を作る．
  /// @param[out] stats DTPGの統計情報
  ///
  /// このオブジェクトに対しては1回行えばよい．
  /// というか1回しか行えない．
  void
  gen_cnf(DtpgStats& stats);

  /// @brief XOR制約のための変数リストを作る．
  /// @return 変数の数を返す．
  ymuint
  make_xor_list();

  /// @brief XOR制約を追加する．
  void
  add_xor_constraint(ymuint num,
		     RandGen& randgen);

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  SatBool3
  dtpg(const TpgFault* fault,
       NodeValList& nodeval_list,
       DtpgStats& stats);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const;

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief CNF 作成を開始する．
  void
  cnf_begin();

  /// @brief CNF 作成を終了する．
  void
  cnf_end(DtpgStats& stats);

  /// @brief 時間計測を開始する．
  void
  timer_start();

  /// @brief 時間計測を終了する．
  USTime
  timer_stop();

  /// @brief SATソルバを返す．
  SatSolver&
  solver();

  /// @brief 1時刻前の正常値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  hvar(const TpgNode* node);

  /// @brief 正常値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  gvar(const TpgNode* node);

  /// @brief 故障値の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  fvar(const TpgNode* node);

  /// @brief 伝搬条件の変数を返す．
  /// @param[in] node 対象のノード
  SatVarId
  dvar(const TpgNode* node);

  /// @brief 1時刻前の正常値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_hvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 正常値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_gvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 故障値値の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_fvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 故障伝搬条件の変数を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] var 設定する変数
  void
  set_dvar(const TpgNode* node,
	   SatVarId var);

  /// @brief 1時刻前の正常値の変数マップを返す．
  const VidMap&
  hvar_map() const;

  /// @brief 正常値の変数マップを返す．
  const VidMap&
  gvar_map() const;

  /// @brief 故障値の変数マップを返す．
  const VidMap&
  fvar_map() const;

  /// @brief 起点となるノードを返す．
  const TpgNode*
  root_node() const;

  /// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
  void
  gen_cnf_base();

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(const TpgNode* node);

  /// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
  /// @param[in] fault 対象の故障
  /// @param[out] assign_list 結果の値割り当てリスト
  void
  make_ffr_condition(const TpgFault* fault,
		     NodeValList& assign_list);

  /// @brief NodeValList に追加する．
  /// @param[in] assign_list 追加するリスト
  /// @param[in] node 対象のノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[in] val 値
  void
  add_assign(NodeValList& assign_list,
	     const TpgNode* node,
	     int time,
	     bool val);

  /// @brief 一つの SAT問題を解く．
  /// @param[in] fault 対象の故障
  /// @param[in] assumptions 値の決まっている変数のリスト
  /// @param[out] nodeval_list 結果の値割り当てリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  SatBool3
  solve(const TpgFault* fault,
	const vector<SatLiteral>& assumptions,
	NodeValList& nodeval_list,
	DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// XOR制約を作る．
  SatLiteral
  make_xor(const vector<SatVarId>& var_list,
	   ymuint start,
	   ymuint end);

  /// @brief TFO マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mNodeList に入れる．<br>
  /// 出力ノードの場合は mOutputList にも入れる．<br>
  /// すでにマークされていたら何もしない．
  void
  set_tfo_mark(const TpgNode* node);

  /// @brief TFI マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mNodeList に入れる．
  void
  set_tfi_mark(const TpgNode* node);

  /// @brief TFI2 マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mNodeList に入れる．
  void
  set_tfi2_mark(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot;

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 故障番号をキーにしてFFR番号を入れる配列
  vector<ymuint> mElemPosMap;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemNum
  vector<SatVarId> mElemVarArray;

  // 関係するノードを入れておくリスト
  vector<const TpgNode*> mNodeList;

  // TFI に含まれる DFF 入れておくリスト
  vector<const TpgDff*> mDffList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mNodeList2;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<ymuint8> mMarkArray;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // バックトレーサー
  BackTracer& mBackTracer;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  StopWatch mTimer;

  ymuint mXorNum1;
  ymuint mXorNum2;
  vector<const TpgNode*> mXorNodeList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief SATソルバを返す．
inline
SatSolver&
Dtpg2Impl::solver()
{
  return mSolver;
}

// @brief 対象のネットワークを返す．
inline
const TpgNetwork&
Dtpg2Impl::network() const
{
  return mNetwork;
}

// @brief ノード番号の最大値を返す．
inline
ymuint
Dtpg2Impl::max_node_id() const
{
  return network().node_num();
}

// @brief 起点となるノードを返す．
inline
const TpgNode*
Dtpg2Impl::root_node() const
{
  return mRoot;
}

// @brief 1時刻前の正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg2Impl::hvar(const TpgNode* node)
{
  ASSERT_COND( mHvarMap(node) != kSatVarIdIllegal );

  return mHvarMap(node);
}

// @brief 正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg2Impl::gvar(const TpgNode* node)
{
  return mGvarMap(node);
}

// @brief 故障値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg2Impl::fvar(const TpgNode* node)
{
  return mFvarMap(node);
}

// @brief 伝搬条件の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg2Impl::dvar(const TpgNode* node)
{
  return mDvarMap(node);
}

// @brief 1時刻前の正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg2Impl::set_hvar(const TpgNode* node,
		   SatVarId var)
{
  mHvarMap.set_vid(node, var);
}

// @brief 正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg2Impl::set_gvar(const TpgNode* node,
		   SatVarId var)
{
  mGvarMap.set_vid(node, var);
}

// @brief 故障値値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg2Impl::set_fvar(const TpgNode* node,
		   SatVarId var)
{
  mFvarMap.set_vid(node, var);
}

// @brief 故障伝搬条件の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg2Impl::set_dvar(const TpgNode* node,
		   SatVarId var)
{
  mDvarMap.set_vid(node, var);
}

// @brief 1時刻前の正常値の変数マップを返す．
inline
const VidMap&
Dtpg2Impl::hvar_map() const
{
  return mHvarMap;
}

// @brief 正常値の変数マップを返す．
inline
const VidMap&
Dtpg2Impl::gvar_map() const
{
  return mGvarMap;
}

// @brief 故障値の変数マップを返す．
inline
const VidMap&
Dtpg2Impl::fvar_map() const
{
  return mFvarMap;
}

// @brief TFO マークをつける．
inline
void
Dtpg2Impl::set_tfo_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
    mMarkArray[id] |= 1U;
    mNodeList.push_back(node);
    if ( node->is_ppo() ) {
      mOutputList.push_back(node);
    }
  }
}

// @brief TFI マークをつける．
inline
void
Dtpg2Impl::set_tfi_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( (mMarkArray[id] & 3U) == 0U ) {
    mMarkArray[id] |= 2U;
    mNodeList.push_back(node);
    if ( node->is_dff_output() ) {
      mDffList.push_back(node->dff());
    }
  }
}

// @brief TFI2 マークをつける．
inline
void
Dtpg2Impl::set_tfi2_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( ((mMarkArray[id] >> 2) & 1U) == 0U ) {
    mMarkArray[id] |= 4U;
    mNodeList2.push_back(node);
  }
}

END_NAMESPACE_YM_SATPG

#endif // DTPG2IMPL_H
