#ifndef FAULTTYPE_H
#define FAULTTYPE_H

/// @file FaultType.h
/// @brief FaultType の定義ファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief 故障の種類を表す列挙型
//////////////////////////////////////////////////////////////////////
enum FaultType {
  /// @brief 縮退故障
  kFtStuckAt,
  /// @brief 遷移故障
  kFtTransitionDelay,
  /// @brief 不正な値
  kFtNone
};

/// @brief FaultType のストリーム出力演算子
inline
ostream&
operator<<(ostream& s,
	   FaultType ftype)
{
  switch ( ftype ) {
  case kFtStuckAt :         s << "stuck-at fault"; break;
  case kFtTransitionDelay : s << "transition-delay fault"; break;
  case kFtNone:             s << "NONE"; break;
  }
  return s;
}

END_NAMESPACE_YM_SATPG

#endif // FAULTTYPE_H
