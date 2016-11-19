﻿#ifndef DTPGTSATS_H
#define DTPGTSATS_H

/// @file DtpgTSatS.h
/// @brief DtpgTSatS のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgTSat.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgTSatS DtpgTSatS.h "DtpgTSatS.h"
/// @brief 1つの故障を対象とした CNF を生成する DtpgTSat
//////////////////////////////////////////////////////////////////////
class DtpgTSatS :
  public DtpgTSat
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  /// @param[in] dop パタンが求められた時に実行されるファンクタ
  /// @param[in] uop 検出不能と判定された時に実行されるファンクタ
  DtpgTSatS(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    BackTracer2& bt,
	    DetectOp2& dop,
	    UntestOp& uop);

  /// @brief デストラクタ
  virtual
  ~DtpgTSatS();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] network 対象のネットワーク
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] fault_list 対象の故障リスト
  /// @param[out] stats 結果を格納する構造体
  virtual
  void
  run(TpgNetwork& network,
      FaultMgr& fmgr,
      Fsim& fsim,
      const vector<const TpgFault*>& fault_list,
      DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  void
  run_single(const TpgFault* fault);

  /// @brief TFO マークを調べる．
  bool
  tfo_mark(const TpgNode* node) const;

  /// @brief TFO マークをつける．
  void
  set_tfo_mark(const TpgNode* node);

  /// @brief TFI マークを調べる．
  bool
  tfi_mark(const TpgNode* node) const;

  /// @brief TFI マークをつける．
  void
  set_tfi_mark(const TpgNode* node);

  /// @brief TFI2 マークを調べる．
  bool
  tfi2_mark(const TpgNode* node) const;

  /// @brief TFI2 マークをつける．
  void
  set_tfi2_mark(const TpgNode* node);

  /// @brief TFO マークと TFI マークのいづれかがついていたら true を返す．
  bool
  mark(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  ymuint mMaxNodeId;

  vector<const TpgNode*> mNodeList;

  vector<const TpgNode*> mNodeList2;

  vector<ymuint8> mMarkArray;

};

// @brief TFO マークを調べる．
inline
bool
DtpgTSatS::tfo_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
}

// @brief TFO マークをつける．
inline
void
DtpgTSatS::set_tfo_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
    mMarkArray[id] = 1U;
    mNodeList.push_back(node);
  }
}

// @brief TFI マークを調べる．
inline
bool
DtpgTSatS::tfi_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
}

// @brief TFI マークをつける．
inline
void
DtpgTSatS::set_tfi_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( mMarkArray[id] == 0U ) {
    mMarkArray[id] = 2U;
    mNodeList.push_back(node);
  }
}

// @brief TFI2 マークを調べる．
inline
bool
DtpgTSatS::tfi2_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 2) & 1U);
}

// @brief TFI2 マークをつける．
inline
void
DtpgTSatS::set_tfi2_mark(const TpgNode* node)
{
  ymuint id = node->id();
  if ( (mMarkArray[id] & 4U) == 0U ) {
    mMarkArray[id] |= 4U;
    mNodeList2.push_back(node);
  }
}

// @brief TFO マークと TFI マークのいづれかがついていたら true を返す．
inline
bool
DtpgTSatS::mark(const TpgNode* node)
{
  if ( mMarkArray[node->id()] ) {
    return true;
  }
  return false;
}

END_NAMESPACE_YM_SATPG

#endif // DTPGTSATS_H
