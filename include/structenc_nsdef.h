#ifndef STRUCT_ENC_NSDEF_H
#define STRUCT_ENC_NSDEF_H

/// @file struct_enc_nsdef.h
/// @brief StructEnc 用の名前空間の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG_STRUCTENC \
BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE(nsStructEnc)

#define END_NAMESPACE_YM_SATPG_STRUCTENC \
END_NAMESPACE(nsStructEnc) \
END_NAMESPACE_YM_SATPG


BEGIN_NAMESPACE_YM_SATPG_STRUCTENC

//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class StructEnc;
class PropCone;

END_NAMESPACE_YM_SATPG_STRUCTENC


//////////////////////////////////////////////////////////////////////
// 外部から用いられるクラス名のエクスポート
//////////////////////////////////////////////////////////////////////

BEGIN_NAMESPACE_YM_SATPG

using nsStructEnc::StructEnc;

END_NAMESPACE_YM_SATPG


#endif // STRUCT_ENC_NSDEF_H
