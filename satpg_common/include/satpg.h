#ifndef SATPG_H
#define SATPG_H

/// @file satpg.h
/// @brief SATPG 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2013, 2017 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSatpg)

#define END_NAMESPACE_YM_SATPG \
END_NAMESPACE(nsSatpg) \
END_NAMESPACE_YM


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class TpgNetwork;
class TpgDff;
class TpgGateInfo;
class TpgNode;
class TpgMFFC;
class TpgFFR;

class TpgFault;
class TpgFaultMgr;

class NodeValList;
class InputVector;
class DffVector;
class TestVector;
class TvMgr;

class Dtpg;
class DetectOp;
class DopVerifyResult;
class UntestOp;

class BackTracer;
class Justifier;

class VidMap;
class ValMap;

class Fsim;

enum class GateType;
enum class Val3;

END_NAMESPACE_YM_SATPG

#endif // SATPG_H
