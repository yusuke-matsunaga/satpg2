
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( tpg_network )
add_subdirectory( sa_fsim2 )
add_subdirectory( sa_fsim3 )
add_subdirectory( dtpg )


# ===================================================================
# テストターゲットの定義
# ===================================================================

ym_add_gtest(Val3Test
  Val3Test.cc
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

ym_add_gtest(GateTypeTest
  GateTypeTest.cc
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
