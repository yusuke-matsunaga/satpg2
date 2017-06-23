#
# @file CXX_TpgNode.pxd
# @brief TpgNode 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_TpgDff cimport TpgDff
from CXX_TpgFault cimport TpgFault

ctypedef unsigned int ymuint


cdef extern from "TpgNode.h" namespace "nsYm::nsSatpg" :

    ## @brief TpgNode の cython バージョン
    cdef cppclass TpgNode :
        ymuint id()
        const char* name()
        bool is_primary_input()
        bool is_primary_output()
        bool is_dff_input()
        bool is_dff_output()
        bool is_dff_clock()
        bool is_dff_clear()
        bool is_dff_preset()
        bool is_ppi()
        bool is_ppo()
        bool is_logic()
        ymuint input_id()
        ymuint output_id()
        ymuint output_id2()
        TpgDff* dff()
#        GateType gate_type()
#        Val3 cval()
#        Val3 nval()
#        Val3 coval()
#        Val3 noval()
        ymuint fanin_num()
        TpgNode* fanin(ymuint)
        ymuint fanout_num()
        TpgNode* fanout(ymuint)
        const TpgNode* ffr_root()
#        const TpgFFR* ffr()
        const TpgNode* mffc_root()
#        const TpgMFFC* mffc()
        const TpgNode* imm_dom()
        ymuint fault_num()
        const TpgFault* fault(ymuint)
