#
# @file CXX_FaultType.pxd
# @brief FaultType 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "FaultType.h" namespace "nsYm::nsSatpg" :

    cdef enum FaultType :
        kFtStuckAt,
        kFtTransitionDelay,
        kFtNone
