#ifndef TPGLOGICC1_H
#define TPGLOGICC1_H

/// @file TpgLogicC1.h
/// @brief TpgLogicC1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC1 TpgLogicC1.h "TpgLogicC1.h"
/// @brief constant-1 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC1 :
  public TpgLogic0
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgLogicC1(ymuint id);

  /// @brief デストラクタ
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
  gate_type() const;

#if 0
  /// @brief 入出力の関係を表す CNF 式を生成する．
  /// @param[in] solver SAT ソルバ
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  virtual
  void
  make_cnf(SatSolver& solver,
	   const GateLitMap& lit_map) const;
#endif

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_YM_SATPG

#endif // TPGLOGICC1_H
