#ifndef TPGLOGICC1_H
#define TPGLOGICC1_H

/// @file TpgLogicC1.h
/// @brief TpgLogicC1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC1 TpgLogicC1.h "TpgLogicC1.h"
/// @brief constant-1 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC1 :
  public TpgLogic0
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogicC1(int id);

  /// @brief デストラクタ
  virtual
  ~TpgLogicC1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  virtual
  GateType
  gate_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_SATPG

#endif // TPGLOGICC1_H