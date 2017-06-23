#
# @file CXX_CellLibrary.pxd
# @brief CellLibrary の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2016 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string

ctypedef unsigned int ymuint


cdef extern from "ym/CellLibrary.h" namespace "nsYm::nsCell" :

    # CellLibrary の cython バージョン
    cdef cppclass CellLibrary :

        @staticmethod
        CellLibrary* new_obj()
        string name()

    bool read_mislib(const string&, CellLibrary*)
    bool read_liberty(const string&, CellLibrary*)
