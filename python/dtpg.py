#! /usr/bin/env python3

### @file dtpg.py
### @brief DTPG を行うクラス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from satpg_core import DtpgEngine, DtpgEngineFFR, DtpgEngineMFFC
from satpg_core import FaultStatus, FaultStatusMgr
from satpg_core import SatBool3


### @brief DTPG を行うクラス
class Dtpg :

    ### @brief 初期化
    def init(self, network, fault_type) :
        self.__network = network
        self.__fault_type = fault_type
        self.__fsmgr = FaultStatusMgr(network)
        self.__tvlist = []

    ### @brief single mode でパタン生成を行う．
    def single_mode(self)
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for fault in self.__network.rep_fault_list() :
            onode = fault.onode
            dtpg = DtpgEngine(self.__network, self.__fault_type, onode)
            self.__call_dtpg(dtpg, fault)

    ### @brief FFR mode でパタン生成を行う．
    def ffr_mode(self) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for ffr in self.__network.ffr_list() :
            dtpg = DtpgEngineFFR(self.__network, self.__fault_type, ffr)
            for fault in ffr.fault_list() :
                self.__call_dtpg(dtpg, fault)

    ### @brief MFFC mode でパタン生成を行う．
    def mffc_mode(self) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        for mffc in self.__network.mffc_list() :
            dtpg = DtpgEngineMFFC(self.__network, self.__fault_type, mffc)
            for fault in mffc.fault_list() :
                self.__call_dtpg(dtpg, fault)

    ### @brief 全モードで共通な処理
    def __call_dtpg(self, dtpg, fault) :
        stat, testvect = dtpg(fault)
        if stat == SatBool3._True :
            self.__ndet += 1
            # fault を検出可能故障と記録
            self.__fsmgr.set(fault, FaultStatus.Detected)
            # fault のパタンとして testvect を記録
            self.__tvlist.append(testvect)
        elif stat == SatBool3._False :
            self.__nunt += 1
            # fault をテスト不能故障と記録
            self.__fsmgr.set(fault, FaultStatus.Untestable)
        else stat == SatBool3._X :
            self.__nabt += 1
        else :
            assert False
