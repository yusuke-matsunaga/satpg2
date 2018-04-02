#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/Alloc.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFault
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR に含まれる代表故障のリスト
/// 一度設定された不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgFFR
{
public:

  /// @brief コンストラクタ
  TpgFFR();

  /// @brief デストラクタ
  ~TpgFFR();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const;

  /// @brief このFFRに含まれる代表故障の数を返す．
  int
  fault_num() const;

  /// @brief このFFRに含まれる代表故障のリストを返す．
  Array<const TpgFault*>
  fault_list() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  /// @param[in] root 根のノード
  /// @param[in] fault_list 故障のリスト
  /// @param[in] alloc メモリアロケータ
  void
  set(const TpgNode* root,
      vector<TpgFault*>& fault_list,
      Alloc& alloc);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  const TpgNode* mRoot;

  // 故障数
  int mFaultNum;

  // 故障の配列
  const TpgFault** mFaultList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
TpgFFR::TpgFFR()
{
  mRoot = nullptr;
  mFaultNum = 0;
  mFaultList = nullptr;
}

// @brief デストラクタ
inline
TpgFFR::~TpgFFR()
{
}

// @brief 根のノードを返す．
inline
const TpgNode*
TpgFFR::root() const
{
  return mRoot;
}

// @brief このFFRに含まれる代表故障の数を返す．
inline
int
TpgFFR::fault_num() const
{
  return mFaultNum;
}

// @brief このFFRに含まれる代表故障のリストを返す．
inline
Array<const TpgFault*>
TpgFFR::fault_list() const
{
  return Array<const TpgFault*>(mFaultList, 0, fault_num());
}

END_NAMESPACE_YM_SATPG

#endif // TPGFFR_H
