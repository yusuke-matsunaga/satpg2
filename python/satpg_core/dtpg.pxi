
### @file dtpg.pxi
### @brief dtpg の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_Dtpg cimport Dtpg as CXX_Dtpg
from CXX_TpgFault cimport TpgFault as CXX_TpgFault
from CXX_NodeValList cimport NodeValList as CXX_NodeValList
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3


### @brief Dtpg の Python バージョン
cdef class Dtpg :
    cdef CXX_Dtpg* _thisptr

    ### @brief 初期化
    def __cinit__(Dtpg self, ) :
        pass

    ### @brief パタン生成を行う．
    def __call__(Dtpg self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef CXX_NodeValList c_nodeval_list
        cdef CXX_SatBool3 c_bool3 = self._thisptr.dtpg(c_fault, c_nodeval_list)
        return (to_SatBool3(c_bool3), to_NodeValList(c_nodeval_list))

    ### @brief 統計情報を得る．
    @property
    def stats(Dtpg self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)
