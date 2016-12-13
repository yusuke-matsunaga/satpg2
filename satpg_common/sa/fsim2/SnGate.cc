﻿
/// @file SnGate.cc
/// @brief SnGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SnGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// @class SnGate SimNode.h
// @brief 多入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate::SnGate(ymuint id,
	       const vector<SimNode*>& inputs) :
  SimNode(id),
  mFaninNum(inputs.size()),
  mFanins(new SimNode*[mFaninNum])
{
  ymuint max_level = 0;
  for (ymuint i = 0; i < mFaninNum; ++ i) {
    SimNode* input = inputs[i];
    ASSERT_COND(input );
    mFanins[i] = input;
    ymuint level = input->level() + 1;
    if ( max_level < level ) {
      max_level = level;
    }
  }
  set_level(max_level);
}

// @brief デストラクタ
SnGate::~SnGate()
{
  delete [] mFanins;
}

// @brief ファンイン数を得る．
ymuint
SnGate::fanin_num() const
{
  return _fanin_num();
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate::fanin(ymuint pos) const
{
  ASSERT_COND( pos < _fanin_num() );
  return _fanin(pos);
}


//////////////////////////////////////////////////////////////////////
// @class SnGate1 SimNode.h
// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate1::SnGate1(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 1 );
  ASSERT_COND(inputs[0] );
  mFanin = inputs[0];
  set_level(mFanin->level() + 1);
}

// @brief デストラクタ
SnGate1::~SnGate1()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate1::fanin_num() const
{
  return 1;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate1::fanin(ymuint pos) const
{
  return mFanin;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate2 SimNode.h
// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate2::SnGate2(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 2 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate2::~SnGate2()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate2::fanin_num() const
{
  return 2;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate2::fanin(ymuint pos) const
{
  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// @class SnGate3 SimNode.h
// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate3::SnGate3(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 3 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  ASSERT_COND(inputs[2] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  if ( level < mFanins[2]->level() ) {
    level = mFanins[2]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate3::~SnGate3()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate3::fanin_num() const
{
  return 3;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate3::fanin(ymuint pos) const
{
  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// @class SnGate4 SimNode.h
// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate4::SnGate4(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 4 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  ASSERT_COND(inputs[2] );
  ASSERT_COND(inputs[3] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  mFanins[3] = inputs[3];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  if ( level < mFanins[2]->level() ) {
    level = mFanins[2]->level();
  }
  if ( level < mFanins[3]->level() ) {
    level = mFanins[3]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate4::~SnGate4()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate4::fanin_num() const
{
  return 4;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate4::fanin(ymuint pos) const
{
  return mFanins[pos];
}

END_NAMESPACE_YM_SATPG_FSIM
