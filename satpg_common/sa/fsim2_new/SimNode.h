﻿#ifndef FSIM_SIMNODE_H
#define FSIM_SIMNODE_H

/// @file SimNode.h
/// @brief SimNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "fsim2_nsdef.h"
#include "TpgNode.h"
#include "SimPrim.h"
#include "PackedVal.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimNode SimNode.h "SimNode.h"
/// @brief 故障シミュレーション用のノード
//////////////////////////////////////////////////////////////////////
class SimNode
{
  friend class EventQ;
public:

  /// @brief コンストラクタ
  SimNode(ymuint id,
	  SimPrim* gval,
	  SimPrim* fval);

  /// @brief デストラクタ
  virtual
  ~SimNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief pos 番目のファンアウトを得る．
  SimNode*
  fanout(ymuint pos) const;

  /// @brief 最初のファンアウト先の入力位置を得る．
  ymuint
  fanout_ipos() const;

  /// @brief FFR の根のノードの時 true を返す．
  bool
  is_ffr_root() const;

  /// @brief レベルを得る．
  ymuint
  level() const;

  /// @brief 出力ノードの時 true を返す．
  bool
  is_output() const;

  /// @brief 正常値計算用のプリミティブを得る．
  SimPrim*
  gval() const;

  /// @brief 故障値計算用のプリミティブを得る．
  SimPrim*
  fval() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の設定用関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力マークをつける．
  void
  set_output();

  /// @brief FFR の根の印をつける．
  void
  set_ffr_root();

  /// @brief レベルを設定する．
  void
  set_level(ymuint level);

  /// @brief ファンアウトリストを作成する．
  void
  set_fanout_list(const vector<SimNode*>& fo_list,
		  ymuint ipos);


public:
  //////////////////////////////////////////////////////////////////////
  // 2値の故障シミュレーションに関する情報の取得/設定
  //////////////////////////////////////////////////////////////////////
#if 0
  /// @brief 故障値のイベントをセットする．
  /// @param[in] mask 反転マスク
  void
  flip_fval(PackedVal mask);

  /// @brief 故障値を計算する．
  /// @param[in] mask マスク
  PackedVal
  calc_fval(PackedVal mask);
#endif

  /// @brief 故障値をクリアする．(2値版)
  void
  clear_fval();


private:
  //////////////////////////////////////////////////////////////////////
  // EventQ 用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに積まれていたら true を返す．
  bool
  in_queue() const;

  /// @brief キューフラグをセットする．
  void
  set_queue();

  /// @brief キューフラグをクリアする．
  void
  clear_queue();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint mId;

  // ファンアウトリストの要素数
  // その他以下の情報もパックして持つ．
  // - EventQ に入っているかどうかを示すマーク
  // - 最初のファンアウトの入力位置(FFR内のノードのみ意味を持つ)
  // - 出力のマーク
  // - lobs の計算マーク
  ymuint mFanoutNum;

  // ファンアウトリスト
  SimNode** mFanouts;

  // レベル
  ymuint mLevel;

  // イベントキューの次の要素
  SimNode* mLink;

  // 正常値
  SimPrim* mGvalPrim;

  // 故障値
  SimPrim* mFvalPrim;

};


//////////////////////////////////////////////////////////////////////
// SimNode のインライン関数
//////////////////////////////////////////////////////////////////////

// @brief ID番号を返す．
inline
ymuint
SimNode::id() const
{
  return mId;
}

// @brief ファンアウト数を得る．
inline
ymuint
SimNode::fanout_num() const
{
  return mFanoutNum >> 16;
}

// @brief pos 番目のファンアウトを得る．
inline
SimNode*
SimNode::fanout(ymuint pos) const
{
  return mFanouts[pos];
}

// @brief 最初のファンアウト先の入力位置を得る．
inline
ymuint
SimNode::fanout_ipos() const
{
  return (mFanoutNum >> 4) & 0x0FFFU;
}

// @brief FFR の根のノードの時 true を返す．
inline
bool
SimNode::is_ffr_root() const
{
  return static_cast<bool>((mFanoutNum >> 1) & 1U);
}

// @brief レベルを得る．
inline
ymuint
SimNode::level() const
{
  return mLevel;
}

// @brief 出力ノードの時 true を返す．
inline
bool
SimNode::is_output() const
{
  return (mFanoutNum & 1U) == 1U;
}

// @brief 出力マークをつける．
inline
void
SimNode::set_output()
{
  mFanoutNum |= 1U;
}

// @brief FFR の根の印をつける．
inline
void
SimNode::set_ffr_root()
{
  mFanoutNum |= 2U;
}

// @brief レベルを設定する．
inline
void
SimNode::set_level(ymuint level)
{
  mLevel = level;
}

// @brief 故障値をクリアする．
inline
void
SimNode::clear_fval()
{
  mFvalPrim->set_val(mGvalPrim->val());
}

// @brief キューに積まれていたら true を返す．
inline
bool
SimNode::in_queue() const
{
  return static_cast<bool>((mFanoutNum >> 3) & 1U);
}

// @brief キューフラグをセットする．
inline
void
SimNode::set_queue()
{
  mFanoutNum |= 1U << 3;
}

// @brief キューフラグをクリアする．
inline
void
SimNode::clear_queue()
{
  mFanoutNum &= ~(1U << 3);
}

// @brief 正常値計算用のプリミティブを得る．
inline
SimPrim*
SimNode::gval() const
{
  return mGvalPrim;
}

// @brief 故障値計算用のプリミティブを得る．
inline
SimPrim*
SimNode::fval() const
{
  return mFvalPrim;
}

END_NAMESPACE_YM_SATPG_FSIM

#endif // FSIM_SIMNODE_H