
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "FsimX.h"

#include "TpgNetwork.h"
#include "TpgDff.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "GateType.h"

#include "TestVector.h"
#include "NodeValList.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "InputVals.h"

#include "ym/HashSet.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// Val3 を PackedVal/PackedVal3 に変換する．
inline
FSIM_VALTYPE
val3_to_packedval(Val3 val)
{
#if FSIM_VAL2
  // kValX は kVal0 とみなす．
  return (val == Val3::_1) ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  switch ( val ) {
  case Val3::_X: return PackedVal3(kPvAll0, kPvAll0);
  case Val3::_0: return PackedVal3(kPvAll1, kPvAll0);
  case Val3::_1: return PackedVal3(kPvAll0, kPvAll1);
  }
#endif
}

// PackedVal/PackedVal3 を Val3 に変換する．
// 最下位ビットだけで判断する．
inline
Val3
packedval_to_val3(FSIM_VALTYPE pval)
{
#if FSIM_VAL2
  return (pval & 1UL) ? Val3::_1 : Val3::_0;
#elif FSIM_VAL3
  if ( pval.val0() & 1UL) {
    return Val3::_0;
  }
  else if ( pval.val1() & 1UL ) {
    return Val3::_1;
  }
  else {
    return Val3::_X;
  }
#endif
}

END_NONAMESPACE

Fsim*
new_Fsim(const TpgNetwork& network)
{
  return new FSIM_CLASSNAME(network);
}


//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] network ネットワーク
FSIM_CLASSNAME::FSIM_CLASSNAME(const TpgNetwork& network)
{
  mPatMap = kPvAll0;
  set_network(network);
}

// @brief デストラクタ
FSIM_CLASSNAME::~FSIM_CLASSNAME()
{
  clear();
}

// @brief ネットワークをセットする関数
// @param[in] network ネットワーク
void
FSIM_CLASSNAME::set_network(const TpgNetwork& network)
{
  mInputNum = network.input_num();
  mOutputNum = network.output_num();
  mDffNum = network.dff_num();

  int nn = network.node_num();
  int ni = network.ppi_num();
  int no = network.ppo_num();

  ASSERT_COND( ni == mInputNum + mDffNum );
  ASSERT_COND( no == mOutputNum + mDffNum );

  // 対応付けを行うマップの初期化
  vector<SimNode*> simmap(nn);
  mPPIArray.resize(ni);
  mPPOArray.resize(no);

  mPrevValArray.resize(nn);

  int nf = 0;
  for (int i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    nf += tpgnode->fault_num();

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mPPIArray[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(GateType::Buff, vector<SimNode*>(1, inode));
      node->set_output();
      mPPOArray[tpgnode->output_id()] = node;
    }
    else if ( tpgnode->is_dff_clock() ||
	      tpgnode->is_dff_clear() ||
	      tpgnode->is_dff_preset() ) {
      // DFFの制御端子に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(GateType::Buff, vector<SimNode*>(1, inode));
      node->set_output();
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      int ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs(ni);
      for (int i = 0; i < ni; ++ i) {
	const TpgNode* itpgnode = tpgnode->fanin(i);
	SimNode* inode = simmap[itpgnode->id()];
	ASSERT_COND(inode );
	inputs[i] = inode;
      }

      // 出力の論理を表す SimNode を作る．
      GateType type = tpgnode->gate_type();
      node = make_gate(type, inputs);
    }
    // 対応表に登録しておく．
    simmap[tpgnode->id()] = node;
  }

  // 各ノードのファンアウトリストの設定
  int node_num = mNodeArray.size();
  {
    vector<vector<SimNode*> > fanout_lists(node_num);
    vector<int> ipos(node_num);
    for (vector<SimNode*>::iterator p = mNodeArray.begin();
	 p != mNodeArray.end(); ++ p) {
      SimNode* node = *p;
      int ni = node->fanin_num();
      for (int i = 0; i < ni; ++ i) {
	SimNode* inode = node->fanin(i);
	fanout_lists[inode->id()].push_back(node);
	ipos[inode->id()] = i;
      }
    }
    for (int i = 0; i < node_num; ++ i) {
      SimNode* node = mNodeArray[i];
      node->set_fanout_list(fanout_lists[i], ipos[i]);
    }
  }

  // FFR の設定
  int ffr_num = 0;
  for (int i = node_num; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }
  mFFRMap.resize(mNodeArray.size());
  mFFRArray.resize(ffr_num);
  ffr_num = 0;
  for (int i = node_num; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      SimFFR* ffr = &mFFRArray[ffr_num];
      node->set_ffr_root();
      mFFRMap[node->id()] = ffr;
      ffr->set_root(node);
      ++ ffr_num;
    }
    else {
      SimNode* fo_node = node->fanout_top();
      SimFFR* ffr = mFFRMap[fo_node->id()];
      mFFRMap[node->id()] = ffr;
    }
  }

  // 最大レベルを求め，イベントキューを初期化する．
  int max_level = 0;
  for (int i = 0; i < no; ++ i) {
    SimNode* inode = mPPOArray[i];
    if ( max_level < inode->level() ) {
      max_level = inode->level();
    }
  }
  mEventQ.init(max_level, mNodeArray.size());


  //////////////////////////////////////////////////////////////////////
  // 故障リストの設定
  //////////////////////////////////////////////////////////////////////

  // 同時に各 SimFFR 内の故障リストも再構築する．
  mSimFaults.resize(nf);
  mDetFaultArray.resize(nf);
  mFaultArray.resize(network.max_fault_id());
  int fid = 0;
  for (int i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    SimNode* simnode = simmap[tpgnode->id()];
    SimFFR* ffr = mFFRMap[simnode->id()];
    int nf1 = tpgnode->fault_num();
    for (int j = 0; j < nf1; ++ j) {
      const TpgFault* fault = tpgnode->fault(j);
      SimNode* isimnode = nullptr;
      int ipos = 0;
      if ( fault->is_branch_fault() ) {
	ipos = fault->tpg_pos();
	const TpgNode* inode = tpgnode->fanin(ipos);
	isimnode = simmap[inode->id()];
      }
      else {
	isimnode = simnode;
      }
      mSimFaults[fid].set(fault, simnode, ipos, isimnode);
      SimFault* ff = &mSimFaults[fid];
      mFaultArray[fault->id()] = ff;
      ff->mSkip = false;
      ffr->add_fault(ff);
      ++ fid;
    }
  }
}

// @brief 全ての故障にスキップマークをつける．
void
FSIM_CLASSNAME::set_skip_all()
{
  for (int i = 0; i < mSimFaults.size(); ++ i) {
    mSimFaults[i].mSkip = true;
  }
}

// @brief 故障にスキップマークをつける．
// @param[in] f 対象の故障
void
FSIM_CLASSNAME::set_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = true;
}

// @brief 全ての故障のスキップマークを消す．
void
FSIM_CLASSNAME::clear_skip_all()
{
  for (int i = 0; i < mSimFaults.size(); ++ i) {
    mSimFaults[i].mSkip = false;
  }
}

// @brief 故障のスキップマークを消す．
// @param[in] f 対象の故障
void
FSIM_CLASSNAME::clear_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = false;
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::spsfp(const TestVector* tv,
		      const TpgFault* f)
{
  TvInputVals iv(tv);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::spsfp(const NodeValList& assign_list,
		      const TpgFault* f)
{
  NvlInputVals iv(assign_list);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _spsfp(f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
FSIM_CLASSNAME::sppfp(const TestVector* tv)
{
  TvInputVals iv(tv);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
int
FSIM_CLASSNAME::sppfp(const NodeValList& assign_list)
{
  NvlInputVals iv(assign_list);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _sppfp();
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
int
FSIM_CLASSNAME::ppsfp()
{
  if ( mPatMap == kPvAll0 ) {
    // パタンが一つも設定されていない．
    mDetNum = 0;
    return 0;
  }

  Tv2InputVals iv(mPatMap, mPatBuff);

  // 正常値の計算を行う．
  _calc_gval(iv);

  // 故障伝搬を行う．
  return _ppsfp();
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
FSIM_CLASSNAME::clear_patterns()
{
  mPatMap = kPvAll0;
  mPatFirstBit = kPvBitLen;
}

// @brief ppsfp 用のパタンを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
// @param[in] tv テストベクタ
void
FSIM_CLASSNAME::set_pattern(int pos,
			    const TestVector* tv)
{
  ASSERT_COND( pos < kPvBitLen );

  mPatBuff[pos] = tv;
  mPatMap |= (1ULL << pos);

  if ( mPatFirstBit > pos ) {
    mPatFirstBit = pos;
  }
}

// @brief 設定した ppsfp 用のパタンを読み出す．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
const TestVector*
FSIM_CLASSNAME::get_pattern(int pos)
{
  ASSERT_COND( pos < kPvBitLen );

  if ( mPatMap & (1ULL << pos) ) {
    return mPatBuff[pos];
  }
  else {
    return nullptr;
  }
}

// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
int
FSIM_CLASSNAME::det_fault_num()
{
  return mDetNum;
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
const TpgFault*
FSIM_CLASSNAME::det_fault(int pos)
{
  ASSERT_COND( pos < det_fault_num() );

  return mDetFaultArray[pos].mFault;
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
PackedVal
FSIM_CLASSNAME::det_fault_pat(int pos)
{
  ASSERT_COND( pos < det_fault_num() );

  return mDetFaultArray[pos].mPat;
}

// @brief SPSFP故障シミュレーションの本体
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::_spsfp(const TpgFault* f)
{
  SimFault* ff = mFaultArray[f->id()];

  // FFR の根までの伝搬条件を求める．
  PackedVal obs = _fault_prop(ff);

  // obs が 0 ならその後のシミュレーションを行う必要はない．
  if ( obs == kPvAll0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  SimNode* root = ff->mNode->ffr_root();

  // root からの故障伝搬シミュレーションを行う．
  obs = _prop_sim(root, kPvAll1);

  return (obs != kPvAll0);
}

// @brief SPPFP故障シミュレーションの本体
// @return 検出された故障数を返す．
int
FSIM_CLASSNAME::_sppfp()
{
  mDetNum = 0;
  int bitpos = 0;
  const SimFFR* ffr_buff[kPvBitLen];
  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    const SimFFR& ffr = *p;
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = _foreach_faults(ffr.fault_list());

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    SimNode* root = ffr.root();
    if ( root->is_output() ) {
      // 常に観測可能
      _fault_sweep(ffr.fault_list());
      continue;
    }

    // キューに積んでおく
    PackedVal bitmask = 1ULL << bitpos;
    mEventQ.put_trigger(root, bitmask, false);
    ffr_buff[bitpos] = &ffr;

    ++ bitpos;
    if ( bitpos == kPvBitLen ) {
      PackedVal obs = mEventQ.simulate();
      for (int i = 0; i < bitpos; ++ i, obs >>= 1) {
	if ( obs & 1ULL ) {
	  _fault_sweep(ffr_buff[i]->fault_list());
	}
      }
      bitpos = 0;
    }
  }
  if ( bitpos > 0 ) {
    PackedVal obs = mEventQ.simulate();
    for (int i = 0; i < bitpos; ++ i, obs >>= 1) {
      if ( obs & 1ULL ) {
	_fault_sweep(ffr_buff[i]->fault_list());
      }
    }
  }

  return mDetNum;
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
int
FSIM_CLASSNAME::_ppsfp()
{
  // FFR ごとに処理を行う．
  mDetNum = 0;
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    const SimFFR& ffr = *p;
    const vector<SimFault*>& fault_list = ffr.fault_list();
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = _foreach_faults(fault_list) & mPatMap;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    PackedVal obs = _prop_sim(ffr.root(), ffr_req);

    _fault_sweep(fault_list, obs);
  }

  return mDetNum;
}

// @brief 状態を設定する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
FSIM_CLASSNAME::set_state(const InputVector& i_vect,
			  const DffVector& f_vect)
{
  int ni = mInputNum;
  for (int i = 0; i < ni; ++ i) {
    SimNode* simnode = mPPIArray[i];
    Val3 val3 = i_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
  }

  int nf = mDffNum;
  for (int i = 0; i < nf; ++ i) {
    SimNode* simnode = mPPIArray[i + ni];
    Val3 val3 = f_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 各信号線の値を計算する．
  _calc_val();

  // 1時刻シフトする．

  // mPrevValArray に値をコピーする．
  for (vector<SimNode*>::iterator p = mPPIArray.begin();
       p != mPPIArray.end(); ++ p) {
    SimNode* node = *p;
    mPrevValArray[node->id()] = node->val();
  }
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for (int i = 0; i < mDffNum; ++ i) {
    SimNode* onode = mPPOArray[i + mOutputNum];
    SimNode* inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }
}

// @brief 状態を取得する．
// @param[in] i_vect 外部入力のビットベクタ
// @param[in] f_vect FFの値のビットベクタ
void
FSIM_CLASSNAME::get_state(InputVector& i_vect,
			  DffVector& f_vect)
{
  int ni = mInputNum;
  for (int i = 0; i < ni; ++ i) {
    SimNode* simnode = mPPIArray[i];
    Val3 val = packedval_to_val3(simnode->val());
    i_vect.set_val(i, val);
  }

  int nf = mDffNum;
  for (int i = 0; i < nf; ++ i) {
    SimNode* simnode = mPPIArray[i + ni];
    Val3 val = packedval_to_val3(simnode->val());
    f_vect.set_val(i, val);
  }
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
// @param[in] i_vect 外部入力のビットベクタ
int
FSIM_CLASSNAME::calc_wsa(const InputVector& i_vect,
			 bool weighted)
{
  int ni = input_num();
  for (int i = 0; i < ni; ++ i) {
    SimNode* simnode = mPPIArray[i];
    Val3 val3 = i_vect.val(i);
    simnode->set_val(val3_to_packedval(val3));
  }

  // 各信号線の値を計算する．
  _calc_val();

  // 遷移回数を数える．
  int wsa = 0;
  for (vector<SimNode*>::iterator p = mPPIArray.begin();
       p != mPPIArray.end(); ++ p) {
    SimNode* node = *p;
    wsa += _calc_wsa(node, weighted);
  }
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    wsa += _calc_wsa(node, weighted);
  }

  // 1時刻シフトする．

  // mPrevValArray に値をコピーする．
  for (vector<SimNode*>::iterator p = mPPIArray.begin();
       p != mPPIArray.end(); ++ p) {
    SimNode* node = *p;
    mPrevValArray[node->id()] = node->val();
  }
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for (int i = 0; i < mDffNum; ++ i) {
    SimNode* onode = mPPOArray[i + mOutputNum];
    SimNode* inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }

  return wsa;
}

// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
// @param[in] tv テストベクタ
//
// - 外部入力以外は無視する．
// - 時刻1の割り当ても無視する
// weightedの意味は以下の通り
// - false: ゲートの出力の遷移回数の和
// - true : ゲートの出力の遷移回数に(ファンアウト数＋１)を掛けたものの和
int
FSIM_CLASSNAME::calc_wsa(const TestVector* tv,
			 bool weighted)
{
  set_state(tv->input_vector(), tv->dff_vector());
  return calc_wsa(tv->aux_input_vector(), weighted);
}

// @brief ノードの出力の(重み付き)信号遷移回数を求める．
int
FSIM_CLASSNAME::_calc_wsa(SimNode* node,
			  bool weighted)
{
  int wsa = 0;
  if ( mPrevValArray[node->id()] != node->val() ) {
    wsa = 1;
    if ( weighted ) {
      wsa += node->fanout_num();
    }
  }
  return wsa;
}

#if FSIM_SA
// @brief 正常値の計算を行う．(縮退故障用)
// @param[in] input_vals 入力値
void
FSIM_CLASSNAME::_calc_gval(const InputVals& input_vals)
{
  // 入力の設定を行う．
  input_vals.set_val(*this);

  // 正常値の計算を行う．
  _calc_val();
}
#endif

#if FSIM_TD
// @brief 正常値の計算を行う．(遷移故障用)
// @param[in] input_vals 入力値
void
FSIM_CLASSNAME::_calc_gval(const InputVals& input_vals)
{
  // 1時刻目の入力を設定する．
  input_vals.set_val1(*this);

  // 1時刻目の正常値の計算を行う．
  _calc_val();

  // 1時刻シフトする．

  // mPrevValArray に値をコピーする．
  for (vector<SimNode*>::iterator p = mPPIArray.begin();
       p != mPPIArray.end(); ++ p) {
    SimNode* node = *p;
    mPrevValArray[node->id()] = node->val();
  }
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    mPrevValArray[node->id()] = node->val();
  }

  // DFF の出力の値を入力にコピーする．
  for (int i = 0; i < mDffNum; ++ i) {
    SimNode* onode = mPPOArray[i + mOutputNum];
    SimNode* inode = mPPIArray[i + mInputNum];
    inode->set_val(onode->val());
  }

  // 2時刻目の入力を設定する．
  input_vals.set_val2(*this);

  // 2時刻目の正常値の計算を行う．
  _calc_val();
}
#endif

// @brief 値の計算を行う．
//
// 入力ノードに値の設定は済んでいるものとする．
void
FSIM_CLASSNAME::_calc_val()
{
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    node->calc_val();
  }
}

// @brief 個々の故障に FaultProp を適用する．
// @param[in] fault_list 故障のリスト
// @return 全ての故障の伝搬結果のORを返す．
PackedVal
FSIM_CLASSNAME::_foreach_faults(const vector<SimFault*>& fault_list)
{
  PackedVal ffr_req = kPvAll0;
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip ) {
      continue;
    }

    PackedVal obs = _fault_prop(ff);

    ff->mObsMask = obs;
    ffr_req |= obs;
  }

  return ffr_req;
}

// @brief 故障をスキャンして結果をセットする(sppfp用)
// @param[in] fault_list 故障のリスト
void
FSIM_CLASSNAME::_fault_sweep(const vector<SimFault*>& fault_list)
{
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip || ff->mObsMask == kPvAll0 ) {
      continue;
    }
    const TpgFault* f = ff->mOrigF;
    mDetFaultArray[mDetNum].mFault = f;
    mDetFaultArray[mDetNum].mPat = kPvAll1; // ダミー
    ++ mDetNum;
  }
}

// @brief 故障をスキャンして結果をセットする(ppsfp用)
// @param[in] fault_list 故障のリスト
// @param[in] pat 検出パタン
void
FSIM_CLASSNAME::_fault_sweep(const vector<SimFault*>& fault_list,
			     PackedVal mask)
{
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip ) {
      continue;
    }
    PackedVal pat = ff->mObsMask & mask;
    if ( pat != kPvAll0 ) {
      const TpgFault* f = ff->mOrigF;
      mDetFaultArray[mDetNum].mFault = f;
      mDetFaultArray[mDetNum].mPat = pat;
      ++ mDetNum;
    }
  }
}

// @brief 現在保持している SimNode のネットワークを破棄する．
void
FSIM_CLASSNAME::clear()
{
  // mNodeArray が全てのノードを持っている
  for (vector<SimNode*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    delete *p;
  }
  mNodeArray.clear();
  mPPIArray.clear();
  mPPOArray.clear();
  mLogicArray.clear();

  mPrevValArray.clear();

  mFFRArray.clear();
  mFFRMap.clear();

  mSimFaults.clear();
  mFaultArray.clear();
}

// @brief 外部入力ノードを作る．
SimNode*
FSIM_CLASSNAME::make_input()
{
  int id = mNodeArray.size();
  SimNode* node = SimNode::new_input(id);
  mNodeArray.push_back(node);
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
FSIM_CLASSNAME::make_gate(GateType type,
			  const vector<SimNode*>& inputs)
{
  int id = mNodeArray.size();
  SimNode* node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(node);
  mLogicArray.push_back(node);
  return node;
}

END_NAMESPACE_YM_SATPG_FSIM
