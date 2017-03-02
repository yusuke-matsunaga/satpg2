
/// @file BtJust2.cc
/// @brief BtJust2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BtJust2.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
// クラス BtJust2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BtJust2::BtJust2() :
  mAlloc(sizeof(NodeList), 1024)
{
}

// @brief デストラクタ
BtJust2::~BtJust2()
{
}

// @brief ノードID番号の最大値を設定する．
// @param[in] max_id ID番号の最大値
void
BtJust2::set_max_id(ymuint max_id)
{
  BtJustBase::set_max_id(max_id);
  mMaxId = max_id;
  mJustArray.clear();
  mJustArray.resize(max_id, nullptr);
  mJust0Array.clear();
  mJust0Array.resize(max_id, nullptr);
}

// @brief バックトレースを行なう．
// @param[in] ffr_root 故障のあるFFRの根のノード
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[in] val_map ノードの値を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
//
// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
// を入れる．
// val_map には ffr_root のファンアウトコーン上の故障値と関係する
// 回路全体の正常値が入っている．
void
BtJust2::run(const TpgNode* ffr_root,
	     const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list)
{
  mTfoMark.clear();
  mTfoMark.resize(mMaxId, false);
  mark_tfo(ffr_root);

  pi_assign_list.clear();

  // assign_list の値を正当化する．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    NodeList* node_list = nullptr;
    if ( nv.time() == 0 ) {
      node_list = justify0(node, val_map);
    }
    else {
      node_list = justify(node, val_map);
    }
  }

  // 故障差の伝搬している外部出力を選ぶ．
  ymuint nmin = 0;
  NodeList* best_list = nullptr;
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( val_map.gval(node) != val_map.fval(node) ) {
      // 正当化を行う．
      NodeList* node_list = justify(node, val_map);
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
    record_value(node, val_map, time, pi_assign_list);
  }

  // 一連の処理でつけたマークを消す．
  clear_justified();
}

// @brief clear_justified() 中で呼ばれるフック関数
void
BtJust2::clear_justified_hook(const TpgNode* node)
{
  ASSERT_COND( node->id() < mJustArray.size() );
  list_free(mJustArray[node->id()]);
  mJustArray[node->id()] = nullptr;
}

// @brief solve 中で変数割り当ての正当化を行なう．
// @param[in] node 対象のノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
BtJust2::NodeList*
BtJust2::justify(const TpgNode* node,
		 const ValMap& val_map)
{
  if ( justified_mark(node) ) {
    return mJustArray[node->id()];
  }
  set_justified(node);

  if ( node->is_primary_input() ) {
    // val を記録
    mJustArray[node->id()] = new_list_cell(node, 1);
    return mJustArray[node->id()];
  }

#warning "TODO: node->is_dff_output() の処理"

  Val3 gval = val_map.gval(node);
  Val3 fval = val_map.fval(node);

  if ( mTfoMark[node->id()] && gval != fval ) {
    // 正常値と故障値が異なっていたら
    // すべてのファンインをたどる．
    return just_sub1(node, val_map);
  }

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just_sub1(node, val_map);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node, val_map);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, val_map, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just_sub2(node, val_map, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node, val_map);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, val_map, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node, val_map);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just_sub1(node, val_map);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just_sub2(node, val_map, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just_sub1(node, val_map);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map SATの値の割り当て結果を収めた配列
BtJust2::NodeList*
BtJust2::just_sub1(const TpgNode* node,
		   const ValMap& val_map)
{
  NodeList*& node_list = mJustArray[node->id()];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify(inode, val_map);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map SATの値の割り当て結果を収めた配列
// @param[in] val 値
BtJust2::NodeList*
BtJust2::just_sub2(const TpgNode* node,
		   const ValMap& val_map,
		   Val3 val)
{
  ymuint ni = node->fanin_num();
  // まず gval と fval が等しい場合を探す．
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = val_map.gval(inode);
    Val3 ifval = mTfoMark[node->id()] ? val_map.fval(inode) : val_map.gval(inode);
    if ( igval != ifval || igval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode, val_map);
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
    Val3 igval = val_map.gval(inode);
    Val3 ifval = val_map.fval(inode);
    if ( igval != val && ifval != val ) {
      continue;
    }
    NodeList* node_list1 = justify(inode, val_map);
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
// @param[in] val_map ノードの値の割当を保持するクラス
// @note node の値割り当てを正当化する．
// @note 正当化に用いられているノードには mJustifiedMark がつく．
// @note mJustifiedMmark がついたノードは mJustifiedNodeList に格納される．
BtJust2::NodeList*
BtJust2::justify0(const TpgNode* node,
		  const ValMap& val_map)
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

  Val3 gval = val_map.gval(node, 0);

  switch ( node->gate_type() ) {
  case kGateBUFF:
  case kGateNOT:
    // 無条件で唯一のファンインをたどる．
    return just0_sub1(node, val_map);

  case kGateAND:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node, val_map);
    }
    else if ( gval == kVal0 ) {
      // 0の値を持つ最初のノードをたどる．
      return just0_sub2(node, val_map, kVal0);
    }
    break;

  case kGateNAND:
    if ( gval == kVal1 ) {
      // 0の値を持つ最初のノードをたどる．
      return just0_sub2(node, val_map, kVal0);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node, val_map);
    }
    break;

  case kGateOR:
    if ( gval == kVal1 ) {
      // 1の値を持つ最初のノードをたどる．
      return just0_sub2(node, val_map, kVal1);
    }
    else if ( gval == kVal0 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node, val_map);
    }
    break;

  case kGateNOR:
    if ( gval == kVal1 ) {
      // すべてのファンインノードをたどる．
      return just0_sub1(node, val_map);
    }
    else if ( gval == kVal0 ) {
      // 1の値を持つ最初のノードをたどる．
      return just0_sub2(node, val_map, kVal1);
    }
    break;

  case kGateXOR:
  case kGateXNOR:
    // すべてのファンインノードをたどる．
    return just0_sub1(node, val_map);
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }

  return nullptr;
}

// @brief すべてのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map SATの値の割り当て結果を収めた配列
BtJust2::NodeList*
BtJust2::just0_sub1(const TpgNode* node,
		    const ValMap& val_map)
{
  NodeList*& node_list = mJust0Array[node->id()];
  ymuint ni = node->fanin_num();
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    NodeList* node_list1 = justify0(inode, val_map);
    list_merge(node_list, node_list1);
  }
  return node_list;
}

// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
// @param[in] node 対象のノード
// @param[in] val_map SATの値の割り当て結果を収めた配列
// @param[in] val 値
BtJust2::NodeList*
BtJust2::just0_sub2(const TpgNode* node,
		    const ValMap& val_map,
		    Val3 val)
{
  ymuint ni = node->fanin_num();
  ymuint pos = ni;
  ymuint min = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    const TpgNode* inode = node->fanin(i);
    Val3 igval = val_map.gval(inode, 0);
    if ( igval != val ) {
      continue;
    }
    NodeList* node_list1 = justify0(inode, val_map);
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

END_NAMESPACE_YM_SATPG_TD
