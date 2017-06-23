#
# @file testvector.pxi
# @brief testvector の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_TvMgr cimport TvMgr as CXX_TvMgr


## @brief TestVector の Python バージョン
cdef class TestVector :
    cdef CXX_TestVector* _thisptr
    cdef TvMgr _tvmgr

    ## @brief 初期化
    def __cinit__(TestVector self) :
        self._thisptr = NULL

    ## @brief 終了処理
    def __dealloc__(TestVector self) :
        if self._thisptr != NULL :
            self._tvmgr.delete_vector(self)


    ## @brief 適正な値を持っている時に True を返す．
    @property
    def is_valid(TestVector self) :
        return self._thisptr != NULL

    ## @brief 入力数を返す．
    @property
    def input_num(TestVector self) :
        if self.is_valid :
            return self._thisptr.input_num()
        return 0

    ## @brief DFF数を返す．
    @property
    def dff_num(TestVector self) :
        if self.is_valid :
            return self._thisptr.dff_num()
        return 0

    ## @brief 疑似入力数を返す．
    @property
    def ppi_num(TestVector self) :
        if self.is_valid :
            return self._thisptr.ppi_num()
        return 0

    ## @brief 縮退故障モードの時 True を返す．
    @property
    def is_sa_mode(TestVector self) :
        return self.is_valid and self._thisptr.is_sa_mode()

    ## @brief 遷移故障モードの時 True を返す．
    @property
    def is_td_mode(TestVector self) :
        return self.is_valid and self._thisptr.is_td_mode()

    ## @brief ビット長を返す．
    @property
    def vect_len(TestVector self) :
        if self.is_valid :
            return self._thisptr.vect_len()
        return 0

    ## @brief 疑似入力番号から対応した値を返す．
    # @param[in] pos 位置番号 ( 0 <= pos < ppi_num )
    #
    # 縮退モードの時に用いられる．
    def ppi_val(TestVector self, pos) :
        if self.is_valid :
            return to_Val3(self._thisptr.ppi_val(pos))
        return Val3.X

    ## @brief 入力番号から対応した値を返す．
    # @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    #
    # 遷移故障モードの時に用いられる．
    def input_val(TestVector self, pos) :
        if self.is_valid :
            return to_Val3(self._thisptr.input_val(pos))
        return Val3.X

    ## @brief DFF番号から対応した値を返す．
    # @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
    #
    # 遷移故障モードの時に用いられる．
    def dff_val(TestVector self, pos) :
        if self.is_valid :
            return to_Val3(self._thisptr.dff_val(pos))
        return Val3.X

    ## @brief 入力番号から対応した２時刻目の値を返す．
    # @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    #
    # 遷移故障モードの時に用いられる．
    def aux_input_val(TestVector self, pos) :
        if self.is_valid :
            return to_Val3(self._thisptr.aux_input_val(pos))
        return Val3.X

    ## @brief X値の個数を返す．
    @property
    def x_num(TestVector self) :
        if self.is_valid :
            return self._thisptr.x_num()
        return 0

    ## @brief 2進表記の文字列を返す．
    @property
    def bin_str(TestVector self) :
        cdef string c_str
        if self.is_valid :
            c_str = self._thisptr.bin_str()
            return c_str.decode('UTF-8')
        return None

    ## @brief 16進表記の文字列を返す．
    @property
    def hex_str(TestVector self) :
        cdef string c_str
        if self.is_valid :
            c_str = self._thisptr.hex_str()
            return c_str.decode('UTF-8')
        return None
