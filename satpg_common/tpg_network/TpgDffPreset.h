#ifndef TPGDFFPRESET_H
#define TPGDFFPRESET_H

/// @file TpgDffPreset.h
/// @brief TpgDffPreset のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgDffControl.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgDffPreset TpgDffPreset.h "TpgDffPreset.h"
/// @brief DFFのプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffPreset :
  public TpgDffControl
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanin ファンインのノード
  TpgDffPreset(int id,
	       const TpgDff* dff,
	       TpgNode* fanin);

  /// @brief デストラクタ
  virtual
  ~TpgDffPreset();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_preset() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SATPG

#endif // TPGDFFPRESET_H
