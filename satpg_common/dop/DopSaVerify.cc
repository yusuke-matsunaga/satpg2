
/// @file DopSaVerify.cc
/// @brief DopSaVerify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopSaVerify.h"
#include "TpgFault.h"
#include "Fsim.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopSaVerify(Fsim& fsim)
{
  return new DopSaVerify(fsim);
}


//////////////////////////////////////////////////////////////////////
// クラス DopSaVerify
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
DopSaVerify::DopSaVerify(Fsim& fsim) :
  mFsim(fsim)
{
}

// @brief デストラクタ
DopSaVerify::~DopSaVerify()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopSaVerify::operator()(const TpgFault* f,
			const NodeValList& assign_list)
{
  bool detect = mFsim.sa_spsfp(assign_list, f);
  if ( !detect ) {
    cout << "Error: " << f->str() << " is not detected with "
	 << assign_list << endl;
  }
}

END_NAMESPACE_YM_SATPG
