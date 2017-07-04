#ifndef TPGGATEINFO_H
#define TPGGATEINFO_H

/// @file TpgGateInfo.h
/// @brief TpgGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "GateType.h"
#include "Val3.h"
#include "ym/ym_logic.h"
#include "ym/HashMap.h"
#include "ym/TvFunc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgGateInfo TpgGateInfo.h "TpgGateInfo.h"
/// @brief TpgNode の論理関数の情報を格納するクラス
///
/// - 追加ノード数
/// - 制御値
/// の情報を持つ．
//////////////////////////////////////////////////////////////////////
class TpgGateInfo
{
public:

  /// @brief デストラクタ
  virtual
  ~TpgGateInfo() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const = 0;

  /// @brief 論理式を返す．
  virtual
  Expr
  expr() const = 0;

  /// @brief 追加ノード数を返す．
  virtual
  ymuint
  extra_node_num() const = 0;

  /// @brief 制御値を返す．
  /// @param[in] pos 入力位置
  /// @param[in] val 値
  virtual
  Val3
  cval(ymuint pos,
       Val3 val) const = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgGateInfoMgr TpgGateInfoMgr.h "TpgGateInfoMgr.h"
/// @brief TpgGateInfo を管理するクラス
//////////////////////////////////////////////////////////////////////
class TpgGateInfoMgr
{
public:

  /// @brief コンストラクタ
  TpgGateInfoMgr();

  /// @brief デストラクタ
  ~TpgGateInfoMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込み型のオブジェクトを返す．
  /// @param[in] gate_type ゲートタイプ
  const TpgGateInfo*
  simple_type(GateType gate_type);

  /// @brief 複合型のオブジェクトを返す．
  /// @param[in] ni 入力数
  /// @param[in] expr 論理式
  const TpgGateInfo*
  complex_type(ymuint ni,
	       const Expr& expr);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 組み込み型のオブジェクトの配列
  TpgGateInfo* mSimpleType[10];

  // 普通のオブジェクトのリスト
  vector<TpgGateInfo*> mList;

};

END_NAMESPACE_YM_SATPG

#endif // TPGGATEINFO_H
