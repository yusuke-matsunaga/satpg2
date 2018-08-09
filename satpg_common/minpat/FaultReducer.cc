
/// @file FaultReducer.cc
/// @brief FaultReducer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "FaultReducer.h"
#include "DtpgFFR.h"
#include "UndetChecker.h"
#include "DomChecker.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "NodeValList.h"
#include "MatrixGen.h"
#include "ym/Range.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// オプション文字列をパーズする
// * オプションは
//   <オプションキーワード>[:<オプション値>][, <オプションキーワード>[:<オプション値>]]
//   の形式と仮定する．
// * 空白の除去は行わない．単純に ',' と ':' のみで区切る．
// * 結果を opt_list に格納する．
void
parse_option(const string& option_str,
	     vector<pair<string, string>>& opt_list)
{
  // ',' で区切る
  string tmp_str(option_str);
  for ( ; ; ) {
    string::size_type p = tmp_str.find_first_of(',');
    string tmp = tmp_str.substr(0, p);
    // tmp を ':' で区切る．
    string::size_type q = tmp.find_first_of(':');
    if ( q == string::npos ) {
      // ':' がなかった
      opt_list.push_back(make_pair(tmp, string()));
    }
    else {
      string l_str = tmp.substr(0, q);
      string r_str = tmp.substr(q + 1, string::npos);
      opt_list.push_back(make_pair(l_str, r_str));
    }
    if ( p == string::npos ) {
      // 末尾だったので終わる．
      break;
    }
    // tmp_str を切り詰める．
    tmp_str = tmp_str.substr(p + 1, string::npos);
  }
}

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
FaultReducer::FaultReducer(const TpgNetwork& network,
			   FaultType fault_type) :
  mNetwork(network),
  mFaultType(fault_type),
  mDebug(false)
{
}

// @brief デストラクタ
FaultReducer::~FaultReducer()
{
}

// @breif 内部で用いる SAT ソルバのタイプの設定を行う．
// @param[in] solver_type SATソルバのタイプ
void
FaultReducer::set_solver_type(const SatSolverType& solver_type)
{
  mSolverType = solver_type;
}

// @brief デバッグフラグをセットする．
// @param[in] debug 設定する値 (true/false)
void
FaultReducer::set_debug(bool debug)
{
  mDebug = debug;
}

// @brief 故障の支配関係を調べて故障リストを縮約する．
// @param[inout] fault_list 対象の故障リスト
// @param[in] algorithm アルゴリズム
void
FaultReducer::fault_reduction(vector<const TpgFault*>& fault_list,
			      const string& algorithm)
{
  // algorithm を解析する．
  vector<pair<string, string>> opt_list;
  parse_option(algorithm, opt_list);

  // 初期化する．
  init(fault_list);

  // algorithm に従って縮約を行う．
  for ( auto opt_pair: opt_list ) {
    auto alg = opt_pair.first;
    auto opt = opt_pair.second;
    if ( alg == "red1" ) {
      dom_reduction1();
    }
    else if ( alg == "red2" ) {
      dom_reduction2();
    }
  }

  // mDelMark のついていない故障を fault_list に入れる．
  fault_list.clear();
  for ( auto fault: mFaultList ) {
    if ( !mDelMark[fault->id()] ) {
      fault_list.push_back(fault);
    }
  }
}

// @brief 内部のデータ構造を初期化する．
// @param[in] fi_list 故障情報のリスト
void
FaultReducer::init(const vector<const TpgFault*>& fault_list)
{
  if ( mDebug ) {
    cout << "# of initial faults:                   " << fault_list.size() << endl;
    mTimer.reset();
    mTimer.start();
  }

  // fault_list に含まれていない故障の mDelMark を true にする．
  mDelMark.clear();
  mDelMark.resize(mNetwork.max_fault_id(), true);
  for ( auto fault: fault_list ) {
    mDelMark[fault->id()] = false;
  }

  string just_type;
  vector<TestVector> tv_list;
  RandGen randgen;
  for ( auto& ffr: mNetwork.ffr_list() ) {
    // FFR ごとに検出可能な故障をもとめる．
    DtpgFFR dtpg(mNetwork, mFaultType, ffr, just_type);
    vector<const TpgFault*> tmp_fault_list;
    vector<NodeValList> mand_cond_list;
    for ( auto fault: ffr.fault_list() ) {
      if ( mDelMark[fault->id()] ) {
	// すでに対象外だった．
	continue;
      }
      NodeValList ffr_cond = ffr_propagate_condition(fault, mFaultType);
      vector<SatLiteral> assumptions;
      dtpg.conv_to_assumptions(ffr_cond, assumptions);
      SatBool3 sat_res = dtpg.solve(assumptions);
      if ( sat_res == SatBool3::True ) {
	tmp_fault_list.push_back(fault);
	NodeValList suf_cond = dtpg.get_sufficient_condition(fault);
	// 必要割り当てを求めておく．
	NodeValList mand_cond = dtpg.get_mandatory_condition(fault, suf_cond);
	mand_cond_list.push_back(mand_cond);
	// テストパタンを求めておく．
	suf_cond.merge(ffr_cond);
	TestVector testvect = dtpg.backtrace(fault, suf_cond);
	testvect.fix_x_from_random(randgen);
	tv_list.push_back(testvect);
      }
    }

    // 支配関係を調べ，代表故障のみを残す．
    int nf = tmp_fault_list.size();
    for ( auto i1: Range(nf) ) {
      auto fault1 = tmp_fault_list[i1];
      const NodeValList& mand_cond1 = mand_cond_list[i1];
      // mand_cond1 を否定した節を加える．
      // 制御変数は clit1
      SatVarId cvar1 = dtpg.new_variable();
      SatLiteral clit1(cvar1);
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(mand_cond1.size() + 1);
      tmp_lits.push_back(~clit1);
      for ( auto nv: mand_cond1 ) {
	SatLiteral lit1 = dtpg.conv_to_literal(nv);
	tmp_lits.push_back(~lit1);
      }
      dtpg.add_clause(tmp_lits);

      for ( auto i2: Range(nf) ) {
	if ( i2 == i1 ) {
	  continue;
	}
	auto fault2 = tmp_fault_list[i2];
	if ( mDelMark[fault2->id()] ) {
	  continue;
	}
	const NodeValList& mand_cond2 = mand_cond_list[i2];
	vector<SatLiteral> assumptions;
	assumptions.reserve(mand_cond2.size() + 1);
	dtpg.conv_to_assumptions(mand_cond2, assumptions);
	assumptions.push_back(clit1);
	SatBool3 sat_res = dtpg.check(assumptions);
	if ( sat_res == SatBool3::False ) {
	  // fault2 を検出する条件のもとで fault1 を検出しない
	  // 割り当てが存在しない．→ fault1 は支配されている．
	  mDelMark[fault1->id()] = true;
	  break;
	}
      }
    }
  }

  // 現時点で mDelMark の付いていない故障を mFaultList に入れる．
  // 同時に故障番号をキーにして mFaultList 上の位置を格納する配列を作る．
  mFaultList.clear();
  mFaultList.reserve(fault_list.size());
  mRowIdMap.clear();
  mRowIdMap.resize(mNetwork.max_fault_id(), -1);
  for ( auto fault: fault_list ) {
    if ( !mDelMark[fault->id()] ) {
      mRowIdMap[fault->id()] = mFaultList.size();
      mFaultList.push_back(fault);
    }
  }

  // 被覆行列の生成
  MatrixGen matgen(mFaultList, tv_list, mNetwork, mFaultType);
  mMatrix = matgen.generate();

  if ( mDebug ) {
    mTimer.stop();
    cout << "after FFR dominance reduction:         " << mFaultList.size() << endl;
    cout << "CPU time:                              " << mTimer.time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction1()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  int check_num = 0;
  int success_num = 0;
  for ( auto fault1: mFaultList ) {
    UndetChecker undet_checker(mNetwork, mFaultType, fault1, mSolverType);

    // fault2 が fault1 を支配している時
    // fault2 に含まれる列は必ず fault1 にも含まれなければならない．
    vector<bool> col_mark(mMatrix.col_size(), false);
    int row1 = mRowIdMap[fault1->id()];
    for ( auto col: mMatrix.row_list(row1) ) {
      ASSERT_COND( col >= 0 && col < col_mark.size() );
      col_mark[col] = true;
    }
    for ( auto fault2: mFaultList ) {
      if ( fault2 == fault1 || mDelMark[fault2->id()] ) {
	continue;
      }
      if ( fault1->tpg_onode()->ffr_root() == fault2->tpg_onode()->ffr_root() ) {
	// 同じ FFR ならチェック済み
	continue;
      }
      bool not_covered = false;
      int row2 = mRowIdMap[fault2->id()];
      for ( auto col: mMatrix.row_list(row2) ) {
	ASSERT_COND( col >= 0 && col < col_mark.size() );
	if ( !col_mark[col] ) {
	  not_covered = true;
	  break;
	}
      }
      if ( not_covered ) {
	continue;
      }
      ++ check_num;
      SatBool3 res = undet_checker.check(fault2);
      if ( res == SatBool3::False ) {
	++ success_num;
	// fault2 が検出可能の条件のもとで fault が検出不能となることはない．
	// fault2 が fault を支配している．
	mDelMark[fault1->id()] = true;
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = 0;
    for ( auto fault: mFaultList ) {
      if ( !mDelMark[fault->id()] ) {
	++ n;
      }
    }
    cout << "after semi-global dominance reduction: " << n << endl
	 << "    # of total checks:                 " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "CPU time:                              " << mTimer.time() << endl;
  }
}

// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
void
FaultReducer::dom_reduction2()
{
  if ( mDebug ) {
    mTimer.reset();
    mTimer.start();
  }

  int check_num = 0;
  int dom_num = 0;
  int success_num = 0;
  for ( auto fault1: mFaultList ) {
    if ( mDelMark[fault1->id()] ) {
      continue;
    }
    // fault2 が fault1 を支配している時
    // fault2 に含まれる列は必ず fault1 にも含まれなければならない．
    vector<bool> col_mark(mMatrix.col_size(), false);
    int row1 = mRowIdMap[fault1->id()];
    for ( auto col: mMatrix.row_list(row1) ) {
      col_mark[col] = true;
    }
    for ( auto& ffr2: mNetwork.ffr_list() ) {
      if ( ffr2.root() == fault1->tpg_onode()->ffr_root() ) {
	continue;
      }
      vector<const TpgFault*> fault2_list;
      for ( auto fault2: ffr2.fault_list() ) {
	if ( mDelMark[fault2->id()] ) {
	  continue;
	}
	bool not_covered = false;
	int row2 = mRowIdMap[fault2->id()];
	for ( auto col: mMatrix.row_list(row2) ) {
	  if ( !col_mark[col] ) {
	    not_covered = true;
	    break;
	  }
	}
	if ( !not_covered ) {
	  fault2_list.push_back(fault2);
	}
      }
      if ( fault2_list.empty() ) {
	continue;
      }
      ++ dom_num;
      DomChecker dom_checker(mNetwork, mFaultType, ffr2.root(), fault1, mSolverType);
      for ( auto fault2: fault2_list ) {
	++ check_num;
	SatBool3 res = dom_checker.check_detectable(fault2);
	if ( res == SatBool3::False ) {
	  ++ success_num;
	  // fault2 が検出可能の条件のもとで fault1 が検出不能となることはない．
	  // fault2 が fault1 を支配している．
	  mDelMark[fault1->id()] = true;
	  break;
	}
      }
      if ( mDelMark[fault1->id()] ) {
	break;
      }
    }
  }

  if ( mDebug ) {
    mTimer.stop();
    int n = 0;
    for ( auto fault: mFaultList ) {
      if ( !mDelMark[fault->id()] ) {
	++ n;
      }
    }
    cout << "after global dominance reduction:      " << n << endl;
    cout << "    # of total checkes:                " << check_num << endl
	 << "    # of total successes:              " << success_num << endl
	 << "    # of DomCheckers:                  " << dom_num << endl
	 << "CPU time:                              " << mTimer.time() << endl;
  }
}

END_NAMESPACE_YM_SATPG
