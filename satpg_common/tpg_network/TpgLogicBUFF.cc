
/// @file TpgLogicBUFF.cc
/// @brief TpgLogicBUFF の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicBUFF.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicBUFF
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin ファンイン
TpgLogicBUFF::TpgLogicBUFF(ymuint id,
			   TpgNode* fanin) :
  TpgLogic1(id, fanin)
{
}

// @brief デストラクタ
TpgLogicBUFF::~TpgLogicBUFF()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicBUFF::gate_type() const
{
  return kGateBUFF;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicBUFF::cval() const
{
  return kValX;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicBUFF::nval() const
{
  return kValX;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicBUFF::coval() const
{
  return kValX;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicBUFF::noval() const
{
  return kValX;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicBUFF::make_cnf(SatSolver& solver,
		       const GateLitMap& lit_map) const
{
  SatLiteral ilit = lit_map.input(0);
  SatLiteral olit = lit_map.output();
  solver.add_eq_rel(ilit, olit);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicBUFF::make_faulty_cnf(SatSolver& solver,
			      ymuint fpos,
			      int fval,
			      const GateLitMap& lit_map) const
{
  SatLiteral olit = lit_map.output();
  if ( fval == 0 ) {
    solver.add_clause(~olit);
  }
  else {
    solver.add_clause( olit);
  }
}
#endif

END_NAMESPACE_YM_SATPG
