
# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( fsim )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (tpg_network_SOURCES
  tpg_network/TpgNetwork.cc
  tpg_network/TpgDff.cc
  tpg_network/TpgGateInfo.cc
  tpg_network/TpgNode.cc
  tpg_network/NodeMap.cc
  tpg_network/TpgPPI.cc
  tpg_network/TpgPPO.cc
  tpg_network/TpgInput.cc
  tpg_network/TpgOutput.cc
  tpg_network/TpgDffInput.cc
  tpg_network/TpgDffOutput.cc
  tpg_network/TpgDffControl.cc
  tpg_network/TpgDffClock.cc
  tpg_network/TpgDffClear.cc
  tpg_network/TpgDffPreset.cc
  tpg_network/TpgLogic.cc
  tpg_network/TpgLogicC0.cc
  tpg_network/TpgLogicC1.cc
  tpg_network/TpgLogicBUFF.cc
  tpg_network/TpgLogicNOT.cc
  tpg_network/TpgLogicAND.cc
  tpg_network/TpgLogicNAND.cc
  tpg_network/TpgLogicOR.cc
  tpg_network/TpgLogicNOR.cc
  tpg_network/TpgLogicXOR.cc
  tpg_network/TpgLogicXNOR.cc
  tpg_network/TpgFFR.cc
  tpg_network/TpgMFFC.cc

  tpg_network/TpgFault.cc
  tpg_network/TpgFaultMgr.cc

  tvect/BitVector.cc
  tvect/TestVector.cc
  tvect/TvMgr.cc

  tpg_network/NodeValList.cc
  )

set (fsim_SOURCES
  fsim/Fsim.cc
  )

set (struct_sat_SOURCES
  struct_sat/FoCone.cc
  struct_sat/StructSat.cc

  struct_sat/Extractor.cc

  #struct_sat/GateLitMap_vect.cc
  struct_sat/GateLitMap_vid.cc
  struct_sat/GateLitMap_vid2.cc

  struct_sat/ValMap_model.cc
  )

set (dtpg_SOURCES
  dtpg/Dtpg.cc
  #dtpg/DtpgImpl.cc
  dtpg/DtpgImplM.cc

  dtpg/DtpgImpl_new.cc

  dtpg/Dtpg2.cc
  dtpg/Dtpg2Impl.cc
  )

set (dop_SOURCES
  dop/DopBase.cc
  dop/DopDrop.cc
  dop/DopDummy.cc
  dop/DopList.cc
  dop/DopTvList.cc
  dop/DopVerify.cc
  dop/DopVerifyResult.cc
  )

set (uop_SOURCES
  uop/UopBase.cc
  uop/UopDummy.cc
  uop/UopList.cc
  uop/UopSkip.cc
  )

set (bt_SOURCES
  bt/BackTracer.cc
  bt/BtImpl.cc
  bt/BtJust1.cc
  bt/BtJust2.cc
  bt/BtSimple.cc
  )

set (jt_SOURCES
  jt/Just1.cc
  jt/Just2.cc
  jt/JustBase.cc
  jt/JustSimple.cc
  )

set (sa_SOURCES
  sa/minpat/Verifier.cc
  sa/minpat/MinPatBase.cc
  sa/minpat/MinPatNaive.cc
  sa/minpat/MinPatSimple.cc
  sa/minpat/MinPatSimple2.cc
  sa/minpat/MinPatDsatur.cc
  sa/minpat/MinPatDsatur2.cc
  sa/minpat/FaultAnalyzer.cc
  sa/minpat/EqChecker.cc
  sa/minpat/DomChecker.cc
  sa/minpat/ConflictChecker.cc
  sa/minpat/FgMgrBase.cc
  sa/minpat/Compactor.cc
  sa/minpat/EqSet.cc
  sa/minpat/McCompactor.cc

  sa/minpat/StructSat.cc
  sa/minpat/FoCone.cc
  sa/minpat/Extractor.cc

  sa/rtpg/RtpgImpl.cc
  )

set (td_SOURCES
  td/rtpg/RtpgP1.cc
  td/rtpg/RtpgP2.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( satpg_common
  ${tpg_network_SOURCES}
  ${fsim_SOURCES}
  ${struct_sat_SOURCES}
  ${dtpg_SOURCES}
  ${dop_SOURCES}
  ${uop_SOURCES}
  ${bt_SOURCES}
  ${jt_SOURCES}
  ${sa_SOURCES}
  ${td_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
