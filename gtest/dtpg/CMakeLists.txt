
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ../../tests/dtpgtest
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

add_executable ( DtpgTest
  DtpgTest.cc
  ../../tests/dtpgtest/DtpgTest.cc
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

target_compile_options ( DtpgTest
  PRIVATE "-g"
  )

target_compile_definitions ( DtpgTest
  PRIVATE "-DDATAPATH=\"${CMAKE_CURRENT_SOURCE_DIR}/data/\""
  )

target_link_libraries ( DtpgTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( DtpgTest
  DtpgTest
  )
