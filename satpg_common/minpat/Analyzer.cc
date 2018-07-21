
/// @file Analyzer.cc
/// @brief Analyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Analyzer.h"
#include "FaultInfo.h"
#include "DtpgFFR.h"
#include "TpgFFR.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
Analyzer::Analyzer(const TpgNetwork& network,
		   FaultType fault_type) :
  mNetwork(network),
  mFaultType(fault_type)
{
}

// @brief デストラクタ
Analyzer::~Analyzer()
{
}

// @brief 初期化する
void
Analyzer::init()
{
  // まず検出可能故障を見つける．
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;
  string just_type;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    DtpgFFR dtpg(sat_type, sat_option, sat_outp, mFaultType, just_type, mNetwork, ffr);
    vector<FaultInfo*> tmp_fault_list;
    tmp_fault_list.reserve(ffr.fault_list().num());
    for ( auto fault: ffr.fault_list() ) {
      NodeValList ffr_cond = dtpg.make_ffr_condition(fault);
      vector<SatLiteral> assumptions;
      dtpg.conv_to_assumptions(ffr_cond, assumptions);
      vector<SatBool3> model;
      SatBool3 sat_res = dtpg.solve(assumptions, model);
      if ( sat_res == SatBool3::True ) {
	// 検出可能
	// 十分条件を求める．
	auto tmp_cond = dtpg.get_sufficient_condition(fault, model);
	// 必要条件を求める．
	NodeValList mand_cond;
	NodeValList suff_cond;
	for ( auto nv: tmp_cond ) {
	  // nv を否定した条件を加えて解があるか調べる．
	  SatLiteral lit1 = dtpg.conv_to_literal(nv);
	  vector<SatLiteral> assumptions1(assumptions);
	  assumptions1.push_back(~lit1);
	  vector<SatBool3> dummy;
	  if ( dtpg.solve(assumptions1, dummy) == SatBool3::False ) {
	    // 充足不能なので nv は必要割当
	    mand_cond.add(nv);
	  }
	  else {
	    // 充足可能なので nv は十分割り当て
	    suff_cond.add(nv);
	  }
	}
	if ( suff_cond.size() > 0 ) {
	  // 残った十分割り当ての否定を追加してSATを解く
	  SatVarId cvar1 = dtog.solver().new_variable();
	  SatLiteral clit1(cvar1);
	  vector<SatLiteral> tmp_lits;
	  tmp_lits.reserve(suff_cond.size() + 1);
	  tmp_lits.push_back(~clit1);
	  for ( auto nv: suff_cond ) {
	    SatLiteral lit = dtpg.conv_to_literal(nv);
	    tmp_lits.push_back(~lit);
	  }
	  vector<SatLiteral> assumptions1(assumptions);
	  vector<SatBool3> model;
	  if ( dtpg.solve(assumptions1, model) == SatBool3::False ) {
	  }
	}
	auto fi = new FaultInfo(fault, ffr_cond, suff_cond, mand_cond);
	tmp_fault_list.push_back(fi);
      }
    }

    int nf = tmp_fault_list.size();
    vector<pair<FaultInfo*, FaultInfo*>> tmp_conflict_list;
    vector<pair<FaultInfo*, FaultInfo*>> tmp_compatible_list;
    for ( auto i1: Range(nf - 1) ) {
      FaultInfo* fi1 = tmp_fault_list[i1];
      if ( fi1->is_dominated() ) {
	continue;
      }
      const TpgFault* fault1 = fi1->fault();
      auto suff_cond1 = fi1->sufficient_cond();
      for ( auto i2: Range(i1 + 1, nf) ) {
	FaultInfo* fi2 = tmp_fault_list[i2];
	if ( fi2->is_dominated() ) {
	  continue;
	}
	const TpgFault* fault2 = fi2->fault();
	auto suff_cond2 = fi2->sufficient_cond();
	int res = compare(suff_cond1, suff_cond2);
	if ( res == -1 ) {
	  // ２つの故障は衝突している．
	  tmp_conflict_list.push_back(make_pair(fi1, fi2));
	}
	else if ( res & 1 ) { // res == 1 or res == 3
	  // fault1 が fault2 を支配している．
	  fi2->set_dominated();
	}
	else if ( res & 2 ) {
	  // fault2 が fault1 を支配している．
	  fi1->set_dominated();
	}
	else {
	  // fault1 と fault2 は両立する可能性がある．
	  tmp_compatible_list.push_back(make_pair(fi1, fi2));
	}
      }
    }
    // mark のついた故障をドロップする．
    fault_list.clear();
    fault_list.reserve(nf);
    for ( auto fi: tmp_fault_list ) {
      if ( !fi->is_dominated() ) {
	fault_list.push_back(fi);
      }
    }

    // 衝突リストを作る．
    int n_confl = 0;
    int n_compat = 0;
    for ( auto fi_pair: tmp_conflict_list ) {
      FaultInfo* fi1 = fi_pair.first;
      FaultInfo* fi2 = fi_pair.second;
      if ( !fi1->is_dominated() && !fi2->is_dominated() ) {
	fi1->add_conflict(fi2);
	fi2->add_conflict(fi1);
	++ n_confl;
      }
    }

    // 両立関係を調べて両立リストを作る．
    for ( auto fi_pair: tmp_compatible_list ) {
      FaultInfo* fi1 = fi_pair.first;
      FaultInfo* fi2 = fi_pair.second;
      if ( fi1->is_dominated() || fi2->is_dominated() ) {
	continue;
      }
      auto suff_cond1 = fi1->sufficient_cond();
      auto suff_cond2 = fi2->sufficient_cond();
      vector<SatLiteral> assumptions;
      dtpg.conv_to_assumptions(suff_cond1, assumptions);
      dtpg.conv_to_assumptions(suff_cond2, assumptions);
      vector<SatBool3> model;
      SatBool3 sat_res = dtpg.solve(assumptions, model);
      if ( sat_res == SatBool3::True ) {
	// 両立している．
	fi1->add_compatible(fi2);
	fi2->add_compatible(fi1);
	++ n_compat;
      }
      else {
	// 衝突している．
	fi1->add_conflict(fi2);
	fi2->add_conflict(fi1);
	++ n_confl;
      }
    }
  }
}

END_NAMESPACE_YM_SATPG
