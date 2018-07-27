
/// @file DtpgFFR.cc
/// @brief DtpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgFFR.h"

#include "TpgFault.h"
#include "TpgFFR.h"
#include "NodeValList.h"
#include "TestVector.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgFFR::DtpgFFR(const string& sat_type,
		 const string& sat_option,
		 ostream* sat_outp,
		 FaultType fault_type,
		 const string& just_type,
		 const TpgNetwork& network,
		 const TpgFFR& ffr) :
  DtpgEngine(sat_type, sat_option, sat_outp, fault_type, just_type, network, ffr.root())
{
  cnf_begin();

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件(正確には mRoot から外部出力までの故障の伝搬条件)
  //////////////////////////////////////////////////////////////////////
  {
    vector<SatLiteral> odiff;
    odiff.reserve(output_list().size());
    for ( auto node: output_list() ) {
      SatLiteral dlit(dvar(node));
      odiff.push_back(dlit);
    }
    solver().add_clause(odiff);

    if ( !root_node()->is_ppo() ) {
      // root_node() の dlit が1でなければならない．
      SatLiteral dlit0(dvar(root_node()));
      solver().add_clause(dlit0);
    }
  }

  cnf_end();
}

// @brief デストラクタ
DtpgFFR::~DtpgFFR()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
DtpgResult
DtpgFFR::gen_pattern(const TpgFault* fault)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  vector<SatBool3> model;
  SatBool3 sat_res = solve(assumptions, model);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition(fault, model);
    suf_cond.merge(ffr_cond);
    TestVector testvect = backtrace(fault, suf_cond, model);
    return DtpgResult(testvect);
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[in] k 求めるベクタ数
// @param[out] tv_list ベクタを入れるリスト
// @return 結果を返す．
//
// * tv_list[0] は DtpgResult のベクタと同じ．
// * tv_list の要素数が k より少ない場合がある．
DtpgResult
DtpgFFR::gen_k_patterns(const TpgFault* fault,
			int k,
			vector<TestVector>& tv_list)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = ffr_propagate_condition(fault, fault_type());

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  vector<SatBool3> model;
  SatBool3 sat_res = solve(assumptions, model);
  if ( sat_res == SatBool3::True ) {
    NodeValList suf_cond = get_sufficient_condition(fault, model);
    suf_cond.merge(ffr_cond);
    TestVector testvect = backtrace(fault, suf_cond, model);
    DtpgResult ans(testvect);
    tv_list.clear();
    tv_list.push_back(testvect);

    if ( k > 1 ) {
      // ここで追加するCNF式を制御する変数
      SatVarId cvar = solver().new_variable();
      SatLiteral clit(cvar);

      for ( auto i: Range(k - 1) ) {
	// testvect の否定を表すCNF式を追加する．
	vector<SatLiteral> tmp_lits;
	tmp_lits.push_back(~clit);
	for ( auto pos: Range(testvect.ppi_num()) ) {
	  Val3 val = testvect.ppi_val(pos);
	  if ( val == Val3::_X ) {
	    continue;
	  }
	  const TpgNode* node = network().ppi(pos);
	  SatLiteral lit(gvar(node));
	  if ( val == Val3::_0 ) {
	    tmp_lits.push_back(lit);
	  }
	  else {
	    tmp_lits.push_back(~lit);
	  }
	}
	solver().add_clause(tmp_lits);
	vector<SatLiteral> assumptions1(assumptions);
	assumptions1.push_back(clit);
	vector<SatBool3> model;
	SatBool3 sat_res = solve(assumptions1, model);
	if ( sat_res == SatBool3::True ) {
	  NodeValList suf_cond = get_sufficient_condition(fault, model);
	  suf_cond.merge(ffr_cond);
	  testvect = backtrace(fault, suf_cond, model);
	  tv_list.push_back(testvect);
	}
	else {
	  break;
	}
      }
    }

    return ans;
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

END_NAMESPACE_YM_SATPG
