#! /usr/bin/env python3

### @file dtpg.py
### @brief DTPG を行うクラス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from satpg_core import DtpgEngine, DtpgEngineFFR, DtpgEngineMFFC
from satpg_core import Fsim
from satpg_core import FaultStatus, FaultStatusMgr
from satpg_core import UdGraph
from satpg_core import coloring


### @brief DTPG を行うクラス
class Dtpg :

    ### @brief 初期化
    def __init__(self, network, fault_type) :
        self.__network = network
        self.__fault_type = fault_type
        self.__fsim3 = Fsim('Fsim3', network, fault_type)
        self.__fsim3.clear_skip_all()
        self.__fsmgr = FaultStatusMgr(network)
        self.__tvlist = []

    ### @brief single mode でパタン生成を行う．
    def single_mode(self) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for fault in self.__network.rep_fault_list() :
            if self.__fsmgr.get(fault) == FaultStatus.Undetected :
                onode = fault.onode
                dtpg = DtpgEngine(self.__network, self.__fault_type, onode)
                self.__call_dtpg(dtpg, fault)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief FFR mode でパタン生成を行う．
    def ffr_mode(self) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for ffr in self.__network.ffr_list() :
            dtpg = DtpgEngineFFR(self.__network, self.__fault_type, ffr)
            for fault in ffr.fault_list() :
                if self.__fsmgr.get(fault) == FaultStatus.Undetected :
                    self.__call_dtpg(dtpg, fault)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief MFFC mode でパタン生成を行う．
    def mffc_mode(self) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for mffc in self.__network.mffc_list() :
            dtpg = DtpgEngineMFFC(self.__network, self.__fault_type, mffc)
            for fault in mffc.fault_list() :
                if self.__fsmgr.get(fault) == FaultStatus.Undetected :
                    self.__call_dtpg(dtpg, fault)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief 全モードで共通な処理
    def __call_dtpg(self, dtpg, fault) :
        stat, testvect = dtpg(fault)
        if stat == FaultStatus.Detected :
            self.__ndet += 1
            # fault を検出可能故障と記録
            self.__fsmgr.set(fault, FaultStatus.Detected)
            self.__fsim3.set_skip(fault)
            # fault のパタンとして testvect を記録
            self.__tvlist.append(testvect)
            # このパタンで検出される他の故障を調べる．
            for fault in self.__fsim3.sppfp(testvect) :
                self.__fsmgr.set(fault, FaultStatus.Detected)
                self.__fsim3.set_skip(fault)
                self.__ndet += 1
        elif stat == FaultStatus.Untestable :
            self.__nunt += 1
            # fault をテスト不能故障と記録
            self.__fsmgr.set(fault, FaultStatus.Untestable)
            self.__fsim3.set_skip(fault)
        elif stat == FaultStatus.Undetected :
            self.__nabt += 1
        else :
            assert False

    ### @brief static compaction を行う．
    def compaction(self) :
        n = len(self.__tvlist)
        graph = UdGraph(n)
        for i in range(n - 1) :
            tv1 = self.__tvlist[i]
            for j in range(i + 1, n) :
                tv2 = self.__tvlist[j]
                if not (tv1 & tv2) :
                    graph.connect(i, j)
        nc, color_map = coloring(graph)
        print('# of initial patterns: {}'.format(n))
        print('# of reduced patterns: {}'.format(nc))

    ### @brief テストパタンのリストを返す．
    @property
    def tvlist(self) :
        return self.__tvlist
