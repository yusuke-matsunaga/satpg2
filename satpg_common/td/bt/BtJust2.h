#ifndef BTJUST2_H
#define BTJUST2_H

/// @file BtJust2.h
/// @brief BtJust2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014 Yusuke Matsunaga
/// All rights reserved.


#include "BtJustBase.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class BtJust2 BtJust2.h "BtJust2.h"
/// @brief 必要なノードのみ正当化する BackTracer
//////////////////////////////////////////////////////////////////////
class BtJust2 :
  public BtJustBase
{
public:

  /// @brief コンストラクタ
  BtJust2();

  /// @brief デストラクタ
  virtual
  ~BtJust2();


public:
  //////////////////////////////////////////////////////////////////////
  // BackTracer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID番号の最大値を設定する．
  /// @param[in] max_id ID番号の最大値
  virtual
  void
  set_max_id(ymuint max_id);

  /// @brief バックトレースを行なう．
  /// @param[in] ffr_root 故障のあるFFRの根のノード
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] output_list 故障に関係する出力ノードのリスト
  /// @param[in] val_map ノードの値を保持するクラス
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  ///
  /// assign_list には故障の活性化条件と ffr_root までの故障伝搬条件
  /// を入れる．
  /// val_map には ffr_root のファンアウトコーン上の故障値と関係する
  /// 回路全体の正常値が入っている．
  virtual
  void
  run(const TpgNode* ffr_root,
      const NodeValList& assign_list,
      const vector<const TpgNode*>& output_list,
      const ValMap& val_map,
      NodeValList& pi_assign_list);


protected:
  //////////////////////////////////////////////////////////////////////
  // BtJustBase の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief clear_justified() 中で呼ばれるフック関数
  virtual
  void
  clear_justified_hook(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いるデータ構造
  //////////////////////////////////////////////////////////////////////

  struct NodeList
  {
    const TpgNode* mNode;

    int mTime;

    NodeList* mLink;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  NodeList*
  justify(const TpgNode* node,
	  const ValMap& val_map);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  NodeList*
  just_sub1(const TpgNode* node,
	    const ValMap& val_map);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  NodeList*
  just_sub2(const TpgNode* node,
	    const ValMap& val_map,
	    Val3 val);

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  NodeList*
  justify0(const TpgNode* node,
	   const ValMap& val_map);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  NodeList*
  just0_sub1(const TpgNode* node,
	     const ValMap& val_map);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  NodeList*
  just0_sub2(const TpgNode* node,
	     const ValMap& val_map,
	     Val3 val);

  /// @brief 新しいリストのセルを返す．
  NodeList*
  new_list_cell(const TpgNode* node,
		int time);

  /// @brief リストをマージする．
  void
  list_merge(NodeList*& dst_list,
	     NodeList* src_list);

  /// @brief リストのサイズを返す．
  static
  ymuint
  list_size(NodeList* node_list);

  /// @brief リストを削除する．
  void
  list_free(NodeList* node_list);

  /// @brief ノードの TFO に印をつける．
  void
  mark_tfo(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // メモリ確保用のオブジェクト
  UnitAlloc mAlloc;

  // ノード番号の最大値
  ymuint mMaxId;

  // node->id() をキーにして入力番号のリストを納める配列
  vector<NodeList*> mJustArray;

  // node->id() をキーにして入力番号のリストを納める配列
  vector<NodeList*> mJust0Array;

  // 故障の TFO につけるマーク
  vector<bool> mTfoMark;

};

END_NAMESPACE_YM_SATPG_TD

#endif // BTJUST2_H
