
/// @file TpgLogicC0.cc
/// @brief TpgLogicC0 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicC0.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogcC0
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicC0::TpgLogicC0(ymuint id) :
  TpgLogic0(id)
{
}

// @brief デストラクタ
TpgLogicC0::~TpgLogicC0()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicC0::gate_type() const
{
  return kGateCONST0;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicC0::make_cnf(SatSolver& solver,
		     const GateLitMap& lit_map) const
{
  SatLiteral olit = lit_map.output();
  solver.add_clause(~olit);
}
#endif

END_NAMESPACE_YM_SATPG
