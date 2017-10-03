
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

END_NAMESPACE_YM_SATPG
