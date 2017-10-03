
/// @file DtpgImplM.cc
/// @brief DtpgImplM の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgImplM.h"

#include "TpgFault.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "GateLitMap_vect.h"

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPGM
int debug_dtpgm = 1;
#else
const int debug_dtpgm = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の型
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] mffc 対象のMFFC
DtpgImplM::DtpgImplM(const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp,
		     FaultType fault_type,
		     BackTracer& bt,
		     const TpgNetwork& network,
		     const TpgMFFC* mffc) :
  DtpgImpl(sat_type, sat_option, sat_outp, fault_type, bt, network, mffc->root()),
  mElemArray(mffc->elem_num()),
  mElemVarArray(mffc->elem_num()),
  mElemPosMap(network.max_fault_id(), -1)
{
  for (ymuint i = 0; i < mffc->elem_num(); ++ i ) {
    const TpgFFR* ffr = mffc->elem(i);
    mElemArray[i] = ffr->root();
    ASSERT_COND( ffr->root() != nullptr );
    ymuint nf = ffr->fault_num();
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* f = ffr->fault(j);
      mElemPosMap[f->id()] = i;
    }
  }
}

// @brief デストラクタ
DtpgImplM::~DtpgImplM()
{
}

// @brief CNF 式を作る．
void
DtpgImplM::gen_cnf(DtpgStats& stats)
{
  cnf_begin();

  // root 以降の伝搬条件を作る．
  gen_cnf_base();

  make_mffc_condition();

  cnf_end(stats);
}

// @briefテスト生成を行う．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgImplM::dtpg(const TpgFault* fault,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  if ( mElemPosMap[fault->id()] == -1 ) {
    cerr << "Error[DtpgImplM::dtpg()]: " << fault << " is not within the MFFC" << endl;
    return kB3X;
  }

  vector<SatLiteral> assumptions;
  if ( mElemArray.size() > 1 ) {
    // FFR の根の出力に故障を挿入する．
    assumptions.reserve(mElemArray.size());
    ymuint elem_pos = mElemPosMap[fault->id()];
    for (ymuint i = 0; i < mElemVarArray.size(); ++ i) {
      SatVarId evar = mElemVarArray[i];
      bool inv = (i != elem_pos);
      assumptions.push_back(SatLiteral(evar, inv));
    }
  }

  // 故障に対するテスト生成を行なう．
  SatBool3 ans = solve(fault, assumptions, nodeval_list, stats);

  return ans;
}

// @brief MFFC 内部の故障伝搬条件を表すCNFを作る．
void
DtpgImplM::make_mffc_condition()
{
  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    mElemVarArray[i] = solver().new_variable();

    if ( debug_dtpgm ) {
      DEBUG_OUT << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
    }
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  vector<int> elem_map(max_node_id(), -1);
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    elem_map[node->id()] = i;
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_dtpgm ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_variable();
	set_fvar(onode, var);
	node_list.push_back(onode);

	if ( debug_dtpgm ) {
	  DEBUG_OUT << "fvar(Node#" << onode->id() << ") = " << var << endl;
	}
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    SatVarId fvar = solver().new_variable();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    ymuint ni = node->fanin_num();
    vector<SatVarId> ivars(ni);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      ivars[i] = fvar(inode);
    }
    SatVarId ovar = fvar(node);
    ymuint elem_pos = elem_map[node->id()];
    if ( elem_pos != -1 ) {
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[elem_pos]
      ovar = solver().new_variable();
      inject_fault(elem_pos, ovar);
    }
    // ほとんど GateLitMap_vid(node, fvar_map()) を使いたいのだが
    // ovar が fvar(node) ではない！
    make_node_cnf(node, GateLitMap_vect(ivars, ovar));

    if ( debug_dtpgm ) {
      DEBUG_OUT << "Node#" << node->id() << ": ofvar("
		<< ovar << ") := " << node->gate_type()
		<< "(";
      for (ymuint i = 0; i < ni; ++ i) {
	DEBUG_OUT << " " << ivars[i];
      }
      DEBUG_OUT << ")" << endl;
    }
  }
}

// @brief 故障挿入回路のCNFを作る．
// @param[in] elem_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
DtpgImplM::inject_fault(ymuint elem_pos,
			SatVarId ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[elem_pos]);
  const TpgNode* node = mElemArray[elem_pos];
  SatLiteral olit(fvar(node));

  solver().add_xorgate_rel(lit1, lit2, olit);

  if ( debug_dtpgm ) {
    DEBUG_OUT << "inject fault: " << ovar << " -> " << fvar(node)
	      << " with cvar = " << mElemVarArray[elem_pos] << endl;
  }
}

END_NAMESPACE_YM_SATPG
