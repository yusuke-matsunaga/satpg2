
### @file minpatmgr.pxi
### @brief minpatmgr の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.


cdef class MinPatMgr :
    cdef TpgNetwork __network

    ### @brief 初期化
    ### @param[in] network 対象のネットワーク
    ### @param[in] fault_type 故障の種類
    def __init__(MinPatMgr self, TpgNetwork network, fault_type) :
        self.__network = network
        self.__fault_type = fault_type
        self.__fsarray = [ FaultStatus.Undetected for i in range(network.max_fault_id) ]
        self.__fault_list = []

    ### @brief 故障の状態を設定する．
    def set_fault_status(MinPatMgr self, TpgFault fault, status) :
        assert self.__fsarray[fault.id] == FaultStatus.Undetected
        self.__fsarray[fault.id] = status
        if status == FaultStatus.Detected :
            self.__fault_list.append(fault)

    ### @brief テストパタンを追加する．
    ### @param[in] tv テストパタン
    def add_pat(MinPatMgr self, TestVector tv) :
        self.__tvlist.append(tv)

    ### @brief 検出行列を作る．
    def gen_detection_matrix(MinPatMgr self) :
        cdef Fsim fsim = Fsim('Fsim3', self.__network, self.__fault_type)
        fsim.set_skip_all()
        for fault in self.__fault_list :
            fsim.clear_skip(fault)

def gen_compat_graph(tv_list) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int n = len(tv_list)
    cdef UdGraph graph = UdGraph(n)
    for id1 in range(n - 1) :
        tv1 = tv_list[id1]
        for id2 in range(id1 + 1, n) :
            tv2 = tv_list[id2]
            if not TestVector.is_compatible(tv1, tv2) :
                graph.connect(id1, id2)
    return graph

def gen_colcov(fault_list, tv_list, network, fault_type) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int nv = len(tv_list)
    cdef int nf = len(fault_list)
    cdef Fsim fsim = Fsim('Fsim3', network, fault_type)
    cdef TestVector tv
    cdef int wpos = 0
    cdef int tid_base = 0
    cdef int fid = 0
    cdef ColCov colcov = ColCov(nf, nv)
    fid_dict = {}
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    tv_buff = []
    elem_list = []
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat_id_list in fp_list :
                fid = fid_dict[fault.id]
                for pat_id in pat_id_list :
                    colcov.insert_elem(fid, tid_base + pat_id)
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, pat_id_list in fp_list :
            fid = fid_dict[fault.id]
            for pat_id in pat_id_list :
                colcov.insert_elem(fid, tid_base + pat_id)

    for id1 in range(nv - 1) :
        tv1 = tv_list[id1]
        for id2 in range(id1 + 1, nv) :
            tv2 = tv_list[id2]
            if not TestVector.is_compatible(tv1, tv2) :
                colcov.insert_conflict(id1, id2)

    return colcov

def gen_conflict_list(tv_list) :
    cdef int id1, id2
    cdef TestVector tv1, tv2
    cdef int n = len(tv_list)
    #conflict_list = []
    for id1 in range(n - 1) :
        tv1 = tv_list[id1]
        for id2 in range(id1 + 1, n) :
            tv2 = tv_list[id2]
            if not TestVector.is_compatible(tv1, tv2) :
                #conflict_list.append( (id1, id2) )
                yield id1, id2
    #return conflict_list

def gen_elem_list(fault_list, tv_list, network, fault_type) :
    cdef int nf = len(fault_list)
    cdef Fsim fsim = Fsim('Fsim3', network, fault_type)
    cdef TestVector tv
    cdef int wpos = 0
    cdef int tid_base = 0
    cdef int fid = 0
    fid_dict = {}
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    tv_buff = []
    elem_list = []
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat_id_list in fp_list :
                fid = fid_dict[fault.id]
                for pat_id in pat_id_list :
                    elem_list.append( (fid, tid_base + pat_id) )
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, pat_id_list in fp_list :
            fid = fid_dict[fault.id]
            for pat_id in pat_id_list :
                elem_list.append( (fid, tid_base + pat_id) )
    return elem_list

"""
def gen_covering_matrix(tv_list, network, fault_type) :
    fsim = Fsim('Fsim3', network, fault_type)
    tv_buff = [ TestVector() for i in range(64) ]
    wpos = 0
    pat_list_dict = dict()
    for tv in tv_list :
        tv_buff[wpos] = tv
        wpos += 1
        if wpos == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, pat in fp_list :
                for i in range(64) :
                    if pat & (1 << i) :
                        if not fault.id in pat_list_dict :
                            pat_list_dict[fault.id] = list()
                        pat_list_dict[fault.id].append(tv_buff[i])
            wpos = 0
    if wpos > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for ault, pat in fp_list :
            for i in range(wpos) :
                if pat & (1 << i) :
                    if not fault.id in pat_list_dict :
                        pat_list_dict[fault.id] = list()
                    pat_list_dict[fault.id].append(tv_buff[i])
"""
