#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "td/Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief デストラクタ
  virtual
  ~Justifier() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] val_map 値の割り当て結果
  /// @param[out] pi_assign_list 外部入力上の値の割当リスト
  ///
  /// val_map は gval() しか用いない．
  virtual
  void
  operator()(const NodeValList& assign_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list) = 0;

};

/// @brief JustSimple を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_JustSimple(bool td_mode,
	       ymuint max_id);

/// @brief Just1 を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just1(bool td_mode,
	  ymuint max_id);

/// @brief Just2 を生成する．
/// @param[in] td_mode 遷移故障モードの時 true にするフラグ
/// @param[in] max_id ID番号の最大値
extern
Justifier*
new_Just2(bool td_mode,
	  ymuint max_id);

END_NAMESPACE_YM_SATPG

#endif // JUSTIFIER_H
