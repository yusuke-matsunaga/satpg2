#
# @file tvmgr.pxi
# @brief tvmgr の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from CXX_TvMgr cimport TvMgr as CXX_TvMgr
from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_TpgNetwork cimport TpgNetwork as CXX_TpgNetwork


## @brief TvMgr の Python バージョン
cdef class TvMgr :
    cdef CXX_TvMgr _this

    ## @brief クリアする
    def clear(TvMgr self) :
        self._this.clear()

    ## @brief 初期化する．
    # @param[in] network 対象のネットワーク
    def init(TvMgr self, TpgNetwork network) :
        self._this.init(network._this)

    ## @brief 入力数を得る．
    @property
    def input_num(TvMgr self) :
        return self._this.input_num()

    ## @brief DFF数を得る．
    @property
    def dff_num(TvMgr self) :
        return self._this.dff_num()

    ## @brief 縮退故障モードのベクタ長を返す．
    @property
    def sa_vect_len(TvMgr self) :
        return self._this.sa_vect_len()

    ## @brief 遷移故障モードのベクタ長を返す．
    @property
    def td_vect_len(TvMgr self) :
        return self._this.td_vect_len()

    ## @brief 縮退故障用のテストベクタを生成する．
    def new_sa_vector(TvMgr self) :
        cdef CXX_TestVector* c_vect = self._this.new_sa_vector()
        ans = TestVector()
        ans._thisptr = c_vect
        ans._tvmgr = self
        return ans

    ## @brief 縮退故障用のテストベクタを生成する．
    def new_td_vector(TvMgr self) :
        cdef CXX_TestVector* c_vect = self._this.new_td_vector()
        ans = TestVector()
        ans._thisptr = c_vect
        ans._tvmgr = self
        return ans

    ## @brief テストベクタを削除する．
    def delete_vector(TvMgr self, TestVector vect) :
        if not vect.is_valid :
            return
        self._this.delete_vector(vect._thisptr)
