
### @file CXX_MFFC.pxd
### @brief CXX_MFFC 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgNode cimport TpgNode


cdef extern from "TpgMFFC.h" namespace "nsYm::nsSatpg" :

    ### @brief TpgMFFC の Cython バージョン
    cdef cppclass TpgMFFC :
        TpgMFFC()
        const TpgNode* root()
        int fault_num()
        const TpgFault* fault(int)
