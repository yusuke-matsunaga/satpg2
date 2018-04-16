
### @file CXX_Dtpg.pxd
### @brief Dtpg 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.


from libcpp cimport bool
from libcpp.string cimport string
from CXX_FaultType cimport FaultType
from CXX_Justifier cimport Justifier
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgNode cimport TpgNode
from CXX_TpgFFR cimport TpgFFR
from CXX_TpgMFFC cimport TpgMFFC
from CXX_TpgFault cimport TpgFault
from CXX_DtpgStats cimport DtpgStats
from CXX_SatBool3 cimport SatBool3
from CXX_NodeValList cimport NodeValList

cdef extern from "<iostream>" namespace "std":
    cdef cppclass ostream:
        ostream& write(const char*, int) except +

# obviously std::ios_base isn't a namespace, but this lets
# Cython generate the correct C++ code
cdef extern from "<iostream>" namespace "std::ios_base":
    cdef cppclass open_mode:
        pass
    cdef open_mode binary
    # you can define other constants as needed

cdef extern from "<fstream>" namespace "std":
    cdef cppclass ofstream(ostream):
        # constructors
        ofstream(const char*) except +
        ofstream(const char*, open_mode) except+

cdef extern from "Dtpg.h" namespace "nsYm::nsSatpg" :

    ## @brief Dtpg の Cython バージョン
    cdef cppclass Dtpg :
        Dtpg(const string&, const string&, ostream*, FaultType, Justifier&,
             const TpgNetwork&, const TpgNode*)
        Dtpg(const string&, const string&, ostream*, FaultType, Justifier&,
             const TpgNetwork&, const TpgFFR&)
        Dtpg(const string&, const string&, ostream*, FaultType, Justifier&,
             const TpgNetwork&, const TpgMFFC&)
        SatBool3 dtpg(const TpgFault*, NodeValList&)
        const DtpgStats& stats()