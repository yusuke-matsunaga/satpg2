#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2013, 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim.h"
#include "fsim_nsdef.h"
#include "PackedVal.h"
#include "EventQ.h"
#include "SimFault.h"
#include "TpgNode.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

class SimFFR;
class SimNode;
class InputVals;
class FaultProp;

//////////////////////////////////////////////////////////////////////
/// @class FSIM_CLASSNAME FsimX.h "FsimX.h"
/// @brief 故障シミュレーションを行うモジュール
///
/// 実際のクラス名は Fsim2 か Fsim3 である．
//////////////////////////////////////////////////////////////////////
class FSIM_CLASSNAME :
  public Fsim
{
  friend class SaFaultProp;
  friend class TdFaultProp;

public:

  /// @brief コンストラクタ
  /// @param[in] network ネットワーク
  FSIM_CLASSNAME (const TpgNetwork& network);

  /// @brief デストラクタ
  virtual
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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


public:
  //////////////////////////////////////////////////////////////////////
  // 縮退故障用の故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  sa_spsfp(const TestVector* tv,
	   const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  sa_spsfp(const NodeValList& assign_list,
	   const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sa_sppfp(const TestVector* tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sa_sppfp(const NodeValList& assign_list);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  ymuint
  sa_ppsfp();


public:
  //////////////////////////////////////////////////////////////////////
  // 遷移故障用の故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  td_spsfp(const TestVector* tv,
	   const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  td_spsfp(const NodeValList& assign_list,
	   const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  td_sppfp(const TestVector* tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  td_sppfp(const NodeValList& assign_list);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  ymuint
  td_ppsfp();

  /// @brief 与えられたパタンに対する信号遷移回数を計算する．
  /// @param[in] tv テストベクタ
  /// @param[in] weighted 重み付けをするかどうかのフラグ
  ///
  /// weightedの意味は以下の通り
  /// - false: ゲートの出力の遷移回数の和
  /// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
  virtual
  ymuint
  td_calc_wsa(const TestVector* tv,
	      bool weighted);


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

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


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

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


public:
  //////////////////////////////////////////////////////////////////////
  // 内部のデータ構造にアクセスする関数
  // InputVals が用いる．
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力数を返す．
  ymuint
  input_num() const;

  /// @brief PPI数を返す．
  ymuint
  ppi_num() const;

  /// @brief PPI のノードを返す．
  /// @param[in] id PPI番号 ( 0 <= id < ppi_num() )
  SimNode*
  ppi(ymuint id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  /// @param[in] network ネットワーク
  ///
  /// 全ての故障のスキップマークはクリアされる．
  void
  set_network(const TpgNetwork& network);

  /// @brief SPSFP故障シミュレーションの本体
  /// @param[in] f 対象の故障
  /// @param[in] fault_prop 故障伝搬を行うファンクタ
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _spsfp(const TpgFault* f,
	 FaultProp& fault_prop);

  /// @brief SPPFP故障シミュレーションの本体
  /// @param[in] fault_prop 故障伝搬を行うファンクタ
  /// @return 検出された故障数を返す．
  ymuint
  _sppfp(FaultProp& fault_prop);

  /// @brief PPSFP故障シミュレーションの本体
  /// @param[in] fault_prop 故障伝搬を行うファンクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  ymuint
  _ppsfp(FaultProp& fault_prop);

  /// @brief 正常値の計算を行う．(縮退故障用)
  /// @param[in] input_vals 入力値
  void
  _sa_calc_gval(const InputVals& input_vals);

  /// @brief 正常値の計算を行う．(遷移故障用)
  /// @param[in] input_vals 入力値
  void
  _td_calc_gval(const InputVals& input_vals);

  /// @brief 値の計算を行う．
  ///
  /// 入力ノードに値の設定は済んでいるものとする．
  void
  _calc_val();

  /// @brief ノードの出力の(重み付き)信号遷移回数を求める．
  ymuint
  _calc_wsa(SimNode* node,
	    bool weighted);

  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @param[in] root FFRの根のノード
  /// @param[in] obs_mask ビットマスク
  /// @return 伝搬したビットに1を立てたビットベクタ
  ///
  /// obs_mask が0のビットのイベントはマスクされる．
  PackedVal
  _prop_sim(SimNode* root,
	    PackedVal obs_mask);

  /// @brief FFR内の伝搬条件を求める．
  /// @param[in] fault 対象の故障
  PackedVal
  _ffr_prop(SimFault* fault);

  /// @brief FFR内の故障シミュレーションを行う．(縮退故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _sa_fault_prop(SimFault* fault);

  /// @brief FFR内の故障シミュレーションを行う．(遷移故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _td_fault_prop(SimFault* fault);

  /// @brief 個々の故障に FaultProp を適用する．
  /// @param[in] fault_list 故障のリスト
  /// @param[in] 故障伝搬を行うファンクタ
  /// @return 全ての故障の伝搬結果のORを返す．
  PackedVal
  _foreach_faults(const vector<SimFault*>& fault_list,
		  FaultProp& fault_prop);

  /// @brief 故障の活性化条件を求める．
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_cond(SimFault* fault);

  /// @brief 故障の活性化条件を求める．(遷移故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_prev_cond(SimFault* fault);

  /// @brief 故障をスキャンして結果をセットする(sppfp用)
  /// @param[in] fault_list 故障のリスト
  void
  _fault_sweep(const vector<SimFault*>& fault_list);

  /// @brief 故障をスキャンして結果をセットする(ppsfp用)
  /// @param[in] fault_list 故障のリスト
  /// @param[in] pat 検出パタン
  void
  _fault_sweep(const vector<SimFault*>& fault_list,
	       PackedVal pat);


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在保持している SimNode のネットワークを破棄する．
  /// 内部で clear_faults() を呼ぶ．
  void
  clear();

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

  /// @brief logic ノードを作る．
  SimNode*
  make_gate(GateType type,
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

  // 外部入力数
  ymuint mInputNum;

  // 外部出力数
  ymuint mOutputNum;

  // DFF数
  ymuint mDffNum;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  vector<SimNode*> mPPIArray;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  vector<SimNode*> mPPOArray;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // ブロードサイド方式用の１時刻前の値を保持する配列
  // サイズは mNodeArray.size()
  vector<FSIM_VALTYPE> mPrevValArray;

  // FFR を納めた配列
  vector<SimFFR> mFFRArray;

  // SimNode->id() をキーにして所属する FFR を納めた配列
  vector<SimFFR*> mFFRMap;

  // パタンの設定状況を表すビットベクタ
  PackedVal mPatMap;

  // mPatMap の最初の1のビット位置
  // 全て０の場合には kPvBitLen が入る．
  ymuint mPatFirstBit;

  // パタンバッファ
  const TestVector* mPatBuff[kPvBitLen];

  // イベントキュー
  EventQ mEventQ;

  // 故障シミュレーション用の故障の配列
  vector<SimFault> mSimFaults;

  // TpgFault::id() をキーとして SimFault を格納する配列
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

// @brief 外部入力数を返す．
inline
ymuint
FSIM_CLASSNAME::input_num() const
{
  return mInputNum;
}

// @brief PPI数を返す．
inline
ymuint
FSIM_CLASSNAME::ppi_num() const
{
  return mPPIArray.size();
}

// @brief PPI のノードを返す．
// @param[in] id PPI番号 ( 0 <= id < ppi_num() )
inline
SimNode*
FSIM_CLASSNAME::ppi(ymuint id) const
{
  ASSERT_COND( id < ppi_num() );

  return mPPIArray[id];
}

BEGIN_NONAMESPACE

// 故障の活性化条件を返す．
inline
PackedVal
_fault_diff(const TpgFault* f,
	    FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return ( f->val() == 1 ) ? ~val : val;
#elif FSIM_VAL3
  return ( f->val() == 1 ) ? val.val0() : val.val1();
#endif
}

// 遷移故障の初期化条件を返す．
inline
PackedVal
_fault_eq(const TpgFault* f,
	  FSIM_VALTYPE val)
{
#if FSIM_VAL2
  return ( f->val() == 1 ) ? val : ~val;
#elif FSIM_VAL3
  return ( f->val() == 1 ) ? val.val1() : val.val0();
#endif
}

END_NONAMESPACE

// @brief FFR内の故障シミュレーションを行う．(縮退故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_sa_fault_prop(SimFault* fault)
{
  // 故障の活性化条件を求める．
  PackedVal cval = _fault_cond(fault);

  // FFR 内の故障伝搬を行う．
  PackedVal lobs = _ffr_prop(fault);

  return cval & lobs;
}

// @brief FFR内の故障シミュレーションを行う．(遷移故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_td_fault_prop(SimFault* fault)
{
  // 故障の活性化条件を求める．
  PackedVal cval = _fault_cond(fault);

  // 1時刻前の条件を求める．
  PackedVal pval = _fault_prev_cond(fault);

  // FFR 内の故障伝搬を行う．
  PackedVal lobs = _ffr_prop(fault);

  return lobs & cval & pval;
}

// @brief FFR内の伝搬条件を求める．
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_ffr_prop(SimFault* fault)
{
  PackedVal lobs = kPvAll1;

  SimNode* f_node = fault->mNode;
  for (SimNode* node = f_node; !node->is_ffr_root(); ) {
    SimNode* onode = node->fanout_top();
    ymuint pos = node->fanout_ipos();
    lobs &= onode->_calc_gobs(pos);
    node = onode;
  }

  const TpgFault* f = fault->mOrigF;
  if ( f->is_branch_fault() ) {
    // 入力の故障
    ymuint ipos = fault->mIpos;
    lobs &= f_node->_calc_gobs(ipos);
  }

  return lobs;
}

// @brief 故障の活性化条件を求める．(縮退故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_fault_cond(SimFault* fault)
{
  // 故障の入力側のノードの値
  FSIM_VALTYPE ival = fault->mInode->val();

  // それが故障値と異なっていることが条件
  PackedVal valdiff = _fault_diff(fault->mOrigF, ival);

  return valdiff;
}

// @brief 故障の活性化条件を求める．(遷移故障用)
// @param[in] fault 対象の故障
inline
PackedVal
FSIM_CLASSNAME::_fault_prev_cond(SimFault* fault)
{
  // １時刻前の値が故障値と同じである必要がある．
  FSIM_VALTYPE pval = mPrevValArray[fault->mInode->id()];
  PackedVal valeq = _fault_eq(fault->mOrigF, pval);

  return valeq;
}

// @brief FFR の根から故障伝搬シミュレーションを行う．
// @param[in] root FFRの根のノード
// @param[in] obs_mask ビットマスク
// @return 伝搬したビットに1を立てたビットベクタ
//
// obs_mask が0のビットのイベントはマスクされる．
inline
PackedVal
FSIM_CLASSNAME::_prop_sim(SimNode* root,
			  PackedVal obs_mask)
{
  if ( root->is_output() ) {
    // 外部出力の場合は無条件で伝搬している．
    return kPvAll1;
  }

  // それ以外はイベントドリヴンシミュレーションを行う．
  mEventQ.put_trigger(root, obs_mask, true);
  PackedVal obs = mEventQ.simulate();

  return obs;
}

END_NAMESPACE_YM_SATPG_FSIM

#endif // FSIMX_H
