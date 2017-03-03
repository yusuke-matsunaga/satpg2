#ifndef DOPTDVERIFY_H
#define DOPTDVERIFY_H

/// @file DopTdVerify.h
/// @brief DopTdVerify のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopTdVerify DopTdVerify.h "DopTdVerify.h"
/// @brief 故障シミュレーションを行なった故障ドロップを行なうクラス
//////////////////////////////////////////////////////////////////////
class DopTdVerify :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fsim 故障シミュレータ
  DopTdVerify(Fsim& fsim);

  /// @brief デストラクタ
  virtual
  ~DopTdVerify();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] assign_list 値割当のリスト
  virtual
  void
  operator()(const TpgFault* f,
	     const NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障シミュレータ
  Fsim& mFsim;

};

END_NAMESPACE_YM_SATPG

#endif // DOPTDVERIFY_H
