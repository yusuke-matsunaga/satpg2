#ifndef GATELITMAP_VID_H
#define GATELITMAP_VID_H

/// @file GateLitMap_vid.h
/// @brief GateLitMap_vid のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "GateLitMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class GateLitMap_vid GateLitMap_vid.h "GateLitMap_vid.h"
/// @brief VidMap を用いた GateLitMap
//////////////////////////////////////////////////////////////////////
class GateLitMap_vid :
  public GateLitMap
{
public:

  /// @brief コンストラクタ
  /// @param[in] node 対象のノード
  /// @param[in] vid_map 変数番号のマップ
  GateLitMap_vid(const TpgNode* node,
		 const VidMap& vid_map);

  /// @brief デストラクタ
  virtual
  ~GateLitMap_vid();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  virtual
  ymuint
  input_size() const;

  /// @brief 入力のリテラルを返す．
  /// @param[in] pos 入力位置 ( 0 <= pos < input_size() )
  virtual
  SatLiteral
  input(ymuint pos) const;

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

  // ノード
  const TpgNode* mNode;

  // 変数番号のマップ
  const VidMap& mVidMap;

};

END_NAMESPACE_YM_SATPG

#endif // GATELITMAP_VID_H
