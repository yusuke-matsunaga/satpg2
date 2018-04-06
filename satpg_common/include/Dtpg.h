#ifndef DTPG_H
#define DTPG_H

/// @file Dtpg.h
/// @brief Dtpg のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgStats.h"
#include "NodeValList.h"
#include "FaultType.h"

#include "ym/ym_sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"
#include "ym/HashMap.h"

#include "VidMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Dtpg Dtpg.h "Dtpg.h"
/// @brief Dtpg の実装用のクラス
//////////////////////////////////////////////////////////////////////
class Dtpg
{
public:

  /// @brief コンストラクタ(FFRモード)
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] bt バックトレーサー
  /// @param[in] network 対象のネットワーク
  /// @param[in] root 故障伝搬の起点となるノード
  Dtpg(const string& sat_type,
       const string& sat_option,
       ostream* sat_outp,
       FaultType fault_type,
       Justifier& jt,
       const TpgNetwork& network,
       const TpgFFR& ffr,
       DtpgStats& stats);

  /// @brief コンストラクタ(MFFCモード)
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] bt バックトレーサー
  /// @param[in] network 対象のネットワーク
  /// @param[in] root 故障伝搬の起点となるノード
  Dtpg(const string& sat_type,
       const string& sat_option,
       ostream* sat_outp,
       FaultType fault_type,
       Justifier& jt,
       const TpgNetwork& network,
       const TpgMFFC& mffc,
       DtpgStats& stats);

  /// @brief デストラクタ
  ~Dtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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
  int
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

  /// @brief 関係するノードのリストを返す．
  const vector<const TpgNode*>&
  cur_node_list() const;

  /// @brief 関係する１時刻前のノードのリストを返す．
  const vector<const TpgNode*>&
  prev_node_list() const;

  /// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
  void
  gen_cnf_base();

  /// @brief mffc 内の影響が root まで伝搬する条件のCNF式を作る．
  void
  gen_cnf_mffc();

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(const TpgNode* node);

  /// @brief 故障挿入回路のCNFを作る．
  /// @param[in] elem_pos 要素番号
  /// @param[in] ovar ゲートの出力の変数
  void
  inject_fault(int elem_pos,
	       SatVarId ovar);

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

  /// @brief TFO マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfoList に入れる．<br>
  /// 出力ノードの場合は mOutputList にも入れる．<br>
  /// すでにマークされていたら何もしない．
  void
  set_tfo_mark(const TpgNode* node);

  /// @brief TFI マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfiList に入れる．
  void
  set_tfi_mark(const TpgNode* node);

  /// @brief TFI2 マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// と同時に mTfi2List に入れる．
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

  // 故障の種類
  FaultType mFaultType;

  // 故障伝搬の起点となるノード
  const TpgNode* mRoot;

  // TFOノードを入れておくリスト
  vector<const TpgNode*> mTfoList;

  // TFIノードを入れておくリスト
  vector<const TpgNode*> mTfiList;

  // TFI に含まれる DFF 入れておくリスト
  vector<const TpgDff*> mDffList;

  // 1時刻前関係するノードを入れておくリスト
  vector<const TpgNode*> mTfi2List;

  // 関係する出力ノードを入れておくリスト
  vector<const TpgNode*> mOutputList;

  // 作業用のマークを入れておく配列
  // サイズは mMaxNodeId
  vector<ymuint8> mMarkArray;

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemArray.size()
  vector<SatVarId> mElemVarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  HashMap<int, int> mElemPosMap;

  // 1時刻前の正常値を表す変数のマップ
  VidMap mHvarMap;

  // 正常値を表す変数のマップ
  VidMap mGvarMap;

  // 故障値を表す変数のマップ
  VidMap mFvarMap;

  // 故障伝搬条件を表す変数のマップ
  VidMap mDvarMap;

  // バックトレーサー
  Justifier& mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  StopWatch mTimer;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief SATソルバを返す．
inline
SatSolver&
Dtpg::solver()
{
  return mSolver;
}

// @brief 対象のネットワークを返す．
inline
const TpgNetwork&
Dtpg::network() const
{
  return mNetwork;
}

// @brief ノード番号の最大値を返す．
inline
int
Dtpg::max_node_id() const
{
  return network().node_num();
}

// @brief 起点となるノードを返す．
inline
const TpgNode*
Dtpg::root_node() const
{
  return mRoot;
}

// @brief 1時刻前の正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg::hvar(const TpgNode* node)
{
  ASSERT_COND( mHvarMap(node) != kSatVarIdIllegal );

  return mHvarMap(node);
}

// @brief 正常値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg::gvar(const TpgNode* node)
{
  return mGvarMap(node);
}

// @brief 故障値の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg::fvar(const TpgNode* node)
{
  return mFvarMap(node);
}

// @brief 伝搬条件の変数を返す．
// @param[in] node 対象のノード
inline
SatVarId
Dtpg::dvar(const TpgNode* node)
{
  return mDvarMap(node);
}

// @brief 1時刻前の正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg::set_hvar(const TpgNode* node,
	       SatVarId var)
{
  mHvarMap.set_vid(node, var);
}

// @brief 正常値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg::set_gvar(const TpgNode* node,
	       SatVarId var)
{
  mGvarMap.set_vid(node, var);
}

// @brief 故障値値の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg::set_fvar(const TpgNode* node,
	       SatVarId var)
{
  mFvarMap.set_vid(node, var);
}

// @brief 故障伝搬条件の変数を設定する．
// @param[in] node 対象のノード
// @param[in] var 設定する変数
inline
void
Dtpg::set_dvar(const TpgNode* node,
	       SatVarId var)
{
  mDvarMap.set_vid(node, var);
}

// @brief 1時刻前の正常値の変数マップを返す．
inline
const VidMap&
Dtpg::hvar_map() const
{
  return mHvarMap;
}

// @brief 正常値の変数マップを返す．
inline
const VidMap&
Dtpg::gvar_map() const
{
  return mGvarMap;
}

// @brief 故障値の変数マップを返す．
inline
const VidMap&
Dtpg::fvar_map() const
{
  return mFvarMap;
}

// @brief TFO マークをつける．
inline
void
Dtpg::set_tfo_mark(const TpgNode* node)
{
  int id = node->id();
  if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
    mMarkArray[id] |= 1U;
    mTfoList.push_back(node);
    if ( node->is_ppo() ) {
      mOutputList.push_back(node);
    }
  }
}

// @brief TFI マークをつける．
inline
void
Dtpg::set_tfi_mark(const TpgNode* node)
{
  int id = node->id();
  if ( (mMarkArray[id] & 3U) == 0U ) {
    mMarkArray[id] |= 2U;
    mTfiList.push_back(node);
    if ( mFaultType == FaultType::TransitionDelay && node->is_dff_output() ) {
      mDffList.push_back(node->dff());
    }
  }
}

// @brief TFI2 マークをつける．
inline
void
Dtpg::set_tfi2_mark(const TpgNode* node)
{
  int id = node->id();
  if ( ((mMarkArray[id] >> 2) & 1U) == 0U ) {
    mMarkArray[id] |= 4U;
    mTfi2List.push_back(node);
  }
}

END_NAMESPACE_YM_SATPG

#endif // DTPG_H
