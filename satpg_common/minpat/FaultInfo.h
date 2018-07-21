#ifndef FAULTINFO_H
#define FAULTINFO_H

/// @file FaultInfo.h
/// @brief FaultInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class FaultInfo FaultInfo.h "FaultInfo.h"
/// @brief MinPat 用の故障情報を表すクラス
//////////////////////////////////////////////////////////////////////
class FaultInfo
{
public:

  /// @brief コンストラクタ
  /// @param[in] fault 対象の故障
  /// @param[in] sufficient_cond 十分条件(左辺値)
  FaultInfo(const TpgFault* fault,
	    const NodeValList& sufficient_cond);

  /// @brief コンストラクタ
  /// @param[in] fault 対象の故障
  /// @param[in] sufficient_cond 十分条件(右辺値)
  FaultInfo(const TpgFault* fault,
	    NodeValList&& sufficient_cond);

  /// @brief デストラクタ
  ~FaultInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を返す．
  const TpgFault*
  fault() const;

  /// @brief 十分条件の割当リストを返す．
  const NodeValList&
  sufficient_cond() const;

  /// @brief 被支配フラグを立てる．
  void
  set_dominated();

  /// @brief 被支配フラグを得る．
  bool
  is_dominated() const;

  /// @brief 衝突している故障を追加する．
  /// @param[in] fi 追加する故障情報
  void
  add_conflict(FaultInfo* fi);

  /// @brief 衝突リストを返す．
  const vector<FaultInfo*>&
  conflict_list() const;

  /// @brief 両立している故障を追加する．
  /// @param[in] fi 追加する故障情報
  void
  add_compatible(FaultInfo* fi);

  /// @brief 両立している故障リストを返す．
  const vector<FaultInfo*>&
  compatible_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障
  const TpgFault* mFault;

  // 十分条件
  NodeValList mSufficientCond;

  // 被支配フラグ
  bool mDominated;

  // 衝突している故障のリスト
  vector<FaultInfo*> mConflictList;

  // 両立している故障のリスト
  vector<FaultInfo*> mCompatibleList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fault 対象の故障
// @param[in] sufficient_cond 十分条件(左辺値)
inline
FaultInfo::FaultInfo(const TpgFault* fault,
		     const NodeValList& sufficient_cond) :
  mFault(fault),
  mSufficientCond(sufficient_cond),
  mDominated(false)
{
}

// @brief コンストラクタ
// @param[in] fault 対象の故障
// @param[in] sufficient_cond 十分条件(右辺値)
inline
FaultInfo::FaultInfo(const TpgFault* fault,
		     NodeValList&& sufficient_cond) :
  mFault(fault),
  mSufficientCond(sufficient_cond),
  mDominated(false)
{
}

// @brief デストラクタ
inline
FaultInfo::~FaultInfo()
{
}

// @brief 故障を返す．
inline
const TpgFault*
FaultInfo::fault() const
{
  return mFault;
}

// @brief 十分条件の割当リストを返す．
inline
const NodeValList&
FaultInfo::sufficient_cond() const
{
  return mSufficientCond;
}

// @brief 被支配フラグを立てる．
inline
void
FaultInfo::set_dominated()
{
  mDominated = true;
}

// @brief 被支配フラグを得る．
inline
bool
FaultInfo::is_dominated() const
{
  return mDominated;
}

// @brief 衝突している故障を追加する．
inline
void
FaultInfo::add_conflict(FaultInfo* fi)
{
  mConflictList.push_back(fi);
}

// @brief 衝突リストを返す．
inline
const vector<FaultInfo*>&
FaultInfo::conflict_list() const
{
  return mConflictList;
}

// @brief 両立している故障を追加する．
// @param[in] fi 追加する故障情報
inline
void
FaultInfo::add_compatible(FaultInfo* fi)
{
  mCompatibleList.push_back(fi);
}

// @brief 両立している故障リストを返す．
inline
const vector<FaultInfo*>&
FaultInfo::compatible_list() const
{
  return mCompatibleList;
}

END_NAMESPACE_YM_SATPG

#endif // FAULTINFO_H
