
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
