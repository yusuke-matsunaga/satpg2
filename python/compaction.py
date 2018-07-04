#! /usr/bin/env python3

### @file compaction.py
### @brief compaction を行うクラス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from satpg_core import TestVector
from satpg_core import Fsim
from satpg_core import UdGraph
from satpg_core import MinCov
from satpg_core import gen_compat_graph
from satpg_core import Val3


### @brief グラフ彩色を用いた圧縮を行う．
def coloring(tv_list, algorithm = '') :
    n = len(tv_list)
    print('*** Graph Coloring({}) ***'.format(algorithm))
    print('# of initial patterns: {:8d}'.format(n))
    graph = gen_compat_graph(tv_list)
    print('gen_compat_graph() end')
    nc, color_map = graph.coloring(algorithm)
    print('# of reduced patterns: {:8d}'.format(nc))

    # color_map から色番号ごとのパタン番号リストを作る．
    pat_list_array = [ [] for i in range(nc) ]
    for i in range(n) :
        c = color_map[i]
        assert c > 0 and c <= nc
        tv = tv_list[i]
        pat_list_array[c - 1].append(tv)

    # 各色に対応するパタンを作る．
    new_tv_list = []
    for i in range(nc) :
        tv = TestVector.merge(pat_list_array[i])
        new_tv_list.append(tv)

    return new_tv_list


### @brief 最小被覆を用いた圧縮を行う．
def mincov(fault_list, tv_list, network, fault_type, algorithm = '') :

    print('*** Minimum Covering ***')
    print('# of initial patterns: {:8d}'.format(len(tv_list)))

    nf = len(fault_list)
    fid_dict = {}
    fid = 0
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    assert fid == nf

    def do_fsim(fsim, tv_buff, fid_dict, tid_base, mincov) :
        assert len(tv_buff) <= 64

        fp_list = fsim.ppsfp(tv_buff)
        for fault, patid_list in fp_list :
            fid = fid_dict[fault.id]
            for patid in patid_list :
                mincov.insert_elem(fid, tid_base + patid)

    mincov = MinCov(nf, len(tv_list))
    fsim = Fsim('Fsim3', network, fault_type)
    tv_buff = []
    wpos = 0
    tid_base = 0
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            do_fsim(fsim, tv_buff, fid_dict, tid_base, mincov)
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        do_fsim(fsim, tv_buff, fid_dict, tid_base, mincov)

    cost, solution = mincov.heuristic()

    print('# of reduced patterns: {:8d}'.format(cost))

    return [ tv_list[id] for id in solution ]
