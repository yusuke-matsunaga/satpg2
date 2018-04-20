

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/ym-base/cython-src
  ${PROJECT_SOURCE_DIR}/ym-sat/cython-src
  ${PROJECT_SOURCE_DIR}/ym-cell/cython-src
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( cython-src )

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

cython_add_module ( satpg_core
  ${CYTHON_SOURCES}
  $<TARGET_OBJECTS:satpg_common>
  $<TARGET_OBJECTS:satpg_fsimsa2>
  $<TARGET_OBJECTS:satpg_fsimsa3>
  $<TARGET_OBJECTS:satpg_fsimtd2>
  $<TARGET_OBJECTS:satpg_fsimtd3>
  $<TARGET_OBJECTS:ym_base>
  $<TARGET_OBJECTS:ym_logic>
  $<TARGET_OBJECTS:ym_cell>
  $<TARGET_OBJECTS:ym_bnet>
  $<TARGET_OBJECTS:ym_udgraph>
  $<TARGET_OBJECTS:ym_mincov>
  $<TARGET_OBJECTS:ym_sat>
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
