#ifndef BTIMPL_H
#define BTIMPL_H

/// @file BtImpl.h
/// @brief BtImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/BackTracer.h"
#include "NodeValList.h"
#include "ValMap.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class BtImpl BtImpl.h "BtImpl.h"
/// @brief BackTracer の基本クラス
//////////////////////////////////////////////////////////////////////
class BtImpl
{
public:

  /// @brief コンストラクタ
  BtImpl();

  /// @brief デストラクタ
  virtual
  ~BtImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // Bt2Impl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードID番号の最大値を設定する．
  /// @param[in] max_id ID番号の最大値
  ///
  /// このクラスの実装ではなにもしない．
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
      NodeValList& pi_assign_list) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードの値を記録する．
  /// @param[in] node 対象の外部入力ノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] time 時刻 (0 or 1)
  /// @param[out] assign_list 値の割当リスト
  static
  void
  record_value(const TpgNode* node,
	       const ValMap& val_map,
	       int time,
	       NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 入力ノードの値を記録する．
// @param[in] node 対象の外部入力ノード
// @param[in] val_map ノードの値の割当を保持するクラス
// @param[in] time 時刻 (0 or 1)
// @param[out] assign_list 値の割当リスト
inline
void
BtImpl::record_value(const TpgNode* node,
		     const ValMap& val_map,
		     int time,
		     NodeValList& assign_list)
{
  Val3 v = val_map.gval(node, time);
  if ( v != kValX ) {
    bool bval = (v == kVal1);
    assign_list.add(node, time, bval);
  }
}

END_NAMESPACE_YM_SATPG_TD

#endif // BTIMPL_H
