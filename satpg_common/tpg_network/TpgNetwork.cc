
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgGateInfo.h"
#include "TpgFault.h"
#include "TpgStemFault.h"
#include "TpgBranchFault.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"

#include "NodeMap.h"
#include "AuxNodeInfo.h"

#include "ym/BnBlifReader.h"
#include "ym/BnIscas89Reader.h"
#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// BnFuncType を GateType に変換する．
inline
GateType
conv_to_gate_type(BnNodeType type)
{
  switch ( type ) {
  case kBnLogic_C0:   return kGateCONST0;
  case kBnLogic_C1:   return kGateCONST1;
  case kBnLogic_BUFF: return kGateBUFF;
  case kBnLogic_NOT:  return kGateNOT;
  case kBnLogic_AND:  return kGateAND;
  case kBnLogic_NAND: return kGateNAND;
  case kBnLogic_OR:   return kGateOR;
  case kBnLogic_NOR:  return kGateNOR;
  case kBnLogic_XOR:  return kGateXOR;
  case kBnLogic_XNOR: return kGateXNOR;
  case kBnLogic_EXPR: return kGateCPLX;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return kGateCPLX;
}

// immediate dominator リストをマージする．
const TpgNode*
merge(const TpgNode* node1,
      const TpgNode* node2)
{
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( node1 == nullptr || node2 == nullptr ) {
      return nullptr;
    }
    ymuint id1 = node1->id();
    ymuint id2 = node2->id();
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
    }
  }
}

// pair の最初の要素で比較を行なうファンクター
// こういうのは lambda 表記があると簡潔にすむ．
struct Lt
{
  bool
  operator()(const pair<ymuint, ymuint>& left,
	     const pair<ymuint, ymuint>& right)
  {
    return left.first < right.first;
  }

};


void
check_network_connection(const TpgNetwork& network)
{
  // fanin/fanout の sanity check
  bool error = false;

  ymuint nn = network.node_num();
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* node = network.node(i);
    ymuint nfi = node->fanin_num();
    for (ymuint j = 0; j < nfi; ++ j) {
      const TpgNode* inode = node->fanin(j);
      ymuint nfo = inode->fanout_num();
      bool found = false;
      for (ymuint k = 0; k < nfo; ++ k) {
	if ( inode->fanout(k) == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: inode(" << inode->id() << ") is a fanin of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanout of "
	     << "inode(" << inode->id() << ")" << endl;
      }
    }
    ymuint nfo = node->fanout_num();
    for (ymuint j = 0; j < nfo; ++ j) {
      const TpgNode* onode = node->fanout(j);
      ymuint nfi = onode->fanin_num();
      bool found = false;
      for (ymuint k = 0; k < nfi; ++ k) {
	if ( onode->fanin(k) == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: onode(" << onode->id() << ") is a fanout of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanin of "
	     << "onode(" << onode->id() << ")" << endl;
      }
    }
  }
  if ( error ) {
    cout << "network connectivity check failed" << endl;
    abort();
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgNetwork::TpgNetwork() :
  mAlloc(4096)
{
  mInputNum = 0;
  mOutputNum = 0;
  mDffNum = 0;
  mDffArray = nullptr;
  mNodeNum = 0;
  mNodeArray = nullptr;
  mAuxInfoArray = nullptr;
  mPPIArray = nullptr;
  mPPOArray = nullptr;
  mPPOArray2 = nullptr;
  mMffcNum = 0;
  mMffcArray = nullptr;
  mFfrNum = 0;
  mFfrArray = nullptr;
  mFaultNum = 0;
  mRepFaultNum = 0;
  mRepFaultArray = nullptr;
}

// @brief デストラクタ
TpgNetwork::~TpgNetwork()
{
  clear();
}

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @return 読み込みが成功したら true を返す．
bool
TpgNetwork::read_blif(const string& filename,
		      const CellLibrary* cell_library)
{
  BnNetwork network;
  bool stat = BnBlifReader::read(network, filename, cell_library);
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief iscas89 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @return 読み込みが成功したら true を返す．
bool
TpgNetwork::read_iscas89(const string& filename)
{
  BnNetwork network;
  bool stat = BnIscas89Reader::read(network, filename);
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief 出力の故障を得る．
// @param[in] id ノードID ( 0 <= id < node_num() )
// @param[in] val 故障値 ( 0 / 1 )
TpgFault*
TpgNetwork::_node_output_fault(ymuint id,
			       int val)
{
  ASSERT_COND( id < mNodeNum );

  return mAuxInfoArray[id].output_fault(val);
}

// @brief 入力の故障を得る．
// @param[in] id ノードID ( 0 <= id < node_num() )
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] pos 入力の位置番号
TpgFault*
TpgNetwork::_node_input_fault(ymuint id,
			      int val,
			      ymuint pos)
{
  ASSERT_COND( id < mNodeNum );

  return mAuxInfoArray[id].input_fault(pos, val);
}

BEGIN_NONAMESPACE

// @brief ノードの TFI にマークをつける．
ymuint
tfimark(const TpgNode* node,
	vector<bool>& mark)
{
  if ( mark[node->id()] ) {
    return 0;
  }
  mark[node->id()] = true;

  ymuint ni = node->fanin_num();
  ymuint n = 1;
  for (ymuint i = 0; i < ni; ++ i) {
    n += tfimark(node->fanin(i), mark);
  }
  return n;
}

END_NONAMESPACE

// @brief 内容をクリアする．
void
TpgNetwork::clear()
{
  // この配列以外は mAlloc で管理しているので
  // 個別に delete する必要はない．
  delete [] mDffArray;
  delete [] mNodeArray;
  delete [] mAuxInfoArray;
  delete [] mPPIArray;
  delete [] mPPOArray;
  delete [] mPPOArray2;
  delete [] mMffcArray;
  delete [] mFfrArray;
  delete [] mRepFaultArray;

  mAlloc.destroy();

  mDffArray = nullptr;
  mNodeArray = nullptr;
  mAuxInfoArray = nullptr;
  mPPIArray = nullptr;
  mPPOArray = nullptr;
  mPPOArray2 = nullptr;
  mMffcArray = nullptr;
  mFfrArray = nullptr;
  mRepFaultArray = nullptr;
}

// @brief 内容を設定する．
// @param[in] network 設定元のネットワーク
void
TpgNetwork::set(const BnNetwork& network)
{
  // まずクリアしておく．
  clear();

  //////////////////////////////////////////////////////////////////////
  // NodeInfoMgr にノードの論理関数を登録する．
  //////////////////////////////////////////////////////////////////////
  TpgGateInfoMgr node_info_mgr;
  ymuint nexpr = network.expr_num();
  vector<const TpgGateInfo*> node_info_list(nexpr);
  ymuint extra_node_num = 0;
  for (ymuint i = 0; i < nexpr; ++ i) {
    Expr expr = network.expr(i);
    ymuint ni = expr.input_size();
    const TpgGateInfo* node_info = node_info_mgr.complex_type(ni, expr);
    node_info_list[i] = node_info;
  }


  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  ymuint nl = network.logic_num();
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* src_node = network.logic(i);
    BnNodeType logic_type = src_node->type();
    if ( logic_type == kBnLogic_EXPR ) {
      const TpgGateInfo* node_info = node_info_list[src_node->func_id()];
      extra_node_num += node_info->extra_node_num();
    }
    else if ( logic_type == kBnLogic_XOR || logic_type == kBnLogic_XNOR ) {
      ymuint ni = src_node->fanin_num();
      extra_node_num += (ni - 2);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  // BnPort は複数ビットの場合があり，さらに入出力が一緒なのでめんどくさい
  vector<ymuint> input_map;
  vector<ymuint> output_map;
  ymuint np = network.port_num();
  for (ymuint i = 0; i < np; ++ i) {
    const BnPort* port = network.port(i);
    ymuint nb = port->bit_width();
    for (ymuint j = 0; j < nb; ++ j) {
      ymuint id = port->bit(j);
      const BnNode* node = network.node(id);
      if ( node->is_input() ) {
	input_map.push_back(id);
      }
      else if ( node->is_output() ) {
	output_map.push_back(id);
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
  }
  mInputNum = input_map.size();
  mOutputNum = output_map.size();
  mDffNum = network.dff_num();

  ymuint dff_control_num = 0;
  for (ymuint i = 0; i < mDffNum; ++ i) {
    const BnDff* dff = network.dff(i);
    // まずクロックで一つ
    ++ dff_control_num;
    if ( dff->clear() != kBnNullId ) {
      // クリア端子で一つ
      ++ dff_control_num;
    }
    if ( dff->preset() != kBnNullId ) {
      // プリセット端子で一つ
      ++ dff_control_num;
    }
  }

  mDffArray = new TpgDff[mDffNum];
  for (ymuint i = 0; i < mDffNum; ++ i) {
    mDffArray[i].mId = i;
  }

  ymuint nn = mInputNum + mOutputNum + mDffNum * 2 + nl + extra_node_num + dff_control_num;
  mNodeArray = new TpgNode*[nn];
  mAuxInfoArray = new AuxNodeInfo[nn];

  ymuint nppi = mInputNum + mDffNum;
  mPPIArray = new TpgNode*[nppi];

  ymuint nppo = mOutputNum + mDffNum;
  mPPOArray = new TpgNode*[nppo];
  mPPOArray2 = new TpgNode*[nppo];

  NodeMap node_map;

  mNodeNum = 0;
  mFaultNum = 0;


  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < mInputNum; ++ i) {
    ymuint id = input_map[i];
    const BnNode* src_node = network.node(id);
    ASSERT_COND( src_node->is_input() );
    ymuint nfo = src_node->fanout_num();
    TpgNode* node = make_input_node(i, src_node->name(), nfo);
    mPPIArray[i] = node;

    node_map.reg(id, node);
  }


  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < mDffNum; ++ i) {
    const BnDff* src_dff = network.dff(i);
    const BnNode* src_node = network.node(src_dff->output());
    ASSERT_COND( src_node->is_input() );
    ymuint nfo = src_node->fanout_num();
    TpgDff* dff = &mDffArray[i];
    ymuint iid = i + mInputNum;
    TpgNode* node = make_dff_output_node(iid, dff, src_node->name(), nfo);
    mPPIArray[iid] = node;
    dff->mOutput = node;

    node_map.reg(src_node->id(), node);
  }


  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnNetwork::logic() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < nl; ++ i) {
    const BnNode* src_node = network.logic(i);
    const TpgGateInfo* node_info = nullptr;
    BnNodeType logic_type = src_node->type();
    if ( logic_type == kBnLogic_EXPR ) {
      node_info = node_info_list[src_node->func_id()];
    }
    else {
      ASSERT_COND( logic_type != kBnLogic_TV );
      GateType gate_type = conv_to_gate_type(logic_type);
      node_info = node_info_mgr.simple_type(gate_type);
    }

    // ファンインのノードを取ってくる．
    ymuint ni = src_node->fanin_num();
    vector<TpgNode*> fanin_array(ni);
    for (ymuint j = 0; j < ni; ++ j) {
      fanin_array[j] = node_map.get(src_node->fanin(j));
    }
    ymuint nfo = src_node->fanout_num();
    TpgNode* node = make_logic_node(src_node->name(), node_info, fanin_array, nfo);

    // ノードを登録する．
    node_map.reg(src_node->id(), node);
  }


  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < mOutputNum; ++ i) {
    ymuint id = output_map[i];
    const BnNode* src_node = network.node(id);
    ASSERT_COND( src_node->is_output() );
    TpgNode* inode = node_map.get(src_node->fanin());
    string buf = "*";
    buf += src_node->name();
    TpgNode* node = make_output_node(i, buf, inode);
    mPPOArray[i] = node;
  }


  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < mDffNum; ++ i) {
    const BnDff* src_dff = network.dff(i);
    const BnNode* src_node = network.node(src_dff->input());

    TpgNode* inode = node_map.get(src_node->fanin());
    string dff_name = src_dff->name();
    string input_name = dff_name + ".input";
    TpgDff* dff = &mDffArray[i];
    ymuint oid = i + mOutputNum;
    TpgNode* node = make_dff_input_node(oid, dff, input_name, inode);
    mPPOArray[oid] = node;
    dff->mInput = node;

    // クロック端子を作る．
    const BnNode* src_clock = network.node(src_dff->clock());
    TpgNode* clock_fanin = node_map.get(src_clock->fanin());
    string clock_name = dff_name + ".clock";
    TpgNode* clock = make_dff_clock_node(dff, clock_name, clock_fanin);
    dff->mClock = clock;

    // クリア端子を作る．
    if ( src_dff->clear() != kBnNullId ) {
      const BnNode* src_clear = network.node(src_dff->clear());
      TpgNode* clear_fanin = node_map.get(src_clear->fanin());
      string clear_name = dff_name + ".clear";
      TpgNode* clear = make_dff_clear_node(dff, clear_name, clear_fanin);
      dff->mClear = clear;
    }

    // プリセット端子を作る．
    if ( src_dff->preset() != kBnNullId ) {
      const BnNode* src_preset = network.node(src_dff->preset());
      TpgNode* preset_fanin = node_map.get(src_preset->fanin());
      string preset_name = dff_name + ".preset";
      TpgNode* preset = make_dff_preset_node(dff, preset_name, preset_fanin);
      dff->mPreset = preset;
    }
  }

  ASSERT_COND( mNodeNum == nn );


  //////////////////////////////////////////////////////////////////////
  // ファンアウトをセットする．
  //////////////////////////////////////////////////////////////////////
  vector<ymuint> nfo_array(mNodeNum, 0);
  for (ymuint i = 0; i < mNodeNum; ++ i) {
    TpgNode* node = mNodeArray[i];
    ymuint ni = node->fanin_num();
    for (ymuint j = 0; j < ni; ++ j) {
      TpgNode* inode = node->fanin(j);
      ymuint& fo_pos = nfo_array[inode->id()];
      inode->set_fanout(fo_pos, node);
      ++ fo_pos;
    }
  }
  { // 検証
    ymuint error = 0;
    for (ymuint i = 0; i < mNodeNum; ++ i) {
      TpgNode* node = mNodeArray[i];
      if ( nfo_array[node->id()] != node->fanout_num() ) {
	if ( error == 0 ) {
	  cerr << "Error in TpgNetwork()" << endl;
	}
	cerr << "nfo_array[Node#" << node->id() << "] = " << nfo_array[node->id()] << endl
	     << "node->fanout_num()    = " << node->fanout_num() << endl;
	++ error;
      }
    }
    if ( error ) {
      abort();
    }
    // 接続が正しいかチェックする．
    check_network_connection(*this);
  }


  //////////////////////////////////////////////////////////////////////
  // データ系のノードに印をつける．
  //////////////////////////////////////////////////////////////////////
  vector<bool> dmarks(mNodeNum, false);
  for (ymuint i = 0; i < ppo_num(); ++ i) {
    const TpgNode* node = ppo(i);
    tfimark(node, dmarks);
  }


  //////////////////////////////////////////////////////////////////////
  // 代表故障を求める．
  //////////////////////////////////////////////////////////////////////
  mRepFaultNum = 0;
  for (ymuint i = 0; i < mNodeNum; ++ i) {
    // ノードごとに代表故障を設定する．
    // この処理は出力側から行う必要がある．
    TpgNode* node = mNodeArray[mNodeNum - i - 1];
    if ( dmarks[node->id()] ) {
      set_rep_faults(node);
      ymuint nf = node->fault_num();
      mRepFaultNum += nf;
    }
  }
  mRepFaultArray = new const TpgFault*[mRepFaultNum];
  ymuint wpos = 0;
  for (ymuint i = 0; i < mNodeNum; ++ i) {
    TpgNode* node = mNodeArray[i];
    ymuint nf = node->fault_num();
    for (ymuint j = 0; j < nf; ++ j, ++ wpos) {
      const TpgFault* fault = node->fault(j);
      mRepFaultArray[wpos] = fault;
    }
  }


  //////////////////////////////////////////////////////////////////////
  // TFI のサイズの昇順に並べた出力順を
  // mPPOArray2 に記録する．
  //////////////////////////////////////////////////////////////////////
  ymuint npo = ppo_num();
  vector<pair<ymuint, ymuint> > tmp_list(npo);
  for (ymuint i = 0; i < npo; ++ i) {
    const TpgNode* onode = ppo(i);
    // onode の TFI のノード数を計算する．
    vector<bool> mark(nn, false);
    ymuint n = tfimark(onode, mark);
    tmp_list[i] = make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(), Lt());
  // tmp_list の順に mPPOArray2 にセットする．
  for (ymuint i = 0; i < npo; ++ i) {
    ymuint opos = tmp_list[i].second;
    TpgNode* onode = mPPOArray[opos];
    mPPOArray2[i] = onode;
    onode->set_output_id2(i);
  }

  // immediate dominator を求める．
  for (ymuint i = 0; i < mNodeNum; ++ i) {
    TpgNode* node = mNodeArray[mNodeNum - i - 1];
    const TpgNode* imm_dom = nullptr;
    if ( !node->is_ppo() ) {
      ymuint nfo = node->fanout_num();
      if ( nfo > 0 ) {
	imm_dom = node->fanout(0);
	for (ymuint i = 1; imm_dom != nullptr && i < nfo; ++ i) {
	  imm_dom = merge(imm_dom, node->fanout(i));
	}
      }
    }
    node->set_imm_dom(imm_dom);
  }


  //////////////////////////////////////////////////////////////////////
  // FFR と MFFC の根のノードを求める．
  //////////////////////////////////////////////////////////////////////
  vector<TpgNode*> ffr_root_list;
  vector<TpgNode*> mffc_root_list;
  for (ymuint i = 0; i < mNodeNum; ++ i) {
    TpgNode* node = mNodeArray[i];
    if ( !dmarks[node->id()] ) {
      // データ系のノードでなければスキップ
      continue;
    }
    if ( node->ffr_root() == node ) {
      ffr_root_list.push_back(node);

      // MFFC の根は必ず FFR の根でもある．
      if ( node->imm_dom() == nullptr ) {
	mffc_root_list.push_back(node);
      }
    }
  }


  //////////////////////////////////////////////////////////////////////
  // FFR の情報を作る．
  //////////////////////////////////////////////////////////////////////
  mFfrNum = ffr_root_list.size();
  mFfrArray = new TpgFFR[mFfrNum];
  for (ymuint i = 0; i < mFfrNum; ++ i) {
    TpgNode* node = ffr_root_list[i];
    TpgFFR* ffr = &mFfrArray[i];
    set_ffr(node, ffr);
  }


  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  mMffcNum = mffc_root_list.size();
  mMffcArray = new TpgMFFC[mMffcNum];
  for (ymuint i = 0; i < mMffcNum; ++ i) {
    TpgNode* node = mffc_root_list[i];
    TpgMFFC* mffc = &mMffcArray[i];
    set_mffc(node, mffc);
  }
}

BEGIN_NONAMESPACE

class FoNodeLt
{
public:

  // コンストラクタ
  FoNodeLt(const vector<ymuint>& level_array) :
    mLevelArray(level_array)
  {
  }

  bool
  operator()(TpgNode* left,
	     TpgNode* right)
  {
    return mLevelArray[left->id()] < mLevelArray[right->id()];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  const vector<ymuint>& mLevelArray;

};

END_NONAMESPACE

// @brief 入力ノードを生成する．
// @param[in] iid 入力の番号
// @param[in] name ノード名
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_input_node(ymuint iid,
			    const string& name,
			    ymuint fanout_num)
{
  TpgNode* node = TpgNode::make_input(mNodeNum, name, iid, fanout_num, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(0, mAlloc);

  // 出力位置の故障を生成
  const char* c_name = node->name();
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  return node;
}

// @brief 出力ノードを生成する．
// @param[in] oid 出力の番号
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_output_node(ymuint oid,
			     const string& name,
			     TpgNode* inode)
{
  TpgNode* node = TpgNode::make_output(mNodeNum, name, oid, inode, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node->name();
  ymuint ipos = 0;
  for (int val = 0; val < 2; ++ val) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFの入力ノードを生成する．
// @param[in] oid 出力の番号
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_dff_input_node(ymuint oid,
				TpgDff* dff,
				const string& name,
				TpgNode* inode)
{
  TpgNode* node = TpgNode::make_dff_input(mNodeNum, name, oid, dff, inode, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node->name();
  ymuint ipos = 0;
  for (int val = 0; val < 2; ++ val) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief 入力ノードを生成する．
// @param[in] iid 入力の番号
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_dff_output_node(ymuint iid,
				 TpgDff* dff,
				 const string& name,
				 ymuint fanout_num)
{
  TpgNode* node = TpgNode::make_dff_output(mNodeNum, name, iid, dff, fanout_num, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(0, mAlloc);

  // 出力位置の故障を生成
  const char* c_name = node->name();
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  return node;
}

// @brief DFFのクロック端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_dff_clock_node(TpgDff* dff,
				const string& name,
				TpgNode* inode)
{
  TpgNode* node = TpgNode::make_dff_clock(mNodeNum, name, dff, inode, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node->name();
  ymuint ipos = 0;
  for (int val = 0; val < 2; ++ val) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのクリア端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_dff_clear_node(TpgDff* dff,
				const string& name,
				TpgNode* inode)
{
  TpgNode* node = TpgNode::make_dff_clear(mNodeNum, name, dff, inode, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node->name();
  ymuint ipos = 0;
  for (int val = 0; val < 2; ++ val) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief DFFのプリセット端子を生成する．
// @param[in] dff 接続しているDFF
// @param[in] name ノード名
// @param[in] inode 入力のノード
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_dff_preset_node(TpgDff* dff,
				 const string& name,
				 TpgNode* inode)
{
  TpgNode* node = TpgNode::make_dff_preset(mNodeNum, name, dff, inode, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  mAuxInfoArray[node->id()].init(1, mAlloc);

  // 入力位置の故障を生成
  const char* c_name = node->name();
  ymuint ipos = 0;
  for (int val = 0; val < 2; ++ val) {
    new_ifault(c_name, ipos, val, InodeInfo(node, ipos), nullptr);
  }

  return node;
}

// @brief 論理ノードを生成する．
// @param[in] src_name ノード名
// @param[in] node_info 論理関数の情報
// @param[in] fanin_list ファンインのリスト
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_logic_node(const string& src_name,
			    const TpgGateInfo* node_info,
			    const vector<TpgNode*>& fanin_list,
			    ymuint fanout_num)
{
  ymuint ni = fanin_list.size();

  // 複合型の場合の入力ノードを納める配列
  vector<InodeInfo> inode_array(ni);

  GateType gate_type = node_info->gate_type();
  TpgNode* node = nullptr;
  if ( gate_type != kGateCPLX ) {
    // 組み込み型の場合．
    // 2入力以上の XOR/XNOR ゲートを2入力に分解する．
    if ( gate_type == kGateXOR && ni > 2 ) {
      vector<TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      TpgNode* tmp_node = make_prim_node(string(), kGateXOR, tmp_list, 1);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for (ymuint i = 2; i < ni; ++ i) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string(), kGateXOR, tmp_list, 1);
	}
	else {
	  tmp_node = make_prim_node(src_name, kGateXOR, tmp_list, fanout_num);
	}
	inode_array[i].set(tmp_node, 1);
      }
      node = tmp_node;
    }
    else if ( gate_type == kGateXNOR && ni > 2 ) {
      vector<TpgNode*> tmp_list(2);
      tmp_list[0] = fanin_list[0];
      tmp_list[1] = fanin_list[1];
      TpgNode* tmp_node = make_prim_node(string(), kGateXOR, tmp_list, 1);
      inode_array[0].set(tmp_node, 0);
      inode_array[1].set(tmp_node, 1);
      for (ymuint i = 2; i < ni; ++ i) {
	tmp_list[0] = tmp_node;
	tmp_list[1] = fanin_list[i];
	if ( i < ni - 1 ) {
	  tmp_node = make_prim_node(string(), kGateXOR, tmp_list, 1);
	}
	else {
	  tmp_node = make_prim_node(src_name, kGateXNOR, tmp_list, fanout_num);
	}
	inode_array[i].set(tmp_node, i);
      }
      node = tmp_node;
    }
    else {
      node = make_prim_node(src_name, gate_type, fanin_list, fanout_num);
      for (ymuint i = 0; i < ni; ++ i) {
	inode_array[i].set(node, i);
      }
    }
  }
  else {
    Expr expr = node_info->expr();

    // 論理式の葉(リテラル)に対応するノードを入れる配列．
    // pos * 2 + 0: 肯定のリテラル
    // pos * 2 + 1: 否定のリテラルに対応する．
    vector<TpgNode*> leaf_nodes(ni * 2, nullptr);
    for (ymuint i = 0; i < ni; ++ i) {
      ymuint p_num = expr.litnum(VarId(i), false);
      ymuint n_num = expr.litnum(VarId(i), true);
      TpgNode* inode = fanin_list[i];
      if ( n_num == 0 ) {
	if ( p_num == 1 ) {
	  // 肯定のリテラルが1回だけ現れている場合
	  // 本当のファンインを直接つなぐ
	  leaf_nodes[i * 2 + 0] = inode;
	}
	else {
	  // 肯定のリテラルが2回以上現れている場合
	  // ブランチの故障に対応するためにダミーのバッファをつくる．
	  TpgNode* dummy_buff = make_prim_node(string(), kGateBUFF,
					       vector<TpgNode*>(1, inode), p_num);
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	  // このバッファの入力が故障位置となる．
	  inode_array[i].set(dummy_buff, 0);
	}
      }
      else {
	if ( p_num > 0 ) {
	  // 肯定と否定のリテラルがともに現れる場合
	  // ブランチの故障に対応するためにダミーのバッファを作る．
	  TpgNode* dummy_buff = make_prim_node(string(), kGateBUFF,
					       vector<TpgNode*>(1, inode), p_num + 1);
	  inode = dummy_buff;
	  leaf_nodes[i * 2 + 0] = dummy_buff;
	}

	// 否定のリテラルに対応するNOTゲートを作る．
	TpgNode* not_gate = make_prim_node(string(), kGateNOT,
					   vector<TpgNode*>(1, inode), n_num);
	leaf_nodes[i * 2 + 1] = not_gate;

	if ( p_num > 0 ) {
	  inode_array[i].set(inode, 0);
	}
	else {
	  inode_array[i].set(not_gate, 0);
	}
      }
    }

    // expr の内容を表す TpgNode の木を作る．
    node = make_cplx_node(src_name, expr, leaf_nodes, inode_array, fanout_num);
  }

  // 出力位置の故障を生成
  const char* c_name = node->name();
  for (int val = 0; val < 2; ++ val) {
    new_ofault(c_name, val, node);
  }

  // 入力位置の故障を生成
  mAuxInfoArray[node->id()].init(ni, mAlloc);
  for (ymuint i = 0; i < ni; ++ i) {
    Val3 oval0 = node_info->cval(i, kVal0);
    Val3 oval1 = node_info->cval(i, kVal1);

    TpgFault* rep0 = nullptr;
    if ( oval0 == kVal0 ) {
      rep0 = _node_output_fault(node->id(), 0);
    }
    else if ( oval0 == kVal1 ) {
      rep0 = _node_output_fault(node->id(), 1);
    }

    TpgFault* rep1 = nullptr;
    if ( oval1 == kVal0 ) {
      rep1 = _node_output_fault(node->id(), 0);
    }
    else if ( oval1 == kVal1 ) {
      rep1 = _node_output_fault(node->id(), 1);
    }
    new_ifault(c_name, i, 0, inode_array[i], rep0);
    new_ifault(c_name, i, 1, inode_array[i], rep1);
  }

  return node;
}

// @brief 論理式から TpgNode の木を生成する．
// @param[in] name ノード名
// @param[in] expr 式
// @param[in] leaf_nodes 式のリテラルに対応するノードの配列
// @param[in] inode_array ファンインの対応関係を収める配列
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
//
// leaf_nodes は 変数番号 * 2 + (0/1) に
// 該当する変数の肯定/否定のリテラルが入っている．
TpgNode*
TpgNetwork::make_cplx_node(const string& name,
			   const Expr& expr,
			   const vector<TpgNode*>& leaf_nodes,
			   vector<InodeInfo>& inode_array,
			   ymuint fanout_num)
{
  if ( expr.is_posiliteral() ) {
    ymuint iid = expr.varid().val();
    return leaf_nodes[iid * 2 + 0];
  }
  if ( expr.is_negaliteral() ) {
    ymuint iid = expr.varid().val();
    return leaf_nodes[iid * 2 + 1];
  }

  GateType gate_type;
  if ( expr.is_and() ) {
    gate_type = kGateAND;
  }
  else if ( expr.is_or() ) {
    gate_type = kGateOR;
  }
  else if ( expr.is_xor() ) {
    gate_type = kGateXOR;
  }
  else {
    ASSERT_NOT_REACHED;
  }

  // 子供の論理式を表すノード(の木)を作る．
  ymuint nc = expr.child_num();
  vector<TpgNode*> fanins(nc);
  for (ymuint i = 0; i < nc; ++ i) {
    const Expr& expr1 = expr.child(i);
    TpgNode* inode = make_cplx_node(string(), expr1, leaf_nodes, inode_array, 1);
    ASSERT_COND( inode != nullptr );
    fanins[i] = inode;
  }
  // fanins[] を確保するオーバーヘッドがあるが，
  // 子供のノードよりも先に親のノードを確保するわけには行かない．
  TpgNode* node = make_prim_node(name, gate_type, fanins, fanout_num);

  // オペランドがリテラルの場合，inode_array[]
  // の設定を行う．
  for (ymuint i = 0; i < nc; ++ i) {
    // 美しくないけどスマートなやり方を思いつかない．
    const Expr& expr1 = expr.child(i);
    if ( expr1.is_posiliteral() ) {
      ymuint iid = expr1.varid().val();
      if ( inode_array[iid].mNode == nullptr ) {
	inode_array[iid].set(node, i);
      }
    }
  }

  return node;
}

// @brief 組み込み型の論理ゲートを生成する．
// @param[in] name ノード名
// @param[in] type ゲートの型
// @param[in] fanin_list ファンインのリスト
// @param[in] fanout_num ファンアウト数
// @return 生成したノードを返す．
TpgNode*
TpgNetwork::make_prim_node(const string& name,
			   GateType type,
			   const vector<TpgNode*>& fanin_list,
			   ymuint fanout_num)
{
  TpgNode* node = TpgNode::make_logic(mNodeNum, name, type, fanin_list, fanout_num, mAlloc);
  mNodeArray[mNodeNum] = node;
  ++ mNodeNum;

  ymuint id = node->id();
  ymuint fanin_num = fanin_list.size();

  mAuxInfoArray[id].init(fanin_num, mAlloc);

  return node;
}

// @brief 出力の故障を作る．
// @param[in] name 故障位置のノード名
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] node 故障位置のノード
void
TpgNetwork::new_ofault(const char* name,
		       int val,
		       TpgNode* node)
{
  void* p = mAlloc.get_memory(sizeof(TpgStemFault));
  TpgFault* f = new (p) TpgStemFault(mFaultNum, name, val, node, nullptr);
  mAuxInfoArray[node->id()].set_output_fault(val, f);
  ++ mFaultNum;
}

// @brief 入力の故障を作る．
// @param[in] name 故障位置のノード名
// @param[in] ipos 故障位置のファンイン番号
// @param[in] node 故障位置のノード
// @param[in] inode_pos node 上の入力位置
// @param[in] val 故障値
// @param[in] rep 代表故障
//
// プリミティブ型の場合は ipos と inode_pos は同一だが
// 複合型の場合には異なる．
void
TpgNetwork::new_ifault(const char* name,
		       ymuint ipos,
		       int val,
		       const InodeInfo& inode_info,
		       TpgFault* rep)
{
  TpgNode* node = inode_info.mNode;
  ymuint inode_pos = inode_info.mPos;
  TpgNode* inode = node->fanin(inode_pos);
  void* p = mAlloc.get_memory(sizeof(TpgBranchFault));
  TpgFault* f = new (p) TpgBranchFault(mFaultNum, name, val, ipos, node, inode, inode_pos, rep);
  mAuxInfoArray[node->id()].set_input_fault(inode_pos, val, f);
  ++ mFaultNum;
}

// @brief 代表故障を設定する．
// @param[in] node 対象のノード
void
TpgNetwork::set_rep_faults(TpgNode* node)
{
  vector<TpgFault*> fault_list;

  if ( node->fanout_num() == 1 ) {
    TpgNode* onode = node->fanout(0);
    // ファンアウト先が一つならばそのファンイン
    // ブランチの故障と出力の故障は等価
    ymuint ni = onode->fanin_num();
    ymuint ipos = ni;
    for (ymuint i = 0; i < ni; ++ i) {
      if ( onode->fanin(i) == node ) {
	ipos = i;
	break;
      }
    }
    ASSERT_COND( ipos < ni );

    TpgFault* rep0 = _node_input_fault(onode->id(), 0, ipos);
    TpgFault* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      of0->set_rep(rep0);
    }

    TpgFault* rep1 = _node_input_fault(onode->id(), 1, ipos);
    TpgFault* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ){
      of1->set_rep(rep1);
    }
  }

  if ( !node->is_ppo() ) {
    TpgFault* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      TpgFault* rep0 = of0->_rep_fault();
      if ( rep0 == nullptr ) {
	of0->set_rep(of0);
	fault_list.push_back(of0);
      }
      else {
	of0->set_rep(rep0->_rep_fault());
      }
    }

    TpgFault* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ) {
      TpgFault* rep1 = of1->_rep_fault();
      if ( rep1 == nullptr ) {
	of1->set_rep(of1);
	fault_list.push_back(of1);
      }
      else {
	of1->set_rep(rep1->_rep_fault());
      }
    }
  }

  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    TpgFault* if0 = _node_input_fault(node->id(), 0, i);
    if ( if0 != nullptr ) {
      TpgFault* rep0 = if0->_rep_fault();
      if ( rep0 == nullptr ) {
	if0->set_rep(if0);
	fault_list.push_back(if0);
      }
      else {
	if0->set_rep(rep0->_rep_fault());
      }
    }

    TpgFault* if1 = _node_input_fault(node->id(), 1, i);
    if ( if1 != nullptr ) {
      TpgFault* rep1 = if1->_rep_fault();
      if ( rep1 == nullptr ) {
	if1->set_rep(if1);
	fault_list.push_back(if1);
      }
      else {
	if1->set_rep(rep1->_rep_fault());
      }
    }
  }

  // node の代表故障をセットする．
  node->set_fault_list(fault_list, mAlloc);
}

// @brief FFR の情報を設定する．
// @param[in] root FFR の根のノード
// @param[in] ffr 対象の FFR
void
TpgNetwork::set_ffr(TpgNode* root,
		    TpgFFR* ffr)
{
  // root を根とするFFRの故障リストを求める．
  vector<TpgNode*> node_list;
  vector<TpgFault*> fault_list;

  node_list.push_back(root);
  while ( !node_list.empty() ) {
    TpgNode* node = node_list.back();
    node_list.pop_back();

    node->add_to_fault_list(fault_list);

    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( inode->ffr_root() != inode ) {
	node_list.push_back(inode);
      }
    }
  }

  root->set_ffr(ffr);
  ffr->set(root, fault_list, mAlloc);
}

// @brief MFFC の情報を設定する．
// @param[in] root MFFCの根のノード
// @param[in] mffc 対象のMFFC
void
TpgNetwork::set_mffc(TpgNode* root,
		     TpgMFFC* mffc)
{
  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num());
  vector<TpgNode*> node_list;
  vector<const TpgFFR*> ffr_list;
  vector<TpgFault*> fault_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    TpgNode* node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      ffr_list.push_back(node->ffr());
    }

    node->add_to_fault_list(fault_list);

    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      TpgNode* inode = node->fanin(i);
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }

  root->set_mffc(mffc);
  mffc->set(root, ffr_list, fault_list, mAlloc);
}

// @brief MFFC を返す．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
const TpgMFFC*
TpgNetwork::mffc(ymuint pos) const
{
  ASSERT_COND( pos < mffc_num() );

  return &mMffcArray[pos];
}

// @brief FFR を返す．
// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
const TpgFFR*
TpgNetwork::ffr(ymuint pos) const
{
  ASSERT_COND( pos < ffr_num() );

  return &mFfrArray[pos];
}

// @brief TpgNetwork の内容を出力する関数
// @param[in] s 出力先のストリーム
// @param[in] network 対象のネットワーク
void
print_network(ostream& s,
	      const TpgNetwork& network)
{
  ymuint n = network.node_num();
  for (ymuint i = 0; i < n; ++ i) {
    const TpgNode* node = network.node(i);
    print_node(s, node);
    s << ": ";
    if ( node->is_primary_input() ) {
      s << "INPUT#" << node->input_id();
    }
    else if ( node->is_dff_output() ) {
      s << "INPUT#" << node->input_id()
	<< "(DFF#" << node->dff()->id() << ".output)";
    }
    else if ( node->is_primary_output() ) {
      s << "OUTPUT#" << node->output_id();
      const TpgNode* inode = node->fanin(0);
      s << " = ";
      print_node(s, inode);
    }
    else if ( node->is_dff_input() ) {
      s << "OUTPUT#" << node->output_id()
	<< "(DFF#" << node->dff()->id() << ".input)";
      const TpgNode* inode = node->fanin(0);
      s << " = ";
      print_node(s, inode);
    }
    else if ( node->is_dff_clock() ) {
      s << "DFF#" << node->dff()->id() << ".clock";
    }
    else if ( node->is_dff_clear() ) {
      s << "DFF#" << node->dff()->id() << ".clear";
    }
    else if ( node->is_dff_preset() ) {
      s << "DFF#" << node->dff()->id() << ".preset";
    }
    else if ( node->is_logic() ) {
      s << node->gate_type();
      ymuint ni = node->fanin_num();
      if ( ni > 0 ) {
	s << "(";
	for (ymuint j = 0; j < ni; ++ j) {
	  const TpgNode* inode = node->fanin(j);
	  s << " ";
	  print_node(s, inode);
	}
	s << " )";
      }
    }
    else {
      ASSERT_NOT_REACHED;
    }
    s << endl;
  }
  s << endl;
}

// @brief ノード名を出力する
// @param[in] s 出力先のストリーム
// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNode* node)
{
  s << "NODE#" << node->id() << ": " << node->name();
}


//////////////////////////////////////////////////////////////////////
// クラス AuxNodeInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AuxNodeInfo::AuxNodeInfo()
{
  mOutputFaults[0] = nullptr;
  mOutputFaults[1] = nullptr;
  mFaninNum = 0;
  mInputFaults = nullptr;
}

// @brief デストラクタ
AuxNodeInfo::~AuxNodeInfo()
{
  // このクラスに関係するメモリはすべて
  // TpgNetwork::mAlloc が管理しているので
  // ここではなにもする必要はない．
}

// @brief 初期化する．
// @param[in] ni 入力数
// @param[in] alloc メモリアロケータ
void
AuxNodeInfo::init(ymuint ni,
		  Alloc& alloc)
{
  mFaninNum = ni;

  ymuint ni2 = ni * 2;
  void* q = alloc.get_memory(sizeof(TpgFault*) * ni2);
  mInputFaults = new (q) TpgFault*[ni2];
  for (ymuint i = 0; i < ni2; ++ i) {
    mInputFaults[i] = nullptr;
  }
}

// @brief 出力の故障を設定する．
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] f 故障
void
AuxNodeInfo::set_output_fault(int val,
			      TpgFault* f)
{
  ASSERT_COND( val == 0 || val == 1 );

  mOutputFaults[val] = f;
}

// @brief 入力の故障を設定する．
// @param[in] ipos 入力位置
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] f 故障
void
AuxNodeInfo::set_input_fault(ymuint ipos,
			     int val,
			     TpgFault* f)
{
  ASSERT_COND( val == 0 || val == 1 );
  ASSERT_COND( ipos < mFaninNum );

  mInputFaults[(ipos * 2) + val] = f;
}

END_NAMESPACE_YM_SATPG
