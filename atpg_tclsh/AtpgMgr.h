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
#include "ym/ym_cell.h"
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

  /// @brief TpgFaultMgr を取り出す．
  TpgFaultMgr&
  _fault_mgr();

  /// @brief 2値の故障シミュレータを取り出す．
  Fsim&
  _fsim2();

  /// @brief 3値の故障シミュレータを返す．
  Fsim&
  _fsim3();

  /// @brief TvMgr を取り出す．
  TvMgr&
  _tv_mgr();

  /// @brief テストベクタのリストを取り出す．
  vector<const TestVector*>&
  _sa_tv_list();

  /// @brief テストベクタのリストを取り出す．
  vector<const TestVector*>&
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

  // 故障マネージャ
  TpgFaultMgr* mFaultMgr;

  // 2値の故障シミュレータ
  Fsim* mFsim2;

  // 3値の故障シミュレータ
  Fsim* mFsim3;

  // テストベクタを管理するオブジェクト
  TvMgr* mTvMgr;

  // テストベクタのリスト
  vector<const TestVector*> mSaTvList;

  // テストベクタのリスト
  vector<const TestVector*> mTdTvList;

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

// @brief FaultMgr を取り出す．
inline
TpgFaultMgr&
AtpgMgr::_fault_mgr()
{
  return *mFaultMgr;
}

// @brief 2値の故障シミュレータを取り出す．
inline
Fsim&
AtpgMgr::_fsim2()
{
  return *mFsim2;
}

// @brief 3値の故障シミュレータを返す．
inline
Fsim&
AtpgMgr::_fsim3()
{
  return *mFsim3;
}

// @brief TvMgr を取り出す．
inline
TvMgr&
AtpgMgr::_tv_mgr()
{
  return *mTvMgr;
}

// @brief テストベクタのリストを取り出す．
inline
vector<const TestVector*>&
AtpgMgr::_sa_tv_list()
{
  return mSaTvList;
}

// @brief テストベクタのリストを取り出す．
inline
vector<const TestVector*>&
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
