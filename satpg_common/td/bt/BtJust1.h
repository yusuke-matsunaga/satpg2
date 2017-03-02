#ifndef BTJUST1_H
#define BTJUST1_H

/// @file BtJust1.h
/// @brief BtJust1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BtJustBase.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class BtJust1 BtJust1.h "BtJust1.h"
/// @brief 必要なノードのみ正当化する BackTracer
//////////////////////////////////////////////////////////////////////
class BtJust1 :
  public BtJustBase
{
public:

  /// @brief コンストラクタ
  BtJust1();

  /// @brief デストラクタ
  virtual
  ~BtJust1();


public:
  //////////////////////////////////////////////////////////////////////
  // BackTracer の仮想関数
  //////////////////////////////////////////////////////////////////////

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
  // 継承クラスから用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[out] assign_list 値の割当リスト
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  void
  justify(const TpgNode* node,
	  const ValMap& val_map,
	  NodeValList& assign_list);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[out] assign_list 値の割当リスト
  void
  just_sub1(const TpgNode* node,
	    const ValMap& val_map,
	    NodeValList& assign_list);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  /// @param[out] assign_list 値の割当リスト
  void
  just_sub2(const TpgNode* node,
	    const ValMap& val_map,
	    Val3 val,
	    NodeValList& assign_list);

  /// @brief solve 中で変数割り当ての正当化を行なう．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[out] assign_list 値の割当リスト
  /// @note node の値割り当てを正当化する．
  /// @note 正当化に用いられているノードには mark3 がつく．
  /// @note mark3 がついたノードは mBwdNodeList に格納される．
  void
  justify0(const TpgNode* node,
	   const ValMap& val_map,
	   NodeValList& assign_list);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[out] assign_list 値の割当リスト
  void
  just0_sub1(const TpgNode* node,
	     const ValMap& val_map,
	     NodeValList& assign_list);

  /// @brief 指定した値を持つのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] val 値
  /// @param[out] assign_list 値の割当リスト
  void
  just0_sub2(const TpgNode* node,
	     const ValMap& val_map,
	     Val3 val,
	     NodeValList& assign_list);

};

END_NAMESPACE_YM_SATPG_TD

#endif // BTJUST1_H
