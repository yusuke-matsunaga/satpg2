#ifndef RTPG_H
#define RTPG_H

/// @file Rtpg.h
/// @brief Rtpg のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Rtpg Rtpg.h "Rtpg.h"
/// @brief ランダムパタンによる故障シミュレーションでテストパタンを求めるクラス
//////////////////////////////////////////////////////////////////////
class Rtpg
{
public:

  /// @brief コンストラクタ
  /// @param[in] tvmgr TvMgr
  /// @param[in] td_mode 遷移故障モードの時 true にするフラグ
  Rtpg(TvMgr& tvmgr,
       bool td_mode);

  /// @brief デストラクタ
  ~Rtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 乱数生成器を初期化する．
  /// @param[in] seed 乱数の種
  void
  randgen_init(ymuint32 seed);

  /// @brief ネットワークを設定する．
  /// @param[in] network 対象のネットワーク
  void
  set_network(const TpgNetwork& network);

  /// @brief 1セット(kPvBitLen個)のパタンで故障シミュレーションを行う．
  /// @return 新たに検出された故障数を返す．
  ymuint
  do_fsim();

  /// @brief 検出された故障のリストを返す．
  const vector<const TpgFault*>&
  det_fault_list() const;

  /// @brief 故障を検出したパタンのリストを返す．
  const vector<const TestVector*>&
  pattern_list() const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 遷移故障モードの時 true を返す．
  bool
  td_mode() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 遷移故障モードの時 true にするフラグ
  bool mTdMode;

  // 乱数発生器
  RandGen mRandGen;

  // TvMgr
  TvMgr& mTvMgr;

  // 故障シミュレータ
  Fsim* mFsim;

  // 現在のパタンを入れておくバッファ
  TestVector* tv_array[kPvBitLen];

  // 検出された故障のリスト
  vector<const TpgFault*> mDetFaultList;

  // 故障を検出したパタンのリスト
  vector<const TestVector*> mPatternList;

};

END_NAMESPACE_YM_SATPG

#endif // RTPG_H
