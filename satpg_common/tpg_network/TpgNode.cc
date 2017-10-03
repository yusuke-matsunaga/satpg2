
/// @file TpgNode.cc
/// @brief TpgNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"
#include "TpgFFR.h"

#include "TpgInput.h"
#include "TpgOutput.h"

#include "TpgDffInput.h"
#include "TpgDffOutput.h"
#include "TpgDffClock.h"
#include "TpgDffClear.h"
#include "TpgDffPreset.h"

#include "TpgLogicC0.h"
#include "TpgLogicC1.h"
#include "TpgLogicBUFF.h"
#include "TpgLogicNOT.h"
#include "TpgLogicAND.h"
#include "TpgLogicNAND.h"
#include "TpgLogicOR.h"
#include "TpgLogicNOR.h"
#include "TpgLogicXOR.h"
#include "TpgLogicXNOR.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief GateType のストリーム演算子
ostream&
operator<<(ostream& s,
	   GateType gate_type)
{
  switch ( gate_type ) {
  case kGateCONST0: s << "CONST-0"; break;
  case kGateCONST1: s << "CONST-1"; break;
  case kGateINPUT:  s << "INPUT"; break;
  case kGateBUFF:   s << "BUFF"; break;
  case kGateNOT:    s << "NOT"; break;
  case kGateAND:    s << "AND"; break;
  case kGateNAND:   s << "NAND"; break;
  case kGateOR:     s << "OR"; break;
  case kGateNOR:    s << "NOR"; break;
  case kGateXOR:    s << "XOR"; break;
  case kGateXNOR:   s << "XNOR"; break;
  default:          s << "---"; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgNode
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードを作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] iid 入力番号
// @param[in] fanout_num ファンアウト数
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_input(ymuint id,
		    const string& name,
		    ymuint iid,
		    ymuint fanout_num,
		    Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgInput));
  TpgNode* node = new (p) TpgInput(id, iid);
  node->set_name(name, alloc);
  node->set_fanout_num(fanout_num, alloc);

  return node;
}

// @brief 出力ノードを作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] oid 出力番号
// @param[in] inode 入力ノード
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_output(ymuint id,
		     const string& name,
		     ymuint oid,
		     TpgNode* inode,
		     Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgOutput));
  TpgNode* node = new (p) TpgOutput(id, oid, inode);
  node->set_name(name, alloc);

  return node;
}

// @brief DFFの入力ノードを作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] oid 出力番号
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_dff_input(ymuint id,
			const string& name,
			ymuint oid,
			TpgDff* dff,
			TpgNode* inode,
			Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgDffInput));
  TpgNode* node = new (p) TpgDffInput(id, oid, dff, inode);
  node->set_name(name, alloc);

  return node;
}

// @brief DFFの出力ノードを作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] iid 入力番号
// @param[in] dff 接続しているDFF
// @param[in] fanout_num ファンアウト数
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_dff_output(ymuint id,
			 const string& name,
			 ymuint iid,
			 TpgDff* dff,
			 ymuint fanout_num,
			 Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgDffOutput));
  TpgNode* node = new (p) TpgDffOutput(id, iid, dff);
  node->set_name(name, alloc);
  node->set_fanout_num(fanout_num, alloc);

  return node;
}

// @brief DFFのクロック端子を作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_dff_clock(ymuint id,
			const string& name,
			TpgDff* dff,
			TpgNode* inode,
			Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgDffClock));
  TpgNode* node = new (p) TpgDffClock(id, dff, inode);
  node->set_name(name, alloc);

  return node;
}

// @brief DFFのクリア端子を作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_dff_clear(ymuint id,
			const string& name,
			TpgDff* dff,
			TpgNode* inode,
			Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgDffClear));
  TpgNode* node = new (p) TpgDffClear(id, dff, inode);
  node->set_name(name, alloc);

  return node;
}

// @brief DFFのプリセット端子を作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] dff 接続しているDFF
// @param[in] inode 入力ノード
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_dff_preset(ymuint id,
			 const string& name,
			 TpgDff* dff,
			 TpgNode* inode,
			 Alloc& alloc)
{
  void* p = alloc.get_memory(sizeof(TpgDffPreset));
  TpgNode* node = new (p) TpgDffPreset(id, dff, inode);
  node->set_name(name, alloc);

  return node;
}

// @brief 論理ノードを作る．
// @param[in] id ノード番号
// @param[in] name ノード名
// @param[in] gate_type ゲートタイプ
// @param[in] inode_list 入力ノードのリスト
// @param[in] fanout_num ファンアウト数
// @param[in] alloc メモリアロケータ
// @return 作成したノードを返す．
TpgNode*
TpgNode::make_logic(ymuint id,
		    const string& name,
		    GateType gate_type,
		    const vector<TpgNode*>& inode_list,
		    ymuint fanout_num,
		    Alloc& alloc)
{
  void* p;
  ymuint ni = inode_list.size();
  TpgNode* node = nullptr;
  switch ( gate_type ) {
  case kGateCONST0:
    ASSERT_COND( ni == 0 );

    p = alloc.get_memory(sizeof(TpgLogicC0));
    node = new (p) TpgLogicC0(id);
    break;

  case kGateCONST1:
    ASSERT_COND( ni == 0 );

    p = alloc.get_memory(sizeof(TpgLogicC1));
    node = new (p) TpgLogicC1(id);
    break;

  case kGateBUFF:
    ASSERT_COND( ni == 1 );

    p = alloc.get_memory(sizeof(TpgLogicBUFF));
    node = new (p) TpgLogicBUFF(id, inode_list[0]);
    break;

  case kGateNOT:
    ASSERT_COND( ni == 1 );

    p = alloc.get_memory(sizeof(TpgLogicNOT));
    node = new (p) TpgLogicNOT(id, inode_list[0]);
    break;

  case kGateAND:
    switch ( ni ) {
    case 2:
      p = alloc.get_memory(sizeof(TpgLogicAND2));
      node = new (p) TpgLogicAND2(id, inode_list);
      break;

    case 3:
      p = alloc.get_memory(sizeof(TpgLogicAND3));
      node = new (p) TpgLogicAND3(id, inode_list);
      break;

    case 4:
      p = alloc.get_memory(sizeof(TpgLogicAND4));
      node = new (p) TpgLogicAND4(id, inode_list);
      break;

    default:
      p = alloc.get_memory(sizeof(TpgLogicANDN));
      node = new (p) TpgLogicANDN(id);
      node->set_fanin(inode_list, alloc);
      break;
    }
    break;

  case kGateNAND:
    switch ( ni ) {
    case 2:
      p = alloc.get_memory(sizeof(TpgLogicNAND2));
      node = new (p) TpgLogicNAND2(id, inode_list);
      break;

    case 3:
      p = alloc.get_memory(sizeof(TpgLogicNAND3));
      node = new (p) TpgLogicNAND3(id, inode_list);
      break;

    case 4:
      p = alloc.get_memory(sizeof(TpgLogicNAND4));
      node = new (p) TpgLogicNAND4(id, inode_list);
      break;

    default:
      p = alloc.get_memory(sizeof(TpgLogicNANDN));
      node = new (p) TpgLogicNANDN(id);
      node->set_fanin(inode_list, alloc);
      break;
    }
    break;

  case kGateOR:
    switch ( ni ) {
    case 2:
      p = alloc.get_memory(sizeof(TpgLogicOR2));
      node = new (p) TpgLogicOR2(id, inode_list);
      break;

    case 3:
      p = alloc.get_memory(sizeof(TpgLogicOR3));
      node = new (p) TpgLogicOR3(id, inode_list);
      break;

    case 4:
      p = alloc.get_memory(sizeof(TpgLogicOR4));
      node = new (p) TpgLogicOR4(id, inode_list);
      break;

    default:
      p = alloc.get_memory(sizeof(TpgLogicORN));
      node = new (p) TpgLogicORN(id);
      node->set_fanin(inode_list, alloc);
      break;
    }
    break;

  case kGateNOR:
    switch ( ni ) {
    case 2:
      p = alloc.get_memory(sizeof(TpgLogicNOR2));
      node = new (p) TpgLogicNOR2(id, inode_list);
      break;

    case 3:
      p = alloc.get_memory(sizeof(TpgLogicNOR3));
      node = new (p) TpgLogicNOR3(id, inode_list);
      break;

    case 4:
      p = alloc.get_memory(sizeof(TpgLogicNOR4));
      node = new (p) TpgLogicNOR4(id, inode_list);
      break;

    default:
      p = alloc.get_memory(sizeof(TpgLogicNORN));
      node = new (p) TpgLogicNORN(id);
      node->set_fanin(inode_list, alloc);
      break;
    }
    break;

  case kGateXOR:
    ASSERT_COND( ni == 2 );

    p = alloc.get_memory(sizeof(TpgLogicXOR2));
    node = new (p) TpgLogicXOR2(id, inode_list);
    break;

  case kGateXNOR:
    ASSERT_COND( ni == 2 );

    p = alloc.get_memory(sizeof(TpgLogicXNOR2));
    node = new (p) TpgLogicXNOR2(id, inode_list);
    break;

  default:
    // kGateCPLX というタイプはない．
    ASSERT_NOT_REACHED;
  }
  node->set_name(name, alloc);
  node->set_fanout_num(fanout_num, alloc);

  return node;
}

// @brief コンストラクタ
// @param[in] id ID番号
TpgNode::TpgNode(ymuint id) :
  mId(id),
  mName(nullptr),
  mFanoutNum(0),
  mFanoutList(nullptr),
  mFfr(nullptr),
  mMffc(nullptr),
  mImmDom(nullptr),
  mFaultNum(0),
  mFaultList(nullptr)
{
}

// @brief デストラクタ
TpgNode::~TpgNode()
{
}

// @brief 外部入力タイプの時 true を返す．
bool
TpgNode::is_primary_input() const
{
  return false;
}

// @brief 外部出力タイプの時 true を返す．
bool
TpgNode::is_primary_output() const
{
  return false;
}

// @brief DFF の入力に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_input() const
{
  return false;
}

// @brief DFF の出力に接続している入力タイプの時 true を返す．
bool
TpgNode::is_dff_output() const
{
  return false;
}

// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_clock() const
{
  return false;
}

// @brief DFF のクリア端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_clear() const
{
  return false;
}

// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
bool
TpgNode::is_dff_preset() const
{
  return false;
}

// @brief 入力タイプの時 true を返す．
//
// 具体的には is_input() || is_dff_output()
bool
TpgNode::is_ppi() const
{
  return false;
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_output() || is_dff_input()
bool
TpgNode::is_ppo() const
{
  return false;
}

// @brief logic タイプの時 true を返す．
bool
TpgNode::is_logic() const
{
  return false;
}

// @brief 外部入力タイプの時に入力番号を返す．
//
// node = TpgNetwork::ppi(node->input_id()
// の関係を満たす．
// is_ppi() が false の場合の返り値は不定
ymuint
TpgNode::input_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 外部出力タイプの時に出力番号を返す．
//
// node = TpgNetwork::ppo(node->output_id())
// の関係を満たす．
// is_ppo() が false の場合の返り値は不定
ymuint
TpgNode::output_id() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
ymuint
TpgNode::output_id2() const
{
  ASSERT_NOT_REACHED;
  return 0;
}

// @brief 接続している DFF を返す．
//
// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
// の時に意味を持つ．
TpgDff*
TpgNode::dff() const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgNode::gate_type() const
{
  ASSERT_NOT_REACHED;
  return kGateCONST0;
}

// @brief controling value を得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::cval() const
{
  ASSERT_COND( is_ppo() );
  return kValX;
}

// @brief noncontroling valueを得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::nval() const
{
  ASSERT_COND( is_ppo() );
  return kValX;
}

// @brief controling output value を得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::coval() const
{
  ASSERT_COND( is_ppo() );
  return kValX;;
}

// @brief noncontroling output value を得る．
// @note ない場合は kValX を返す．
Val3
TpgNode::noval() const
{
  ASSERT_COND( is_ppo() );
  return kValX;
}

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgNode::set_output_id2(ymuint id)
{
  ASSERT_NOT_REACHED;
}

// @brief ファンインを設定する．
// @param[in] inode_list ファンインのリスト
//
// と同時にファンイン用の配列も確保する．
// 多入力ゲートのみ意味を持つ仮想関数
void
TpgNode::set_fanin(const vector<TpgNode*>& inode_list,
		   Alloc& alloc)
{
  ASSERT_NOT_REACHED;
}

// @brief ファンアウトを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
// @param[in] fo_node ファンアウト先のノード
void
TpgNode::set_fanout(ymuint pos,
		    TpgNode* fo_node)
{
  ASSERT_COND( pos < fanout_num() );
  mFanoutList[pos] = fo_node;
}

// @brief immediate dominator をセットする．
// @param[in] dom dominator ノード
void
TpgNode::set_imm_dom(const TpgNode* dom)
{
  mImmDom = dom;
}

// @brief ノード名を設定する．
// @param[in] name ノード名
// @param[in] alloc メモリアロケータ
void
TpgNode::set_name(const string& name,
		  Alloc& alloc)
{
  // name が空文字列( l == 0 )でも正しく動く．
  ymuint l = name.size();
  void* p = alloc.get_memory(sizeof(char) * (l + 1));
  mName = new (p) char[l + 1];
  for (ymuint i = 0; i < l; ++ i) {
    mName[i] = name[i];
  }
  mName[l] = '\0';
}

// @brief ファンアウト数を設定する．
// @param[in] fanout_num
// @param[in] alloc メモリアロケータ
//
// 同時にファンアウト用の配列も確保する．
void
TpgNode::set_fanout_num(ymuint fanout_num,
			Alloc& alloc)
{
  mFanoutNum = fanout_num;
  if ( fanout_num > 0 ) {
    void* p = alloc.get_memory(sizeof(TpgNode*) * fanout_num);
    mFanoutList = new (p) TpgNode*[fanout_num];
  }
}

// @brief 故障リストを設定する．
void
TpgNode::set_fault_list(const vector<TpgFault*>& fault_list,
			Alloc& alloc)
{
  mFaultNum = fault_list.size();
  if ( mFaultNum > 0 ) {
    void* p = alloc.get_memory(sizeof(TpgFault*) * mFaultNum);
    mFaultList = new (p) TpgFault*[mFaultNum];
    for (ymuint i = 0; i < mFaultNum; ++ i) {
      mFaultList[i] = fault_list[i];
    }
  }
}

// @brief MFFC を設定する．
// @param[in] mffc このノードを根とするMFFC
void
TpgNode::set_mffc(const TpgMFFC* mffc)
{
  mMffc = mffc;
}

// @brief FFR を設定する．
// @param[in] ffr このノードが含まれるFFR
void
TpgNode::set_ffr(TpgFFR* ffr)
{
  mFfr = ffr;
}

// @brief このノードが持っている代表故障をリストに追加する．
void
TpgNode::add_to_fault_list(vector<TpgFault*>& fault_list)
{
  for (ymuint i = 0; i < mFaultNum; ++ i) {
    fault_list.push_back(mFaultList[i]);
  }
}

END_NAMESPACE_YM_SATPG
