
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  DtpgTest
# ===================================================================

set ( DtpgTest_SOURCES
  DtpgTest.cc
  )

add_executable ( DtpgTest
  DtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_a>
  $<TARGET_OBJECTS:satpg_fsim2_a>
  $<TARGET_OBJECTS:satpg_fsim3_a>
  $<TARGET_OBJECTS:ym_common_a>
  $<TARGET_OBJECTS:ym_logic_a>
  $<TARGET_OBJECTS:ym_cell_a>
  $<TARGET_OBJECTS:ym_bnet_a>
  $<TARGET_OBJECTS:ym_sat_a>
  $<TARGET_OBJECTS:ym_mincov_a>
  $<TARGET_OBJECTS:ym_udgraph_a>
  )

target_compile_options ( DtpgTest
  PRIVATE "-O3"
  )

target_compile_definitions ( DtpgTest
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( DtpgTest
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( DtpgTest
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( DtpgTest_d
  DtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsim2_d>
  $<TARGET_OBJECTS:satpg_fsim3_d>
  $<TARGET_OBJECTS:ym_common_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( DtpgTest_d
  PRIVATE "-g"
  )

target_link_libraries ( DtpgTest_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( DtpgTest_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


# ===================================================================
#  TdDtpgTest
# ===================================================================

set ( TdDtpgTest_SOURCES
  TdDtpgTest.cc
  )

add_executable ( TdDtpgTest
  TdDtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_a>
  $<TARGET_OBJECTS:satpg_fsim2_a>
  $<TARGET_OBJECTS:satpg_fsim3_a>
  $<TARGET_OBJECTS:ym_common_a>
  $<TARGET_OBJECTS:ym_logic_a>
  $<TARGET_OBJECTS:ym_cell_a>
  $<TARGET_OBJECTS:ym_bnet_a>
  $<TARGET_OBJECTS:ym_sat_a>
  $<TARGET_OBJECTS:ym_mincov_a>
  $<TARGET_OBJECTS:ym_udgraph_a>
  )

target_compile_options ( TdDtpgTest
  PRIVATE "-O3"
  )

target_compile_definitions ( TdDtpgTest
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( TdDtpgTest
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TdDtpgTest
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( TdDtpgTest_d
  TdDtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsim2_d>
  $<TARGET_OBJECTS:satpg_fsim3_d>
  $<TARGET_OBJECTS:ym_common_d>
  $<TARGET_OBJECTS:ym_logic_d>
  $<TARGET_OBJECTS:ym_cell_d>
  $<TARGET_OBJECTS:ym_bnet_d>
  $<TARGET_OBJECTS:ym_sat_d>
  $<TARGET_OBJECTS:ym_mincov_d>
  $<TARGET_OBJECTS:ym_udgraph_d>
  )

target_compile_options ( TdDtpgTest_d
  PRIVATE "-g"
  )

target_link_libraries ( TdDtpgTest_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( TdDtpgTest_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()
