﻿#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "LitMap.h"
#include "Val3.h"
#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SATPG

class TpgMap;

//////////////////////////////////////////////////////////////////////
/// @class TpgNode TpgNode.h "TpgNode.h"
/// @brief SATPG 用のノードを表すクラス
///
/// 基本的には一つの TpgNode 一つのゲートに対応しているが，
/// もとのゲートが組み込み型でない場合には複数の TpgNode を組み合わ
/// せてもとのゲートを表す．
/// そのため，場合によってはファンインの故障を表すための仮想的な
/// ノードを挿入する場合もある．
//////////////////////////////////////////////////////////////////////
class TpgNode
{
  friend class TpgNetwork;

public:

  //////////////////////////////////////////////////////////////////////
  /// @brief 論理ゲートの種類を表す列挙型
  //////////////////////////////////////////////////////////////////////
  enum GateType {
    /// @brief 定数0
    kGateConst0 = 0,

    /// @brief 定数1
    kGateConst1 = 1,

    /// @brief バッファ
    kGateBuff   = 2,

    /// @brief NOT
    kGateNot    = 3,

    /// @brief AND
    kGateAnd    = 4,

    /// @brief NAND
    kGateNand   = 5,

    /// @brief OR
    kGateOr     = 6,

    /// @brief NOR
    kGateNor    = 7,

    /// @brief XOR
    kGateXor    = 8,

    /// @brief XNOR
    kGateXnor   = 9,

    /// @brief 上記以外
    kGateCplx   = 10
  };


public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgNode(ymuint id);

  /// @brief デストラクタ
  virtual
  ~TpgNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  ymuint
  id() const;

  /// @brief 名前を得る．
  ///
  /// nullptr が返される場合もある．
  const char*
  name() const;

  /// @brief 外部入力タイプの時 true を返す．
  /// @note FF 出力もここに含まれる．
  virtual
  bool
  is_input() const;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::input(node->input_id()
  /// の関係を満たす．
  /// is_input() が false の場合の返り値は不定
  virtual
  ymuint
  input_id() const;

  /// @brief 外部出力タイプの時 true を返す．
  /// @note FF 入力もここに含まれる．
  virtual
  bool
  is_output() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::output(node->output_id())
  /// の関係を満たす．
  /// is_output() が false の場合の返り値は不定
  virtual
  ymuint
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  virtual
  ymuint
  output_id2() const;

  /// @brief logic タイプの時 true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  virtual
  GateType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  noval() const;

  /// @brief 根のノードの時 true を返す．
  ///
  /// is_logic() が true の時のみ意味を持つ．
  bool
  is_root() const;

  /// @brief 内部ノードの時 true を返す．
  ///
  /// is_logic() が true の時のみ意味を持つ．
  bool
  is_internal() const;

  /// @brief もとのゲートのファンインに対応するノードを返す．
  /// @param[in] pos もとの BnNode の入力の位置番号 (!!!)
  ///
  /// is_root() が true の時のみ意味を持つ．
  const TpgNode*
  input_map(ymuint pos) const;

  /// @brief BnNode のファンインに対応するノードのファンイン番号を返す．
  /// @param[in] pos もとの BnNode の入力の位置番号 (!!!)
  ///
  /// is_root() が true の時のみ意味を持つ．
  ymuint
  ipos_map(ymuint pos) const;

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(ymuint pos) const;

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief ファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  TpgNode*
  fanout(ymuint pos) const;

  /// @brief アクティブなファンアウト数を得る．
  ymuint
  active_fanout_num() const;

  /// @brief アクティブなファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < active_fanout_num() )
  TpgNode*
  active_fanout(ymuint pos) const;

  /// @brief アクティブの場合 true を返す．
  bool
  is_active() const;

  /// @brief pos 番目のPOのTFIに含まれている時 true を返す．
  bool
  is_in_TFI_of(ymuint pos) const;

  /// @brief 直近の dominator を得る．
  TpgNode*
  imm_dom() const;


public:
  //////////////////////////////////////////////////////////////////////
  // CNF の生成に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入出力の関係を表す CNF 式を生成する．
  /// @param[in] solver SAT ソルバ
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  virtual
  void
  make_cnf(SatSolver& solver,
	   const LitMap& lit_map);


public:
  //////////////////////////////////////////////////////////////////////
  // 故障に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障を得る．
  /// @param[in] val 故障値 ( 0 / 1 )
  virtual
  const TpgFault*
  output_fault(int val) const;

  /// @brief 入力の故障を得る．
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] pos 入力の位置番号
  virtual
  const TpgFault*
  input_fault(int val,
	      ymuint pos) const;

  /// @brief このノードに関係する故障数を返す．
  virtual
  ymuint
  fault_num() const;

  /// @brief このノードに関係する故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  virtual
  const TpgFault*
  fault(ymuint pos) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief アクティブにする．
  void
  set_active();

  /// @brief アクティブフラグを消す．
  void
  clear_active();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint32 mId;

  // 名前
  const char* mName;

  // もとのゲートの入力の対応関係
  // nullptr の時は内部ノード
  const TpgMap* mMap;

  // ファンアウト数
  ymuint32 mFanoutNum;

  // ファンアウトの配列
  TpgNode** mFanouts;

  // アクティブなファンアウト数
  ymuint32 mActFanoutNum;

  // アクティブなファンアウトの配列
  TpgNode** mActFanouts;

  // いくつかのマークを納めるビットベクタ
  ymuint32 mMarks;

  // TFIマークを表すビットアレイ
  ymuint64* mTFIbits;

  // immediate dominator
  TpgNode* mImmDom;

};

/// @relates TpgNode
/// @brief ノード名を出力する
/// @param[in] s 出力先のストリーム
/// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNode* node);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ファンアウト数を得る．
inline
ymuint
TpgNode::fanout_num() const
{
  if ( is_output() ) {
    return 0;
  }
  return mFanoutNum;
}

// @brief ファンアウトを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
inline
TpgNode*
TpgNode::fanout(ymuint pos) const
{
  ASSERT_COND( pos < fanout_num() );
  return mFanouts[pos];
}

// @brief アクティブなファンアウト数を得る．
inline
ymuint
TpgNode::active_fanout_num() const
{
  return mActFanoutNum;
}

// @brief アクティブなファンアウトを得る．
// @param[in] pos 位置番号 ( 0 <= pos < active_fanout_num() )
inline
TpgNode*
TpgNode::active_fanout(ymuint pos) const
{
  ASSERT_COND( pos < mActFanoutNum );
  return mActFanouts[pos];
}

// @brief アクティブの場合 true を返す．
inline
bool
TpgNode::is_active() const
{
  return static_cast<bool>(mMarks & 1U);
}

// @brief アクティブにする．
inline
void
TpgNode::set_active()
{
  mMarks |= 1U;
}

// @brief アクティブフラグを消す．
inline
void
TpgNode::clear_active()
{
  mMarks &= ~1U;
}

// @brief pos 番目のPOのTFIに含まれている時 true を返す．
inline
bool
TpgNode::is_in_TFI_of(ymuint pos) const
{
  ymuint blk = pos / 64;
  ymuint sft = pos % 64;
  return static_cast<bool>((mTFIbits[blk] >> sft) & 1U);
}

// @brief 直近の dominator を得る．
inline
TpgNode*
TpgNode::imm_dom() const
{
  return mImmDom;
}

END_NAMESPACE_YM_SATPG

#endif // TPGNODE_H
