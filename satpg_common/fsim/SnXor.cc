
/// @file SnXor.cc
/// @brief SnXor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SnXor.h"
#include "GateType.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// SnXor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor::SnXor(ymuint id,
	     const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnXor::~SnXor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor::gate_type() const
{
  return GateType::Xor;
}

// @brief 出力値の計算を行う．(3値版)
FSIM_VALTYPE
SnXor::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val ^= _fanin(i)->val();
  }
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．(3値版)
PackedVal
SnXor::_calc_gobs(ymuint ipos)
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return kPvAll1;
#elif FSIM_VAL3
  // 3値はめんどくさい
  // 条件は ipos 以外が X でないこと
  PackedVal obs = kPvAll1;
  for (ymuint i = 0; i < ipos; ++ i) {
    const SimNode* inode = _fanin(i);
    FSIM_VALTYPE ival = inode->val();
    obs &= ival.val01();
  }
  ymuint n = _fanin_num();
  for (ymuint i = ipos + 1; i < n; ++ i) {
    const SimNode* inode = _fanin(i);
    FSIM_VALTYPE ival = inode->val();
    obs &= ival.val01();
  }
  return obs;
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor2::SnXor2(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnXor2::~SnXor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor2::gate_type() const
{
  return GateType::Xor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXor2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 ^ val1;
  return val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnXor2::_calc_gobs(ymuint ipos)
{
#if FSIM_VAL2
  // 2値なら常に観測可能
  return kPvAll1;
#elif FSIM_VAL3
  // 3値の場合，Xでないことが条件
  ymuint alt_pos = ipos ^ 1;
  SimNode* inode = _fanin(alt_pos);
  FSIM_VALTYPE ival = inode->val();
  return ival.val01();
#endif
}


//////////////////////////////////////////////////////////////////////
// SnXnor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor::SnXnor(ymuint id,
	       const vector<SimNode*>& inputs) :
  SnXor(id, inputs)
{
}

// @brief デストラクタ
SnXnor::~SnXnor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor::gate_type() const
{
  return GateType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor::_calc_val()
{
  ymuint n = _fanin_num();
  FSIM_VALTYPE val = _fanin(0)->val();
  for (ymuint i = 1; i < n; ++ i) {
    val ^= _fanin(i)->val();
  }
  return ~val;
}


//////////////////////////////////////////////////////////////////////
// SnXnor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor2::SnXnor2(ymuint id,
		 const vector<SimNode*>& inputs) :
  SnXor2(id, inputs)
{
}

// @brief デストラクタ
SnXnor2::~SnXnor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor2::gate_type() const
{
  return GateType::Xnor;
}

// @brief 出力値の計算を行う．
FSIM_VALTYPE
SnXnor2::_calc_val()
{
  SimNode* inode0 = _fanin(0);
  SimNode* inode1 = _fanin(1);
  FSIM_VALTYPE val0 = inode0->val();
  FSIM_VALTYPE val1 = inode1->val();
  FSIM_VALTYPE val = val0 ^ val1;
  return ~val;
}

END_NAMESPACE_YM_SATPG_FSIM
