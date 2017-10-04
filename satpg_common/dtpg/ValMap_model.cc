
/// @file ValMap_model.cc
/// @brief ValMap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "ValMap_model.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス ValMap
//////////////////////////////////////////////////////////////////////

// @brief 縮退故障用のコンストラクタ
// @param[in] gvar_map 正常値の変数マップ
// @param[in] fvar_map 故障値の変数マップ
// @param[in] model SATソルバの作ったモデル
ValMap_model::ValMap_model(const VidMap& gvar_map,
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
ValMap_model::ValMap_model(const VidMap& hvar_map,
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
ValMap_model::~ValMap_model()
{
}

// @brief ノードの正常値を返す．
// @param[in] node 対象のノード
// @param[in] time 時刻 (0 or 1)
Val3
ValMap_model::gval(const TpgNode* node,
		   int time) const
{
  SatVarId vid = (time == 0) ? mHvarMap(node) : mGvarMap(node);
  ASSERT_COND( vid != kSatVarIdIllegal );
  return val(vid);
}

// @brief ノードの故障値を返す．
// @param[in] node 対象のノード
Val3
ValMap_model::fval(const TpgNode* node) const
{
  SatVarId vid = mFvarMap(node);
  ASSERT_COND( vid != kSatVarIdIllegal );
  return val(vid);
}

END_NAMESPACE_YM_SATPG
