
/// @file TvMgr.cc
/// @brief TvMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TvMgr.h"
#include "TpgNetwork.h"
#include "InputVector.h"
#include "FFVector.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// @brief ベクタ長からバイトサイズを計算する．
// @param[in] vectlen ベクタ長
inline
ymuint
calc_size(ymuint vectlen)
{
  if ( vectlen == 0 ) {
    vectlen = 1;
  }
  return sizeof(BitfffVector) + kPvBitLen * (BitVector::block_num(vectlen) - 1);
}

END_NONAMESPACE

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
TvMgr::TvMgr(const TpgNetwork& network) :
  mInputNum(network.input_num()),
  mDffNum(network.dff_num()),
  mIvSize(calc_size(mInputNum)),
  mFvSize(calc_size(mDffNum)),
  mInputVectorAlloc(mIvSize, 1024),
  mFFVectorAlloc(mFvSize, 1024)
{
}

// @brief デストラクタ
TvMgr::~TvMgr()
{
  clear();
}

/// @brief 内容を空にする．
void
TvMgr::clear()
{
  mInputVectorAlloc.destroy();
  mFFVectorAlloc.destroy();
}

// @brief 新しい入力用ベクタを生成する．
// @return 生成されたパタンを返す．
//
// パタンは0で初期化される．
InputVector*
TvMgr::new_input_vector()
{
  void* p = mInputVectorAlloc.get_memory(mIvSize);
  InputVector* vect = new (p) InputVector(mInputNum);

  return vect;
}

// @brief 入力用ベクタを削除する．
void
TvMgr::delete_vector(InputVector* vect)
{
  mInputVectorAlloc.put_memory(mIvSize, (void*)vect);
}

// @brief 新しいFF用ベクタを生成する．
// @return 生成されたベクタを返す．
//
// パタンは0で初期化される．
FFVector*
TvMgr::new_ff_vector()
{
  void* p = mFFVectorAlloc.get_memory(mFvSize);
  FFVector* vect = new (p) FFVector(mDffNum);

  return vect;
}

// @brief FF用ベクタを削除する．
// @param[in] vect 削除するベクタ
void
TvMgr::delete_vector(FFVector* vect)
{
  mFFVectorAlloc.put_memory(mFvSize, (void*)vect);
}

END_NAMESPACE_YM_SATPG
