
# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set (fsim_SOURCES
  EventQ.cc
  FsimX.cc
  InputVals.cc
  SimNode.cc
  SnAnd.cc
  SnBuff.cc
  SnGate.cc
  SnInput.cc
  SnOr.cc
  SnXor.cc
  )


# ===================================================================
# ターゲットの設定
# ===================================================================
ym_add_object_library ( satpg_fsim2
  ${fsim_SOURCES}
  )

target_compile_definitions ( satpg_fsim2
  PRIVATE "-DFSIM_VAL2"
  )

target_compile_definitions ( satpg_fsim2_a
  PRIVATE "-DFSIM_VAL2"
  )

target_compile_definitions ( satpg_fsim2_d
  PRIVATE "-DFSIM_VAL2"
  )

target_compile_definitions ( satpg_fsim2_p
  PRIVATE "-DFSIM_VAL2"
  )


ym_add_object_library ( satpg_fsim3
  ${fsim_SOURCES}
  )

target_compile_definitions ( satpg_fsim3
  PRIVATE "-DFSIM_VAL3"
  )

target_compile_definitions ( satpg_fsim3_a
  PRIVATE "-DFSIM_VAL3"
  )

target_compile_definitions ( satpg_fsim3_d
  PRIVATE "-DFSIM_VAL3"
  )

target_compile_definitions ( satpg_fsim3_p
  PRIVATE "-DFSIM_VAL3"
  )


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
