
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
#  dtpg_test
# ===================================================================

set ( dtpg_test_SOURCES
  dtpgtest.cc
  )

add_executable ( dtpg_test
  dtpg_test.cc
  DtpgTest.cc
  $<TARGET_OBJECTS:satpg_common_a>
  $<TARGET_OBJECTS:satpg_fsimsa2_a>
  $<TARGET_OBJECTS:satpg_fsimsa3_a>
  $<TARGET_OBJECTS:satpg_fsimtd2_a>
  $<TARGET_OBJECTS:satpg_fsimtd3_a>
  $<TARGET_OBJECTS:ym_base_a>
  $<TARGET_OBJECTS:ym_logic_a>
  $<TARGET_OBJECTS:ym_cell_a>
  $<TARGET_OBJECTS:ym_bnet_a>
  $<TARGET_OBJECTS:ym_sat_a>
  $<TARGET_OBJECTS:ym_mincov_a>
  $<TARGET_OBJECTS:ym_udgraph_a>
  )

target_compile_options ( dtpg_test
  PRIVATE "-O3"
  )

target_compile_definitions ( dtpg_test
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( dtpg_test
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dtpg_test
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()


add_executable ( dtpg_test_d
  dtpg_test.cc
  DtpgTest.cc
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
  )

target_compile_options ( dtpg_test_d
  PRIVATE "-g"
  )

target_link_libraries ( dtpg_test_d
  ${YM_LIB_DEPENDS}
  )

if ( GPERFTOOLS_FOUND )
  target_link_libraries ( dtpg_test_d
    ${GPERFTOOLS_LIBRARIES}
    )
endif ()
