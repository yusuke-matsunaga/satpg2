#ifndef BTIMPL_H
#define BTIMPL_H

/// @file BtImpl.h
/// @brief BtImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "BackTracer.h"
#include "NodeValList.h"
#include "../dtpg/ValMap.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class BtImpl BtImpl.h "BtImpl.h"
/// @brief BackTracer の基本クラス
//////////////////////////////////////////////////////////////////////
class BtImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  /// @param[in] fault_type 故障の型
  BtImpl(ymuint max_id,
	 FaultType fault_type);

  /// @brief デストラクタ
  virtual
  ~BtImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
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
  run(const NodeValList& assign_list,
      const vector<const TpgNode*>& output_list,
      const ValMap& val_map,
      NodeValList& pi_assign_list);



private:
  //////////////////////////////////////////////////////////////////////
  // BtImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief バックトレースを行なう．
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
  _run(const NodeValList& assign_list,
       const vector<const TpgNode*>& output_list,
       NodeValList& pi_assign_list) = 0;

  /// @brief 処理の終了後に作業領域をクリアするためのフック関数
  /// @param[in] id ノード番号
  ///
  /// デフォルトの実装では何もしない．
  virtual
  void
  _clear_hook(ymuint id);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の型を返す．
  FaultType
  fault_type() const;

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  set_justified(const TpgNode* node,
		int time);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  bool
  justified_mark(const TpgNode* node,
		 int time);

  /// @brief ノードの正常値を返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  Val3
  gval(const TpgNode* node,
       int time) const;

  /// @brief ノードの故障地を返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  ///
  /// time = 0 のときは gval(node, 0) を返す．
  Val3
  fval(const TpgNode* node,
       int time) const;

  /// @brief 入力ノードの値を記録する．
  /// @param[in] node 対象の外部入力ノード
  /// @param[in] val_map ノードの値の割当を保持するクラス
  /// @param[in] time 時刻 (0 or 1)
  /// @param[out] assign_list 値の割当リスト
  void
  record_value(const TpgNode* node,
	       int time,
	       NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の型
  FaultType mFaultType;

  // ノードの値を保持するクラス
  const ValMap* mValMap;

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

  // mMarkArray に印を付けたノードの番号のリスト
  // クリアするために使う．
  vector<ymuint> mNodeIdList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 故障の型を返す．
inline
FaultType
BtImpl::fault_type() const
{
  return mFaultType;
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
void
BtImpl::set_justified(const TpgNode* node,
		      int time)
{
  // 念のため time の値を制限する．
  time &= 1;
  ymuint id = node->id();
  if ( mMarkArray[id] == 0U ) {
    // クリアのために値の変わったノード番号を記憶しておく．
    mNodeIdList.push_back(id);
  }
  mMarkArray[id] |= (1U << time);
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
bool
BtImpl::justified_mark(const TpgNode* node,
		       int time)
{
  // 念のため time の値を制限する．
  time &= 1;
  return static_cast<bool>((mMarkArray[node->id()] >> time) & 1U);
}

// @brief ノードの正常値を返す．
// @param[in] node ノード
// @param[in] time 時刻 ( 0 or 1 )
inline
Val3
BtImpl::gval(const TpgNode* node,
	     int time) const
{
  return mValMap->gval(node, time);
}

// @brief ノードの故障地を返す．
// @param[in] node ノード
// @param[in] time 時刻 ( 0 or 1 )
inline
Val3
BtImpl::fval(const TpgNode* node,
	     int time) const
{
  if ( time == 0 ) {
    return mValMap->gval(node, 0);
  }
  else {
    return mValMap->fval(node);
  }
}

// @brief 入力ノードの値を記録する．
// @param[in] node 対象の外部入力ノード
// @param[in] time 時刻 (0 or 1)
// @param[out] assign_list 値の割当リスト
inline
void
BtImpl::record_value(const TpgNode* node,
		     int time,
		     NodeValList& assign_list)
{
  Val3 v = gval(node, time);
  if ( v != kValX ) {
    bool bval = (v == kVal1);
    assign_list.add(node, time, bval);
  }
}

END_NAMESPACE_YM_SATPG

#endif // BTIMPL_H
