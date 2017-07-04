
/// @file Rtpg.cc
/// @brief Rtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Rtpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス Rtpg
//////////////////////////////////////////////////////////////////////

/// @brief コンストラクタ
/// @param[in] tvmgr TvMgr
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
Rtpg::Rtpg(TvMgr& tvmgr,
	   bool td_mode) :
  mTdMode(td_mode),
  mTvMgr(tvmgr)
{
  mFsim = Fsim::new_Fsim3();

  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    tv_array[i] = mTvMgr.new_vector(!mTdMode);
  }
}

// @brief デストラクタ
Rtpg::~Rtpg()
{
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    mTvMgr.delete_vector(tv_array[i]);
  }
}

// @brief 乱数生成器を初期化する．
// @param[in] seed 乱数の種
void
Rtpg::randgen_init(ymuint32 seed)
{
  mRandGen.init(seed);
}

// @brief ネットワークを設定する．
// @param[in] network 対象のネットワーク
void
Rtpg::set_network(const TpgNetwork& network)
{
  mFsim->set_network(network);

  mDetFaultList.clear();
  mPatternList.clear();
}

// @brief 1セット(kPvBitLen個)のパタンで故障シミュレーションを行う．
// @return 新たに検出された故障数を返す．
ymuint
Rtpg::do_fsim()
{
}

// @brief 検出された故障のリストを返す．
const vector<const TpgFault*>&
Rtpg::det_fault_list() const
{
  return mDetFaultList;
}

// @brief 故障を検出したパタンのリストを返す．
const vector<const TestVector*>&
Rtpg::pattern_list() const
{
  return mPatternList;
}

END_NAMESPACE_YM_SATPG
