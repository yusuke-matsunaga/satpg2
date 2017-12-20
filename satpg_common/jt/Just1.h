#ifndef JUST1_H
#define JUST1_H

/// @file Just1.h
/// @brief Just1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"
#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Just1 Just1.h "td/Just1.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Just1 :
  public JustBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] td_mode 遷移故障モードの時 true にするフラグ
  /// @param[in] max_id ID番号の最大値
  Just1(bool td_mode,
	ymuint max_id);

  /// @brief デストラクタ
  virtual
  ~Just1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] val_map ノードの値を保持するクラス
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  virtual
  void
  operator()(const NodeValList& assign_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  justify(const TpgNode* node,
	  int time,
	  NodeValList& pi_assign_list);

  /// @brief すべてのファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  just_all(const TpgNode* node,
	   int time,
	   NodeValList& pi_assign_list);

  /// @brief 指定した値を持つファンインに対して justify() を呼ぶ．
  /// @param[in] node 対象のノード
  /// @param[in] val 値
  /// @param[in] time タイムフレーム ( 0 or 1 )
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  void
  just_one(const TpgNode* node,
	   Val3 val,
	   int time,
	   NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // JUST1_H
