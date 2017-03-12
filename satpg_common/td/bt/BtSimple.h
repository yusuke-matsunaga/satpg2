#ifndef BTSIMPLE_H
#define BTSIMPLE_H

/// @file BtSimple.h
/// @brief BtSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "BtImpl.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class BtSimple BtSimple.h "BtSimple.h"
/// @brief 簡単な BackTracer
//////////////////////////////////////////////////////////////////////
class BtSimple :
  public BtImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  /// @param[in] val_map ノードの値を保持するクラス
  BtSimple(ymuint max_id,
	   const ValMap& val_map);

  /// @brief デストラクタ
  virtual
  ~BtSimple();


public:
  //////////////////////////////////////////////////////////////////////
  // BtSimple の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
  /// @param[in] ffr_root 故障のあるFFRの根のノード
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] output_list 故障に関係する出力ノードのリスト
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
      NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief node のファンインのうち外部入力を記録する．
  /// @param[in] node ノード
  /// @param[in] val_map 値のマップ
  /// @param[out] assign_list 値割当の結果を入れるリスト
  void
  tfi_recur(const TpgNode* node,
	    NodeValList& assign_list);

  /// @brief node のファンインのうち外部入力を記録する．
  /// @param[in] node ノード
  /// @param[in] val_map 値のマップ
  /// @param[out] assign_list 値割当の結果を入れるリスト
  void
  tfi_recur0(const TpgNode* node,
	     NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxId;

  // tfi_recur/tfi_recur0 で用いるマークの配列
  vector<ymuint8> mMarkArray;

};

END_NAMESPACE_YM_SATPG_TD

#endif // BTSIMPLE_H
