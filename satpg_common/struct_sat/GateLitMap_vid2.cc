
/// @file GateLitMap_vid2.cc
/// @brief GateLitMap_vid2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap_vid2.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス GateLitMap_vid2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateLitMap_vid2::GateLitMap_vid2(const TpgNode* node,
				 const VidMap& vid_map,
				 SatVarId ovar) :
  GateLitMap_vid(node, vid_map),
  mOvar(ovar)
{
}

// @brief デストラクタ
GateLitMap_vid2::~GateLitMap_vid2()
{
}

// @brief 出力のリテラルを返す．
SatLiteral
GateLitMap_vid2::output() const
{
  return SatLiteral(mOvar);
}

END_NAMESPACE_YM_SATPG
