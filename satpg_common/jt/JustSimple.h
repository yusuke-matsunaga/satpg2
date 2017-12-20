#ifndef JUSTSIMPLE_H
#define JUSTSIMPLE_H

/// @file JustSimple.h
/// @brief JustSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "JustBase.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class JustSimple JustSimple.h "JustSimple.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class JustSimple :
  public JustBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] td_mode 遷移故障モードの時 true にするフラグ
  /// @param[in] max_id ID番号の最大値
  JustSimple(bool td_mode,
	     ymuint max_id);

  /// @brief デストラクタ
  virtual
  ~JustSimple();


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


protected:
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // JUSTSIMPLE_H
