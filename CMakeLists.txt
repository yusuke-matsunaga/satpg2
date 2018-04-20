# ===================================================================
# CMAKE のおまじない
# ===================================================================
cmake_minimum_required (VERSION 3.0)


# C++11 の指定
set (CMAKE_CXX_STANDARD 11)


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================
project (satpg)
set (SATPG_VERSION_MAJOR 1)
set (SATPG_VERSION_MINOR 0)
set (SATPG_VERSION ${SATPG_VERSION_MAJOR}.${SATPG_VERSION_MINOR})

# 名前空間
set (SATPG_NAMESPACE "nsSatpg")

# ライブラリのインストールディレクトリ
set ( YM_INSTALL_LIBDIR "lib/satpg" )


# ===================================================================
# オプション
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================
list ( APPEND CMAKE_MODULE_PATH
  "${PROJECT_SOURCE_DIR}/ym-common/cmake"
  )

include (FindDoxygen)

# Python のバージョン指定
set( Python_ADDITIONAL_VERSIONS 3.6 3.5 3.4 3.3 3.2 3.1 )
include( FindPythonInterp )
include( FindPythonLibs )

include( UseCython )

find_package(Gperftools)

include (YmUtils)

ym_init ( "" )


# ===================================================================
# google-test は内蔵のものを使う．
# ===================================================================
ym_use_gtest ()


# ===================================================================
# CTest モジュールの読み込み
# ===================================================================
include (CTest)
enable_testing()


# ===================================================================
# ヘッダファイルの生成
# ===================================================================

configure_file(
  "${PROJECT_SOURCE_DIR}/etc/version_config.h.in"
  "${PROJECT_BINARY_DIR}/version_config.h"
  )


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  ${PROJECT_SOURCE_DIR}/satpg_common/include
  ${PROJECT_SOURCE_DIR}/ym-base/include
  ${PROJECT_SOURCE_DIR}/ym-bnet/include
  ${PROJECT_SOURCE_DIR}/ym-cell/include
  ${PROJECT_SOURCE_DIR}/ym-common/include
  ${PROJECT_SOURCE_DIR}/ym-logic/include
  ${PROJECT_SOURCE_DIR}/ym-mincov/include
  ${PROJECT_SOURCE_DIR}/ym-sat/include
  ${PROJECT_SOURCE_DIR}/ym-tclpp/include
  ${PROJECT_SOURCE_DIR}/ym-udgraph/include
  ${PROJECT_BINARY_DIR}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( ym-common )
add_subdirectory ( ym-base )
add_subdirectory ( ym-logic )
add_subdirectory ( ym-cell )
add_subdirectory ( ym-bnet )
add_subdirectory ( ym-udgraph )
add_subdirectory ( ym-mincov )
add_subdirectory ( ym-sat )
add_subdirectory ( ym-tclpp )

add_subdirectory ( tests/fsimtest )
add_subdirectory ( tests/dtpgtest )
add_subdirectory ( tests/tpgnetworktest )
#add_subdirectory ( tests )
add_subdirectory ( gtest )
add_subdirectory ( satpg_common )
add_subdirectory ( atpg_tclsh )
add_subdirectory ( python )

if ( DOXYGEN_FOUND )
  add_subdirectory ( doxygen-src )
endif ()


# ===================================================================
#  テストの設定
# ===================================================================


# ===================================================================
#  インストールターゲットの設定
# ===================================================================
