#ifndef TVMGR_H
#define TVMGR_H

/// @file TvMgr.h
/// @brief TvMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/UnitAlloc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TvMgr TvMgr.h "TvMgr.h"
/// @brief テストベクタを管理するクラス
//////////////////////////////////////////////////////////////////////
class TvMgr
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  TvMgr(const TpgNetwork& network);

  /// @brief デストラクタ
  ///
  /// このオブジェクトが確保したすべてのテストベクタを開放する．
  ~TvMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を空にする．
  ///
  /// このオブジェクトが生成した全てのテストベクタが開放される．
  void
  clear();

  /// @brief 入力数を返す．
  ymuint
  input_num() const;

  /// @brief DFF数を返す．
  ymuint
  dff_num() const;

  /// @brief 新しい入力用ベクタを生成する．
  /// @return 生成されたベクタを返す．
  ///
  /// パタンは0で初期化される．
  InputVector*
  new_input_vector();

  /// @brief 入力用ベクタを削除する．
  /// @param[in] vect 削除するベクタ
  void
  delete_vector(InputVector* vect);

  /// @brief 新しいFF用ベクタを生成する．
  /// @return 生成されたベクタを返す．
  ///
  /// パタンは0で初期化される．
  FFVector*
  new_ff_vector();

  /// @brief FF用ベクタを削除する．
  /// @param[in] vect 削除するベクタ
  void
  delete_vector(FFVector* vect);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ベクタ長からバイトサイズを計算する．
  /// @param[in] vectlen ベクタ長
  ///
  /// BitVector::block_num() にアクセスするためにクラスメソッドにしている．
  static
  ymuint
  calc_size(ymuint vectlen);


private:
  //////////////////////////////////////////////////////////////////////
  // 使用禁止にするためのプライベート宣言
  //////////////////////////////////////////////////////////////////////

  /// @brief コピーコンストラクタ
  ///
  /// 使用禁止なので実装しない．
  TvMgr(const TvMgr& src);

  /// @brief 代入演算子
  ///
  /// 使用禁止なので実装しない．
  const TvMgr&
  operator=(const TvMgr& src);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象回路の入力数
  ymuint mInputNum;

  // 対象回路の DFF 数
  ymuint mDffNum;

  // InputVector の実際のサイズ
  ymuint mIvSize;

  // 入力ベクタのメモリ確保用のアロケータ
  UnitAlloc mInputVectorAlloc;

  // FFVector の実際のサイズ
  ymuint mFvSize;

  // FFベクタのメモリ確保用のアロケータ
  UnitAlloc mFFVectorAlloc;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 入力数を返す．
inline
ymuint
TvMgr::input_num() const
{
  return mInputNum;
}

// @brief DFF数を返す．
inline
ymuint
TvMgr::dff_num() const
{
  return mDffNum;
}

END_NAMESPACE_YM_SATPG

#endif // TVMGR_H
