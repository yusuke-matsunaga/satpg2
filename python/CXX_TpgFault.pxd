#
# @file CXX_TpgFault.pxd
# @brief TpgFault 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_TpgNode cimport TpgNode

ctypedef unsigned int ymuint


cdef extern from "TpgFault.h" namespace "nsYm::nsSatpg" :

    ## @brief TpgFault の Cython バージョン
    cdef cppclass TpgFault :
        ymuint id()
        const TpgNode* tpg_inode()
        const TpgNode* tpg_onode()
        bool is_stem_fault()
        bool is_branch_fault()
        ymuint fault_pos()
        ymuint tpg_pos()
        int val()
        string str()
        bool is_rep()
        const TpgFault* rep_fault()
#        TpgFFR* ffr()
#        TpgMFFC* mffc()
