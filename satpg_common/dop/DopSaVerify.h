#ifndef DOPSAVERIFY_H
#define DOPSAVERIFY_H

/// @file DopSaVerify.h
/// @brief DopSaVerify のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopSaVerify DopSaVerify.h "DopSaVerify.h"
/// @brief 故障シミュレーションを行なった故障ドロップを行なうクラス
//////////////////////////////////////////////////////////////////////
class DopSaVerify :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] fsim 故障シミュレータ
  DopSaVerify(Fsim& fsim);

  /// @brief デストラクタ
  virtual
  ~DopSaVerify();


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

#endif // DOPSAVERIFY_H
