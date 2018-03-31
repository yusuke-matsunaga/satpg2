
/// @file SnAnd.cc
/// @brief SnAnd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnAnd.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// @brief 可観測性の条件を返す．
inline
PackedVal
_obs_val(FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return val;
#elif FSIM_VAL3
  return val.val1();
#endif
}
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SnAnd
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd::SnAnd(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnAnd::~SnAnd()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd::gate_type() const
{
  return GateType::And;
}

// @brief 故障値の計算を行う．
FSIM_VALTYPE
SnAnd::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val &= _fanin(i)->val();
  }
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd::_calc_gobs(ymuint ipos)
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
// SnAnd2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd2::SnAnd2(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnAnd2::~SnAnd2()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd2::gate_type() const
{
  return GateType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 & val1;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd2::_calc_gobs(ymuint ipos)
{
  ymuint alt_pos = ipos ^ 1;
  return _obs_val(_fanin(alt_pos)->val());
}


//////////////////////////////////////////////////////////////////////
// SnAnd3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd3::SnAnd3(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnGate3(id, inputs)
{
}

// @brief デストラクタ
SnAnd3::~SnAnd3()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd3::gate_type() const
{
  return GateType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd3::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val = val0 & val1 & val2;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd3::_calc_gobs(ymuint ipos)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  switch ( ipos ) {
  case 0: val0 = _fanin(1)->val(); val1 = _fanin(2)->val(); break;
  case 1: val0 = _fanin(0)->val(); val1 = _fanin(2)->val(); break;
  case 2: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); break;
  default: ASSERT_NOT_REACHED; break;
  }
  return _obs_val(val0) & _obs_val(val1);
}


//////////////////////////////////////////////////////////////////////
// SnAnd4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnAnd4::SnAnd4(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnGate4(id, inputs)
{
}

// @brief デストラクタ
SnAnd4::~SnAnd4()
{
}

// @brief ゲートタイプを返す．
GateType
SnAnd4::gate_type() const
{
  return GateType::And;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnAnd4::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val3 = inode3->val();
  FSIM_VALTYPE val = val0 & val1 & val2 & val3;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnAnd4::_calc_gobs(ymuint ipos)
{
  FSIM_VALTYPE val0;
  FSIM_VALTYPE val1;
  FSIM_VALTYPE val2;
  switch ( ipos ) {
  case 0: val0 = _fanin(1)->val(); val1 = _fanin(2)->val(); val2 = _fanin(3)->val(); break;
  case 1: val0 = _fanin(0)->val(); val1 = _fanin(2)->val(); val2 = _fanin(3)->val(); break;
  case 2: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); val2 = _fanin(3)->val(); break;
  case 3: val0 = _fanin(0)->val(); val1 = _fanin(1)->val(); val2 = _fanin(2)->val(); break;
  default: ASSERT_NOT_REACHED; break;
  }
  return _obs_val(val0) & _obs_val(val1) & _obs_val(val2);
}


//////////////////////////////////////////////////////////////////////
// SnNand
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNand::SnNand(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnAnd(id, inputs)
{
}

// @brief デストラクタ
SnNand::~SnNand()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand::gate_type() const
{
  return GateType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val &= _fanin(i)->val();
  }
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNand2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNand2::SnNand2(ymuint id,
		 const vector<SimNode*>& inputs) :
  SnAnd2(id, inputs)
{
}

// @brief デストラクタ
SnNand2::~SnNand2()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand2::gate_type() const
{
  return GateType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 & val1;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNand3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNand3::SnNand3(ymuint id,
		 const vector<SimNode*>& inputs) :
  SnAnd3(id, inputs)
{
}

// @brief デストラクタ
SnNand3::~SnNand3()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand3::gate_type() const
{
  return GateType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand3::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val = val0 & val1 & val2;
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnNand4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNand4::SnNand4(ymuint id,
		 const vector<SimNode*>& inputs) :
  SnAnd4(id, inputs)
{
}

// @brief デストラクタ
SnNand4::~SnNand4()
{
}

// @brief ゲートタイプを返す．
GateType
SnNand4::gate_type() const
{
  return GateType::Nand;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnNand4::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  SimNode* inode2 = _fanin(2);
  SimNode* inode3 = _fanin(3);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val2 = inode2->val();
  FSIM_VALTYPE val3 = inode3->val();
  FSIM_VALTYPE val = val0 & val1 & val2 & val3;
  return ~val;
}

END_NAMESPACE_YM_SATPG_FSIM
