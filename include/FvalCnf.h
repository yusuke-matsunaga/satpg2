#ifndef FVALCNF_H
#define FVALCNF_H

/// @file FvalCnf.h
/// @brief FvalCnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "GvalCnf.h"
#include "TpgNode.h"
#include "GenVidMap.h"
#include "Val3.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class FvalCnf FvalCnf.h "FvalCnf.h"
/// @brief 故障回路のCNF式を作るためのクラス
//////////////////////////////////////////////////////////////////////
class FvalCnf
{
public:

  /// @brief コンストラクタ
  /// @param[in] max_node_id ノード番号の最大値
  /// @param[in] gval_cnf 正常回路のCNFを作るクラス
  FvalCnf(ymuint max_node_id,
	  GvalCnf& gval_cnf);

  /// @brief デストラクタ
  ~FvalCnf();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_node_id() const;

  /// @brief 十分割当リストを求める．
  /// @param[in] sat_model SAT問題の解
  /// @param[in] fault 故障
  /// @param[in] node_set 故障に関連するノード集合
  /// @param[out] suf_list 十分割当リストを格納する変数
  void
  get_suf_list(const vector<SatBool3>& sat_model,
	       const TpgFault* fault,
	       const NodeSet& node_set,
	       NodeValList& suf_list);

  /// @brief 十分割当リストを求める．
  /// @param[in] sat_model SAT問題の解
  /// @param[in] fault 故障
  /// @param[in] node_set 故障に関連するノード集合
  /// @param[out] suf_list 十分割当リストを格納する変数
  /// @param[out] pi_suf_list 外部入力上の十分割当リストを格納する変数
  void
  get_pi_suf_list(const vector<SatBool3>& sat_model,
		  const TpgFault* fault,
		  const NodeSet& node_set,
		  NodeValList& suf_list,
		  NodeValList& pi_suf_list);

  /// @brief 正常回路のCNFを生成するクラスを返す．
  GvalCnf&
  gval_cnf();

  /// @brief 正常回路のCNFを生成するクラスを返す．
  const GvalCnf&
  gval_cnf() const;

  /// @brief 正常回路の変数マップを得る．
  const VidMap&
  gvar_map() const;

  /// @brief 故障回路の変数マップを得る．
  const VidMap&
  fvar_map() const;

  /// @brief 伝搬条件の変数マップを得る．
  const VidMap&
  dvar_map() const;

  /// @brief 正常値の変数を得る．
  SatVarId
  gvar(const TpgNode* node) const;

  /// @brief 故障値の変数を得る．
  SatVarId
  fvar(const TpgNode* node) const;

  /// @brief 伝搬値の変数を得る．
  SatVarId
  dvar(const TpgNode* node) const;

  /// @brief 故障検出用の変数番号を返す．
  SatVarId
  fd_var() const;

  /// @brief ノードに故障値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] fvar 故障値の変数番号
  void
  set_fvar(const TpgNode* node,
	   SatVarId fvar);

  /// @brief ノードに伝搬値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] dvar 伝搬値の変数番号
  void
  set_dvar(const TpgNode* node,
	   SatVarId dvar);

  /// @brief 故障検出用の変数番号を割り当てる．
  void
  set_fdvar(SatVarId fdvar);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ノード番号の最大値
  ymuint mMaxId;

  // 正常回路のCNFを作るクラス
  GvalCnf& mGvalCnf;

  // 故障値の変数マップ
  GenVidMap mFvarMap;

  // 故障伝搬値の変数マップ
  GenVidMap mDvarMap;

  // 故障検出用の変数番号
  SatVarId mFdVar;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード番号の最大値を返す．
inline
ymuint
FvalCnf::max_node_id() const
{
  return mMaxId;
}

// @brief 正常回路のCNFを生成するクラスを返す．
inline
GvalCnf&
FvalCnf::gval_cnf()
{
  return mGvalCnf;
}

// @brief 正常回路のCNFを生成するクラスを返す．
inline
const GvalCnf&
FvalCnf::gval_cnf() const
{
  return mGvalCnf;
}

// @brief 正常回路の変数マップを得る．
inline
const VidMap&
FvalCnf::gvar_map() const
{
  return gval_cnf().var_map();
}

// @brief 故障回路の変数マップを得る．
inline
const VidMap&
FvalCnf::fvar_map() const
{
  return mFvarMap;
}

// @brief 伝搬条件の変数マップを得る．
inline
const VidMap&
FvalCnf::dvar_map() const
{
  return mDvarMap;
}

// @brief 正常値の変数を得る．
inline
SatVarId
FvalCnf::gvar(const TpgNode* node) const
{
  return gval_cnf().var(node);
}

// @brief 故障値の変数を得る．
inline
SatVarId
FvalCnf::fvar(const TpgNode* node) const
{
  return mFvarMap(node);
}

// @brief 伝搬値の変数を得る．
inline
SatVarId
FvalCnf::dvar(const TpgNode* node) const
{
  return mDvarMap(node);
}

// @brief 故障検出用の変数番号を返す．
inline
SatVarId
FvalCnf::fd_var() const
{
  return mFdVar;
}

// @brief ノードに故障値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] fvar 故障値の変数番号
inline
void
FvalCnf::set_fvar(const TpgNode* node,
		  SatVarId fvar)
{
  mFvarMap.set_vid(node, fvar);
}

// @brief ノードに伝搬値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] dvar 伝搬値の変数番号
inline
void
FvalCnf::set_dvar(const TpgNode* node,
		  SatVarId dvar)
{
  mDvarMap.set_vid(node, dvar);
}

// @brief 故障検出用の変数番号を割り当てる．
inline
void
FvalCnf::set_fdvar(SatVarId fdvar)
{
  mFdVar = fdvar;
}

END_NAMESPACE_YM_SATPG

#endif // FVALCNF_H
