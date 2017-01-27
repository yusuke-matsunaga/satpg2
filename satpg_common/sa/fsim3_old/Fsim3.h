﻿#ifndef FSIM3_H
#define FSIM3_H

/// @file Fsim3.h
/// @brief Fsim3 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "sa/Fsim.h"

#include "fsim3_nsdef.h"
#include "PackedVal.h"
#include "EventQ.h"
#include "SimFault.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

class SimFFR;
class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class Fsim3 Fsim3.h "Fsim3.h"
/// @brief 故障シミュレーションを行うモジュール
/// @sa ModBase
//////////////////////////////////////////////////////////////////////
class Fsim3 :
  public Fsim
{
public:

  /// @brief コンストラクタ
  Fsim3();

  /// @brief デストラクタ
  virtual
  ~Fsim3();


public:
  //////////////////////////////////////////////////////////////////////
  // Fsim の仮想関数
  /////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  /// @param[in] network ネットワーク
  virtual
  void
  set_network(const TpgNetwork& network);

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  /// @param[in] f 対象の故障
  virtual
  void
  set_skip(const TpgFault* f);

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  /// @param[in] f 対象の故障
  virtual
  void
  clear_skip(const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const TestVector* tv,
	const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const NodeValList& assign_list,
	const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sppfp(const TestVector* tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sppfp(const NodeValList& assign_list);

  /// @brief ppsfp 用のパタンバッファをクリアする．
  virtual
  void
  clear_patterns();

  /// @brief ppsfp 用のパタンを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  /// @param[in] tv テストベクタ
  virtual
  void
  set_pattern(ymuint pos,
	      const TestVector* tv);

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  virtual
  const TestVector*
  get_pattern(ymuint pos);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  ymuint
  ppsfp();

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  virtual
  ymuint
  det_fault_num();

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  const TpgFault*
  det_fault(ymuint pos);

  /// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  PackedVal
  det_fault_pat(ymuint pos);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションの本体
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(const TpgFault* f);

  /// @brief SPPFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  ymuint
  _sppfp();

  /// @brief FFR 内の故障シミュレーションを行う．
  /// @param[in] ffr 対象のFFR
  ///
  /// ffr 内の対象故障に対して故障が伝搬するかを調べる．
  /// 結果は各故障の mObsMask に設定される．
  /// また，すべての mObsMask の bitwise-OR を返す．
  ///
  /// 故障は SimFFR::fault_list() に格納されているが，
  /// スキップフラグが立った故障はリストから取り除かれる．
  PackedVal
  ffr_simulate(SimFFR* ffr);

  /// @brief 正常値の計算を行う．
  ///
  /// 値の変わったノードは mGvalClearArray に積まれる．
  void
  calc_gval();

  /// @brief 正常値が更新されたときの処理を行なう．
  void
  update_gval(SimNode* node);

  /// @brief 正常値をクリアする．
  ///
  /// mGvalClearArray を使う．
  void
  clear_gval();

  /// @brief 故障値の計算を行う．
  ///
  /// この関数を抜けた時点で故障値はクリアされている．
  PackedVal
  calc_fval();

  /// @brief 故障値が更新されたときの処理を行なう．
  void
  update_fval(SimNode* node);

  /// @brief ffr 内の故障が検出可能か調べる．
  /// @param[in] ffr 対象の FFR
  /// @param[out] fault_list 検出された故障のリスト
  ///
  /// ここでは各FFR の fault_list() は変化しない．
  void
  _fault_sweep(const SimFFR& ffr);


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode/SimFault 関係の設定関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在保持している SimNode のネットワークを破棄する．
  void
  clear();

  /// @brief node に対応する SimNode を得る．
  SimNode*
  find_simnode(const TpgNode* node) const;

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

  /// @brief logic ノードを作る．
  SimNode*
  make_node(GateType type,
	    const vector<SimNode*>& inputs);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // ppsfp の結果を格納する構造体
  struct FaultPat
  {
    // 故障
    const TpgFault* mFault;
    // 検出したビットパタン
    PackedVal mPat;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork* mNetwork;

  // TpgNode の id をキーにして SimNode を入れる配列
  vector<SimNode*> mSimMap;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // 外部入力に対応する SimNode を納めた配列
  // サイズは mNetwork->pseudo_input_num()
  vector<SimNode*> mInputArray;

  // 外部出力に対応する SimNode を納めた配列
  // サイズは mNetwork->pseudo_output_num()
  vector<SimNode*> mOutputArray;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // FFR を納めた配列
  vector<SimFFR> mFFRArray;

  // パタンの設定状況を表すビットベクタ
  PackedVal mPatMap;

  // パタンバッファ
  const TestVector* mPatBuff[kPvBitLen];

  // イベントキュー
  EventQ mEventQ;

  // 正常値を消去する必要のあるノードを入れておく配列
  vector<SimNode*> mGvalClearArray;

  // 故障値を消去する必要のあるノードを入れておく配列
  vector<SimNode*> mFvalClearArray;

  // 故障シミュレーション用の故障の配列
  vector<SimFault> mSimFaults;

  // TpgFault::id() をキーにして SimFault を格納する配列
  vector<SimFault*> mFaultArray;

  // 検出された故障を格納する配列
  // サイズは常に mSimFaults.size();
  vector<FaultPat> mDetFaultArray;

  // 検出された故障数
  ymuint mDetNum;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 正常値が更新されたときの処理を行なう．
inline
void
Fsim3::update_gval(SimNode* node)
{
  mGvalClearArray.push_back(node);
  ymuint no = node->nfo();
  for (ymuint i = 0; i < no; ++ i) {
    mEventQ.put(node->fanout(i));
  }
}

// @brief 故障値が更新されたときの処理を行なう．
void
Fsim3::update_fval(SimNode* node)
{
  mFvalClearArray.push_back(node);
  ymuint no = node->nfo();
  for (ymuint i = 0; i < no; ++ i) {
    mEventQ.put(node->fanout(i));
  }
}

END_NAMESPACE_YM_SATPG_FSIM

#endif // FSIM3_H