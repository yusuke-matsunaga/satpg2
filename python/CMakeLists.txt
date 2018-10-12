

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${YM_SUBMODULE_CYTHON_LIST}
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( cython-src )
add_subdirectory ( debug )


# ===================================================================
#  cython 用のソースファイルの設定
# ===================================================================

set ( CYTHON_SOURCES
  cython-src/satpg_core.pyx
  )

set_source_files_properties (
  ${CYTHON_SOURCES}
  PROPERTIES CYTHON_IS_CXX TRUE
  )


# ===================================================================
#  module ターゲットの設定
# ===================================================================

list ( REMOVE_ITEM YM_SUBMODULE_LIST "$<TARGET_OBJECTS:ym_tclpp>" )
cython_add_module ( satpg_core
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:satpg_common>
  $<TARGET_OBJECTS:satpg_fsimsa2>
  $<TARGET_OBJECTS:satpg_fsimsa3>
  $<TARGET_OBJECTS:satpg_fsimtd2>
  $<TARGET_OBJECTS:satpg_fsimtd3>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_options ( satpg_core
  PRIVATE "-O3"
  )

target_compile_definitions ( satpg_core
  PRIVATE "-DNDEBUG"
  )

target_link_libraries ( satpg_core
  ${YM_LIB_DEPENDS}
  )

install (TARGETS
  satpg_core
  DESTINATION lib/python/site-packages
  )


cython_add_module ( satpg_core_d
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:satpg_common_d>
  $<TARGET_OBJECTS:satpg_fsimsa2_d>
  $<TARGET_OBJECTS:satpg_fsimsa3_d>
  $<TARGET_OBJECTS:satpg_fsimtd2_d>
  $<TARGET_OBJECTS:satpg_fsimtd3_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( satpg_core_d
  PRIVATE "-g"
  )

target_link_libraries ( satpg_core_d
  ${YM_LIB_DEPENDS}
  )

add_custom_command (
  TARGET satpg_core_d
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS -E copy $<TARGET_FILE:satpg_core_d> ${CMAKE_CURRENT_BINARY_DIR}/debug/satpg_core.so
  )


# ===================================================================
#  インストールターゲットの設定
# ===================================================================

install ( TARGETS satpg_core
  DESTINATION "${YM_PYTHON_LIBDIR}"
  )
