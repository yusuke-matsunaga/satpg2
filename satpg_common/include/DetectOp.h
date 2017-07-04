#ifndef DETECTOP_H
#define DETECTOP_H

/// @file DetectOp.h
/// @brief DetectOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "FaultType.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DetectOp DetectOp.h "DetectOp.h"
/// @brief テストパタンが見つかったときの処理を行なうファンクタ
//////////////////////////////////////////////////////////////////////
class DetectOp
{
public:

  /// @brief デストラクタ
  virtual
  ~DetectOp() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] assign_list 値割当のリスト
  virtual
  void
  operator()(const TpgFault* f,
	     const NodeValList& assign_list) = 0;

};

/// @brief 'base' タイプを生成する．
/// @param[in] fmgr 故障マネージャ
DetectOp*
new_DopBase(TpgFaultMgr& fmgr);

/// @brief 'drop' タイプを生成する．
/// @param[in] fmgr 故障マネージャ
/// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopDrop(TpgFaultMgr& fmgr,
	    Fsim& fsim);

/// @brief 'tvlist-sa' タイプを生成する．
/// @param[in] tvmgr テストベクタのマネージャ
/// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvListSa(TvMgr& tvmgr,
		vector<const TestVector*>& tvlist);

/// @brief 'tvlist-td' タイプを生成する．
/// @param[in] tvmgr テストベクタのマネージャ
/// @param[in] tvlist テストベクタのリスト
DetectOp*
new_DopTvListTd(TvMgr& tvmgr,
		vector<const TestVector*>& tvlist);

/// @brief 'verify' タイプを生成する．
/// @param[in] fsim 故障シミュレータ
/// @param[in] result 結果を格納するオブジェクト
/// @param[in] fault_type 故障の種類
DetectOp*
new_DopVerify(Fsim& fsim,
	      DopVerifyResult& result,
	      FaultType fault_type);

/// @brief 'sa-verify' タイプを生成する．
/// @param[in] fsim 故障シミュレータ
/// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopSaVerify(Fsim& fsim,
		DopVerifyResult& result);

/// @brief 'td-verify' タイプを生成する．
/// @param[in] fsim 故障シミュレータ
/// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopTdVerify(Fsim& fsim,
		DopVerifyResult& result);

/// @brief 'dummy' タイプを生成する．
DetectOp*
new_DopDummy();

END_NAMESPACE_YM_SATPG

#endif // DETECTOP_H
