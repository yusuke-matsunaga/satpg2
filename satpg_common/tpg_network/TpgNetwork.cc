
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "AuxNodeInfo.h"

#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG

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

// @brief ノード名を得る．
// @param[in] id ID番号 ( 0 <= id < node_num() )
const char*
TpgNetwork::node_name(int id) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].name();
}

// @brief ノードに関係した代表故障数を返す．
// @param[in] id ID番号 ( 0 <= id < node_num() )
int
TpgNetwork::node_rep_fault_num(int id) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault_num();
}

// @brief ノードに関係した代表故障を返す．
// @param[in] id ID番号 ( 0 <= id < node_num() )
// @param[in] pos 位置番号 ( 0 <= pos < node_rep_fault_num(id) )
const TpgFault*
TpgNetwork::node_rep_fault(int id,
			   int pos) const
{
  ASSERT_COND( id >= 0 && id < node_num() );

  return mAuxInfoArray[id].fault(pos);
}

// @brief 出力の故障を得る．
// @param[in] id ノードID ( 0 <= id < node_num() )
// @param[in] val 故障値 ( 0 / 1 )
TpgFault*
TpgNetwork::_node_output_fault(int id,
			       int val)
{
  ASSERT_COND( id >= 0 && id < mNodeNum );

  return mAuxInfoArray[id].output_fault(val);
}

// @brief 入力の故障を得る．
// @param[in] id ノードID ( 0 <= id < node_num() )
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] pos 入力の位置番号
TpgFault*
TpgNetwork::_node_input_fault(int id,
			      int val,
			      int pos)
{
  ASSERT_COND( id >= 0 && id < mNodeNum );

  return mAuxInfoArray[id].input_fault(pos, val);
}

// @brief DFF を得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
const TpgDff*
TpgNetwork::dff(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < dff_num() );

  return &mDffArray[pos];
}

// @brief MFFC を返す．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
const TpgMFFC*
TpgNetwork::mffc(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < mffc_num() );

  return &mMffcArray[pos];
}

// @brief FFR を返す．
// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
const TpgFFR*
TpgNetwork::ffr(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < ffr_num() );

  return &mFfrArray[pos];
}

// @brief TpgNetwork の内容を出力する関数
// @param[in] s 出力先のストリーム
// @param[in] network 対象のネットワーク
void
print_network(ostream& s,
	      const TpgNetwork& network)
{
  int n = network.node_num();
  for ( int i = 0; i < n; ++ i ) {
    const TpgNode* node = network.node(i);
    print_node(s, network, node);
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
      print_node(s, network, inode);
    }
    else if ( node->is_dff_input() ) {
      s << "OUTPUT#" << node->output_id()
	<< "(DFF#" << node->dff()->id() << ".input)";
      const TpgNode* inode = node->fanin(0);
      s << " = ";
      print_node(s, network, inode);
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
      int ni = node->fanin_num();
      if ( ni > 0 ) {
	s << "(";
	for ( int j = 0; j < ni; ++ j ) {
	  const TpgNode* inode = node->fanin(j);
	  s << " ";
	  print_node(s, network, inode);
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
	   const TpgNetwork& network,
	   const TpgNode* node)
{
  s << "NODE#" << node->id() << ": " << network.node_name(node->id());
}

END_NAMESPACE_YM_SATPG
