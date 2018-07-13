
### @file CXX_DtpgEngine.pxd
### @brief DtpgEngine 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.


from libcpp cimport bool
from libcpp.string cimport string
from CXX_iostream cimport ostream
from CXX_FaultType cimport FaultType
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgNode cimport TpgNode
from CXX_TpgFFR cimport TpgFFR
from CXX_TpgMFFC cimport TpgMFFC
from CXX_TpgFault cimport TpgFault
from CXX_DtpgResult cimport DtpgResult
from CXX_DtpgStats cimport DtpgStats
from CXX_SatBool3 cimport SatBool3
from CXX_TestVector cimport TestVector

cdef extern from "DtpgEngine.h" namespace "nsYm::nsSatpg" :

    ## @brief DtpgEngine の Cython バージョン
    cdef cppclass DtpgEngine :
        DtpgEngine(const string&, const string&, ostream*, FaultType, const string&,
                   const TpgNetwork&, const TpgFFR&)
        DtpgEngine(const string&, const string&, ostream*, FaultType, const string&,
                   const TpgNetwork&, const TpgMFFC&)
        DtpgResult gen_pattern(const TpgFault*)
        const DtpgStats& stats()
