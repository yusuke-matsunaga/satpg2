
/// @file InputVals.cc
/// @brief InputVals の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "FsimX.h"
#include "InputVals.h"
#include "TestVector.h"
#include "NodeValList.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// 初期値を求める．
inline
FSIM_VALTYPE
init_val()
{
#if FSIM_VAL2
  // デフォルトで 0 にする．
  return kPvAll0;
#elif FSIM_VAL3
  // デフォルトで X にする．
  return PackedVal3(kPvAll0, kPvAll0);
#endif
}

// 0/1 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
int_to_packedval(int val)
{
#if FSIM_VAL2
  return val ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  return val ? PackedVal3(kPvAll1) : PackedVal3(kPvAll0);
#endif
}

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(Val3 val)
{
#if FSIM_VAL2
  // kValX は kVal0 とみなす．
  return (val == kVal1) ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  switch ( val ) {
  case kValX: return PackedVal3(kPvAll0, kPvAll0);
  case kVal0: return PackedVal3(kPvAll1, kPvAll0);
  case kVal1: return PackedVal3(kPvAll0, kPvAll1);
  }
#endif
}

// bit のビットに値を設定する．
inline
void
bit_set(FSIM_VALTYPE& val,
	Val3 ival,
	PackedVal bit)
{
#if FSIM_VAL2
  if ( ival == kVal1 ) {
    val |= bit;
  }
#elif FSIM_VAL3
  FSIM_VALTYPE val1 = val3_to_packedval(ival);
  val.set_with_mask(val1, bit);
#endif
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TvInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] testvector テストベクタ
TvInputVals::TvInputVals(const TestVector* testvector) :
  mTestVector(testvector)
{
}

// @brief デストラクタ
TvInputVals::~TvInputVals()
{
}

// @brief 1時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
TvInputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  ymuint npi = fsim.ppi_num();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    Val3 val3 = mTestVector->ppi_val(i);
    simnode->set_val(val3_to_packedval(val3));
  }
}

// @brief 2時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
TvInputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  ymuint ni = fsim.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    Val3 val3 = mTestVector->aux_input_val(i);
    simnode->set_val(val3_to_packedval(val3));
  }
}

//////////////////////////////////////////////////////////////////////
// クラス Tv2InputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] pat_map パタンのセットされているビットに1を立てたビットマップ
// @param[in] pat_array パタンの配列(サイズは kPvBitLen の固定長)
Tv2InputVals::Tv2InputVals(PackedVal pat_map,
			   const TestVector* pat_array[]) :
  mPatMap(pat_map)
{
  mPatFirstBit = kPvBitLen;
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    if ( mPatMap & (1ULL << i) ) {
      mPatArray[i] = pat_array[i];
      if ( mPatFirstBit > i ) {
	mPatFirstBit = i;
      }
    }
    else {
      mPatArray[i] = nullptr;
    }
  }
}

// @brief デストラクタ
Tv2InputVals::~Tv2InputVals()
{
}

// @brief 1時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
Tv2InputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  ymuint npi = fsim.ppi_num();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    FSIM_VALTYPE val = init_val();
    PackedVal bit = 1ULL;
    for (ymuint j = 0; j < kPvBitLen; ++ j, bit <<= 1) {
      ymuint pos = (mPatMap & bit) ? j : mPatFirstBit;
      Val3 ival = mPatArray[pos]->ppi_val(i);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
  }
}

// @brief 2時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
Tv2InputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  ymuint ni = fsim.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    FSIM_VALTYPE val = init_val();
    PackedVal bit = 1ULL;
    for (ymuint j = 0; j < kPvBitLen; ++ j, bit <<= 1) {
      ymuint pos = (mPatMap & bit) ? j : mPatFirstBit;
      Val3 ival = mPatArray[pos]->aux_input_val(i);
      bit_set(val, ival, bit);
    }
    simnode->set_val(val);
  }
}


//////////////////////////////////////////////////////////////////////
// クラス NvlInputVals
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] assign_list 値の割り当てリスト
NvlInputVals::NvlInputVals(const NodeValList& assign_list) :
  mAssignList(assign_list)
{
}

// @brief デストラクタ
NvlInputVals::~NvlInputVals()
{
}

// @brief 1時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
NvlInputVals::set_val1(FSIM_CLASSNAME& fsim) const
{
  FSIM_VALTYPE val0 = init_val();
  ymuint npi = fsim.ppi_num();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    simnode->set_val(val0);
  }

  ymuint n = mAssignList.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = mAssignList[i];
    if ( nv.time() == 0 ) {
      ymuint iid = nv.node()->input_id();
      SimNode* simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

// @brief 2時刻目の値を設定する．
// @param[in] fsim 故障シミュレータ
void
NvlInputVals::set_val2(FSIM_CLASSNAME& fsim) const
{
  FSIM_VALTYPE val0 = init_val();
  ymuint ni = fsim.input_num();
  for (ymuint i = 0; i < ni; ++ i) {
    SimNode* simnode = fsim.ppi(i);
    simnode->set_val(val0);
  }

  ymuint n = mAssignList.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = mAssignList[i];
    if ( nv.time() == 1 ) {
      ymuint iid = nv.node()->input_id();
      SimNode* simnode = fsim.ppi(iid);
      simnode->set_val(int_to_packedval(nv.val()));
    }
  }
}

END_NAMESPACE_YM_SATPG_FSIM
