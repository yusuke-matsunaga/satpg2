#
# @file CXX_TvMgr.pxd
# @brief TvMgr 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TpgNetwork cimport TpgNetwork
from CXX_TestVector cimport TestVector
ctypedef unsigned int ymuint


cdef extern from "TvMgr.h" namespace "nsYm::nsSatpg" :

    ## @brief TvMgr の cython バージョン
    cdef cppclass TvMgr :
        TvMgr()
        void clear()
        void init(const TpgNetwork& network)
        ymuint input_num()
        ymuint dff_num()
        ymuint sa_vect_len()
        ymuint td_vect_len()
        TestVector* new_sa_vector()
        TestVector* new_td_vector()
        void delete_vector(TestVector* tv)
