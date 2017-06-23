#
# @file CXX_TpgDff.pxd
# @brief TpgDff 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TpgNode cimport TpgNode
ctypedef unsigned int ymuint


cdef extern from "TpgDff.h" namespace "nsYm::nsSatpg" :

    ## @brief TpgDff の Cython バージョン
    cdef cppclass TpgDff :
        ymuint id()
        TpgNode* input()
        TpgNode* output()
        TpgNode* clock()
        TpgNode* clear()
        TpgNode* preset()
