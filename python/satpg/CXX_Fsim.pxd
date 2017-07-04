#
# @file CXX_Fsim.pxd
# @brief Fsim 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from libcpp cimport bool
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgFault cimport TpgFault
from CXX_TestVector cimport TestVector
from CXX_NodeValList cimport NodeValList
from CXX_FaultType cimport FaultType
from CXX_vector cimport vector

ctypedef unsigned int ymuint
ctypedef unsigned long PackedVal


cdef extern from "Fsim.h" namespace "nsYm::nsSatpg" :

    ## @brief Fsim の cython バージョン
    cdef cppclass Fsim :
        @staticmethod
        Fsim* new_Fsim2()
        @staticmethod
        Fsim* new_Fsim3()
        void set_network(const TpgNetwork& network)
        void set_skip_all()
        void set_skip(const TpgFault* f)
        void set_skip(const vector[const TpgFault*]& fault_list)
        void clear_skip_all()
        void clear_skip(const TpgFault* f)
        void clear_skip(const vector[const TpgFault*]& fault_list)
        bool sa_spsfp(const TestVector* tv, const TpgFault* f)
        bool sa_spsfp(const NodeValList& assign_list, const TpgFault* f)
        ymuint sa_sppfp(const TestVector* tv)
        ymuint sa_sppfp(const NodeValList& assign_list)
        ymuint sa_ppsfp()
        bool td_spsfp(const TestVector* tv, const TpgFault* f)
        bool td_spsfp(const NodeValList& assign_list, const TpgFault* f)
        ymuint td_sppfp(const TestVector* tv)
        ymuint td_sppfp(const NodeValList& assign_list)
        ymuint td_ppsfp()
        ymuint td_calc_wsa(const TestVector* tv, bool weighted)
        bool spsfp(const TestVector* tv, const TpgFault* f, FaultType fault_type)
        bool spsfp(const NodeValList& assign_list, const TpgFault* f, FaultType fault_type)
        ymuint sppfp(const TestVector* tv, FaultType fault_type)
        ymuint sppfp(const NodeValList& assign_list, FaultType fault_type)
        ymuint ppsfp(FaultType fault_type)
        void clear_patterns()
        void set_pattern(ymuint pos, const TestVector* tv)
        const TestVector* get_pattern(ymuint pos)
        ymuint det_fault_num()
        const TpgFault* det_fault(ymuint pos)
        PackedVal det_fault_pat(ymuint pos)
