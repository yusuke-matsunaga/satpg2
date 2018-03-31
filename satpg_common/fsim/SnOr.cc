
/// @file SnOr.cc
/// @brief SnOr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnOr.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// @brief 可観測性の条件を返す．
inline
PackedVal
_obs_val(FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return ~val;
#elif FSIM_VAL3
  return val.val0();
#endif
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SnOr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr::SnOr(ymuint id,
	   const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnOr::~SnOr()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr::gate_type() const
{
  return GateType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->val();
  }
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr::_calc_gobs(ymuint ipos)
{
  PackedVal obs = kPvAll1;
  for (ymuint i = 0; i < ipos; ++ i) {
    obs &= _obs_val(_fanin(i)->val());
  }
  for (ymuint i = ipos + 1; i < _fanin_num(); ++ i) {
    obs &= _obs_val(_fanin(i)->val());
  }
  return obs;
}


//////////////////////////////////////////////////////////////////////
// SnOr2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr2::SnOr2(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnOr2::~SnOr2()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr2::gate_type() const
{
  return GateType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 | val1;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr2::_calc_gobs(ymuint ipos)
{
  ymuint alt_pos = ipos ^ 1;
  return _obs_val(_fanin(alt_pos)->val());
}


//////////////////////////////////////////////////////////////////////
// SnOr3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr3::SnOr3(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnGate3(id, inputs)
{
}

// @brief デストラクタ
SnOr3::~SnOr3()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr3::gate_type() const
{
  return GateType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr3::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val = val0 | val1 | val2;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr3::_calc_gobs(ymuint ipos)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  switch ( ipos ) {
  case 0: val0 = _fanin(1)->val(); val1 = _fanin(2)->val(); break;
  case 1: val0 = _fanin(0)->val(); val1 = _fanin(2)->val(); break;
  case 2: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); break;
  default: ASSERT_NOT_REACHED;
  }
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnOr4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnOr4::SnOr4(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnGate4(id, inputs)
{
}

// @brief デストラクタ
SnOr4::~SnOr4()
{
}

// @brief ゲートタイプを返す．
GateType
SnOr4::gate_type() const
{
  return GateType::Or;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnOr4::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val3 = inode3->val();
  FSIM_VALTYPE val = val0 | val1 | val2 | val3;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnOr4::_calc_gobs(ymuint ipos)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  FSIM_VALTYPE val2;
  switch ( ipos ) {
  case 0: val0 = _fanin(1)->val(); val1 = _fanin(2)->val(); val2 = _fanin(3)->val(); break;
  case 1: val0 = _fanin(0)->val(); val1 = _fanin(2)->val(); val2 = _fanin(3)->val(); break;
  case 2: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); val2 = _fanin(3)->val(); break;
  case 3: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); val2 = _fanin(2)->val(); break;
  default: ASSERT_NOT_REACHED;
  }
  return _obs_val(val0) & _obs_val(val1) & _obs_val(val2);
}


//////////////////////////////////////////////////////////////////////
// SnNor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor::SnNor(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnOr(id, inputs)
{
}

// @brief デストラクタ
SnNor::~SnNor()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor::gate_type() const
{
  return GateType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val |= _fanin(i)->val();
  }
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor2::SnNor2(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnOr2(id, inputs)
{
}

// @brief デストラクタ
SnNor2::~SnNor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor2::gate_type() const
{
  return GateType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 | val1;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor3::SnNor3(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnOr3(id, inputs)
{
}

// @brief デストラクタ
SnNor3::~SnNor3()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor3::gate_type() const
{
  return GateType::Nor;
}

// @brief 出力値の計算を行う．(3値版)
FSIM_VALTYPE
SnNor3::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val = val0 | val1 | val2;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNor4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNor4::SnNor4(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnOr4(id, inputs)
{
}

// @brief デストラクタ
SnNor4::~SnNor4()
{
}

// @brief ゲートタイプを返す．
GateType
SnNor4::gate_type() const
{
  return GateType::Nor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNor4::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val3 = inode3->val();
  FSIM_VALTYPE val = val0 | val1 | val2 | val3;
  return ~val;
}

END_NAMESPACE_YM_SATPG_FSIM
