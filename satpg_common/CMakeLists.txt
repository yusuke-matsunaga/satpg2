
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

  tvect/TestVector.cc
  tvect/TvMgr.cc

  tpg_network/NodeValList.cc

  tpg_network/GateLitMap_vect.cc
  tpg_network/GateLitMap_vid.cc

  tpg_network/ValMap.cc
  )

set (fsim_SOURCES
  fsim/Fsim.cc
  )

set (dop_SOURCES
  dop/DopBase.cc
  dop/DopDrop.cc
  dop/DopDummy.cc
  dop/DopList.cc
  dop/DopTvList.cc
  dop/DopSaVerify.cc
  dop/DopTdVerify.cc
  )

set (uop_SOURCES
  uop/UopBase.cc
  uop/UopDummy.cc
  uop/UopList.cc
  uop/UopSkip.cc
  )

set (sa_SOURCES
  sa/bt/BackTracer.cc
  sa/bt/BtImpl.cc
  sa/bt/BtJust1.cc
  sa/bt/BtJust2.cc
  sa/bt/BtJustBase.cc
  sa/bt/BtSimple.cc

  sa/dtpg/Dtpg.cc
  sa/dtpg/DtpgImpl.cc
  sa/dtpg/DtpgImplM.cc

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

  sa/rtpg/RtpgImpl.cc

  sa/struct_sat/StructSat.cc
  sa/struct_sat/FoCone.cc
  sa/struct_sat/Extractor.cc
  )

set (td_SOURCES
  td/bt/BackTracer.cc
  td/bt/BtImpl.cc
  td/bt/BtJust1.cc
  td/bt/BtJust2.cc
  td/bt/BtJustBase.cc
  td/bt/BtSimple.cc

  td/dtpg/Dtpg.cc
  td/dtpg/DtpgImpl.cc
  td/dtpg/DtpgImplM.cc

  td/rtpg/FopRtpg.cc
  td/rtpg/RtpgP1.cc
  td/rtpg/RtpgP2.cc

  td/struct_sat/StructSat.cc
  td/struct_sat/ConeBase.cc
  td/struct_sat/FoCone.cc
  td/struct_sat/MffcCone.cc
  td/struct_sat/Extractor.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( satpg_common
  ${tpg_network_SOURCES}
  ${fsim_SOURCES}
  ${dop_SOURCES}
  ${uop_SOURCES}
  ${sa_SOURCES}
  ${td_SOURCES}
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
