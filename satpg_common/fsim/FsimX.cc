
/// @file FsimX.cc
/// @brief FsimX の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "FsimX.h"

#include "TpgNetwork.h"
#include "TpgDff.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "TestVector.h"
#include "NodeValList.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "ym/HashSet.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

#if FSIM_VAL2
Fsim*
new_Fsim2()
{
  return new nsFsim2::Fsim2();
}
#elif FSIM_VAL3
Fsim*
new_Fsim3()
{
  return new nsFsim3::Fsim3();
}
#endif

END_NAMESPACE_YM_SATPG


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// 0/1 を PackedVal2/3 に変換する．
inline
FSIM_VALTYPE
int2pval(int val)
{
#if FSIM_VAL2
  return val ? kPvAll1 : kPvAll0;
#elif FSIM_VAL3
  return val ? PackedVal3(kPvAll1) : PackedVal3(kPvAll0);
#endif
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// FsimX
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
FSIM_CLASSNAME::FSIM_CLASSNAME()
{
  mPatMap = kPvAll0;
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
  clear();

  ymuint nn = network.node_num();
  ymuint ni = network.ppi_num();
  ymuint no = network.ppo_num();

  // 対応付けを行うマップの初期化
  vector<SimNode*> simmap(nn);
  mInputArray.resize(ni);
  mOutputArray.resize(no);

  ymuint nf = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    nf += tpgnode->fault_num();

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mInputArray[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();
      mOutputArray[tpgnode->output_id()] = node;
    }
    else if ( tpgnode->is_dff_clock() ||
	      tpgnode->is_dff_clear() ||
	      tpgnode->is_dff_preset() ) {
      // DFFの制御端子に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      ymuint ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs(ni);
      for (ymuint i = 0; i < ni; ++ i) {
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
  ymuint node_num = mNodeArray.size();
  {
    vector<vector<SimNode*> > fanout_lists(node_num);
    vector<ymuint> ipos(node_num);
    for (vector<SimNode*>::iterator p = mNodeArray.begin();
	 p != mNodeArray.end(); ++ p) {
      SimNode* node = *p;
      ymuint ni = node->fanin_num();
      for (ymuint i = 0; i < ni; ++ i) {
	SimNode* inode = node->fanin(i);
	fanout_lists[inode->id()].push_back(node);
	ipos[inode->id()] = i;
      }
    }
    for (ymuint i = 0; i < node_num; ++ i) {
      SimNode* node = mNodeArray[i];
      node->set_fanout_list(fanout_lists[i], ipos[i]);
    }
  }

  // FFR の設定
  ymuint ffr_num = 0;
  for (ymuint i = node_num; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }
  mFFRMap.resize(mNodeArray.size());
  mFFRArray.resize(ffr_num);
  ffr_num = 0;
  for (ymuint i = node_num; i > 0; ) {
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
  ymuint max_level = 0;
  for (ymuint i = 0; i < no; ++ i) {
    SimNode* inode = mOutputArray[i];
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
  ymuint fid = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    SimNode* simnode = simmap[tpgnode->id()];
    SimFFR* ffr = mFFRMap[simnode->id()];
    ymuint nf1 = tpgnode->fault_num();
    for (ymuint j = 0; j < nf1; ++ j) {
      const TpgFault* fault = tpgnode->fault(j);
      SimNode* isimnode = nullptr;
      ymuint ipos = 0;
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
  for (ymuint i = 0; i < mSimFaults.size(); ++ i) {
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
  for (ymuint i = 0; i < mSimFaults.size(); ++ i) {
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
FSIM_CLASSNAME::sa_spsfp(const TestVector* tv,
			 const TpgFault* f)
{
  _sa_set_sp(tv);
  return _sa_spsfp(f);
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::sa_spsfp(const NodeValList& assign_list,
			 const TpgFault* f)
{
  _sa_set_sp(assign_list);
  return _sa_spsfp(f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
FSIM_CLASSNAME::sa_sppfp(const TestVector* tv)
{
  _sa_set_sp(tv);
  return _sa_sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
FSIM_CLASSNAME::sa_sppfp(const NodeValList& assign_list)
{
  _sa_set_sp(assign_list);
  return _sa_sppfp();
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
ymuint
FSIM_CLASSNAME::sa_ppsfp()
{
  mDetNum = 0;

  if ( mPatMap == kPvAll0 ) {
    // パタンが一つも設定されていない．
    return 0;
  }

  _sa_set_pp();

  // 正常値の計算を行う．
  _calc_gval();

  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    const SimFFR& ffr = *p;
    const vector<SimFault*>& fault_list = ffr.fault_list();
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = _fault_prop(fault_list) & mPatMap;

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    SimNode* root = ffr.root();
    PackedVal obs = kPvAll0;
    if ( root->is_output() ) {
      obs = kPvAll1;
    }
    else {
      mEventQ.put_trigger(root, ffr_req, true);
      obs = mEventQ.simulate();
    }

    _fault_sweep(fault_list, obs);
  }

  return mDetNum;
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
FSIM_CLASSNAME::clear_patterns()
{
  mPatMap = kPvAll0;
}

// @brief ppsfp 用のパタンを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
// @param[in] tv テストベクタ
void
FSIM_CLASSNAME::set_pattern(ymuint pos,
			    const TestVector* tv)
{
  ASSERT_COND( pos < kPvBitLen );
  mPatBuff[pos] = tv;
  mPatMap |= (1ULL << pos);
}

// @brief 設定した ppsfp 用のパタンを読み出す．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
const TestVector*
FSIM_CLASSNAME::get_pattern(ymuint pos)
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
ymuint
FSIM_CLASSNAME::det_fault_num()
{
  return mDetNum;
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
const TpgFault*
FSIM_CLASSNAME::det_fault(ymuint pos)
{
  ASSERT_COND( pos < det_fault_num() );
  return mDetFaultArray[pos].mFault;
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
PackedVal
FSIM_CLASSNAME::det_fault_pat(ymuint pos)
{
  ASSERT_COND( pos < det_fault_num() );
  return mDetFaultArray[pos].mPat;
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] tv 設定するテストベクタ
void
FSIM_CLASSNAME::_sa_set_sp(const TestVector* tv)
{
  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    Val3 val3 = tv->ppi_val(i);
    SimNode* simnode = mInputArray[i];
#if FSIM_VAL2
    PackedVal val = (val3 == kVal1) ? kPvAll1 : kPvAll0;
    simnode->set_val(val);
#elif FSIM_VAL3
    PackedVal val0;
    PackedVal val1;
    switch ( val3 ) {
    case kValX:
      val0 = kPvAll0;
      val1 = kPvAll0;
      break;

    case kVal0:
      val0 = kPvAll1;
      val1 = kPvAll0;
      break;

    case kVal1:
      val0 = kPvAll0;
      val1 = kPvAll1;
      break;
    }
    simnode->set_val(PackedVal3(val0, val1));
#endif
  }
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] assign_list 設定する値の割り当てリスト
void
FSIM_CLASSNAME::_sa_set_sp(const NodeValList& assign_list)
{
#if FSIM_VAL2
  // デフォルトで 0 にする．
  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    simnode->set_val(kPvAll0);
  }
#elif FSIM_VAL3
  // デフォルトで X にする．
  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    simnode->set_val(PackedVal3(kPvAll0, kPvAll0));
  }
#endif

  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    FSIM_VALTYPE pval = int2pval(nv.val());
    SimNode* simnode = mInputArray[nv.node()->input_id()];
    simnode->set_val(pval);
  }
}

// @brief 複数のパタンを設定する．
void
FSIM_CLASSNAME::_sa_set_pp()
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  // 設定されている最初のビット位置を求める．
  ymuint first = 0;
  for ( ; first < kPvBitLen; ++ first) {
    if ( mPatMap & (1ULL << first) ) {
      break;
    }
  }
  ASSERT_COND( first < kPvBitLen );

  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
#if FSIM_VAL2
    PackedVal val = kPvAll0;
    PackedVal bit = 1ULL;
    for (ymuint j = 0; j < kPvBitLen; ++ j, bit <<= 1) {
      ymuint pos = (mPatMap & bit) ? j : first;
      if ( mPatBuff[pos]->ppi_val(i) == kVal1 ) {
	val |= bit;
      }
    }
    SimNode* simnode = mInputArray[i];
    simnode->set_val(val);
#elif FSIM_VAL3
    PackedVal val0 = kPvAll0;
    PackedVal val1 = kPvAll0;
    PackedVal bit = 1ULL;
    for (ymuint j = 0; j < kPvBitLen; ++ j, bit <<= 1) {
      ymuint pos = (mPatMap & bit) ? j : first;
      Val3 val = mPatBuff[pos]->ppi_val(i);
      if ( val == kVal1 ) {
	val1 |= bit;
      }
      else if ( val == kVal0 ) {
	val0 |= bit;
      }
    }
    SimNode* simnode = mInputArray[i];
    simnode->set_val(PackedVal3(val0, val1));
#endif
  }
}

// @brief SPPFP故障シミュレーションの本体
// @return 検出された故障数を返す．
ymuint
FSIM_CLASSNAME::_sa_sppfp()
{
  mDetNum = 0;

  // 正常値の計算を行う．
  _calc_gval();

  ymuint bitpos = 0;
  const SimFFR* ffr_buff[kPvBitLen];
  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    const SimFFR& ffr = *p;
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = _fault_prop(ffr.fault_list());

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
      for (ymuint i = 0; i < bitpos; ++ i, obs >>= 1) {
	if ( obs & 1ULL ) {
	  _fault_sweep(ffr_buff[i]->fault_list());
	}
      }
      bitpos = 0;
    }
  }
  if ( bitpos > 0 ) {
    PackedVal obs = mEventQ.simulate();
    for (ymuint i = 0; i < bitpos; ++ i, obs >>= 1) {
      if ( obs & 1ULL ) {
	_fault_sweep(ffr_buff[i]->fault_list());
      }
    }
  }

  return mDetNum;
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

END_NONAMESPACE

// @brief SPSFP故障シミュレーションの本体
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::_sa_spsfp(const TpgFault* f)
{
  // 正常値の計算を行う．
  _calc_gval();

  // FFR 内の故障伝搬を行う．
  SimFault* ff = mFaultArray[f->id()];

  SimNode* simnode = ff->mNode;

  PackedVal lobs = kPvAll1;
  for (SimNode* node = simnode; !node->is_ffr_root(); ) {
    SimNode* onode = node->fanout_top();
    ymuint pos = node->fanout_ipos();
    lobs &= onode->_calc_gobs(pos);
    node = onode;
  }

  if ( f->is_branch_fault() ) {
    // 入力の故障
    ymuint ipos = ff->mIpos;
    lobs &= simnode->_calc_gobs(ipos);
  }

  FSIM_VALTYPE ival = ff->mInode->val();
  PackedVal valdiff = _fault_diff(f, ival);
  lobs &= valdiff;

  // lobs が 0 ならその後のシミュレーションを行う必要はない．
  if ( lobs == kPvAll0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  SimNode* root = ff->mNode;
  while ( !root->is_ffr_root() ) {
    root = root->fanout_top();
  }

  if ( root->is_output() ) {
    return true;
  }

  mEventQ.put_trigger(root, kPvAll1, true);
  PackedVal obs = mEventQ.simulate();
  return (obs != kPvAll0);
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::td_spsfp(const TestVector* tv,
			 const TpgFault* f)
{
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
FSIM_CLASSNAME::td_spsfp(const NodeValList& assign_list,
			 const TpgFault* f)
{
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
FSIM_CLASSNAME::td_sppfp(const TestVector* tv)
{
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
FSIM_CLASSNAME::td_sppfp(const NodeValList& assign_list)
{
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
ymuint
FSIM_CLASSNAME::td_ppsfp()
{
}

// @brief 正常値の計算を行う．
//
// 入力ノードに gval の設定は済んでいるものとする．
void
FSIM_CLASSNAME::_calc_gval()
{
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    node->calc_val();
  }
}

// @brief FFR内の故障シミュレーションを行う．
// @param[in] fault_list 故障のリスト
PackedVal
FSIM_CLASSNAME::_fault_prop(const vector<SimFault*>& fault_list)
{
  PackedVal ffr_req = kPvAll0;
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip ) {
      continue;
    }

    // ff の故障伝搬を行う．
    PackedVal lobs = kPvAll1;
    SimNode* simnode = ff->mNode;
    for (SimNode* node = simnode; !node->is_ffr_root(); ) {
      SimNode* onode = node->fanout_top();
      ymuint pos = node->fanout_ipos();
      lobs &= onode->_calc_gobs(pos);
      node = onode;
    }

    const TpgFault* f = ff->mOrigF;
    if ( f->is_branch_fault() ) {
      // 入力の故障
      ymuint ipos = ff->mIpos;
      lobs &= simnode->_calc_gobs(ipos);
    }

    FSIM_VALTYPE ival = ff->mInode->val();
    PackedVal valdiff = _fault_diff(f, ival);
    lobs &= valdiff;

    ff->mObsMask = lobs;
    ffr_req |= lobs;
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
  mInputArray.clear();
  mOutputArray.clear();
  mLogicArray.clear();

  mFFRArray.clear();
  mFFRMap.clear();

  mSimFaults.clear();
  mFaultArray.clear();
}

// @brief 外部入力ノードを作る．
SimNode*
FSIM_CLASSNAME::make_input()
{
  ymuint id = mNodeArray.size();
  SimNode* node = SimNode::new_input(id);
  mNodeArray.push_back(node);
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
FSIM_CLASSNAME::make_gate(GateType type,
			  const vector<SimNode*>& inputs)
{
  ymuint id = mNodeArray.size();
  SimNode* node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(node);
  mLogicArray.push_back(node);
  return node;
}

END_NAMESPACE_YM_SATPG_FSIM
