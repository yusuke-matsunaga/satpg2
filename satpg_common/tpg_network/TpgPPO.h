#ifndef TPGPPO_H
#define TPGPPO_H

/// @file TpgPPO.h
/// @brief TpgPPO のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgPPO TpgPPO.h "TpgPPO.h"
/// @brief FF入力も含んだ擬似外部出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgPPO :
  public TpgNode
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] output_id 出力番号
  /// @param[in] fanin ファンインのノード
  TpgPPO(ymuint id,
	 ymuint output_id,
	 TpgNode* fanin);

  /// @brief デストラクタ
  ~TpgPPO();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  virtual
  bool
  is_ppo() const;

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

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ただし疑似出力の場合はバッファタイプとみなす．
  virtual
  GateType
  gate_type() const;

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号2をセットする．
  /// @param[in] id セットする番号
  ///
  /// 出力ノード以外では無効
  virtual
  void
  set_output_id2(ymuint id);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力番号
  ymuint mOutputId;

  // 出力番号2
  ymuint mOutputId2;

  // ファンイン
  TpgNode* mFanin;
};

END_NAMESPACE_YM_SATPG

#endif // TPGPPO_H
