
/// @file TpgPPO.cc
/// @brief TpgPPO の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPO.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgPPO
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] output_id 出力番号
// @param[in] fanin ファンインのノード
TpgPPO::TpgPPO(ymuint id,
	       ymuint output_id,
	       TpgNode* fanin) :
  TpgNode(id),
  mOutputId(output_id),
  mFanin(fanin)
{
  mOutputId2 = 0;
}

// @brief デストラクタ
TpgPPO::~TpgPPO()
{
}

// @brief 出力タイプの時 true を返す．
//
// 具体的には is_primary_output() || is_dff_input()
bool
TpgPPO::is_ppo() const
{
  return true;
}

// @brief 外部出力タイプの時に出力番号を返す．
//
// node = TpgNetwork::output(node->output_id())
// の関係を満たす．
// is_output() が false の場合の返り値は不定
ymuint
TpgPPO::output_id() const
{
  return mOutputId;
}

// @brief TFIサイズの昇順に並べた時の出力番号を返す．
ymuint
TpgPPO::output_id2() const
{
  return mOutputId2;
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgPPO::gate_type() const
{
  return kGateBUFF;
}

// @brief ファンイン数を得る．
ymuint
TpgPPO::fanin_num() const
{
  return 1;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
TpgNode*
TpgPPO::fanin(ymuint pos) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}

#if 0
// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgPPO::make_cnf(SatSolver& solver,
		 const GateLitMap& lit_map) const
{
  SatLiteral ilit = lit_map.input(0);
  SatLiteral olit = lit_map.output();
  solver.add_clause( ilit, ~olit);
  solver.add_clause(~ilit,  olit);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgPPO::make_faulty_cnf(SatSolver& solver,
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

// @brief 出力番号2をセットする．
// @param[in] id セットする番号
//
// 出力ノード以外では無効
void
TpgPPO::set_output_id2(ymuint id)
{
  mOutputId2 = id;
}

END_NAMESPACE_YM_SATPG
