
### @file CXX_TpgFFR.pxd
### @brief CXX_TpgFFR 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgNode cimport TpgNode


cdef extern from "TpgFFR.h" namespace "nsYm::nsSatpg" :

    ### @brief TpgFFR の Cython バージョン
    cdef cppclass TpgFFR :
        TpgFFR()
        const TpgNode* root()
        int fault_num()
