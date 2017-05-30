
/// @file Dtpg2.cc
/// @brief Dtpg2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg2.h"

#include "Dtpg2Impl.h"
#include "TpgFaultMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "DopList.h"
#include "UntestOp.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"

#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
Dtpg2::Dtpg2(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt) :
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mBackTracer(bt),
  mImpl(nullptr)
{
}

// @brief デストラクタ
Dtpg2::~Dtpg2()
{
}

// @brief テスト生成を行う．
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレーター
// @param[in] network 対象のネットワーク
// @param[in] dop 故障検出時に実行されるファンクター
// @param[in] uop 故障が冗長故障の時に実行されるファンクター
// @param[inout] stats DTPGの統計情報
void
Dtpg2::run(TvMgr& tvmgr,
	   TpgFaultMgr& fmgr,
	   Fsim& fsim,
	   const TpgNetwork& network,
	   bool use_xorsampling,
	   DetectOp& dop,
	   UntestOp& uop,
	   DtpgStats& stats)
{
  ymuint nf = network.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      NodeValList nodeval_list;
      SatBool3 ans = dtpg(tvmgr, fsim, network, fault, use_xorsampling,
			  nodeval_list, stats);
      if ( ans == kB3True ) {
	dop(fault, nodeval_list);
      }
      else if ( ans == kB3False ) {
	uop(fault);
      }
    }
  }
}

// @brief テスト生成を行なう．
// @param[in] network 対象のネットワーク
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
SatBool3
Dtpg2::dtpg(TvMgr& tvmgr,
	    Fsim& fsim,
	    const TpgNetwork& network,
	    const TpgFault* fault,
	    bool use_xorsampling,
	    NodeValList& nodeval_list,
	    DtpgStats& stats)
{
  Dtpg2Impl impl(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
  impl.gen_cnf(stats);

  SatBool3 ans = impl.dtpg(fault, nodeval_list, stats);
  if ( !use_xorsampling || ans != kB3True ) {
    return ans;
  }

  TestVector* tv = tvmgr.new_td_vector();
  tv->set_from_assign_list(nodeval_list);
  ymuint wsa = fsim.td_calc_wsa(tv, false);
  tvmgr.delete_vector(tv);

  ymuint min_wsa = wsa;

#if 0
  double dens = 0.0;
  { // 解の個数の予想のためランダムシミュレーションを行う．
    RandGen randgen;
    ymuint max_pat = 1000;
    TestVector* tv_array[kPvBitLen];
    for (ymuint i = 0; i < kPvBitLen; ++ i) {
      tv_array[i] = tvmgr.new_td_vector();
    }

    fsim.set_skip_all();
    fsim.clear_skip(fault);

    ymuint det_count = 0;
    ymuint wpos = 0;
    for (ymuint pat_num = 0; ; ) {
      if ( pat_num < max_pat ) {
	TestVector* tv = tv_array[wpos];
	tv->set_from_random(randgen);
	fsim.set_pattern(wpos, tv);
	++ pat_num;
	++ wpos;
	if ( wpos < kPvBitLen ) {
	  continue;
	}
      }
      else if ( wpos == 0 ) {
	break;
      }

      ymuint det_count1 = fsim.td_ppsfp();
      det_count += det_count1;

      fsim.clear_patterns();
      wpos = 0;
    }
    dens = static_cast<double>(det_count) / static_cast<double>(max_pat);

    for (ymuint i = 0; i < kPvBitLen; ++ i) {
      tvmgr.delete_vector(tv_array[i]);
    }
  }
#endif

  // 今の故障に関係のある PPI の数を数える．
  ymuint xor_num = impl.make_xor_list();
  if ( xor_num > 30 ) {
    xor_num -= 20;
  }
  else if ( xor_num > 20 ) {
    xor_num -= 10;
  }
  if ( xor_num > 5 ) {
    xor_num = 5;
  }
  // とりあえず xor_num ぐらいの制約をつける．

  RandGen randgen;
  ymuint count = 0;
  ymuint fcount = 0;
  for ( ; count < 50; ++ count) {
    Dtpg2Impl impl2(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
    impl2.gen_cnf(stats);
    impl2.make_xor_list();
    impl2.add_xor_constraint(xor_num, randgen);
    NodeValList nodeval_list1;
    SatBool3 ans = impl2.dtpg(fault, nodeval_list1, stats);
    if ( ans != kB3True ) {
      continue;
    }

    TestVector* tv = tvmgr.new_td_vector();
    tv->set_from_assign_list(nodeval_list1);
    ymuint wsa = fsim.td_calc_wsa(tv, false);
    tvmgr.delete_vector(tv);

    if ( min_wsa > wsa ) {
      min_wsa = wsa;
      nodeval_list = nodeval_list1;
    }
    ++ fcount;
    if ( fcount > 20 ) {
      break;
    }
  }

  return kB3True;
}

END_NAMESPACE_YM_SATPG
