#ifndef JUSTBASE_H
#define JUSTBASE_H

/// @file JustBase.h
/// @brief JustBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Justifier.h"
#include "NodeValList.h"
#include "TpgNode.h"
#include "Val3.h"
#include "../dtpg/ValMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class JustBase JustBase.h "JustBase.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class JustBase :
  public Justifier
{
public:

  /// @brief コンストラクタ
  /// @param[in] td_mode 遷移故障モードの時 true にするフラグ
  /// @param[in] max_id ID番号の最大値
  /// @param[in] val_map ノードの値を保持するクラス
  JustBase(bool td_mode,
	   ymuint max_id,
	   const ValMap& val_map);

  /// @brief デストラクタ
  virtual
  ~JustBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる便利関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 遷移故障モードの時 true を返す．
  bool
  td_mode() const;

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  void
  set_justified(const TpgNode* node,
		int time);

  /// @brief justified マークを読む．
  /// @param[in] node 対象のノード
  /// @param[in] time タイムフレーム ( 0 or 1 )
  bool
  justified_mark(const TpgNode* node,
		 int time) const;

  /// @brief ノードの正常値を返す．
  /// @param[in] node ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  Val3
  gval(const TpgNode* node,
       int time) const;

  /// @brief 入力ノードの値を記録する．
  /// @param[in] node 対象の外部入力ノード
  /// @param[in] time 時刻 ( 0 or 1 )
  /// @param[out] assign_list 値の割当リスト
  void
  record_value(const TpgNode* node,
	       int time,
	       NodeValList& assign_list) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 遷移故障モード
  bool mTdMode;

  // ノード番号の最大値
  ymuint mMaxId;

  // ノードの値を保持するクラス
  const ValMap& mValMap;

  // 個々のノードのマークを表す配列
  vector<ymuint8> mMarkArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 遷移故障モードの時 true を返す．
inline
bool
JustBase::td_mode() const
{
  return mTdMode;
}

// @brief justified マークをつける．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
void
JustBase::set_justified(const TpgNode* node,
			int time)
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  mMarkArray[node->id()] |= (1U << time);
}

// @brief justified マークを読む．
// @param[in] node 対象のノード
// @param[in] time タイムフレーム ( 0 or 1 )
inline
bool
JustBase::justified_mark(const TpgNode* node,
			 int time) const
{
  // 念のため time の最下位ビットだけ使う．
  time &= 1;
  return static_cast<bool>((mMarkArray[node->id()] >> time) & 1U);
}

// @brief ノードの正常値を返す．
// @param[in] node ノード
// @param[in] time 時刻 ( 0 or 1 )
inline
Val3
JustBase::gval(const TpgNode* node,
	       int time) const
{
  return mValMap.gval(node, time);
}

// @brief 入力ノードの値を記録する．
// @param[in] node 対象の外部入力ノード
// @param[in] time 時刻 (0 or 1)
// @param[out] assign_list 値の割当リスト
inline
void
JustBase::record_value(const TpgNode* node,
		       int time,
		       NodeValList& assign_list) const
{
  Val3 v = gval(node, time);
  if ( v != kValX ) {
    bool bval = (v == kVal1);
    assign_list.add(node, time, bval);
  }
}

END_NAMESPACE_YM_SATPG

#endif // JUSTBASE_H
