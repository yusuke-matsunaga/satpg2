
/// @file DopVerify.cc
/// @brief DopVerify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopVerify.h"
#include "DopVerifyResult.h"
#include "TpgFault.h"
#include "Fsim.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
// @param[in] fault_type 故障の種類
DetectOp*
new_DopVerify(Fsim& fsim,
	      DopVerifyResult& result,
	      FaultType fault_type)
{
  return new DopVerify(fsim, result, fault_type);
}

// @brief 'sa-verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopSaVerify(Fsim& fsim,
		DopVerifyResult& result)
{
  return new DopVerify(fsim, result, kFtStuckAt);
}

// @brief 'td-verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopTdVerify(Fsim& fsim,
		DopVerifyResult& result)
{
  return new DopVerify(fsim, result, kFtTransitionDelay);
}


//////////////////////////////////////////////////////////////////////
// クラス DopVerify
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
// @param[in] fault_type 故障の種類
DopVerify::DopVerify(Fsim& fsim,
		     DopVerifyResult& result,
		     FaultType fault_type) :
  mFsim(fsim),
  mResult(result),
  mFaultType(fault_type)
{
}

// @brief デストラクタ
DopVerify::~DopVerify()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopVerify::operator()(const TpgFault* f,
		      const NodeValList& assign_list)
{
  bool detect = mFsim.spsfp(assign_list, f, mFaultType);
  if ( detect ) {
    mResult.add_good(f);
  }
  else {
    mResult.add_error(f, assign_list);
  }
}

END_NAMESPACE_YM_SATPG
