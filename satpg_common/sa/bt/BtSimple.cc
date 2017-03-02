
/// @file BtSimple.cc
/// @brief BtSimple の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014 Yusuke Matsunaga
/// All rights reserved.


#include "BtSimple.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
// クラス BtSimple
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
BtSimple::BtSimple()
{
}

// @brief ノードID番号の最大値を設定する．
// @param[in] max_id ID番号の最大値
//
// このクラスの実装ではなにもしない．
void
BtSimple::set_max_id(ymuint max_id)
{
  mMaxId = max_id;
}

// @brief バックトレースを行なう．
// @param[in] fnode 故障のあるノード
// @param[in] assign_list 値の割り当てリスト
// @param[in] output_list 故障に関係する出力ノードのリスト
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[out] pi_assign_list 外部入力上の値の割当リスト
void
BtSimple::run(const TpgNode* fnode,
	      const NodeValList& assign_list,
	      const vector<const TpgNode*>& output_list,
	      const ValMap& val_map,
	      NodeValList& pi_assign_list)
{
  pi_assign_list.clear();

  // 故障の影響が伝搬している output_list のファンインに含まれる入力ノードに印をつける．
  mMark.clear();
  mMark.resize(mMaxId, false);
  for (vector<const TpgNode*>::const_iterator p = output_list.begin();
       p != output_list.end(); ++ p) {
    const TpgNode* node = *p;
    if ( val_map.gval(node) != val_map.fval(node) ) {
      tfi_recur(node, val_map, pi_assign_list);
    }
  }

  // 念のため assign_list に含まれるノードの正当化を行っておく．
  // たぶんすでに処理済みのマークがついているので実質オーバーヘッドはない．
  for (ymuint i = 0; i < assign_list.size(); ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    tfi_recur(node, val_map, pi_assign_list);
  }
}

// @brief node のファンインのうち外部入力を記録する．
// @param[in] node ノード
// @param[in] val_map 値のマップ
// @param[out] assign_list 値割当の結果を入れるリスト
void
BtSimple::tfi_recur(const TpgNode* node,
		    const ValMap& val_map,
		    NodeValList& assign_list)
{
  if ( mMark[node->id()] ) {
    return;
  }
  mMark[node->id()] = true;

  if ( node->is_ppi() ) {
    record_value(node, val_map, assign_list);
  }
  else {
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      tfi_recur(inode, val_map, assign_list);
    }
  }
}

END_NAMESPACE_YM_SATPG_SA
