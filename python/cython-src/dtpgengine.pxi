
### @file dtpgengine.pxi
### @brief DtpgEngine の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_DtpgEngine cimport DtpgEngine as CXX_DtpgEngine
from CXX_TpgFault cimport TpgFault as CXX_TpgFault
from CXX_NodeValList cimport NodeValList as CXX_NodeValList
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3
from cython.operator cimport dereference as deref

### @brief Dtpg の Python バージョン
cdef class DtpgEngine :
    cdef CXX_DtpgEngine* _thisptr

    ### @brief 初期化
    def __cinit__(DtpgEngine self, TpgNetwork network, fault_type, TpgNode root,
                  **kwargs) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        cdef string c_sat_type = kwargs.get('sat_type', '').encode('UTF-8')
        cdef string c_sat_option = kwargs.get('sat_option', '').encode('UTF-8')
        cdef string c_jt = kwargs.get('just_type', 'Just2').encode('UTF-8')
        self._thisptr = new CXX_DtpgEngine(c_sat_type, c_sat_option, NULL, c_ftype, c_jt,
                                           network._this, root._thisptr)

    ### @brief 終了処理
    def __dealloc__(DtpgEngine self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief パタン生成を行う．
    def __call__(DtpgEngine self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef TestVector testvect = TestVector()
        cdef CXX_SatBool3 c_bool3 = self._thisptr.dtpg(c_fault, testvect._this)
        return (to_SatBool3(c_bool3), testvect)

    ### @brief 統計情報を得る．
    @property
    def stats(DtpgEngine self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)


### @brief DtpgEngine の Python バージョン(FFRモード用)
cdef class DtpgEngineFFR :
    cdef CXX_DtpgEngine* _thisptr

    ### @brief 初期化
    def __cinit__(DtpgEngine self, TpgNetwork network, fault_type, TpgFFR ffr,
                  **kwargs) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        cdef string c_sat_type = kwargs.get('sat_type', '').encode('UTF-8')
        cdef string c_sat_option = kwargs.get('sat_option', '').encode('UTF-8')
        cdef string c_jt = kwargs.get('just_type', 'Just2').encode('UTF-8')
        self._thisptr = new CXX_DtpgEngine(c_sat_type, c_sat_option, NULL, c_ftype, c_jt,
                                           network._this, deref(ffr._thisptr))

    ### @brief 終了処理
    def __dealloc__(DtpgEngine self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief パタン生成を行う．
    def __call__(DtpgEngine self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef TestVector testvect = TestVector()
        cdef CXX_SatBool3 c_bool3 = self._thisptr.dtpg(c_fault, testvect._this)
        return (to_SatBool3(c_bool3), testvect)

    ### @brief 統計情報を得る．
    @property
    def stats(DtpgEngine self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)


### @brief DtpgEngine の Python バージョン(MFFCモード用)
cdef class DtpgEngineMFFC :
    cdef CXX_DtpgEngine* _thisptr

    ### @brief 初期化
    def __cinit__(DtpgEngine self, TpgNetwork network, fault_type, TpgMFFC mffc,
                  **kwargs) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        cdef string c_sat_type = kwargs.get('sat_type', '').encode('UTF-8')
        cdef string c_sat_option = kwargs.get('sat_option', '').encode('UTF-8')
        cdef string c_jt = kwargs.get('just_type', 'Just2').encode('UTF-8')
        self._thisptr = new CXX_DtpgEngine(c_sat_type, c_sat_option, NULL, c_ftype, c_jt,
                                           network._this, deref(mffc._thisptr))

    ### @brief 終了処理
    def __dealloc__(DtpgEngine self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief パタン生成を行う．
    def __call__(DtpgEngine self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef TestVector testvect = TestVector()
        cdef CXX_SatBool3 c_bool3 = self._thisptr.dtpg(c_fault, testvect._this)
        return (to_SatBool3(c_bool3), testvect)

    ### @brief 統計情報を得る．
    @property
    def stats(DtpgEngine self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)
