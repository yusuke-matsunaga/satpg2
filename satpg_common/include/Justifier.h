#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

class JustImpl;

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "td/Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief コンストラクタ
  /// @param[in] type 種類を表す文字列
  /// @param[in] max_id ID番号の最大値
  Justifier(const string& type,
	    int max_id);

  /// @brief デストラクタ
  ~Justifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var_map 変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @param[out] testvect テストベクタ
  void
  operator()(const NodeValList& assign_list,
	     const VidMap& var_map,
	     const vector<SatBool3>& model,
	     TestVector& testvect);

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var1_map 1時刻目の変数番号のマップ
  /// @param[in] var2_map 2時刻目の変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @param[out] testvect テストベクタ
  void
  operator()(const NodeValList& assign_list,
	     const VidMap& var1_map,
	     const VidMap& var2_map,
	     const vector<SatBool3>& model,
	     TestVector& testvect);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラス
  std::unique_ptr<JustImpl> mImpl;

};

END_NAMESPACE_YM_SATPG

#endif // JUSTIFIER_H
