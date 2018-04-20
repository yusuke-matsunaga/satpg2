#
# @file CXX_NodeValList.pxd
# @brief CXX_NodeValList 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from libcpp cimport bool
from CXX_TpgNode cimport TpgNode
ctypedef unsigned int ymuint


cdef extern from "NodeValList.h" namespace "nsYm::nsSatpg" :

    ## @brief NodeVal の Cython バージョン
    cdef cppclass NodeVal :
        const TpgNode* node()
        int time()
        bool val()


    ## @brief NodeValList の Cython バージョン
    cdef cppclass NodeValList :
        NodeValList()
        void clear()
        void add(const TpgNode* node, int time, bool val)
        void sort()
        void merge(const NodeValList& src_list)
        void diff(const NodeValList& src_list)
        ymuint size()
        NodeVal elem(ymuint pos)

    bool check_conflict(const NodeValList& src_list1,
                        const NodeValList& src_list2)

    bool check_contain(const NodeValList& src_list1,
                       const NodeValList& src_list2)
