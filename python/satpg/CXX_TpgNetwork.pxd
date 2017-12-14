# @file CXX_TpgNetwork.pxd
# @brief TpgNetwork 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_ClibCellLibrary cimport ClibCellLibrary
from CXX_TpgNode cimport TpgNode
from CXX_TpgDff cimport TpgDff
from CXX_TpgFault cimport TpgFault

ctypedef unsigned int ymuint


cdef extern from "TpgNetwork.h" namespace "nsYm::nsSatpg" :

    ## @brief TpgNetwork の cython バージョン
    cdef cppclass TpgNetwork :
        TpgNetwork()
        bool read_blif(const string& filename)
        bool read_blif(const string& filename, const ClibCellLibrary& cell_library)
        bool read_iscas89(const string& filename)
        ymuint node_num()
        const TpgNode* node(ymuint)
        ymuint input_num()
        const TpgNode* input(ymuint)
        ymuint output_num()
        const TpgNode* output(ymuint)
        const TpgNode* output2(ymuint)
        ymuint ppi_num()
        const TpgNode* ppi(ymuint)
        ymuint ppo_num()
        const TpgNode* ppo(ymuint)
        ymuint dff_num()
        const TpgDff* dff(ymuint pos)
        ymuint max_fault_id()
        ymuint rep_fault_num()
        const TpgFault* rep_fault(ymuint pos)
