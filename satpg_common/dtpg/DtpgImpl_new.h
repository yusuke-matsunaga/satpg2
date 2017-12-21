#ifndef DTPGIMPL_NEW_H
#define DTPGIMPL_NEW_H

/// @file DtpgImpl.h
/// @brief DtpgImpl のヘッダファイル
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
#include "FaultType.h"
#include "StructEnc.h"

#include "ym/ym_sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgImpl DtpgImpl.h "DtpgImpl.h"
/// @brief Dtpg の実装用のクラス
//////////////////////////////////////////////////////////////////////
class DtpgImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] jt 正当化を行うファンクタ
  /// @param[in] max_node_id ノード番号の最大値(+1)
  DtpgImpl(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   Justifier& jt,
	   ymuint max_node_id);

  /// @brief デストラクタ
  virtual
  ~DtpgImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 回路の構造を表すCNF式を作る．
  /// @param[in] root FFR(MFFC)の根のノード
  /// @param[out] stats DTPGの統計情報
  ///
  /// このオブジェクトに対しては1回行えばよい．
  /// というか1回しか行えない．
  void
  gen_cnf(const TpgNode* root,
	  DtpgStats& stats);

  /// @brief 回路の構造を表すCNF式を作る．
  /// @param[out] stats DTPGの統計情報
  ///
  /// このオブジェクトに対しては1回行えばよい．
  /// というか1回しか行えない．
  void
  gen_cnf(const TpgMFFC* mffc,
	  DtpgStats& stats);

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

#if 0
  /// @brief SATソルバを返す．
  SatSolver&
  solver();

  /// @brief StructEnc を返す．
  StructEnc&
  struct_sat();

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const;

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief 起点となるノードを返す．
  const TpgNode*
  root_node() const;
#endif

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // StructEnc の本体
  StructEnc mStructEnc;

  // 故障の種類
  FaultType mFaultType;

  // 正当化を行うファンクタ
  Justifier& mJustifier;

  // 時間計測を行なうかどうかの制御フラグ
  bool mTimerEnable;

  // 時間計測用のタイマー
  StopWatch mTimer;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

#if 0
// @brief SATソルバを返す．
inline
SatSolver&
DtpgImpl::solver()
{
  return mStructEnc.solver();
}

// @brief StructEnc を返す．
inline
StructEnc&
DtpgImpl::struct_sat()
{
  return mStructEnc;
}

// @brief 故障のファンアウトコーンを返す．
inline
const FoCone*
DtpgImpl::focone()
{
  return mFoCone;
}

// @brief 対象のネットワークを返す．
inline
const TpgNetwork&
DtpgImpl::network() const
{
  return mNetwork;
}

// @brief ノード番号の最大値を返す．
inline
ymuint
DtpgImpl::max_node_id() const
{
  return network().node_num();
}

// @brief 起点となるノードを返す．
inline
const TpgNode*
DtpgImpl::root_node() const
{
  return mRoot;
}
#endif

END_NAMESPACE_YM_SATPG

#endif // DTPGIMPL_H
