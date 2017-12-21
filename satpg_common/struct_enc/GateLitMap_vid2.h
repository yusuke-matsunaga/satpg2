#ifndef GATELITMAP_VID2_H
#define GATELITMAP_VID2_H

/// @file GateLitMap_vid2.h
/// @brief GateLitMap_vid2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap_vid.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class GateLitMap_vid2 GateLitMap_vid2.h "GateLitMap_vid2.h"
/// @brief VidMap を用いた GateLitMap
///
/// ただし出力の変数だけ別に指定する．
//////////////////////////////////////////////////////////////////////
class GateLitMap_vid2 :
  public GateLitMap_vid
{
public:

  /// @brief コンストラクタ
  /// @param[in] node 対象のノード
  /// @param[in] vid_map 変数番号のマップ
  /// @param[in] ovar 出力の変数
  GateLitMap_vid2(const TpgNode* node,
		  const VidMap& vid_map,
		  SatVarId ovar);

  /// @brief デストラクタ
  virtual
  ~GateLitMap_vid2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力のリテラルを返す．
  virtual
  SatLiteral
  output() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力の変数
  SatVarId mOvar;

};

END_NAMESPACE_YM_SATPG

#endif // GATELITMAP_VID2_H
