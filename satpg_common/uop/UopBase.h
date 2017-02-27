﻿#ifndef UOPBASE_H
#define UOPBASE_H

/// @file UopBase.h
/// @brief UopBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "UntestOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class UopBase UopBase.h "UopBase.h"
/// @brief UntestOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class UopBase :
  public UntestOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fmgr 故障マネージャ
  UopBase(TpgFaultMgr& fmgr);

  /// @brief デストラクタ
  virtual
  ~UopBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  /// @param[in] f 故障
  virtual
  void
  operator()(const TpgFault* f);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgFaultMgr
  TpgFaultMgr& mMgr;

};

END_NAMESPACE_YM_SATPG

#endif // UOPBASE_H