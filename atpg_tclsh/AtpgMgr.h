#ifndef ATPGMGR_H
#define ATPGMGR_H

/// @file AtpgMgr.h
/// @brief AtpgMgr のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "sa/sa_nsdef.h"
#include "td/td_nsdef.h"

#include "TpgNetwork.h"
#include "ym/Binder.h"
#include "ym/StopWatch.h"
#include "ym/MStopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief タイマーのモード
//////////////////////////////////////////////////////////////////////
enum {
  /// @brief その他
  TM_MISC = 0,
  /// @brief ファイル読み込み
  TM_READ = 1,
  /// @brief DTPG
  TM_DTPG = 2,
  /// @brief 故障シミュレーション
  TM_FSIM = 3,
  /// @brief SAT
  TM_SAT  = 4,
  /// @brief タイマーのモード数
  TM_SIZE = 5
};


//////////////////////////////////////////////////////////////////////
/// @class AtpgMgr AtpgMgr.h "AtpgMgr.h"
/// @brief ATPG に共通のデータを保持しておくためのクラス
//////////////////////////////////////////////////////////////////////
class AtpgMgr
{
public:

  /// @brief コンストラクタ
  AtpgMgr();

  /// @brief デストラクタ
  ~AtpgMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取り出す関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TgNetwork を取り出す．
  TpgNetwork&
  _network();

  /// @brief 縮退故障用の2値の故障シミュレータを取り出す．
  Fsim&
  _sa_fsim2();

  /// @brief 縮退故障用の3値の故障シミュレータを返す．
  Fsim&
  _sa_fsim3();

  /// @brief 縮退故障用の故障マネージャを取り出す．
  FaultStatusMgr&
  _sa_fault_mgr();

  /// @brief 縮退故障用のテストベクタのリストを取り出す．
  vector<TestVector>&
  _sa_tv_list();

  /// @brief 遷移故障用の2値の故障シミュレータを取り出す．
  Fsim&
  _td_fsim2();

  /// @brief 遷移故障用の3値の故障シミュレータを返す．
  Fsim&
  _td_fsim3();

  /// @brief 遷移故障用の故障マネージャを取り出す．
  FaultStatusMgr&
  _td_fault_mgr();

  /// @brief 遷移故障用のテストベクタのリストを取り出す．
  vector<TestVector>&
  _td_tv_list();


public:
  //////////////////////////////////////////////////////////////////////
  // ネットワーク設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークが変更された時に呼ばれる関数
  void
  after_set_network();


public:
  //////////////////////////////////////////////////////////////////////
  // イベントハンドラの登録
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークの変更に関するハンドラを登録する．
  void
  reg_network_handler(T1Binder<const TpgNetwork&>* handler);


public:
  //////////////////////////////////////////////////////////////////////
  // タイマー関係の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファイル読み込みに関わる時間を得る．
  USTime
  read_time() const;

  /// @brief DTPG に関わる時間を得る．
  USTime
  dtpg_time() const;

  /// @brief 故障シミュレーションに関わる時間を得る．
  USTime
  fsim_time() const;

  /// @brief SAT に関わる時間を得る．
  USTime
  sat_time() const;

  /// @brief その他の時間を得る．
  USTime
  misc_time() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  TpgNetwork mNetwork;

  // 縮退故障用の2値の故障シミュレータ
  std::unique_ptr<Fsim> mSaFsim2;

  // 縮退故障用の3値の故障シミュレータ
  std::unique_ptr<Fsim> mSaFsim3;

  // 縮退故障用の故障マネージャ
  FaultStatusMgr* mSaFaultMgr;

  // 縮退故障用のテストベクタのリスト
  vector<TestVector> mSaTvList;

  // 遷移故障用の2値の故障シミュレータ
  std::unique_ptr<Fsim> mTdFsim2;

  // 遷移故障用の3値の故障シミュレータ
  std::unique_ptr<Fsim> mTdFsim3;

  // 遷移故障用の故障マネージャ
  FaultStatusMgr* mTdFaultMgr;

  // 遷移故障用のテストベクタのリスト
  vector<TestVector> mTdTvList;

  // ネットワークが変更された時に呼ばれるイベントハンドラ
  T1BindMgr<const TpgNetwork&> mNtwkBindMgr;

  // タイマー
  MStopWatch mTimer;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief TgNetwork を取り出す．
inline
TpgNetwork&
AtpgMgr::_network()
{
  return mNetwork;
}

// @brief 縮退故障用の2値の故障シミュレータを取り出す．
inline
Fsim&
AtpgMgr::_sa_fsim2()
{
  return *mSaFsim2;
}

// @brief 縮退故障用の3値の故障シミュレータを返す．
inline
Fsim&
AtpgMgr::_sa_fsim3()
{
  return *mSaFsim3;
}

// @brief 縮退故障用の故障マネージャを取り出す．
inline
FaultStatusMgr&
AtpgMgr::_sa_fault_mgr()
{
  return *mSaFaultMgr;
}

// @brief 縮退故障用のテストベクタのリストを取り出す．
inline
vector<TestVector>&
AtpgMgr::_sa_tv_list()
{
  return mSaTvList;
}

// @brief 遷移故障用の2値の故障シミュレータを取り出す．
inline
Fsim&
AtpgMgr::_td_fsim2()
{
  return *mTdFsim2;
}

// @brief 遷移故障用の3値の故障シミュレータを返す．
inline
Fsim&
AtpgMgr::_td_fsim3()
{
  return *mTdFsim3;
}

// @brief 遷移故障用の故障マネージャを取り出す．
inline
FaultStatusMgr&
AtpgMgr::_td_fault_mgr()
{
  return *mTdFaultMgr;
}

// @brief 遷移故障用のテストベクタのリストを取り出す．
inline
vector<TestVector>&
AtpgMgr::_td_tv_list()
{
  return mTdTvList;
}

// @brief ネットワークの変更に関するハンドラを登録する．
inline
void
AtpgMgr::reg_network_handler(T1Binder<const TpgNetwork&>* handler)
{
  mNtwkBindMgr.reg_binder(handler);
}

END_NAMESPACE_YM_SATPG

#endif // ATPGMGR_H
