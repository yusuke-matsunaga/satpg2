#
# @file tpgnode.pxi
# @brief TpgNode の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp.string cimport string
from CXX_TpgNode cimport TpgNode as CXX_TpgNode
from CXX_TpgDff cimport TpgDff as CXX_TpgDff
from CXX_TpgFault cimport TpgFault as CXX_TpgFault


## @brief TpgNode の Python バージョン
cdef class TpgNode :
    cdef const CXX_TpgNode* _thisptr

    ## @brief 初期化
    def __cinit__(self) :
        _thisptr = NULL

    ## @brief 終了処理
    def __dealloc__(self) :
        # なにもしない．
        pass

    ## @brief 適正な値を持っている時に True を返す．
    @property
    def is_valid(self) :
        return self._thisptr != NULL

    ## @brief ID番号を返す．
    @property
    def id(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.id()

    ## @brief 名前を返す．
    @property
    def name(self) :
        if not self.is_valid :
            return ''
        cdef string c_name = string(self._thisptr.name())
        return c_name.decode('UTF-8')

    ## @brief 外部入力の時に True を返す．
    @property
    def is_primary_input(self) :
        if not self.is_valid :
            return False
        return self._thisptr.is_primary_input()

    ## @brief 入力番号を返す．
    #
    # 入力でなければ値は不定
    @property
    def input_id(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.input_id()

    ## @brief 関連するDFFを返す．
    #
    # DFFに関連しないノードの場合は不定
    @property
    def dff(self) :
        cdef CXX_TpgDff* c_dff
        if not self.is_valid :
            return None
        c_dff = self._thisptr.dff()
        return to_TpgDff(c_dff)

    ## @brief ファンイン数を返す．
    #
    # is_logic == True でなければ値は不定
    @property
    def fanin_num(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.fanin_num()

    ## @brief ファンインを返す．
    # @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
    #
    # is_logic == True でなければ値は不定
    def fanin(self, pos) :
        cdef CXX_TpgNode* c_inode
        if not self.is_valid :
            return None
        c_inode = self._thisptr.fanin(pos)
        return to_TpgNode(c_inode)

    ## @brief ファンアウト数を返す．
    @property
    def fanout_num(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.fanout_num()

    ## @brief ファンアウトを返す．
    # @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
    def fanout(self, pos) :
        if not self.is_valid :
            return 0
        return to_TpgNode(self._thisptr.fanout(pos))

    ## @brief FFRの根のノードを返す．
    def ffr_root(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.ffr_root()
        return to_TpgNode(c_node)

    ## @brief MFFCの根のノードを返す．
    def mffc_root(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.mffc_root()
        return to_TpgNode(c_node)

    ## @brief immediate dominator を返す．
    def imm_dom(self) :
        cdef const CXX_TpgNode* c_node
        if not self._is_valid :
            return None
        c_node = self._thisptr.imm_dom()
        return to_TpgNode(c_node)

    ## @brief このノードの故障数を返す．
    @property
    def fault_num(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.fault_num()

    ## @brief 故障を返す．
    # @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
    def fault(self, pos) :
        cdef const CXX_TpgFault* c_fault
        if not self.is_valid :
            return None
        c_fault = self._thisptr.fault(pos)
        return to_TpgFault(c_fault)

## @brief CXX_TpgNode* から TpgNode を作る関数
cdef to_TpgNode(const CXX_TpgNode* c_node) :
    if c_node == NULL :
        return None
    ans = TpgNode()
    ans._thisptr = c_node
    return ans
