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


cdef extern from "TpgNetwork.h" namespace "nsYm::nsSatpg" :

    ## @brief TpgNetwork の cython バージョン
    cdef cppclass TpgNetwork :
        TpgNetwork()
        bool read_blif(const string& filename)
        bool read_blif(const string& filename, const ClibCellLibrary& cell_library)
        bool read_iscas89(const string& filename)
        int node_num()
        const TpgNode* node(int)
        int input_num()
        const TpgNode* input(int)
        int output_num()
        const TpgNode* output(int)
        const TpgNode* output2(int)
        int ppi_num()
        const TpgNode* ppi(int)
        int ppo_num()
        const TpgNode* ppo(int)
        int dff_num()
        const TpgDff& dff(int pos)
        int max_fault_id()
        int rep_fault_num()
        const TpgFault* rep_fault(int pos)
