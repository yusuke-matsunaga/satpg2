
/// @file DopTdVerify.cc
/// @brief DopTdVerify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopTdVerify.h"
#include "TpgFault.h"
#include "Fsim.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopTdVerify(Fsim& fsim)
{
  return new DopTdVerify(fsim);
}


//////////////////////////////////////////////////////////////////////
// クラス DopTdVerify
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
DopTdVerify::DopTdVerify(Fsim& fsim) :
  mFsim(fsim)
{
}

// @brief デストラクタ
DopTdVerify::~DopTdVerify()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopTdVerify::operator()(const TpgFault* f,
			const NodeValList& assign_list)
{
  bool detect = mFsim.td_spsfp(assign_list, f);
  if ( !detect ) {
    cout << "Error: " << f->str() << " is not detected with "
	 << assign_list << endl;
  }
}

END_NAMESPACE_YM_SATPG
