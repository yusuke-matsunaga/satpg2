#! /usr/bin/env python3

### @file compaction.py
### @brief compaction を行うクラス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from satpg_core import TestVector
from satpg_core import UdGraph
from satpg_core import coloring
from satpg_core import gen_compat_graph

### @brief static compaction を行う．
def compaction(tvlist, algorithm) :
    n = len(tvlist)
    print('# of initial patterns: {}'.format(n))
    graph = gen_compat_graph(tvlist)
    nc, color_map = coloring(graph, algorithm)
    print('# of reduced patterns: {}'.format(nc))

    # color_map から色番号ごとのパタン番号リストを作る．
    pat_list_array = [ [] for i in range(nc) ]
    for i in range(n) :
        c = color_map[i]
        assert c > 0 and c <= nc
        tv = tvlist[i]
        pat_list_array[c - 1].append(tv)

    # 各色に対応するパタンを作る．
    new_tvlist = []
    for i in range(nc) :
        tv = TestVector.merge(pat_list_array[i])
        new_tvlist.append(tv)

    return new_tvlist
