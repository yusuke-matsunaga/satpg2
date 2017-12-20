

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${TCL_INCLUDE_PATH}
  )


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (atpg_tclsh_SOURCES
  tclAppInit.cc
  atpg_init.cc
  AtpgCmd.cc
  AtpgMgr.cc
  AtpgMsgHandler.cc
  NtwkIoCmd.cc
  PrintFaultCmd.cc
  PrintPatCmd.cc
  PrintPatStatsCmd.cc
  PrintStatsCmd.cc
  TimeCmd.cc
  RtpgCmd.cc
  Rtpg1Cmd.cc
  DtpgCmd.cc
  #DtpgMinPowerCmd.cc
  RandWsaCmd.cc
  MinPatCmd.cc
  )


# ===================================================================
#  release ターゲットの設定
# ===================================================================

add_executable(atpg_tclsh
  ${atpg_tclsh_SOURCES}
  $<TARGET_OBJECTS:satpg_common>
  $<TARGET_OBJECTS:satpg_fsimsa2>
  $<TARGET_OBJECTS:satpg_fsimsa3>
  $<TARGET_OBJECTS:satpg_fsimtd2>
  $<TARGET_OBJECTS:satpg_fsimtd3>
  $<TARGET_OBJECTS:ym_base_a>
  $<TARGET_OBJECTS:ym_logic_a>
  $<TARGET_OBJECTS:ym_cell_a>
  $<TARGET_OBJECTS:ym_bnet_a>
  $<TARGET_OBJECTS:ym_sat_a>
  $<TARGET_OBJECTS:ym_mincov_a>
  $<TARGET_OBJECTS:ym_udgraph_a>
  $<TARGET_OBJECTS:ym_tclpp_a>
  )

target_compile_options (atpg_tclsh
  PRIVATE "-O3"
  )

target_compile_definitions (atpg_tclsh
  PRIVATE "-DNDEBUG"
  )

target_link_libraries(atpg_tclsh
  ${YM_LIB_DEPENDS}
  ${TCL_LIBRARY}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( atpg_tclsh
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  release with debug info ターゲットの設定
# ===================================================================

add_executable(atpg_tclsh_p
  ${atpg_tclsh_SOURCES}
  $<TARGET_OBJECTS:satpg_common_p>
  $<TARGET_OBJECTS:satpg_fsimsa2_p>
  $<TARGET_OBJECTS:satpg_fsimsa3_p>
  $<TARGET_OBJECTS:satpg_fsimtd2_p>
  $<TARGET_OBJECTS:satpg_fsimtd3_p>
  $<TARGET_OBJECTS:ym_base_p>
  $<TARGET_OBJECTS:ym_logic_p>
  $<TARGET_OBJECTS:ym_cell_p>
  $<TARGET_OBJECTS:ym_bnet_p>
  $<TARGET_OBJECTS:ym_sat_p>
  $<TARGET_OBJECTS:ym_mincov_p>
  $<TARGET_OBJECTS:ym_udgraph_p>
  $<TARGET_OBJECTS:ym_tclpp_p>
  )

target_compile_options (atpg_tclsh_p
  PRIVATE "-O2" "-g"
  )

target_compile_definitions (atpg_tclsh_p
  PRIVATE "-DNDEBUG"
  )

target_link_libraries(atpg_tclsh_p
  ${YM_LIB_DEPENDS}
  ${TCL_LIBRARY}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( atpg_tclsh_p
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  debug ターゲットの設定
# ===================================================================

add_executable(atpg_tclsh_d
  ${atpg_tclsh_SOURCES}
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsimsa2_d>
  $<TARGET_OBJECTS:satpg_fsimsa3_d>
  $<TARGET_OBJECTS:satpg_fsimtd2_d>
  $<TARGET_OBJECTS:satpg_fsimtd3_d>
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  $<TARGET_OBJECTS:ym_tclpp_d>
  )

target_compile_options (atpg_tclsh_d
  PRIVATE "-g"
  )

target_link_libraries(atpg_tclsh_d
  ${YM_LIB_DEPENDS}
  ${TCL_LIBRARY}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( atpg_tclsh_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install (
  TARGETS atpg_tclsh atpg_tclsh_p atpg_tclsh_d
  DESTINATION bin
  )
