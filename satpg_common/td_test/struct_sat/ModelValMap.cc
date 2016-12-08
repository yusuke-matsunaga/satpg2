
/// @file ModevlValMap.cc
/// @brief ModevlValMap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/ModelValMap.h"
#include "VidMap.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
// クラス ModelValMap
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] hvar_map 1時刻前の正常値の変数マップ
// @param[in] gvar_map 正常値の変数マップ
// @param[in] fvar_map 故障値の変数マップ
// @param[in] model SATソルバの作ったモデル
ModelValMap::ModelValMap(const VidMap& hvar_map,
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
ModelValMap::~ModelValMap()
{
}

// @brief ノードの1時刻前の正常値を返す．
// @param[in] node 対象のノード
Val3
ModelValMap::hval(const TpgNode* node) const
{
  return bool3_to_val3(mModel[mHvarMap(node).val()]);
}

// @brief ノードの正常値を返す．
// @param[in] node 対象のノード
Val3
ModelValMap::gval(const TpgNode* node) const
{
  return bool3_to_val3(mModel[mGvarMap(node).val()]);
}

// @brief ノードの故障値を返す．
// @param[in] node 対象のノード
Val3
ModelValMap::fval(const TpgNode* node) const
{
  return bool3_to_val3(mModel[mFvarMap(node).val()]);
}

END_NAMESPACE_YM_SATPG_TD
