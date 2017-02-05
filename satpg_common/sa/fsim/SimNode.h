﻿#ifndef FSIM_SIMNODE_H
#define FSIM_SIMNODE_H

/// @file SimNode.h
/// @brief SimNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "TpgNode.h"
#include "PackedVal.h"
#include "PackedVal3.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimNode SimNode.h "SimNode.h"
/// @brief 故障シミュレーション用のノード
///
/// 出力値の計算はゲートの種類によって異なるので仮想関数にしている．<br>
/// 注意が必要なのがファンアウトの情報．最初のファンアウトだけ個別のポインタで
/// 持ち，２番目以降のファンアウトは配列で保持する．これは多くのノードが
/// 一つしかファンアウトを持たず，その場合に配列を使うとメモリ参照が余分に発生する
/// ため．
//////////////////////////////////////////////////////////////////////
class SimNode
{
  friend class EventQ;
protected:

  /// @brief コンストラクタ
  SimNode(ymuint id);


public:

  /// @brief デストラクタ
  virtual
  ~SimNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 生成用のクラスメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを生成するクラスメソッド
  static
  SimNode*
  new_input(ymuint id);

  /// @brief 論理ノードを生成するクラスメソッド
  static
  SimNode*
  new_gate(ymuint id,
	   GateType type,
	   const vector<SimNode*>& inputs);


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  ymuint
  id() const;

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const = 0;

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const = 0;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(ymuint pos) const = 0;

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief ファンアウトの先頭のノードを得る．
  SimNode*
  fanout_top() const;

  /// @brief 最初のファンアウト先の入力位置を得る．
  ymuint
  fanout_ipos() const;

  /// @brief pos 番目のファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  ///
  /// ただし fanout_num() == 1 の時は使えない．
  SimNode*
  fanout(ymuint pos) const;

  /// @brief FFR の根のノードの時 true を返す．
  bool
  is_ffr_root() const;

  /// @brief レベルを得る．
  ymuint
  level() const;

  /// @brief 出力ノードの時 true を返す．
  bool
  is_output() const;

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の設定用関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力マークをつける．
  void
  set_output();

  /// @brief ファンアウトリストを作成する．
  void
  set_fanout_list(const vector<SimNode*>& fo_list,
		  ymuint ipos);

  /// @brief FFR の根の印をつける．
  void
  set_ffr_root();


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションに関する情報の取得/設定
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値を得る．
  FSIM_VALTYPE
  val() const;

  /// @brief 出力値のセットを行う．
  /// @param[in] val 値
  void
  set_val(FSIM_VALTYPE val);

  /// @brief 出力値のセットを行う(マスク付き)．
  /// @param[in] val 値
  /// @param[in] mask マスク
  void
  set_val(FSIM_VALTYPE val,
	  PackedVal mask);

  /// @brief 出力値を計算する．
  void
  calc_val();

  /// @brief 出力値を計算する(マスク付き)．
  /// @param[in] mask マスク
  ///
  /// mask で1の立っているビットだけ更新する．
  void
  calc_val(PackedVal mask);


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  /// @return 計算結果を返す．
  virtual
  FSIM_VALTYPE
  _calc_val() = 0;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(ymuint ipos) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief レベルを設定する．
  void
  set_level(ymuint level);


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

  /// @brief 反転マスクを持っていたら true を返す．
  bool
  has_flip_mask() const;

  /// @brief 反転フラグをセットする．
  void
  set_flip();

  /// @brief 反転フラグをクリアする．
  void
  clear_flip();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint mId;

  // ファンアウトリストの要素数
  // その他以下の情報もパックして持つ．
  // - 0      : 出力のマーク
  // - 1      : FFRの根のマーク
  // - 2      : EventQ に入っているかどうかを示すマーク
  // - 3      : 反転フラグ
  // - 4 - 15 : 最初のファンアウトの入力位置(FFR内のノードのみ意味を持つ)
  // - 16 -   : ファンアウト数
  ymuint mFanoutNum;

  // ファンアウトの先頭のノード
  SimNode* mFanoutTop;

  // レベル
  ymuint mLevel;

  // イベントキューの次の要素
  SimNode* mLink;

  // 出力値
  FSIM_VALTYPE mVal;

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

// @brief ファンアウトの先頭のノードを得る．
inline
SimNode*
SimNode::fanout_top() const
{
  return mFanoutTop;
}

// @brief pos 番目のファンアウトを得る．
inline
SimNode*
SimNode::fanout(ymuint pos) const
{
  SimNode** fanouts = reinterpret_cast<SimNode**>(mFanoutTop);
  return fanouts[pos];
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
  return static_cast<bool>((mFanoutNum >> 0) & 1U);
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

// @brief 出力値を得る．
inline
FSIM_VALTYPE
SimNode::val() const
{
  return mVal;
}

// @brief 出力値のセットを行う．
// @param[in] val 値
inline
void
SimNode::set_val(FSIM_VALTYPE val)
{
  mVal = val;
}

// @brief 出力値のセットを行う(マスク付き)．
// @param[in] val 値
// @param[in] mask マスク
inline
void
SimNode::set_val(FSIM_VALTYPE val,
		 PackedVal mask)
{
#if FSIM_VAL2
  mVal &= ~mask;
  mVal |= (val & mask);
#elif FSIM_VAL3
  mVal.set_with_mask(val, mask);
#endif
}

// @brief 出力値を計算する．
inline
void
SimNode::calc_val()
{
  set_val(_calc_val());
}

// @brief 出力値を計算する(マスク付き)．
// @param[in] mask マスク
//
// mask で1の立っているビットだけ更新する．
inline
void
SimNode::calc_val(PackedVal mask)
{
  set_val(_calc_val(), mask);
}

// @brief キューに積まれていたら true を返す．
inline
bool
SimNode::in_queue() const
{
  return static_cast<bool>((mFanoutNum >> 2) & 1U);
}

// @brief キューフラグをセットする．
inline
void
SimNode::set_queue()
{
  mFanoutNum |= 1U << 2;
}

// @brief キューフラグをクリアする．
inline
void
SimNode::clear_queue()
{
  mFanoutNum &= ~(1U << 2);
}

// @brief 反転マスクを持っていたら true を返す．
inline
bool
SimNode::has_flip_mask() const
{
  return static_cast<bool>((mFanoutNum >> 3) & 1U);
}

// @brief 反転フラグをセットする．
inline
void
SimNode::set_flip()
{
  mFanoutNum |= 1U << 3;
}

// @brief 反転フラグをクリアする．
inline
void
SimNode::clear_flip()
{
  mFanoutNum &= ~(1U << 3);
}

END_NAMESPACE_YM_SATPG_FSIM

#endif // FSIM_SIMNODE_H