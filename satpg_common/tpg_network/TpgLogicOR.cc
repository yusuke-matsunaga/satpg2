
/// @file TpgLogicOR.cc
/// @brief TpgLogicOR[x] の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicOR.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicOR2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicOR2::TpgLogicOR2(ymuint id,
			 const vector<TpgNode*>& fanin_list) :
  TpgLogic2(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicOR2::~TpgLogicOR2()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicOR2::gate_type() const
{
  return kGateOR;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR2::cval() const
{
  return kVal1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR2::nval() const
{
  return kVal0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR2::coval() const
{
  return kVal1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR2::noval() const
{
  return kVal0;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicOR2::make_cnf(SatSolver& solver,
		      const GateLitMap& lit_map) const
{
  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0 = lit_map.input(0);
  SatLiteral ilit1 = lit_map.input(1);

  solver.add_orgate_rel(olit, ilit0, ilit1);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicOR2::make_faulty_cnf(SatSolver& solver,
			     ymuint fpos,
			     int fval,
			     const GateLitMap& lit_map) const
{
  ASSERT_COND( fval == 0 );

  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0;

  switch ( fpos ) {
  case 0: ilit0 = lit_map.input(1); break;
  case 1: ilit0 = lit_map.input(0); break;
  }

  solver.add_eq_rel(olit, ilit0);
}
#endif

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicOR3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicOR3::TpgLogicOR3(ymuint id,
			 const vector<TpgNode*>& fanin_list) :
  TpgLogic3(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicOR3::~TpgLogicOR3()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicOR3::gate_type() const
{
  return kGateOR;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR3::cval() const
{
  return kVal1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR3::nval() const
{
  return kVal0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR3::coval() const
{
  return kVal1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR3::noval() const
{
  return kVal0;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicOR3::make_cnf(SatSolver& solver,
		      const GateLitMap& lit_map) const
{
  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0 = lit_map.input(0);
  SatLiteral ilit1 = lit_map.input(1);
  SatLiteral ilit2 = lit_map.input(2);

  solver.add_orgate_rel(olit, ilit0, ilit1, ilit2);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicOR3::make_faulty_cnf(SatSolver& solver,
			     ymuint fpos,
			     int fval,
			     const GateLitMap& lit_map) const
{
  ASSERT_COND( fval == 0 );

  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0;
  SatLiteral ilit1;

  switch ( fpos ) {
  case 0:
    ilit0 = lit_map.input(1);
    ilit1 = lit_map.input(2);
    break;

  case 1:
    ilit0 = lit_map.input(0);
    ilit1 = lit_map.input(2);
    break;

  case 2:
    ilit0 = lit_map.input(0);
    ilit1 = lit_map.input(1);
    break;
  }

  solver.add_orgate_rel(olit, ilit0, ilit1);
}
#endif

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicOR4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicOR4::TpgLogicOR4(ymuint id,
			 const vector<TpgNode*>& fanin_list) :
  TpgLogic4(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicOR4::~TpgLogicOR4()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicOR4::gate_type() const
{
  return kGateOR;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR4::cval() const
{
  return kVal1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR4::nval() const
{
  return kVal0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR4::coval() const
{
  return kVal1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicOR4::noval() const
{
  return kVal0;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicOR4::make_cnf(SatSolver& solver,
		      const GateLitMap& lit_map) const
{
  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0 = lit_map.input(0);
  SatLiteral ilit1 = lit_map.input(1);
  SatLiteral ilit2 = lit_map.input(2);
  SatLiteral ilit3 = lit_map.input(3);

  solver.add_orgate_rel(olit, ilit0, ilit1, ilit2, ilit3);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicOR4::make_faulty_cnf(SatSolver& solver,
			     ymuint fpos,
			     int fval,
			     const GateLitMap& lit_map) const
{
  ASSERT_COND( fval == 0 );

  SatLiteral olit  = lit_map.output();
  SatLiteral ilit0;
  SatLiteral ilit1;
  SatLiteral ilit2;

  switch ( fpos ) {
  case 0:
    ilit0 = lit_map.input(1);
    ilit1 = lit_map.input(2);
    ilit2 = lit_map.input(3);
    break;

  case 1:
    ilit0 = lit_map.input(0);
    ilit1 = lit_map.input(2);
    ilit2 = lit_map.input(3);
    break;

  case 2:
    ilit0 = lit_map.input(0);
    ilit1 = lit_map.input(1);
    ilit2 = lit_map.input(3);
    break;

  case 3:
    ilit0 = lit_map.input(0);
    ilit1 = lit_map.input(1);
    ilit2 = lit_map.input(2);
    break;
  }

  solver.add_orgate_rel(olit, ilit0, ilit1, ilit2);
}
#endif

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicORN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicORN::TpgLogicORN(ymuint id) :
  TpgLogicN(id)
{
}

// @brief デストラクタ
TpgLogicORN::~TpgLogicORN()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicORN::gate_type() const
{
  return kGateOR;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicORN::cval() const
{
  return kVal1;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicORN::nval() const
{
  return kVal0;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicORN::coval() const
{
  return kVal1;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicORN::noval() const
{
  return kVal0;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicORN::make_cnf(SatSolver& solver,
		      const GateLitMap& lit_map) const
{
  SatLiteral olit  = lit_map.output();
  ymuint ni = fanin_num();
  vector<SatLiteral> ilits(ni);
  for (ymuint i = 0; i < ni; ++ i) {
    SatLiteral ilit = lit_map.input(i);
    ilits[i] = ilit;
  }

  solver.add_orgate_rel(olit, ilits);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicORN::make_faulty_cnf(SatSolver& solver,
			     ymuint fpos,
			     int fval,
			     const GateLitMap& lit_map) const
{
  ASSERT_COND( fval == 0 );

  SatLiteral olit  = lit_map.output();
  ymuint ni = fanin_num();
  vector<SatLiteral> ilits;
  ilits.reserve(ni - 1);
  for (ymuint i = 0; i < ni; ++ i) {
    if ( i == fpos ) {
      continue;
    }
    SatLiteral ilit = lit_map.input(i);
    ilits.push_back(ilit);
  }

  solver.add_orgate_rel(olit, ilits);
}
#endif

END_NAMESPACE_YM_SATPG
