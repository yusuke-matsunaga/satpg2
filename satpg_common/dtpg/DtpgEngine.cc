
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"

#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateType.h"
#include "GateEnc.h"
#include "Val3.h"
#include "NodeValList.h"
#include "Justifier.h"
#include "TestVector.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

//#define DEBUG_DTPG

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif

bool debug_mffccone = false;

END_NONAMESPACE


BEGIN_NAMESPACE_YM_SATPG

extern
NodeValList
extract(const TpgNode* root,
	const VidMap& gvar_map,
	const VidMap& fvar_map,
	const vector<SatBool3>& model);

// @brief コンストラクタ(ノードモード)
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] network 対象のネットワーク
// @param[in] node 故障のあるノード
DtpgEngine::DtpgEngine(const string& sat_type,
		       const string& sat_option,
		       ostream* sat_outp,
		       FaultType fault_type,
		       const string& just_type,
		       const TpgNetwork& network,
		       const TpgNode* node) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mRoot(node->ffr_root()),
  mMarkArray(mNetwork.node_num(), 0U),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap(network.node_num()),
  mDvarMap(network.node_num()),
  mJustifier(just_type, network),
  mTimerEnable(true)
{
  mTfoList.reserve(network.node_num());
  mTfiList.reserve(network.node_num());
  mTfi2List.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());

  cnf_begin();

  gen_cnf_base();

  cnf_end();
}

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgEngine::DtpgEngine(const string& sat_type,
		       const string& sat_option,
		       ostream* sat_outp,
		       FaultType fault_type,
		       const string& just_type,
		       const TpgNetwork& network,
		       const TpgFFR& ffr) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mRoot(ffr.root()),
  mMarkArray(mNetwork.node_num(), 0U),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap(network.node_num()),
  mDvarMap(network.node_num()),
  mJustifier(just_type, network),
  mTimerEnable(true)
{
  mTfoList.reserve(network.node_num());
  mTfiList.reserve(network.node_num());
  mTfi2List.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());

  cnf_begin();

  gen_cnf_base();

  cnf_end();
}

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] just_type Justifier の種類を表す文字列
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgEngine::DtpgEngine(const string& sat_type,
		       const string& sat_option,
		       ostream* sat_outp,
		       FaultType fault_type,
		       const string& just_type,
		       const TpgNetwork& network,
		       const TpgMFFC& mffc) :
  mSolver(sat_type, sat_option, sat_outp),
  mNetwork(network),
  mFaultType(fault_type),
  mRoot(mffc.root()),
  mMarkArray(mNetwork.node_num(), 0U),
  mElemArray(mffc.ffr_num()),
  mElemVarArray(mffc.ffr_num()),
  mHvarMap(network.node_num()),
  mGvarMap(network.node_num()),
  mFvarMap(network.node_num()),
  mDvarMap(network.node_num()),
  mJustifier(just_type, network),
  mTimerEnable(true)
{
  mTfoList.reserve(network.node_num());
  mTfiList.reserve(network.node_num());
  mTfi2List.reserve(network.node_num());
  mOutputList.reserve(network.ppo_num());

  if ( mffc.ffr_num() > 1 ) {
    int ffr_id = 0;
    for ( auto ffr: mffc.ffr_list() ) {
      mElemArray[ffr_id] = ffr->root();
      ASSERT_COND( ffr->root() != nullptr );
      mElemPosMap.add(ffr->root()->id(), ffr_id);
      ++ ffr_id;
    }
  }

  cnf_begin();

  gen_cnf_base();

  if ( mffc.ffr_num() > 1 ) {
    gen_cnf_mffc();
  }

  cnf_end();
}

// @brief デストラクタ
DtpgEngine::~DtpgEngine()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
DtpgResult
DtpgEngine::gen_pattern(const TpgFault* fault)
{
  vector<SatLiteral> assumptions;

  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  if ( ffr_root != root_node() ) {
    // root のある FFR を活性化する条件を作る．
    int ffr_id;
    bool stat = mElemPosMap.find(ffr_root->id(), ffr_id);
    if ( !stat ) {
      cerr << "Error[DtpgEngine::dtpg()]: "
	   << ffr_root->id() << " is not within the MFFC" << endl;
      return DtpgResult();
    }

    int ffr_num = mElemArray.size();
    if ( ffr_num > 1 ) {
      // FFR の根の出力に故障を挿入する．
      assumptions.reserve(ffr_num);
      for ( int i = 0; i < ffr_num; ++ i ) {
	SatVarId evar = mElemVarArray[i];
	bool inv = (i != ffr_id);
	assumptions.push_back(SatLiteral(evar, inv));
      }
    }
  }

  return solve(fault, assumptions);
}

// @brief タイマーをスタートする．
void
DtpgEngine::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DtpgEngine::cnf_end()
{
  USTime time = timer_stop();
  mStats.mCnfGenTime += time;
  ++ mStats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
DtpgEngine::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
DtpgEngine::timer_stop()
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
DtpgEngine::gen_cnf_base()
{
  // root の TFO を mTfoList に入れる．
  set_tfo_mark(mRoot);
  for ( int rpos = 0; rpos < mTfoList.size(); ++ rpos ) {
    const TpgNode* node = mTfoList[rpos];
    for ( auto onode: node->fanout_list() ) {
      set_tfo_mark(onode);
    }
  }

  // TFO の TFI を mNodeList に入れる．
  for ( auto node: mTfoList ) {
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }
  for ( int rpos = 0; rpos < mTfiList.size(); ++ rpos ) {
    const TpgNode* node = mTfiList[rpos];
    for ( auto inode: node->fanin_list() ) {
      set_tfi_mark(inode);
    }
  }

  // TFI に含まれる DFF のさらに TFI を mTfi2List に入れる．
  if ( mFaultType == FaultType::TransitionDelay ) {
    if ( mRoot->is_dff_output() ) {
      mDffList.push_back(mRoot->dff());
    }
    for ( auto dff: mDffList ) {
      const TpgNode* node = dff->input();
      mTfi2List.push_back(node);
    }
    set_tfi2_mark(mRoot);
    for ( int rpos = 0; rpos < mTfi2List.size(); ++ rpos) {
      const TpgNode* node = mTfi2List[rpos];
      for ( auto inode: node->fanin_list() ) {
	set_tfi2_mark(inode);
      }
    }
  }

  // TFO の部分に変数を割り当てる．
  for ( auto node: mTfoList ) {
    SatVarId gvar = mSolver.new_variable();
    SatVarId fvar = mSolver.new_variable();
    SatVarId dvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, fvar);
    mDvarMap.set_vid(node, dvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
		<< "fvar(Node#" << node->id() << ") = " << fvar << endl
		<< "dvar(Node#" << node->id() << ") = " << dvar << endl;
    }
  }

  // TFI の部分に変数を割り当てる．
  for ( auto node: mTfiList ) {
    SatVarId gvar = mSolver.new_variable();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "gvar(Node#" << node->id() << ") = " << gvar << endl
		<< "fvar(Node#" << node->id() << ") = " << gvar << endl;
    }
  }

  // TFI2 の部分に変数を割り当てる．
  for ( auto node: mTfi2List ) {
    SatVarId hvar = mSolver.new_variable();

    mHvarMap.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << "hvar(Node#" << node->id() << ") = " << hvar << endl;
    }
  }


  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc gval_enc(mSolver, mGvarMap);
  for ( auto node: mTfoList ) {
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
  for ( auto node: mTfi2List ) {
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


  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc fval_enc(mSolver, mFvarMap);
  for ( auto node: mTfoList ) {
    if ( node != mRoot ) {
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
    }
    make_dchain_cnf(node);
  }


  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  int no = mOutputList.size();
  vector<SatLiteral> odiff(no);
  for (int i = 0; i < no; ++ i) {
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

// @brief mffc 内の影響が root まで伝搬する条件のCNF式を作る．
void
DtpgEngine::gen_cnf_mffc()
{
  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    mElemVarArray[i] = mSolver.new_variable();

    if ( debug_mffccone ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  HashMap<int, int> ffr_map;
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    ffr_map.add(node->id(), i);
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = mSolver.new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  for ( int rpos = 0; rpos < node_list.size(); ++ rpos ) {
    const TpgNode* node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    for ( auto onode: node->fanout_list() ) {
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = mSolver.new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_mffccone ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for ( int i = 0; i < mElemArray.size(); ++ i ) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    SatVarId fvar = mSolver.new_variable();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  GateEnc fval_enc(mSolver, mFvarMap);
  for ( int rpos = 0; rpos < node_list.size(); ++ rpos ) {
    const TpgNode* node = node_list[rpos];
    SatVarId ovar = fvar(node);
    int ffr_pos;
    if ( ffr_map.find(node->id(), ffr_pos) ) {
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[ffr_pos]
      ovar = mSolver.new_variable();
      inject_fault(ffr_pos, ovar);
      // ovar が fvar(node) ではない！
      fval_enc.make_cnf(node, ovar);
    }
    else {
      fval_enc.make_cnf(node);
    }

    if ( debug_mffccone ) {
      DEBUG_OUT << "Node#" << node->id() << ": ofvar("
		<< ovar << ") := " << node->gate_type()
		<< "(";
      for ( auto inode: node->fanin_list() ) {
	DEBUG_OUT << " " << fvar(inode);
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 故障挿入回路のCNFを作る．
// @param[in] ffr_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
DtpgEngine::inject_fault(int ffr_pos,
			 SatVarId ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[ffr_pos]);
  const TpgNode* node = mElemArray[ffr_pos];
  SatLiteral olit(fvar(node));

  mSolver.add_xorgate_rel(lit1, lit2, olit);

  if ( debug_mffccone ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[ffr_pos] << endl;
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
DtpgEngine::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(mGvarMap(node));
  SatLiteral flit(mFvarMap(node));
  SatLiteral dlit(mDvarMap(node));

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

  if ( debug_dtpg ) {
    DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
	      << glit << " XOR " << flit << endl;
  }

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

    if ( debug_dtpg ) {
      DEBUG_OUT << "!dvar(Node#" << node->id() << ") -> "
		<< glit << " = " << flit << endl;
    }
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

    if ( debug_dtpg ) {
      DEBUG_OUT << "dvar(Node#" << node->id() << ") -> ";
    }
    int nfo = node->fanout_num();
    if ( nfo == 1 ) {
      SatLiteral odlit(mDvarMap(node->fanout_list()[0]));
      mSolver.add_clause(~dlit, odlit);

      if ( debug_dtpg ) {
	DEBUG_OUT << odlit << endl;
      }
    }
    else {
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(nfo + 1);
      for ( auto onode: node->fanout_list() ) {
	tmp_lits.push_back(SatLiteral(mDvarMap(onode)));

	if ( debug_dtpg ) {
	  DEBUG_OUT << " " << mDvarMap(onode);
	}
      }

      if ( debug_dtpg ) {
	DEBUG_OUT << endl;
      }
      tmp_lits.push_back(~dlit);
      mSolver.add_clause(tmp_lits);

      const TpgNode* imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	SatLiteral odlit(mDvarMap(imm_dom));
	mSolver.add_clause(~dlit, odlit);

	if ( debug_dtpg ) {
	  DEBUG_OUT << "dvar(Node#" << node->id() << ") -> "
		    << odlit << endl;
	}
      }
    }
  }
}

// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
// @param[in] fault 対象の故障
// @param[out] assign_list 結果の値割り当てリスト
NodeValList
DtpgEngine::make_ffr_condition(const TpgFault* fault)
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
    Val3 nval = onode->nval();
    if ( nval != Val3::_X ) {
      bool val = (nval == Val3::_1);
      for ( auto inode1: onode->fanin_list() ) {
	if ( inode1 != inode ) {
	  add_assign(assign_list, inode1, 1, val);
	}
      }
    }
  }

  // FFR の根までの伝搬条件を作る．
  for ( const TpgNode* node = fault->tpg_onode(); node->fanout_num() == 1;
	node = node->fanout_list()[0]) {
    const TpgNode* fonode = node->fanout_list()[0];
    int ni = fonode->fanin_num();
    if ( ni == 1 ) {
      continue;
    }
    Val3 nval = fonode->nval();
    if ( nval == Val3::_X ) {
      continue;
    }
    bool val = (nval == Val3::_1);
    for ( auto inode1: fonode->fanin_list() ) {
      if ( inode1 != node ) {
	add_assign(assign_list, inode1, 1, val);
      }
    }
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
DtpgEngine::add_assign(NodeValList& assign_list,
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

// @brief 一つの SAT問題を解く．
// @param[in] fault 対象の故障
// @param[in] assumptions 値の決まっている変数のリスト
// @param[out] testvect テストパタンを格納する変数
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
DtpgResult
DtpgEngine::solve(const TpgFault* fault,
		  const vector<SatLiteral>& assumptions)
{
  StopWatch timer;
  timer.start();

  SatStats prev_stats;
  mSolver.get_stats(prev_stats);

  // FFR 内の故障伝搬条件を assign_list に入れる．
  NodeValList assign_list = make_ffr_condition(fault);

  // assign_list の内容と assumptions を足したものを assumptions1 に入れる．
  vector<SatLiteral> assumptions1;
  int n0 = assumptions.size();
  int n = assign_list.size();
  assumptions1.reserve(n + n0);
  for ( auto nv: assign_list ) {
    const TpgNode* node = nv.node();
    bool inv = !nv.val();
    SatVarId vid = (nv.time() == 0) ? hvar(node) : gvar(node);
    assumptions1.push_back(SatLiteral(vid, inv));
  }
  for ( auto lit: assumptions ) {
    assumptions1.push_back(lit);
  }

  vector<SatBool3> model;
  SatBool3 ans = mSolver.solve(assumptions1, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mSolver.get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == SatBool3::True ) {
    // パタンが求まった．

    timer.reset();
    timer.start();

    // バックトレースを行う．
    const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
    NodeValList assign_list2 = extract(ffr_root, mGvarMap, mFvarMap, model);
    assign_list2.merge(assign_list);
    TestVector testvect;
    if ( mFaultType == FaultType::TransitionDelay ) {
      testvect = mJustifier(assign_list2, mHvarMap, mGvarMap, model);
    }
    else {
      testvect = mJustifier(assign_list2, mGvarMap, model);
    }

    timer.stop();
    mStats.mBackTraceTime += timer.time();
    mStats.update_det(sat_stats, time);

    return DtpgResult(testvect);
  }
  else if ( ans == SatBool3::False ) {
    // 検出不能と判定された．
    mStats.update_red(sat_stats, time);
    return DtpgResult::make_untestable();
  }
  else {
    // ans == SatBool3::X つまりアボート
    mStats.update_abort(sat_stats, time);
    return DtpgResult();
  }
}

END_NAMESPACE_YM_SATPG
