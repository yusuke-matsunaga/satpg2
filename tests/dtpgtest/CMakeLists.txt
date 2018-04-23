
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
  $<TARGET_OBJECTS:satpg_common_ad>
  $<TARGET_OBJECTS:satpg_fsimsa2_ad>
  $<TARGET_OBJECTS:satpg_fsimsa3_ad>
  $<TARGET_OBJECTS:satpg_fsimtd2_ad>
  $<TARGET_OBJECTS:satpg_fsimtd3_ad>
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_logic_ad>
  $<TARGET_OBJECTS:ym_cell_ad>
  $<TARGET_OBJECTS:ym_bnet_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  $<TARGET_OBJECTS:ym_mincov_ad>
  $<TARGET_OBJECTS:ym_udgraph_ad>
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
