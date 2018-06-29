
/// @file MinPatMgr.cc
/// @brief MinPatMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス MinPatMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
MinPatMgr::MinPatMgr(const vector<const TpgFault*>& fault_list,
		     const vector<TestVector>& tv_list,
		     const TpgNetwork& network,
		     FaultType fault_type) :
  mNetwork(network),
  mFaultList(fault_list),
  mOrigTvList(tv_list),
  mFsim(Fsim::new_Fsim3(network, fault_type))
{
  gen_covering_matrix();
  gen_conflict_list();
}

// @brief デストラクタ
MinPatMgr::~MinPatMgr()
{
}

// @brief 故障シミュレーションを行い被覆表を作る．
void
MinPatMgr::gen_covering_matrix()
{
  mElemList.clear();
  int wpos = 0;
  mFsim->clear_patterns();
  int tv_base = 0;
  for ( auto tv: mOrigTvList ) {
    mFsim->set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      int ndet = mFsim->ppsfp();
      for ( auto i: Range(ndet) ) {
	const TpgFault* fault = mFsim->det_fault(i);
	PackedVal dbits = mFsim->det_fault_pat(i);
	int fid = fault->id();
	for ( auto bit: Range(kPvBitLen) ) {
	  if ( dbits & (1UL << bit) ) {
	    int tvid = tv_base + bit;
	    // (fid, tvid) を記録
	    mElemList.push_back(make_pair(fid, tvid));
	  }
	}
      }
      mFsim->clear_patterns();
      tv_base += kPvBitLen;
    }
  }
  if ( wpos > 0 ) {
    int ndet = mFsim->ppsfp();
    for ( auto i: Range(ndet) ) {
      const TpgFault* fault = mFsim->det_fault(i);
      PackedVal dbits = mFsim->det_fault_pat(i);
      int fid = fault->id();
      for ( auto bit: Range(kPvBitLen) ) {
	if ( dbits & (1UL << bit) ) {
	  int tvid = tv_base + bit;
	  // (fid, tvid) を記録
	  mElemList.push_back(make_pair(fid, tvid));
	}
      }
    }
  }
}

// @brief テストパタンの衝突リストを作る．
void
MinPatMgr::gen_conflict_list()
{
  if ( mOrigTvList.empty() ) {
    return;
  }

  int vs = mOrigTvList[0].vector_size();
  mConflictPairList.resize(vs * 2);
  for ( auto bit: Range(vs) ) {
    int tvid = 0;
    for ( auto tvid: Range(mOrigTvList.size()) ) {
      TestVector tv = mOrigTvList[tvid];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	mConflictPairList[bit * 2 + 0].push_back(tvid);
      }
      else {
	mConflictPairList[bit * 2 + 1].push_back(tvid);
      }
    }
  }
}

END_NAMESPACE_YM_SATPG
