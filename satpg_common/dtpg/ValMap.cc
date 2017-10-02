
/// @file ValMap.cc
/// @brief ValMap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "ValMap.h"
#include "VidMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス ValMap
//////////////////////////////////////////////////////////////////////

// @brief 縮退故障用のコンストラクタ
// @param[in] gvar_map 正常値の変数マップ
// @param[in] fvar_map 故障値の変数マップ
// @param[in] model SATソルバの作ったモデル
ValMap::ValMap(const VidMap& gvar_map,
	       const VidMap& fvar_map,
	       const vector<SatBool3>& model) :
  mHvarMap(gvar_map),
  mGvarMap(gvar_map),
  mFvarMap(fvar_map),
  mModel(model)
{
}

// @brief コンストラクタ
// @param[in] hvar_map 1時刻前の正常値の変数マップ
// @param[in] gvar_map 正常値の変数マップ
// @param[in] fvar_map 故障値の変数マップ
// @param[in] model SATソルバの作ったモデル
ValMap::ValMap(const VidMap& hvar_map,
	       const VidMap& gvar_map,
	       const VidMap& fvar_map,
	       const vector<SatBool3>& model) :
  mHvarMap(hvar_map),
  mGvarMap(gvar_map),
  mFvarMap(fvar_map),
  mModel(model)
{
}

// @brief デストラクタ
ValMap::~ValMap()
{
}

END_NAMESPACE_YM_SATPG
