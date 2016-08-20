
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
// @param[in] fanout_num ファンアウト数
TpgLogicC1::TpgLogicC1(ymuint id,
		       ymuint fanout_num) :
  TpgNode(id, vector<TpgNode*>(), fanout_num)
{
}

// @brief デストラクタ
TpgLogicC1::~TpgLogicC1()
{
}

// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicC1::make_cnf(SatSolver& solver,
		     const LitMap& lit_map) const
{
  SatLiteral olit = lit_map.output();
  solver.add_clause(olit);
}

END_NAMESPACE_YM_SATPG
