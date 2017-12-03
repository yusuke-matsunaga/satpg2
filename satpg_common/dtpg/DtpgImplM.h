#ifndef DTPGIMPLM_H
#define DTPGIMPLM_H

/// @file DtpgImplM.h
/// @brief DtpgImplM のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgImpl_new.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgImplM DtpgImplM.h "DtpgImplM.h"
/// @brief MFFC と FFR の構造を考慮した階層的な DtpgImpl
//////////////////////////////////////////////////////////////////////
class DtpgImplM :
  public DtpgImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の型
  /// @param[in] bt バックトレーサー
  /// @param[in] network 対象のネットワーク
  /// @param[in] mffc 対象のMFFC
  DtpgImplM(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    FaultType fault_type,
	    BackTracer& bt,
	    const TpgNetwork& network,
	    const TpgMFFC* mffc);

  /// @brief デストラクタ
  virtual
  ~DtpgImplM();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF 式を作る．
  /// @param[out] stats DTPGの統計情報
  virtual
  void
  gen_cnf(DtpgStats& stats);

  /// @briefテスト生成を行う．
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  virtual
  SatBool3
  dtpg(const TpgFault* fault,
       NodeValList& nodeval_list,
       DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief MFFC 内部の故障伝搬条件を表すCNFを作る．
  void
  make_mffc_condition();

  /// @brief 故障挿入回路のCNFを作る．
  /// @param[in] elem_pos 要素番号
  /// @param[in] ovar ゲートの出力の変数
  void
  inject_fault(ymuint elem_pos,
	       SatVarId ovar);

  /// @brief 正常値を表す変数を得る．
  SatVarId
  gvar(const TpgNode* node);

  /// @brief 故障値を表す変数マップを得る．
  const VidMap&
  fvar_map() const;

  /// @brief 故障値を表す変数を得る．
  /// @param[in] node ノード
  SatVarId
  fvar(const TpgNode* node);

  /// @brief 故障値を表す変数を設定する．
  /// @param[in] node ノード
  /// @param[in] var 変数
  void
  set_fvar(const TpgNode* node,
	   SatVarId var);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemNum
  vector<SatVarId> mElemVarArray;

  // 故障番号をキーにしてFFR番号を入れる配列
  vector<ymuint> mElemPosMap;

  // MFFC内の故障回路の変数マップ
  VidMap mFvarMap;

};

// @brief 正常値を表す変数を得る．
inline
SatVarId
DtpgImplM::gvar(const TpgNode* node)
{
  return struct_sat().var(node, 1);
}

// @brief 故障値を表す変数マップを得る．
inline
const VidMap&
DtpgImplM::fvar_map() const
{
  return mFvarMap;
}

// @brief 故障値を表す変数を得る．
// @param[in] node ノード
inline
SatVarId
DtpgImplM::fvar(const TpgNode* node)
{
  return mFvarMap(node);
}

// @brief 故障値を表す変数を設定する．
// @param[in] node ノード
// @param[in] var 変数
inline
void
DtpgImplM::set_fvar(const TpgNode* node,
		    SatVarId var)
{
  mFvarMap.set_vid(node, var);
}

END_NAMESPACE_YM_SATPG

#endif // DTPGIMPLM_H
