
/// @file TpgLogicNOT.cc
/// @brief TpgLogicNOT の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicNOT.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicNOT
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin ファンイン
TpgLogicNOT::TpgLogicNOT(ymuint id,
			 TpgNode* fanin) :
  TpgLogic1(id, fanin)
{
}

// @brief デストラクタ
TpgLogicNOT::~TpgLogicNOT()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicNOT::gate_type() const
{
  return kGateNOT;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicNOT::cval() const
{
  return kValX;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicNOT::nval() const
{
  return kValX;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicNOT::coval() const
{
  return kValX;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicNOT::noval() const
{
  return kValX;
}

END_NAMESPACE_YM_SATPG
