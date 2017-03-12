#ifndef BTJUSTBASE_H
#define BTJUSTBASE_H

/// @file BtJustBase.h
/// @brief BtJustBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014 Yusuke Matsunaga
/// All rights reserved.


#include "BtImpl.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class BtJustBase BtJustBase.h "BtJustBase.h"
/// @brief 必要なノードのみ正当化する BackTracer の基底クラス
//////////////////////////////////////////////////////////////////////
class BtJustBase :
  public BtImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_id ノード番号の最大値
  /// @param[in] val_map ノードの値を保持するクラス
  BtJustBase(ymuint max_id,
	     const ValMap& val_map);

  /// @brief デストラクタ
  virtual
  ~BtJustBase();


public:
  //////////////////////////////////////////////////////////////////////
  // BtImpl の仮想関数
  //////////////////////////////////////////////////////////////////////


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  void
  set_justified(const TpgNode* node);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  bool
  justified_mark(const TpgNode* node);

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  void
  set_justified0(const TpgNode* node);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  bool
  justified0_mark(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
// @param[in] val_map ノードの値を保持するクラス
inline
BtJustBase::BtJustBase(ymuint max_id,
		       const ValMap& val_map) :
  BtImpl(val_map),
  mMarkArray(max_id, 0U)
{
}

// @brief デストラクタ
inline
BtJustBase::~BtJustBase()
{
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
inline
void
BtJustBase::set_justified(const TpgNode* node)
{
  mMarkArray[node->id()] |= 1U;
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
inline
bool
BtJustBase::justified_mark(const TpgNode* node)
{
  return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
inline
void
BtJustBase::set_justified0(const TpgNode* node)
{
  mMarkArray[node->id()] |= 2U;
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
inline
bool
BtJustBase::justified0_mark(const TpgNode* node)
{
  return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
}

END_NAMESPACE_YM_SATPG_TD

#endif // BTJUSTBASE_H
