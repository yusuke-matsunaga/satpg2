#
# @file fault_type.pxi
# @brief FaultType の Python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from enum import Enum
from CXX_FaultType cimport FaultType as CXX_FaultType
from CXX_FaultType cimport kFtStuckAt, kFtTransitionDelay, kFtNone


## @brief FaultType の Python バージョン
class FaultType(Enum) :
    StuckAt = 0
    TransitionDelay = 1


## @brief C++ の FaultType を Python の FaultType に変換する．
def to_FaultType(CXX_FaultType c_val) :
    if c_val == kFtStuckAt :
        return FaultType.StuckAt
    elif c_val == kFtTransitionDelay :
        return FaultType.TransitionDelay
    else :
        return None

## @brief Python の FaultType を C++ の FaultType に変換する．
cdef CXX_FaultType from_FaultType(val) :
    if val == FaultType.StuckAt :
        return kFtStuckAt
    elif val == FaultType.TransitionDelay :
        return kFtTransitionDelay
    else :
        return kFtNone
