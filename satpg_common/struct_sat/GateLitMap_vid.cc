
/// @file GateLitMap_vid.cc
/// @brief GateLitMap_vid の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap_vid.h"
#include "VidMap.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス GateLitMap_vid
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateLitMap_vid::GateLitMap_vid(const TpgNode* node,
			       const VidMap& vid_map) :
  mNode(node),
  mVidMap(vid_map)
{
}

// @brief デストラクタ
GateLitMap_vid::~GateLitMap_vid()
{
}

// @brief 入力数を返す．
ymuint
GateLitMap_vid::input_size() const
{
  return mNode->fanin_num();
}

// @brief 入力のリテラルを返す．
SatLiteral
GateLitMap_vid::input(ymuint pos) const
{
  const TpgNode* inode = mNode->fanin(pos);
  return SatLiteral(mVidMap(inode), false);
}

// @brief 出力のリテラルを返す．
SatLiteral
GateLitMap_vid::output() const
{
  return SatLiteral(mVidMap(mNode), false);
}

END_NAMESPACE_YM_SATPG
