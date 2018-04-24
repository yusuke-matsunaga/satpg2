#! /usr/bin/env python3
#
# @file dtpg_test.py
# @brief Dtpg のテストプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

from satpg_core_d import *


fault_type = FaultType.StuckAt
network = TpgNetwork.read_blif('/home/yusuke/data/public/MCNC.blifdata/C432.blif')

fsim2 = Fsim('Fsim2', network, fault_type)

ndet = 0
nunt = 0
nabt = 0

def node_mode(network) :
    for fault in network.rep_fault_list() :
        onode = fault.onode
        dtpg = Dtpg('', '', fault_type, 'Just2', network, onode)
        stat, nodeval_list = dtpg(fault)
        if stat == SatBool3._True :
            ndet += 1
        elif stat == SatBool3._False :
            nunt += 1
        elif stat == SatBool3._X :
            nabt += 1
        else :
            assert False

def ffr_mode(network) :
    for ffr in network.ffr_list() :
        dtpg = Dtpg('', '', fault_type, 'Just2', network, ffr)
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

def mffc_mode(network) :
    for mffc in network.mffc_list() :
        dtpg = Dtpg('', '', fault_type, 'Just2', network, mffc)
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

print('# of total faults:      {}'.format(ndet + nunt + nabt))
print('# of detected faults:   {}'.format(ndet))
print('# of untestable faults: {}'.format(nunt))
print('# of aborted faults:    {}'.format(nabt))
