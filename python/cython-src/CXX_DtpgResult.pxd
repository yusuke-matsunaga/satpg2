#
# @file CXX_DtpgResult.pxd
# @brief CXX_DtpgResult 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.


from CXX_SatBool3 cimport SatBool3
from CXX_TestVector cimport TestVector


cdef extern from "DtpgResult.h" namespace "nsYm::nsSatpg" :

    ### @brief DtpgResult の Cython バージョン
    cdef cppclass DtpgResult :
        DtpgResult()
        DtpgResult(SatBool3)
        DtpgResult(const TestVector&)
        SatBool3 stat()
        const TestVector& testvector()
