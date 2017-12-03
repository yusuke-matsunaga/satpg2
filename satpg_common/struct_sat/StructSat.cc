
/// @file StructSat.cc
/// @brief StructSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "StructSat.h"
#include "FoCone.h"
#include "NodeValList.h"

#include "TpgFault.h"
#include "TpgNode.h"
#include "TpgDff.h"

#include "GateLitMap_vid.h"
#include "GateLitMap_vid2.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

const ymuint debug_make_vars = 1U;
const ymuint debug_make_node_cnf = 2U;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス StructSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_node_id ノード番号の最大値
// @param[in] fault_type 故障の種類
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
StructSat::StructSat(ymuint max_node_id,
		     FaultType fault_type,
		     const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp) :
  mFaultType(fault_type),
  mSolver(sat_type, sat_option, sat_outp),
  mMaxId(max_node_id),
  mMark(max_node_id, false)
{
  for (ymuint i = 0; i < 2; ++ i) {
    mVarMap[i].init(max_node_id);
  }
  mDebugFlag = 0;
}

// @brief デストラクタ
StructSat::~StructSat()
{
  for (ymuint i = 0; i < mFoConeList.size(); ++ i) {
    FoCone* cone = mFoConeList[i];
    delete cone;
  }
}

// @brief fault cone を追加する．
// @param[in] fnode 故障のあるノード
// @param[in] detect 故障を検出する時に true にするフラグ
const FoCone*
StructSat::add_focone(const TpgNode* fnode,
		      bool detect)
{
  return add_focone(fnode, nullptr, detect);
}

// @brief fault cone を追加する．
// @param[in] fnode 故障のあるノード
// @param[in] bnode ブロックノード
// @param[in] detect 故障を検出する時に true にするフラグ
const FoCone*
StructSat::add_focone(const TpgNode* fnode,
		      const TpgNode* bnode,
		      bool detect)
{
  FoCone* focone = new FoCone(*this, fnode, bnode, detect);
  mFoConeList.push_back(focone);

  if ( fault_type() == kFtTransitionDelay ) {
    add_prev_node(fnode);
  }
  make_tfi_list(focone->tfo_node_list());

  return focone;
}

// @brief 故障の検出条件を割当リストに追加する．
// @param[in] fault 故障
// @param[out] assignment 割当リスト
void
StructSat::add_fault_condition(const TpgFault* fault,
			       NodeValList& assignment)
{
  // 故障の活性化条件
  const TpgNode* inode = fault->tpg_inode();
  // 0縮退故障の時 1にする．
  bool val = (fault->val() == 0);
  assignment.add(inode, 1, val);

  if ( fault_type() == kFtTransitionDelay ) {
    // 1時刻前の値が逆の値である条件を作る．
    assignment.add(inode, 0, !val);
  }

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    // 故障の伝搬条件
    const TpgNode* onode = fault->tpg_onode();
    Val3 nval = onode->nval();
    if ( nval != kValX ) {
      ymuint ni = onode->fanin_num();
      bool val = (nval == kVal1);
      // inode -> onode の伝搬条件
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* inode1 = onode->fanin(i);
	if ( inode1 == inode ) {
	  continue;
	}
	assignment.add(inode1, 1, val);
      }
    }
  }
}

// @brief FFR内の故障の伝搬条件を割当リストに追加する．
// @param[in] root_node FFRの根のノード
// @param[in] fault 故障
// @param[out] assignment 割当リスト
void
StructSat::add_ffr_condition(const TpgNode* root_node,
			     const TpgFault* fault,
			     NodeValList& assignment)
{
  // ノードに対する故障の伝搬条件
  add_fault_condition(fault, assignment);

  // FFR の根までの伝搬条件
  for (const TpgNode* node = fault->tpg_onode(); node != root_node;
       node = node->fanout(0)) {
    ASSERT_COND( node->fanout_num() == 1 );
    const TpgNode* onode = node->fanout(0);
    ymuint ni = onode->fanin_num();
    if ( ni == 1 ) {
      // サイドインプットがなければスキップ
      continue;
    }
    Val3 nval = onode->nval();
    if ( nval == kValX ) {
      // 非制御値がなければスキップ
      // 実際には制御値がない時にスキップ
      // 例えば XOR ゲートの場合がそれにあたる．
      continue;
    }
    // サイドインプットの値を非制御値にする．
    bool val = (nval == kVal1);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = onode->fanin(i);
      if ( inode == node ) {
	continue;
      }
      assignment.add(inode, 1, val);
    }
  }
}

// @brief 割当リストに従って値を固定する．
// @param[in] assignment 割当リスト
void
StructSat::add_assignments(const NodeValList& assignment)
{
  ymuint n = assignment.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assignment[i];
    SatLiteral alit = nv_to_lit(nv);
    mSolver.add_clause(alit);
  }
}

// @brief 割当リストの否定の節を加える．
// @param[in] assignment 割当リスト
void
StructSat::add_negation(const NodeValList& assignment)
{
  ymuint n = assignment.size();
  vector<SatLiteral> tmp_lits(n);
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assignment[i];
    SatLiteral alit = nv_to_lit(nv);
    tmp_lits[i] = ~alit;
  }
  mSolver.add_clause(tmp_lits);
}

// @brief 割当リストを仮定のリテラルに変換する．
// @param[in] assign_list 割当リスト
// @param[out] assumptions 仮定を表すリテラルのリスト
//
// 必要に応じて使われているリテラルに関するCNFを追加する．
void
StructSat::conv_to_assumption(const NodeValList& assign_list,
			      vector<SatLiteral>& assumptions)
{
  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    SatLiteral alit = nv_to_lit(nv);
    assumptions.push_back(alit);
  }
}

// @brief 与えられたノード(のリスト)のTFIのリストを作る．
// @param[in] node_list ノードのリスト
void
StructSat::make_tfi_list(const vector<const TpgNode*>& node_list)
{
  // node_list を mCurNodeList に入れる．
  for (ymuint i = 0; i < node_list.size(); ++ i) {
    const TpgNode* node = node_list[i];
    add_cur_node(node);
  }
  for (ymuint rpos = 0; rpos < mCurNodeList.size(); ++ rpos) {
    const TpgNode* node = mCurNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !cur_mark(inode) ) {
	add_cur_node(inode);
      }
    }

    // 遷移故障モードの場合には１時刻前の回路も作る．
    if ( fault_type() == kFtTransitionDelay && node->is_dff_output() ) {
      const TpgNode* inode = node->dff()->input();
      add_prev_node(inode);
    }
  }

  for (ymuint rpos = 0; rpos < mPrevNodeList.size(); ++ rpos) {
    const TpgNode* node = mPrevNodeList[rpos];

    // node のファンインを mCurNodeList に追加する．
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      if ( !prev_mark(inode) ) {
	add_prev_node(inode);
      }
    }
  }
}

// @brief 関係あるノードに変数を割り当てる．
void
StructSat::make_vars()
{
  for (ymuint i = 0; i < mCurNodeList.size(); ++ i) {
    const TpgNode* node = mCurNodeList[i];
    if ( !var_mark(node, 1) ) {
      set_new_var(node, 1);
      if ( debug() & debug_make_vars ) {
	cout << node->name() << "@1 -> " << var(node, 1) << endl;
      }
    }
  }
  for (ymuint i = 0; i < mPrevNodeList.size(); ++ i) {
    const TpgNode* node = mPrevNodeList[i];
    if ( !var_mark(node, 0) ) {
      // FF の入力の場合は1時刻後の出力の変数を用いる．
      if ( node->is_dff_input() ) {
	const TpgNode* onode = node->dff()->output();
	if ( var_mark(onode, 1) ) {
	  _set_var(node, 0, var(onode, 1));
	}
	else {
	  // ここの制御ロジックが美しくない．
	  set_new_var(node, 0);
	}
      }
      else {
	set_new_var(node, 0);
      }
      if ( debug() & debug_make_vars ) {
	cout << node->name() << "@0 -> " << var(node, 0) << endl;
      }
    }
  }

  for (ymuint i = 0; i < mFoConeList.size(); ++ i) {
    FoCone* focone = mFoConeList[i];
    focone->make_vars();
  }
}

// @brief 関係あるノードの入出力の関係を表すCNFを作る．
void
StructSat::make_cnf()
{
  for (ymuint i = 0; i < mCurNodeList.size(); ++ i) {
    const TpgNode* node = mCurNodeList[i];
    if ( !cnf_mark(node, 1) ) {
      set_cnf_mark(node, 1);
      make_node_cnf(node, var_map(1));
    }
  }
  for (ymuint i = 0; i < mPrevNodeList.size(); ++ i) {
    const TpgNode* node = mPrevNodeList[i];
    if ( !cnf_mark(node, 0) ) {
      set_cnf_mark(node, 0);
      make_node_cnf(node, var_map(0));
    }
  }

  for (ymuint i = 0; i < mFoConeList.size(); ++ i) {
    FoCone* focone = mFoConeList[i];
    focone->make_cnf();
  }
}

// @brief node の TFI の部分に変数を割り当てる．
// @param[in] node 対象のノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
void
StructSat::make_tfi_var(const TpgNode* node,
			int time)
{
  if ( var_mark(node, time) ) {
    return;
  }
  set_new_var(node, time);

  // 先に TFI のノードの変数を作る．
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    make_tfi_var(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == kFtTransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_var(inode, 0);
  }
}

// @brief node の TFI の CNF を作る．
// @param[in] node 対象のノード
// @param[in] time 時刻(0 or 1)
//
// 縮退故障モードの場合の時刻は 1
void
StructSat::make_tfi_cnf(const TpgNode* node,
			int time)
{
  if ( cnf_mark(node, time) ) {
    return;
  }
  set_cnf_mark(node, time);

  make_tfi_var(node, time);

  // node の入出力の関係を表す節を作る．
  make_node_cnf(node, var_map(time));

  // TFI のノードの節を作る．
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    make_tfi_cnf(inode, time);
  }

  // 遷移故障モードの時は前の時刻の回路も作る．
  if ( fault_type() == kFtTransitionDelay &&
       node->is_dff_output() && time == 1 ) {
    const TpgNode* inode = node->dff()->input();
    make_tfi_cnf(inode, 0);
  }
}

// @brief チェックを行う．
// @param[out] sat_model SATの場合の解
SatBool3
StructSat::check_sat(vector<SatBool3>& sat_model)
{
  return mSolver.solve(sat_model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] assign_list 割当リスト
// @param[out] sat_model SATの場合の解
SatBool3
StructSat::check_sat(const NodeValList& assign_list,
		     vector<SatBool3>& sat_model)
{
  vector<SatLiteral> assumptions;
  conv_to_assumption(assign_list, assumptions);

  return mSolver.solve(assumptions, sat_model);
}

// @brief 割当リストのもとでチェックを行う．
// @param[in] assign_list1, assign_list2 割当リスト
// @param[out] sat_model SATの場合の解
SatBool3
StructSat::check_sat(const NodeValList& assign_list1,
		     const NodeValList& assign_list2,
		     vector<SatBool3>& sat_model)
{
  vector<SatLiteral> assumptions;

  conv_to_assumption(assign_list1, assumptions);
  conv_to_assumption(assign_list2, assumptions);

  return mSolver.solve(assumptions, sat_model);
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
void
StructSat::make_node_cnf(const TpgNode* node,
			 const VidMap& var_map)
{
  GateLitMap_vid litmap(node, var_map);
  _make_node_cnf(node, litmap);
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
// @param[in] ovar 出力の変数
void
StructSat::make_node_cnf(const TpgNode* node,
			 const VidMap& var_map,
			 SatVarId ovar)
{
  GateLitMap_vid2 litmap(node, var_map, ovar);
  _make_node_cnf(node, litmap);
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] litmap 入出力のリテラル
void
StructSat::_make_node_cnf(const TpgNode* node,
			  const GateLitMap& litmap)
{
  SatLiteral olit = litmap.output();
  ymuint ni = litmap.input_size();
  switch ( node->gate_type() ) {
  case kGateCONST0:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(CONST0): " << "| " << olit << endl;
    }
    mSolver.add_clause(~olit);
    break;

  case kGateCONST1:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(CONST1): " << "| " << olit << endl;
    }
    mSolver.add_clause( olit);
    break;

  case kGateINPUT:
    // なにもしない．
    break;

  case kGateBUFF:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(BUFF): " << litmap.input(0) << "| " << olit << endl;
    }
    {
      SatLiteral ilit = litmap.input(0);
      mSolver.add_eq_rel( ilit,  olit);
    }
    break;

  case kGateNOT:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NOT):  " << litmap.input(0) << "| " << olit << endl;
    }
    {
      SatLiteral ilit = litmap.input(0);
      mSolver.add_eq_rel( ilit, ~olit);
    }
    break;

  case kGateAND:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(AND):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_andgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_andgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateNAND:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NAND):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_nandgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(OR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_orgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_orgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateNOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(NOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	mSolver.add_norgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = litmap.input(0);
	SatLiteral ilit1 = litmap.input(1);
	SatLiteral ilit2 = litmap.input(2);
	SatLiteral ilit3 = litmap.input(3);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (ymuint i = 0; i < ni; ++ i) {
	  ilits[i] = litmap.input(i);
	}
	mSolver.add_norgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case kGateXOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(XOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = litmap.input(0);
      SatLiteral ilit1 = litmap.input(1);
      mSolver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case kGateXNOR:
    if ( debug() & debug_make_node_cnf ) {
      cout << "_make_node_cnf(XNOR):  ";
      for (ymuint i = 0; i < ni; ++ i) {
	cout << " " << litmap.input(i);
      }
      cout << "| " << olit << endl;
    }
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = litmap.input(0);
      SatLiteral ilit1 = litmap.input(1);
      mSolver.add_xnorgate_rel( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

END_NAMESPACE_YM_SATPG
