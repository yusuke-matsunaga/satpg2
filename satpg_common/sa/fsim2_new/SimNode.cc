﻿
/// @file SimNode.cc
/// @brief SimNode の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SimNode.h"
#include "SimPrim.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のノードを表すクラス
//////////////////////////////////////////////////////////////////////

// コンストラクタ
SimNode::SimNode(ymuint id,
		 SimPrim* gval,
		 SimPrim* fval) :
  mId(id),
  mFanoutNum(0),
  mFanouts(nullptr),
  mLevel(0),
  mGvalPrim(gval),
  mFvalPrim(fval)
{
}

// デストラクタ
SimNode::~SimNode()
{
  delete [] mFanouts;
}

// @brief ファンアウトリストを作成する．
void
SimNode::set_fanout_list(const vector<SimNode*>& fo_list,
			 ymuint ipos)
{
  ymuint nfo = fo_list.size();
  mFanouts = new SimNode*[nfo];
  for (ymuint i = 0; i < nfo; ++ i) {
    mFanouts[i] = fo_list[i];
  }

  mFanoutNum |= (nfo << 16) | (ipos << 4);
}

END_NAMESPACE_YM_SATPG_FSIM
