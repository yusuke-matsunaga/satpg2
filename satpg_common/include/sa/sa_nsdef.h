#ifndef SATPG_SA_H
#define SATPG_SA_H

/// @file sa_nsdef.h
/// @brief Stuck-At テストの SATPG 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG_SA \
BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE(nsSa)

#define END_NAMESPACE_YM_SATPG_SA \
END_NAMESPACE(nsSa) \
END_NAMESPACE_YM_SATPG


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class FoCone;
class StructSat;
class Rtpg;
class RtpgStats;
class MinPat;
class MinPatStats;

END_NAMESPACE_YM_SATPG_SA

#endif // SATPG_SA_H
