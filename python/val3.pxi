#
# @file val3.pxi
# @brief val3 の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from enum import Enum
from CXX_Val3 cimport Val3 as CXX_Val3
from CXX_Val3 cimport kValX, kVal0, kVal1

## @brief Val3 の Python バージョン
class Val3(Enum) :
    ValX = 0
    Val0 = 1
    Val1 = 2


## C++ の Val3 を Python の Val3 に変換する
def to_Val3(CXX_Val3 c_val) :
    if c_val == kValX :
        return Val3.ValX
    elif c_val == kVal0 :
        return Val3.Val0
    elif c_val == kVal1 :
        return Val3.Val1
