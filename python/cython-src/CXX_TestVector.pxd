#
# @file CXX_TestVector.pxd
# @brief TestVector 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_FaultType cimport FaultType
from CXX_Val3 cimport Val3

cdef extern from "TestVector.h" namespace "nsYm::nsSatpg" :

    # TestVector の cython ヴァージョン
    cdef cppclass TestVector :
        int input_num()
        int dff_num()
        int ppi_num()
        FaultType fault_type()
        Val3 ppi_val(int pos)
        Val3 input_val(int pos)
        Val3 dff_val(int pos)
        Val3 aux_input_val(int pos)
        int x_count()
        @staticmethod
        bool is_conflict(const TestVector& tv1, const TestVector& tv2)
        bool operator==(const TestVector& right)
        bool operator<(const TestVector& right)
        bool operator<=(const TestVector& right)
        string bin_str()
        string hex_str()
