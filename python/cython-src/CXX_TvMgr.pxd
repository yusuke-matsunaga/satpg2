#
# @file CXX_TvMgr.pxd
# @brief TvMgr 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TpgNetwork cimport TpgNetwork
from CXX_TestVector cimport TestVector
from CXX_FaultType cimport FaultType
ctypedef unsigned int ymuint


cdef extern from "TvMgr.h" namespace "nsYm::nsSatpg" :

    ## @brief TvMgr の cython バージョン
    cdef cppclass TvMgr :
        TvMgr(const TpgNetwork& network, FaultType fault_type)
        void clear()
        ymuint input_num()
        ymuint dff_num()
        TestVector* new_vector()
        void delete_vector(TestVector* tv)
