#ifndef SA_BACKTRACER_H
#define SA_BACKTRACER_H

/// @file sa/BackTracer.h
/// @brief BackTracer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2015 Yusuke Matsunaga
/// All rights reserved.


#include "sa/sa_nsdef.h"


BEGIN_NAMESPACE_YM_SATPG_SA

class BtImpl;

//////////////////////////////////////////////////////////////////////
/// @class BackTracer BackTracer.h "sa/BackTracer.h"
/// @brief テストパタンを求めるためのバックトレースを行なうファンクター
///
/// 通常のテスト生成におけるバックトレースはあとで失敗してバックトラック
/// する可能性があるが，これはすでに充足した割り当てがある時に，その中で
/// 必要な割り当てを選ぶものなので結果の良し悪しはともかく失敗はしない．
/// ただし，割り当て結果は一意には決まらないのでいくつかのアルゴリズムが
/// 考えられる．
/// そこでコンストラクタの引数 xmode で選択できるようになっている
/// 現在，実装されているアルゴリズムは以下の通り
/// - BtSimple: 単純に故障の影響のあるファンアウトコーンのファンインに
///   含まれるすべての外部入力の値を記録する．
///   ある意味最大の割り当て集合を求めるアルゴリズム
/// - BtJust1: ANDゲート/ORゲートなどの機能を考えて出力値が正当化される
///   最低限の割り当てのみを記録する．
///   ただし，最初に見つかった正当化割り当てを用いる．
/// - BtJust2: 正当化割り当てを記録するところは BtJust1 と同じだが，
///   複数の割り当てがある場合にその時点で最も要素数の少ない割り当てを選ぶ．
///   ただし単なる greedy なので最小解の保証はない．
//////////////////////////////////////////////////////////////////////
class BackTracer
{
public:

  /// @brief コンストラクタ
  /// @param[in] xmode モード
  /// @param[in] max_id ID番号の最大値
  BackTracer(ymuint xmode,
	     ymuint max_id);

  /// @brief デストラクタ
  ~BackTracer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
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
  void
  operator()(const TpgNode* ffr_root,
	     const NodeValList& assign_list,
	     const vector<const TpgNode*>& output_list,
	     const ValMap& val_map,
	     NodeValList& pi_assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際の処理を行うクラス
  BtImpl* mImpl;

};

END_NAMESPACE_YM_SATPG_SA

#endif // SA_BACKTRACER_H
