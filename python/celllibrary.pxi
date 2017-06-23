#
# @file celllibrary.pxi
# @brief CellLibrary の python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_CellLibrary cimport CellLibrary as CXX_CellLibrary
from CXX_CellLibrary cimport read_mislib as c_read_mislib
from CXX_CellLibrary cimport read_liberty as c_read_liberty


# @brief CellLibrary の Python バージョン
cdef class CellLibrary :
    cdef CXX_CellLibrary* _thisptr

    def __cinit__(CellLibrary self) :
        self._thisptr = CXX_CellLibrary.new_obj()

    def __dealloc__(CellLibrary self) :
        if self._thisptr != NULL :
            del self._thisptr

    def read_mislib(CellLibrary self, str filename) :
        cdef string c_filename = filename.encode('UTF-8')
        return c_read_mislib(c_filename, self._thisptr)

    def read_liberty(CellLibrary self, str filename) :
        cdef string c_filename = filename.encode('UTF-8')
        return c_read_liberty(c_filename, self._thisptr)
