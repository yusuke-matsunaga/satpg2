
/// @file BtJust2.cc
/// @brief BtJust2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BtJust2.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス BtJust2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] td_mode 遷移故障モードの時 true にするフラグ
// @param[in] val_map ノードの値を保持するクラス
BtJust2::BtJust2(ymuint max_id,
		 bool td_mode,
		 const ValMap& val_map) :
  BtJustBase(max_id, td_mode, val_map),
  mAlloc(sizeof(NodeList), 1024),
  mJustArray(max_id, nullptr),
  mJust0Array(max_id, nullptr),
  mTfoMark(max_id, false)
{
}

// @brief デストラクタ
BtJust2::~BtJust2()
{
}

// @brief バックトレースを行なう．
// @param[in] ffr_root 故障のあるFFRの根のノード
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
void
BtJust2::run(const TpgNode* ffr_root,
	     const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     NodeValList& pi_assign_list)
{
  mark_tfo(ffr_root);

  pi_assign_list.clear();

  // assign_list の値を正当化する．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    NodeList* node_list = nullptr;
    if ( td_mode() ) {
      if ( nv.time() == 0 ) {
	node_list = justify0(node);
      }
      else {
	node_list = justify(node);
      }
    }
    else {
      node_list = justify(node);
    }
  }

  // 故障差の伝搬している外部出力を選ぶ．
  ymuint nmin = 0;
  NodeList* best_list = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( gval(node) != fval(node) ) {
      // 正当化を行う．
      NodeList* node_list = justify(node);
      ymuint n = list_size(node_list);
      if ( nmin == 0 || nmin > n ) {
	nmin = n;
	best_list = node_list;
      }
    }
  }
  ASSERT_COND( nmin > 0 );

  for (NodeList* tmp = best_list; tmp; tmp = tmp->mLink) {
    const TpgNode* node = tmp->mNode;
    int time = tmp->mTime;
    record_value(node, time, pi_assign_list);
  }
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
BtJust2::NodeList*
BtJust2::justify(const TpgNode* node)
{
  if ( justified_mark(node) ) {
    return mJustArray[node->id()];
  }
  set_justified(node);

  if ( node->is_ppi() ) {
    if ( td_mode() ) {
      if ( node->is_primary_input() ) {
	// val を記録
	mJustArray[node->id()] = new_list_cell(node, 1);
	return mJustArray[node->id()];
      }

#warning "TODO: node->is_dff_output() の処理"
    }
    else {
      // val を記録
      mJustArray[node->id()] = new_list_cell(node, 0);
      return mJustArray[node->id()];
    }
  }

  Val3 gval = this->gval(node);
  Val3 fval = this->fval(node);

  if ( mTfoMark[node->id()] && gval != fval ) {
    // 正常値と故障値が異なっていたら
    // すべてのファンインをたどる．
    return just_sub1(node);
  }

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just_sub1(node);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just_sub1(node);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
BtJust2::NodeList*
BtJust2::just_sub1(const TpgNode* node)
{
  NodeList*& node_list = mJustArray[node->id()];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify(inode);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val 値
BtJust2::NodeList*
BtJust2::just_sub2(const TpgNode* node,
		   Val3 val)
{
  ymuint ni = node->fanin_num();
  // まず gval と fval が等しい場合を探す．
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode);
    Val3 ifval = mTfoMark[node->id()] ? fval(inode) : gval(inode);
    if ( igval != ifval || igval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode);
    ymuint n = list_size(node_list1);
    if ( min == 0 || min > n ) {
      pos = i;
      min = n;
    }
  }
  if ( pos < ni ) {
    NodeList*& node_list = mJustArray[node->id()];
    list_merge(node_list, mJustArray[node->fanin(pos)->id()]);
    return node_list;
  }

  ASSERT_COND( mTfoMark[node->id()] );

  ymuint gpos = ni;
  ymuint fpos = ni;
  ymuint gmin = 0;
  ymuint fmin = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode);
    Val3 ifval = fval(inode);
    if ( igval != val && ifval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode);
    ymuint n = list_size(node_list1);
    if ( igval == val ) {
      if ( gmin == 0 || gmin > n ) {
	gpos = i;
	gmin = n;
      }
    }
    if ( ifval == val ) {
      if ( fmin == 0 || fmin > n ) {
	fpos = i;
	fmin = n;
      }
    }
  }
  ASSERT_COND( gpos < ni );
  ASSERT_COND( fpos < ni );
  ASSERT_COND( gpos != fpos );
  NodeList*& node_list = mJustArray[node->id()];
  list_merge(node_list, mJustArray[node->fanin(gpos)->id()]);
  list_merge(node_list, mJustArray[node->fanin(fpos)->id()]);
  return node_list;
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
BtJust2::NodeList*
BtJust2::justify0(const TpgNode* node)
{
  if ( justified0_mark(node) ) {
    return mJust0Array[node->id()];
  }
  set_justified0(node);

  if ( node->is_ppi() ) {
    // val を記録
    mJust0Array[node->id()] = new_list_cell(node, 0);
    return mJust0Array[node->id()];
  }

  Val3 gval = this->gval(node, 0);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just0_sub1(node);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just0_sub2(node, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just0_sub2(node, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just0_sub2(node, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just0_sub2(node, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just0_sub1(node);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
BtJust2::NodeList*
BtJust2::just0_sub1(const TpgNode* node)
{
  NodeList*& node_list = mJust0Array[node->id()];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify0(inode);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val 値
BtJust2::NodeList*
BtJust2::just0_sub2(const TpgNode* node,
		    Val3 val)
{
  ymuint ni = node->fanin_num();
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = gval(inode, 0);
    if ( igval != val ) {
      continue;
    }
    NodeList* node_list1 = justify0(inode);
    ymuint n = list_size(node_list1);
    if ( min == 0 || min > n ) {
      pos = i;
      min = n;
    }
  }
  ASSERT_COND( pos < ni );

  NodeList*& node_list = mJust0Array[node->id()];
  list_merge(node_list, mJust0Array[node->fanin(pos)->id()]);
  return node_list;
}

// @brief 新しいリストのセルを返す．
BtJust2::NodeList*
BtJust2::new_list_cell(const TpgNode* node,
		       int time)
{
  void* p = mAlloc.get_memory(sizeof(NodeList));
  NodeList* tmp = new (p) NodeList;
  tmp->mNode = node;
  tmp->mTime = time;
  tmp->mLink = nullptr;
  return tmp;
}

// @brief リストをマージする．
void
BtJust2::list_merge(NodeList*& dst_list,
		    NodeList* src_list)
{
  NodeList** pdst = &dst_list;
  NodeList* src = src_list;
  while ( *pdst != nullptr && src != nullptr ) {
    if ( (*pdst)->mNode->id() < src->mNode->id() ) {
      pdst = &(*pdst)->mLink;
    }
    else if ( (*pdst)->mNode->id() > src->mNode->id() ) {
      NodeList* tmp = new_list_cell(src->mNode, src->mTime);
      NodeList* next = *pdst;
      *pdst = tmp;
      tmp->mLink = next;
      pdst = &tmp->mLink;
      src = src->mLink;
    }
    else {
      pdst = &(*pdst)->mLink;
      src = src->mLink;
    }
  }
  for ( ; src != nullptr; src = src->mLink) {
    NodeList* tmp = new_list_cell(src->mNode, src->mTime);
    *pdst = tmp;
    pdst = &tmp->mLink;
  }
}

// @brief リストのサイズを返す．
ymuint
BtJust2::list_size(NodeList* node_list)
{
  ymuint n = 0;
  for (NodeList* tmp = node_list; tmp; tmp = tmp->mLink) {
    ++ n;
  }
  return n;
}

// @brief リストを削除する．
void
BtJust2::list_free(NodeList* node_list)
{
  for (NodeList* tmp = node_list; tmp; ) {
    NodeList* next = tmp->mLink;
    mAlloc.put_memory(sizeof(NodeList), tmp);
    tmp = next;
  }
}

// @brief ノードの TFO に印をつける．
void
BtJust2::mark_tfo(const TpgNode* node)
{
  if ( mTfoMark[node->id()] ) {
    return;
  }
  mTfoMark[node->id()] = true;

  ymuint nfo = node->fanout_num();
  for (ymuint i = 0; i < nfo; ++ i) {
    const TpgNode* onode = node->fanout(i);
    mark_tfo(onode);
  }
}

END_NAMESPACE_YM_SATPG
