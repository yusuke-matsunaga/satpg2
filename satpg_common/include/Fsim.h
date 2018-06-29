#ifndef FSIM_H
#define FSIM_H

/// @file Fsim.h
/// @brief Fsim のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "FaultType.h"
#include "PackedVal.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class Fsim Fsim.h "Fsim.h"
/// @brief 故障シミュレーションを行うクラスの基底クラス
///
/// 具体的には故障シミュレーションに特化した回路構造を表すクラスと
/// 故障シミュレーションに特化した故障の情報を表すクラスを持つ．
/// シミュレーションの際に検出された故障を以降のシミュレーションで
/// スキップするかどうかは外からコントロールされるべきなので，
/// このシミュレーションのみ有効な'スキップフラグ'というフラグを
/// 各故障に持たせる．スキップフラグは set_skip(f) で付加され，
/// clear_skip(f) で解除される．
//////////////////////////////////////////////////////////////////////
class Fsim
{
public:

  /// @brief 2値版の故障シミュレータを生成するクラスメソッド
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の型
  static
  std::unique_ptr<Fsim>
  new_Fsim2(const TpgNetwork& network,
	    FaultType fault_type);

  /// @brief 3値版の故障シミュレータを生成するクラスメソッド
  /// @param[in] network ネットワーク
  /// @param[in] fault_type 故障の型
  static
  std::unique_ptr<Fsim>
  new_Fsim3(const TpgNetwork& network,
	    FaultType fault_type);

  virtual
  ~Fsim() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all() = 0;

  /// @brief 故障にスキップマークをつける．
  /// @param[in] f 対象の故障
  virtual
  void
  set_skip(const TpgFault* f) = 0;

  /// @brief 複数の故障にスキップマークをつける．
  /// @param[in] fault_list 故障のリスト
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(const vector<const TpgFault*>& fault_list);

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all() = 0;

  /// @brief 故障のスキップマークを消す．
  /// @param[in] f 対象の故障
  virtual
  void
  clear_skip(const TpgFault* f) = 0;

  /// @brief 複数の故障のスキップマークを消す．
  /// @param[in] fault_list 故障のリスト
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(const vector<const TpgFault*>& fault_list);


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const TestVector& tv,
	const TpgFault* f) = 0;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  spsfp(const NodeValList& assign_list,
	const TpgFault* f) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  int
  sppfp(const TestVector& tv) = 0;

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  int
  sppfp(const NodeValList& assign_list) = 0;

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  int
  ppsfp() = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 順序回路用のシミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  /// @param[in] tv テストベクタ
  ///
  /// - 外部入力以外は無視する．
  /// - 時刻1の割り当ても無視する
  /// weightedの意味は以下の通り
  /// - false: ゲートの出力の遷移回数の和
  /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
  virtual
  int
  calc_wsa(const TestVector& tv,
	   bool weighted) = 0;

  /// @brief 状態を設定する．
  /// @param[in] i_vect 外部入力のビットベクタ
  /// @param[in] f_vect FFの値のビットベクタ
  virtual
  void
  set_state(const InputVector& i_vect,
	    const DffVector& f_vect) = 0;

  /// @brief 状態を取得する．
  /// @param[in] i_vect 外部入力のビットベクタ
  /// @param[in] f_vect FFの値のビットベクタ
  virtual
  void
  get_state(InputVector& i_vect,
	    DffVector& f_vect) = 0;

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  /// @param[in] i_vect 外部入力のビットベクタ
  virtual
  int
  calc_wsa(const InputVector& i_vect,
	   bool weighted) = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  virtual
  void
  clear_patterns() = 0;

  /// @brief ppsfp 用のパタンを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  /// @param[in] tv テストベクタ
  virtual
  void
  set_pattern(int pos,
	      const TestVector& tv) = 0;

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  virtual
  const TestVector&
  get_pattern(int pos) = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  virtual
  int
  det_fault_num() = 0;

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  const TpgFault*
  det_fault(int pos) = 0;

  /// @brief 直前の sppfp/ppsfp で検出された故障のリストを返す．
  virtual
  Array<const TpgFault*>
  det_fault_list() = 0;

  /// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  PackedVal
  det_fault_pat(int pos) = 0;

  /// @brief 直前の ppsfp で検出された故障に対する検出パタンのリストを返す．
  virtual
  Array<PackedVal>
  det_fault_pat_list() = 0;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SATPG

#endif // FSIM_H
