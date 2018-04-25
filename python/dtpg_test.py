#! /usr/bin/env python3
#
# @file dtpg_test.py
# @brief Dtpg のテストプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

from satpg_core_d import *


def node_mode(network, fault_type) :
    ndet = 0
    nunt = 0
    nabt = 0
    for fault in network.rep_fault_list() :
        onode = fault.onode
        dtpg = Dtpg(network, fault_type, onode)
        stat, nodeval_list = dtpg(fault)
        if stat == SatBool3._True :
            ndet += 1
        elif stat == SatBool3._False :
            nunt += 1
        elif stat == SatBool3._X :
            nabt += 1
        else :
            assert False
    return ndet, nunt, nabt

def ffr_mode(network, fault_type) :
    ndet = 0
    nunt = 0
    nabt = 0
    for ffr in network.ffr_list() :
        dtpg = DtpgFFR(network, fault_type, ffr)
        for fault in ffr.fault_list() :
            stat, nodeval_list = dtpg(fault)
            if stat == SatBool3._True :
                ndet += 1
            elif stat == SatBool3._False :
                nunt += 1
            elif stat == SatBool3._X :
                nabt += 1
            else :
                assert False
    return ndet, nunt, nabt

def mffc_mode(network, fault_type) :
    ndet = 0
    nunt = 0
    nabt = 0
    for mffc in network.mffc_list() :
        dtpg = DtpgMFFC(network, fault_type, mffc)
        for fault in mffc.fault_list() :
            stat, nodeval_list = dtpg(fault)
            if stat == SatBool3._True :
                ndet += 1
            elif stat == SatBool3._False :
                nunt += 1
            elif stat == SatBool3._X :
                nabt += 1
            else :
                assert False
    return ndet, nunt, nabt


fault_type = FaultType.StuckAt
network = TpgNetwork.read_blif('/home/yusuke/data/public/MCNC.blifdata/C432.blif')

fsim2 = Fsim('Fsim2', network, fault_type)

ndet, nunt, nabt = ffr_mode(network, fault_type)

print('# of total faults:      {}'.format(ndet + nunt + nabt))
print('# of detected faults:   {}'.format(ndet))
print('# of untestable faults: {}'.format(nunt))
print('# of aborted faults:    {}'.format(nabt))
