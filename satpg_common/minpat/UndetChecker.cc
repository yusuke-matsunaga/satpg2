
/// @file UndetChecker.cc
/// @brief UndetChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "UndetChecker.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "GateType.h"
#include "GateEnc.h"
#include "FaultyGateEnc.h"
#include "Val3.h"
#include "NodeValList.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"
#include "ym/Range.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif

END_NONAMESPACE


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] network 対象のネットワーク
// @param[in] fault 故障伝搬をさせない故障
UndetChecker::UndetChecker(const string& sat_type,
			   const string& sat_option,
			   ostream* sat_outp,
			   FaultType fault_type,
			   const TpgNetwork& network,
			   const TpgFault* fault) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mFault(fault),
  mMarkArray(mNetwork.node_num(), 0U),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap(network.node_num()),
  mTimerEnable(true)
{
  mRoot = fault->tpg_onode();
  mTfiList.reserve(network.node_num());
  mPrevTfiList.reserve(network.node_num());
  mTfoList.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());
  mFvarMap.init(network.node_num());

  // 変数割り当て
  prepare_vars();

  // 正常回路の CNF を生成
  gen_good_cnf();

  // 故障回路の CNF を生成
  gen_faulty_cnf();

  //////////////////////////////////////////////////////////////////////
  // 故障の非検出条件
  // 全ての出力の正常値と故障値が等しい
  //////////////////////////////////////////////////////////////////////
  for ( auto node: mOutputList ) {
    SatLiteral glit(gvar(node));
    SatLiteral flit(fvar(node));
    mSolver.add_clause( glit, ~flit);
    mSolver.add_clause(~glit,  flit);
  }
}

// @brief デストラクタ
UndetChecker::~UndetChecker()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 故障
// @return 結果を返す．
SatBool3
UndetChecker::check_detectable(const TpgFault* fault)
{
  NodeValList ffr_cond = make_ffr_condition(fault);

  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  vector<SatBool3> model;
  SatBool3 res = solve(assumptions, model);

  return res;
}

// @brief タイマーをスタートする．
void
UndetChecker::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
UndetChecker::cnf_end()
{
  USTime time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
UndetChecker::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
UndetChecker::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

// @brief 対象の部分回路の関係を表す変数を用意する．
void
UndetChecker::prepare_vars()
{
  // root[pos] の TFO を mTfoList に入れる．
  set_tfo_mark(mRoot);
  for ( int rpos = 0; rpos < mTfoList.size(); ++ rpos ) {
    const TpgNode* node = mTfoList[rpos];
    for ( auto onode: node->fanout_list() ) {
      set_tfo_mark(onode);
    }
  }

  for ( int rpos = 0; rpos < mTfiList.size(); ++ rpos ) {
    const TpgNode* node = mTfiList[rpos];
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }

  // TFI に含まれる DFF のさらに TFI を mPrevTfiList に入れる．
  if ( mFaultType == FaultType::TransitionDelay ) {
    if ( mRoot->is_dff_output() ) {
      mDffList.push_back(mRoot->dff());
    }
    for ( auto dff: mDffList ) {
      const TpgNode* node = dff->input();
      mPrevTfiList.push_back(node);
    }
    set_prev_tfi_mark(mRoot);
    for ( int rpos = 0; rpos < mPrevTfiList.size(); ++ rpos) {
      const TpgNode* node = mPrevTfiList[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_prev_tfi_mark(inode);
      }
    }
  }

  // TFI の部分に変数を割り当てる．
  for ( auto node: mTfiList ) {
    SatVarId gvar = mSolver.new_variable();

    set_gvar(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl;
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    SatVarId gvar = mSolver.new_variable();
    SatVarId fvar = mSolver.new_variable();

    set_gvar(node, gvar);
    set_fvar(node, fvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << mGvarMap(node) << endl
		<< "fvar(Node#" << node->id() << ") = " << fvar << endl;
    }
  }


  // prev TFI の部分に変数を割り当てる．
  for ( auto node: mPrevTfiList ) {
    SatVarId hvar = mSolver.new_variable();

    set_hvar(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar(Node#" << node->id() << ") = " << hvar << endl;
    }
  }
}

// @brief 対象の部分回路の正常値の関係を表す CNF 式を作る．
void
UndetChecker::gen_good_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc(mSolver, mGvarMap);
  for ( auto node: mTfiList ) {
    gval_enc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << "Node#" << node->id() << ": gvar("
		<< gvar(node) << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << gvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }

  for ( auto dff: mDffList ) {
    const TpgNode* onode = dff->output();
    const TpgNode* inode = dff->input();
    // DFF の入力の1時刻前の値と出力の値が等しい．
    SatLiteral olit(gvar(onode));
    SatLiteral ilit(hvar(inode));
    mSolver.add_eq_rel(olit, ilit);
  }

  GateEnc hval_enc(mSolver, mHvarMap);
  for ( auto node: mPrevTfiList ) {
    hval_enc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << "Node#" << node->id() << ": hvar("
		<< hvar(node) << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << hvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 対象の部分回路の故障値の関係を表す CNF 式を作る．
void
UndetChecker::gen_faulty_cnf()
{
  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  FaultyGateEnc fenc2(mSolver, mFvarMap, mFault);
  fenc2.make_cnf();

  GateEnc fval_enc(mSolver, mFvarMap);
  for ( auto node: mTfoList ) {
    if ( node == mRoot ) continue;
    fval_enc.make_cnf(node);

    if ( debug_dtpg ) {
      DEBUG_OUT << "Node#" << node->id() << ": fvar("
		<< fvar(node) << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << fvar(inode);
      }

      DEBUG_OUT << ")" << endl;
    }
    //make_dchain_cnf(node);
  }
}

// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
// @param[in] fault 対象の故障
// @param[out] assign_list 結果の値割り当てリスト
NodeValList
UndetChecker::make_ffr_condition(const TpgFault* fault)
{
  if ( debug_dtpg ) {
    DEBUG_OUT << "make_ffr_condition" << endl;
  }

  NodeValList assign_list;

  // 故障の活性化条件を作る．
  const TpgNode* inode = fault->tpg_inode();
  // 0 縮退故障の時に 1 にする．
  bool val = (fault->val() == 0);
  add_assign(assign_list, inode, 1, val);

  if ( mFaultType == FaultType::TransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    add_assign(assign_list, inode, 0, !val);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    const TpgNode* onode = fault->tpg_onode();
    add_side_input(onode, fault->tpg_pos(), assign_list);
  }

  // FFR の根までの伝搬条件を作る．
  for ( const TpgNode* node = fault->tpg_onode(); node->fanout_num() == 1;
	node = node->fanout_list()[0]) {
    const TpgNode* fonode = node->fanout_list()[0];
    add_side_input(fonode, node, assign_list);
  }

  if ( debug_dtpg ) {
    DEBUG_OUT << endl;
  }

  return assign_list;
}

// @brief NodeValList に追加する．
// @param[in] assign_list 追加するリスト
// @param[in] node 対象のノード
// @param[in] time 時刻 ( 0 or 1 )
// @param[in] val 値
void
UndetChecker::add_assign(NodeValList& assign_list,
		       const TpgNode* node,
		       int time,
		       bool val)
{
  assign_list.add(node, time, val);

  if ( debug_dtpg ) {
    print_node(DEBUG_OUT, mNetwork, node);
    DEBUG_OUT << "@" << time << ": ";
    if ( val ) {
      DEBUG_OUT << "1" << endl;
    }
    else {
      DEBUG_OUT << "0" << endl;
    }
  }
}

// @brief 値割り当てをリテラルに変換する．
SatLiteral
UndetChecker::conv_to_literal(NodeVal node_val)
{
  const TpgNode* node = node_val.node();
  bool inv = !node_val.val(); // 0 の時が inv = true
  SatVarId vid;
  if ( node_val.time() == 0 ) {
    make_prev_cnf(node);
    vid = hvar(node);
  }
  else {
    make_good_cnf(node);
    vid = gvar(node);
  }
  return SatLiteral(vid, inv);
}

// @brief 値割り当てをリテラルのリストに変換する．
// @param[in] assign_list 値の割り当てリスト
// @param[out] assumptions 変換したリテラルを追加するリスト
void
UndetChecker::conv_to_assumptions(const NodeValList& assign_list,
				  vector<SatLiteral>& assumptions)
{
  int n0 = assumptions.size();
  int n = assign_list.size();
  assumptions.reserve(n + n0);
  for ( auto nv: assign_list ) {
    auto lit = conv_to_literal(nv);
    assumptions.push_back(lit);
  }
}

// @brief 一つの SAT問題を解く．
// @param[in] assumptions 値の決まっている変数のリスト
// @param[out] model SAT モデル
// @return 結果を返す．
SatBool3
UndetChecker::solve(const vector<SatLiteral>& assumptions,
		    vector<SatBool3>& model)
{
  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  mSolver.get_stats(prev_stats);

  SatBool3 ans = mSolver.solve(assumptions, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mSolver.get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == SatBool3::True ) {
    // パタンが求まった．
    mStats.update_det(sat_stats, time);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_red(sat_stats, time);
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(sat_stats, time);
  }

  return ans;
}

// @brief side-input の割り当てを得る．
// @param[in] node ノード
// @param[in] ipos 対象のファンイン番号
// @param[out] nodeval_list 割り当てリスト
//
// * node が非制御値を持たない場合は nodeval_list は空になる．
void
UndetChecker::add_side_input(const TpgNode* node,
			   int ipos,
			   NodeValList& nodeval_list)
{
  Val3 nval = node->nval();
  if ( nval != Val3::_X ) {
    bool val = (nval == Val3::_1);
    for ( auto ipos1: Range(node->fanin_num()) ) {
      if ( ipos1 != ipos ) {
	auto inode1 = node->fanin(ipos1);
	add_assign(nodeval_list, inode1, 1, val);
      }
    }
  }
}

// @brief side-input の割り当てを得る．
// @param[in] node ノード
// @param[in] inode 対象のファンイン
// @param[out] nodeval_list 割り当てリスト
//
// * node が非制御値を持たない場合は nodeval_list は空になる．
// * 上の関数との違いは同じノードが重複してファンインとなっている場合
void
UndetChecker::add_side_input(const TpgNode* node,
			     const TpgNode* inode,
			     NodeValList& nodeval_list)
{
  int ni = node->fanin_num();
  if ( ni == 1 ) {
    return;
  }
  Val3 nval = node->nval();
  if ( nval == Val3::_X ) {
    return;
  }
  bool val = (nval == Val3::_1);
  for ( auto inode1: node->fanin_list() ) {
    if ( inode1 != inode ) {
      add_assign(nodeval_list, inode1, 1, val);
    }
  }
}

// @brief 正常回路の CNF を作る．
void
UndetChecker::make_good_cnf(const TpgNode* node)
{
  if ( has_gvar(node) ) {
    return;
  }
  for ( auto inode: node->fanin_list() ) {
    make_good_cnf(inode);
  }

  SatVarId var = mSolver.new_variable();
  set_gvar(node, var);

  GateEnc gval_enc(mSolver, mGvarMap);
  gval_enc.make_cnf(node);
}

// @brief 1時刻前の正常回路の CNF を作る．
void
UndetChecker::make_prev_cnf(const TpgNode* node)
{
  if ( has_hvar(node) ) {
    return;
  }
  for ( auto inode: node->fanin_list() ) {
    make_prev_cnf(inode);
  }

  SatVarId var = mSolver.new_variable();
  set_hvar(node, var);

  GateEnc hval_enc(mSolver, mHvarMap);
  hval_enc.make_cnf(node);
}

END_NAMESPACE_YM_SATPG
