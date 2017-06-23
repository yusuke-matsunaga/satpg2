#
# @file CXX_Val3.pxd
# @brief Val3 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "Val3.h" namespace "nsYm::nsSatpg" :

    cdef enum Val3 :
        kValX,
        kVal0,
        kVal1
