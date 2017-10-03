
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

END_NAMESPACE_YM_SATPG
