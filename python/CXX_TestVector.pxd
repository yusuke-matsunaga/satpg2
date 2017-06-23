#
# @file CXX_TestVector.pxd
# @brief TestVector 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_Val3 cimport Val3

ctypedef unsigned int ymuint


cdef extern from "TestVector.h" namespace "nsYm::nsSatpg" :

    # TestVector の cython ヴァージョン
    cdef cppclass TestVector :
        ymuint input_num()
        ymuint dff_num()
        ymuint ppi_num()
        bool is_sa_mode()
        bool is_td_mode()
        ymuint vect_len()
        Val3 ppi_val(ymuint pos)
        Val3 input_val(ymuint pos)
        Val3 dff_val(ymuint pos)
        Val3 aux_input_val(ymuint pos)
        ymuint x_num()
        @staticmethod
        bool is_conflict(const TestVector& tv1, const TestVector& tv2)
        bool operator==(const TestVector& right)
        bool operator<(const TestVector& right)
        bool operator<=(const TestVector& right)
        string bin_str()
        string hex_str()
