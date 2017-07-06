
/// @file Dtpg2Impl.cc
/// @brief Dtpg2Impl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#define DEBUG_DTPG 0
#define DEBUG_OUT cout

#include "Dtpg2Impl.h"

#include "TpgNetwork.h"
#include "TpgFault.h"

#include "BackTracer.h"
#include "ValMap.h"

#include "ym/RandGen.h"
#include "ym/RandCombiGen.h"
#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

#include "GateLitMap_vid.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
Dtpg2Impl::Dtpg2Impl(const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp,
		     BackTracer& bt,
		     const TpgNetwork& network,
		     const TpgNode* root) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mRoot(root),
  mMarkArray(mNetwork.node_num(), 0U),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap(network.node_num()),
  mDvarMap(network.node_num()),
  mBackTracer(bt),
  mTimerEnable(true)
{
  mNodeList.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());
}

// @brief デストラクタ
Dtpg2Impl::~Dtpg2Impl()
{
}

// @brief XOR制約のための変数リストを作る．
// @return 変数の数を返す．
ymuint
Dtpg2Impl::make_xor_list()
{
  // 対象の故障に関係する外部入力のリストを作る．
  mXorNodeList.clear();
  for (ymuint i = 0; i < mNodeList.size(); ++ i) {
    const TpgNode* node = mNodeList[i];
    if ( node->is_primary_input() ) {
      mXorNodeList.push_back(node);
    }
  }
  mXorNum1 = mXorNodeList.size();
  for (ymuint i = 0; i < mNodeList2.size(); ++ i) {
    const TpgNode* node = mNodeList2[i];
    if ( node->is_ppi() ) {
      mXorNodeList.push_back(node);
    }
  }
  mXorNum2 = mXorNodeList.size();
  return mXorNum2;
}

// @brief 回路の構造を表すCNF式を作る．
// @param[out] stats DTPGの統計情報
//
// このオブジェクトに対しては1回行えばよい．
// というか1回しか行えない．
void
Dtpg2Impl::gen_cnf(DtpgStats& stats)
{
  cnf_begin();

  gen_cnf_base();

  cnf_end(stats);
}

// @brief XOR制約を追加する．
void
Dtpg2Impl::add_xor_constraint(ymuint num,
			      RandGen& rg)
{
  if ( num == mXorNum2 ) {
    // もともとの入力数が少ないときはランダムに最小項を選ぶ．
    for (ymuint idx = 0; idx < mXorNum2; ++ idx) {
      const TpgNode* node = mXorNodeList[idx];
      SatVarId var;
      if ( idx < mXorNum1 ) {
	var = mGvarMap(node);
      }
      else {
	var = mHvarMap(node);
      }
      SatLiteral lit(var);
      if ( rg.int31() % 2 ) {
	lit = ~lit;
      }
      mSolver.add_clause(lit);
      if ( !mSolver.sane() ) {
	break;
      }
    }
    return;
  }

  ymuint k = mXorNum2 / 2;
  RandCombiGen rcg(mXorNum2, k);
  for (ymuint i = 0; i < num; ++ i) {
    // n / 2 個の変数を選び，そのXORを作る．
    rcg.generate(rg);
    vector<SatVarId> var_list(k);
    for (ymuint j = 0; j < k; ++ j) {
      ymuint idx = rcg.elem(j);
      const TpgNode* node = mXorNodeList[idx];
      if ( idx < mXorNum1 ) {
	SatVarId var = mGvarMap(node);
	var_list[j] = var;
      }
      else {
	SatVarId var = mHvarMap(node);
	var_list[j] = var;
      }
    }
    // 1/2 の確率でパリティを決める．
    SatLiteral xor_lit = make_xor(var_list, 0, k);
    if ( !mSolver.sane() ) {
      break;
    }
    if ( rg.int31() % 2 ) {
      mSolver.add_clause(xor_lit);
    }
    else {
      mSolver.add_clause(~xor_lit);
    }
  }
}

SatLiteral
Dtpg2Impl::make_xor(const vector<SatVarId>& var_list,
		    ymuint start,
		    ymuint end)
{
  if ( !mSolver.sane() ) {
    return kSatLiteralX;
  }

  ymuint n = end - start;

  ASSERT_COND( n > 0 );

  if ( n == 1 ) {
    return SatLiteral(var_list[0]);
  }

  // n >= 2
  ymuint n2 = start + (n / 2);
  SatLiteral lit1 = make_xor(var_list, start, n2);
  SatLiteral lit2 = make_xor(var_list, n2, end);
  SatVarId ovar = mSolver.new_variable();
  SatLiteral olit(ovar);
  mSolver.add_xorgate_rel(olit, lit1, lit2);
  return olit;
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
Dtpg2Impl::dtpg(const TpgFault* fault,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  if ( fault->tpg_onode()->ffr_root() != root_node() ) {
    cerr << "Error[Dtpg2Impl::dtpg()]: " << fault << " is not within mFfrRoot's FFR" << endl;
    cerr << " fautl->ffr_root() = " << fault->tpg_onode()->ffr_root()->name() << endl;
    return kB3X;
  }

  SatBool3 ans = solve(fault, vector<SatLiteral>(), nodeval_list, stats);

  return ans;
}

// @brief タイマーをスタートする．
void
Dtpg2Impl::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
Dtpg2Impl::cnf_end(DtpgStats& stats)
{
  USTime time = timer_stop();
  stats.mCnfGenTime += time;
  ++ stats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
Dtpg2Impl::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
Dtpg2Impl::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
void
Dtpg2Impl::gen_cnf_base()
{
  // root の TFO を mNodeList に入れる．
  set_tfo_mark(mRoot);
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    if ( node->is_dff_output() ) {
      mDffList.push_back(node->dff());
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      set_tfo_mark(onode);
    }
  }
  ymuint tfo_num = mNodeList.size();

  // TFO の TFI を mNodeList に入れる．
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      set_tfi_mark(inode);
    }
  }
  ymuint tfi_num = mNodeList.size();

  // TFI に含まれる DFF のさらに TFI を mNodeList2 に入れる．
  set_tfi2_mark(mRoot);
  for (ymuint i = 0; i < mDffList.size(); ++ i) {
    const TpgDff* dff = mDffList[i];
    const TpgNode* node = dff->input();
    mNodeList2.push_back(node);
  }
  for (ymuint rpos = 0; rpos < mNodeList2.size(); ++ rpos) {
    const TpgNode* node = mNodeList2[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      set_tfi2_mark(inode);
    }
  }
  ymuint tfi2_num = mNodeList2.size();

  // TFO の部分に変数を割り当てる．
  for (ymuint rpos = 0; rpos < tfo_num; ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    SatVarId gvar = mSolver.new_variable();
    SatVarId fvar = mSolver.new_variable();
    SatVarId dvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, fvar);
    mDvarMap.set_vid(node, dvar);

#if DEBUG_DTPG
    DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
	      << "fvar(Node#" << node->id() << ") = " << fvar << endl
	      << "dvar(Node#" << node->id() << ") = " << dvar << endl;
#endif
  }

  // TFI の部分に変数を割り当てる．
  for (ymuint rpos = tfo_num; rpos < tfi_num; ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    SatVarId gvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

#if DEBUG_DTPG
    DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
	      << "fvar(Node#" << node->id() << ") = " << gvar << endl;
#endif
  }

  // TFI2 の部分に変数を割り当てる．
  for (ymuint rpos = 0; rpos < tfi2_num; ++ rpos) {
    const TpgNode* node = mNodeList2[rpos];
    SatVarId hvar = mSolver.new_variable();

    mHvarMap.set_vid(node, hvar);

#if DEBUG_DTPG
    DEBUG_OUT << "hvar(Node#" << node->id() << ") = " << hvar << endl;
#endif
  }


  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfi_num; ++ i) {
    const TpgNode* node = mNodeList[i];
    node->make_cnf(mSolver, GateLitMap_vid(node, mGvarMap));

#if DEBUG_DTPG
    DEBUG_OUT << "Node#" << node->id() << ": gvar("
	      << gvar(node) << ") := " << node->gate_type()
	      << "(";
    for (ymuint j = 0; j < node->fanin_num(); ++ j) {
      const TpgNode* inode = node->fanin(j);
      DEBUG_OUT << " " << gvar(inode);
    }
    DEBUG_OUT << ")" << endl;
#endif
  }

  for (ymuint i = 0; i < mDffList.size(); ++ i) {
    const TpgDff* dff = mDffList[i];
    const TpgNode* onode = dff->output();
    const TpgNode* inode = dff->input();
    // DFF の入力の1時刻前の値と出力の値が等しい．
    SatLiteral olit(gvar(onode));
    SatLiteral ilit(hvar(inode));
    mSolver.add_eq_rel(olit, ilit);
  }

  for (ymuint i = 0; i < tfi2_num; ++ i) {
    const TpgNode* node = mNodeList2[i];
    node->make_cnf(mSolver, GateLitMap_vid(node, mHvarMap));

#if DEBUG_DTPG
    DEBUG_OUT << "Node#" << node->id() << ": hvar("
	      << hvar(node) << ") := " << node->gate_type()
	      << "(";
    for (ymuint j = 0; j < node->fanin_num(); ++ j) {
      const TpgNode* inode = node->fanin(j);
      DEBUG_OUT << " " << hvar(inode);
    }
    DEBUG_OUT << ")" << endl;
#endif
  }


  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_num; ++ i) {
    const TpgNode* node = mNodeList[i];
    if ( node != mRoot ) {
      node->make_cnf(mSolver, GateLitMap_vid(node, mFvarMap));

#if DEBUG_DTPG
      DEBUG_OUT << "Node#" << node->id() << ": fvar("
		<< fvar(node) << ") := " << node->gate_type()
		<< "(";
      for (ymuint j = 0; j < node->fanin_num(); ++ j) {
	const TpgNode* inode = node->fanin(j);
	DEBUG_OUT << " " << fvar(inode);
      }

      DEBUG_OUT << ")" << endl;
#endif
    }
    make_dchain_cnf(node);
  }


  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  ymuint no = mOutputList.size();
  vector<SatLiteral> odiff(no);
  for (ymuint i = 0; i < no; ++ i) {
    const TpgNode* node = mOutputList[i];
    SatLiteral dlit(dvar(node));
    odiff[i] = dlit;
  }
  mSolver.add_clause(odiff);

  if ( !mRoot->is_ppo() ) {
    // mRoot の dlit が1でなければならない．
    mSolver.add_clause(SatLiteral(dvar(mRoot)));
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
Dtpg2Impl::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(mGvarMap(node));
  SatLiteral flit(mFvarMap(node));
  SatLiteral dlit(mDvarMap(node));

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

#if DEBUG_DTPG
  DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
	    << glit << " XOR " << flit << endl;
#endif

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

#if DEBUG_DTPG
    DEBUG_OUT << "!dvar(Node#" << node->id() << ") -> "
	      << glit << " = " << flit << endl;
#endif
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

#if DEBUG_DTPG
    DEBUG_OUT << "dvar(Node#" << node->id() << ") -> ";
#endif
    ymuint nfo = node->fanout_num();
    if ( nfo == 1 ) {
      SatLiteral odlit(mDvarMap(node->fanout(0)));
      mSolver.add_clause(~dlit, odlit);

#if DEBUG_DTPG
      DEBUG_OUT << odlit << endl;
#endif
    }
    else {
      vector<SatLiteral> tmp_lits(nfo + 1);
      for (ymuint i = 0; i < nfo; ++ i) {
	const TpgNode* onode = node->fanout(i);
	tmp_lits[i] = SatLiteral(mDvarMap(onode));

#if DEBUG_DTPG
	DEBUG_OUT << " " << mDvarMap(onode);
#endif
      }

#if DEBUG_DTPG
      DEBUG_OUT << endl;
#endif
      tmp_lits[nfo] = ~dlit;
      mSolver.add_clause(tmp_lits);

      const TpgNode* imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	SatLiteral odlit(mDvarMap(imm_dom));
	mSolver.add_clause(~dlit, odlit);

#if DEBUG_DTPG
	DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
		  << odlit << endl;
#endif
      }
    }
  }
}

// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
// @param[in] fault 対象の故障
// @param[out] assign_list 結果の値割り当てリスト
void
Dtpg2Impl::make_ffr_condition(const TpgFault* fault,
			     NodeValList& assign_list)
{
#if DEBUG_DTPG
  DEBUG_OUT << "make_ffr_condition" << endl;
#endif

  // 故障の活性化条件を作る．
  const TpgNode* inode = fault->tpg_inode();
  // 0 縮退故障の時に 1 にする．
  bool val = (fault->val() == 0);
  add_assign(assign_list, inode, 1, val);

  // 1時刻前の値が逆の値である条件を作る．
  add_assign(assign_list, inode, 0, !val);

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    const TpgNode* onode = fault->tpg_onode();
    Val3 nval = onode->nval();
    if ( nval != kValX ) {
      ymuint ni = onode->fanin_num();
      bool val = (nval == kVal1);
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* inode1 = onode->fanin(i);
	if ( inode1 != inode ) {
	  add_assign(assign_list, inode1, 1, val);
	}
      }
    }
  }

  // FFR の根までの伝搬条件を作る．
  for (const TpgNode* node = fault->tpg_onode(); node->fanout_num() == 1;
       node = node->fanout(0)) {
    const TpgNode* fonode = node->fanout(0);
    ymuint ni = fonode->fanin_num();
    if ( ni == 1 ) {
      continue;
    }
    Val3 nval = fonode->nval();
    if ( nval == kValX ) {
      continue;
    }
    bool val = (nval == kVal1);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode1 = fonode->fanin(i);
      if ( inode1 != node ) {
	add_assign(assign_list, inode1, 1, val);
      }
    }
  }

#if DEBUG_DTPG
  DEBUG_OUT << endl;
#endif
}

// @brief NodeValList に追加する．
// @param[in] assign_list 追加するリスト
// @param[in] node 対象のノード
// @param[in] time 時刻 ( 0 or 1 )
// @param[in] val 値
void
Dtpg2Impl::add_assign(NodeValList& assign_list,
		     const TpgNode* node,
		     int time,
		     bool val)
{
  assign_list.add(node, time, val);

#if DEBUG_DTPG
  print_node(DEBUG_OUT, node);
  DEBUG_OUT << "@" << time << ": ";
  if ( val ) {
    DEBUG_OUT << "1" << endl;
  }
  else {
    DEBUG_OUT << "0" << endl;
  }
#endif
}

// @brief 一つの SAT問題を解く．
// @param[in] fault 対象の故障
// @param[in] assumptions 値の決まっている変数のリスト
// @param[in] root 故障位置のノード
// @param[in] output_list 故障に関係のある外部出力のリスト
// @param[out] nodeval_list 結果の値割り当てリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
Dtpg2Impl::solve(const TpgFault* fault,
		const vector<SatLiteral>& assumptions,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  if ( !mSolver.sane() ) {
    return kB3False;
  }

  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  mSolver.get_stats(prev_stats);

  NodeValList assign_list;
  make_ffr_condition(fault, assign_list);

  ymuint n0 = assumptions.size();
  ymuint n = assign_list.size();
  vector<SatLiteral> assumptions1(n + n0);
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    bool inv = !nv.val();
    SatVarId vid = (nv.time() == 0) ? hvar(node) : gvar(node);
    assumptions1[i] = SatLiteral(vid, inv);
  }
  for (ymuint i = 0; i < n0; ++ i) {
    assumptions1[i + n] = assumptions[i];
  }

  vector<SatBool3> model;
  SatBool3 ans = mSolver.solve(assumptions1, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mSolver.get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == kB3True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // バックトレースを行う．
    const VidMap& hvar_map = mHvarMap;
    ValMap val_map(hvar_map, mGvarMap, mFvarMap, model);
    mBackTracer(assign_list, mOutputList, val_map, nodeval_list);

    timer.stop();
    stats.mBackTraceTime += timer.time();

    stats.update_det(sat_stats, time);
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    stats.update_red(sat_stats, time);
  }
  else {
    // ans == kB3X つまりアボート
    stats.update_abort(sat_stats, time);
  }

  return ans;
}

END_NAMESPACE_YM_SATPG
