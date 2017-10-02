
/// @file TpgLogicC1.cc
/// @brief TpgLogicC1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicC1.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicC1::TpgLogicC1(ymuint id) :
  TpgLogic0(id)
{
}

// @brief デストラクタ
TpgLogicC1::~TpgLogicC1()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicC1::gate_type() const
{
  return kGateCONST1;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicC1::make_cnf(SatSolver& solver,
		     const GateLitMap& lit_map) const
{
  SatLiteral olit = lit_map.output();
  solver.add_clause(olit);
}
#endif

END_NAMESPACE_YM_SATPG
