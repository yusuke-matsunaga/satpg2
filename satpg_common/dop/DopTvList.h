#ifndef DOPTVLIST_H
#define DOPTVLIST_H

/// @file DopTvList.h
/// @brief DopTvList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DetectOp.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DopTvList DopTvList.h "DopTvList.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopTvList :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] tvlist テストベクタのリスト
  /// @param[in] td_mode TDモードの時 true にするフラグ
  DopTvList(TvMgr& tvmgr,
	    vector<const TestVector*>& tvlist,
	    bool td_mode);

  /// @brief デストラクタ
  virtual
  ~DopTvList();


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
	     const NodeValList& assign_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタのマネージャ
  TvMgr& mTvMgr;

  // テストベクタのリスト
  vector<const TestVector*>& mTvList;

  // TDモード
  bool mTdMode;

};

END_NAMESPACE_YM_SATPG

#endif // DOPTVLIST_H
