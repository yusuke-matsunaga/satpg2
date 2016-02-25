
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
  TpgLogic1(id)
{
}

// @brief デストラクタ
TpgLogicC1::~TpgLogicC1()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
TpgNode::GateType
TpgLogicC1::gate_type() const
{
  return kGateCONST1;
}

// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLodicC1::make_cnf(SatSolver& solver,
		     const LitMap& lit_map)
{
  solver.add_clause(lit_map.output());
}

END_NAMESPACE_YM_SATPG
