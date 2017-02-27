
/// @file DopTvList.cc
/// @brief DopTvList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopTvList.h"
#include "TvMgr.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'tvlist-sa' タイプを生成する．
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvListSa(TvMgr& tvmgr,
		vector<const TestVector*>& tvlist)
{
  return new DopTvList(tvmgr, tvlist, false);
}

// @brief 'tvlist-td' タイプを生成する．
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvListTd(TvMgr& tvmgr,
		vector<const TestVector*>& tvlist)
{
  return new DopTvList(tvmgr, tvlist, true);
}


//////////////////////////////////////////////////////////////////////
// クラス DopTvList
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] tvlist テストベクタのリスト
// @param[in] td_mode TDモードの時 true にするフラグ
DopTvList::DopTvList(TvMgr& tvmgr,
		     vector<const TestVector*>& tvlist,
		     bool td_mode) :
  mTvMgr(tvmgr),
  mTvList(tvlist),
  mTdMode(td_mode)
{
}

// @brief デストラクタ
DopTvList::~DopTvList()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopTvList::operator()(const TpgFault* f,
		      const NodeValList& assign_list)
{
  TestVector* tv = mTdMode ? mTvMgr.new_td_vector() : mTvMgr.new_sa_vector();
  tv->set_from_assign_list(assign_list);
  mTvList.push_back(tv);
}

END_NAMESPACE_YM_SATPG
