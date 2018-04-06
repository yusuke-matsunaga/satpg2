#ifndef TPGFAULT_H
#define TPGFAULT_H

/// @file TpgFault.h
/// @brief TpgFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"
#include "ym/HashFunc.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgFault TpgFault.h "TpgFault.h"
/// @brief 単一縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgFault
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] val 故障値
  /// @param[in] rep_fault 代表故障
  TpgFault(int id,
	   int val,
	   TpgFault* rep_fault);

  /// @brief デストラクタ
  virtual
  ~TpgFault();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  int
  id() const;

  /// @brief 故障の入力側の TpgNode を返す．
  virtual
  const TpgNode*
  tpg_inode() const = 0;

  /// @brief 故障の出力側の TpgNode を返す．
  ///
  /// is_stem_fault() == true の時は tpg_inode() と同じになる．
  virtual
  const TpgNode*
  tpg_onode() const = 0;

  /// @brief ステムの故障の時 true を返す．
  virtual
  bool
  is_stem_fault() const = 0;

  /// @brief ブランチの故障の時 true を返す．
  bool
  is_branch_fault() const;

  /// @brief ブランチの入力位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  virtual
  int
  fault_pos() const = 0;

  /// @brief tpg_onode 上の故障位置を返す．
  ///
  /// is_branch_fault() == true の時のみ意味を持つ．
  /// tpg_onode(tpg_pos()) == tpg_inode() が成り立つ．
  virtual
  int
  tpg_pos() const = 0;

  /// @brief 故障値を返す．
  /// @note 返す値は 0 か 1
  int
  val() const;

  /// @brief 故障値を3値型で返す．
  Val3
  val3() const;

  /// @brief 故障の内容を表す文字列を返す．
  virtual
  string
  str() const = 0;

  /// @brief 代表故障の時 true を返す．
  bool
  is_rep() const;

  /// @brief 代表故障を返す．
  ///
  /// 代表故障の時は自分自身を返す．
  const TpgFault*
  rep_fault() const;

  /// @brief この故障の属しているFFRを返す．
  const TpgFFR&
  ffr() const;

  /// @brief この故障の属しているMFFCを返す．
  const TpgMFFC&
  mffc() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 微妙な関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 代表故障を設定する．
  /// @param[in] rep 代表故障
  void
  set_rep(TpgFault* rep);

  /// @brief 代表故障を返す．
  TpgFault*
  _rep_fault();

  /// @brief FFRを設定する．
  void
  set_ffr(const TpgFFR* ffr);

  /// @brief MFFCを設定する．
  void
  set_mffc(const TpgMFFC* mffc);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号 + 故障値(最下位ビット)
  ymuint mIdVal;

  // 代表故障
  TpgFault* mRepFault;

  // FFR
  const TpgFFR* mFFR;

  // MFFC
  const TpgMFFC* mMFFC;

};

/// @relates TpgFault
/// @brief ストリーム出力演算子
/// @param[in] s 出力先のストリーム
/// @param[in] f 故障
ostream&
operator<<(ostream& s,
	   const TpgFault* f);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を返す．
inline
int
TpgFault::id() const
{
  return static_cast<int>(mIdVal >> 1);
}

// @brief ブランチの故障の時 true を返す．
inline
bool
TpgFault::is_branch_fault() const
{
  return !is_stem_fault();
}

// @brief 故障値を返す．
// @note 返す値は 0 か 1
inline
int
TpgFault::val() const
{
  return static_cast<int>(mIdVal & 1UL);
}

// @brief 代表故障の時 true を返す．
inline
bool
TpgFault::is_rep() const
{
  return mRepFault == this;
}

// @brief 代表故障を返す．
// @note 代表故障の時は自分自身を返す．
inline
const TpgFault*
TpgFault::rep_fault() const
{
  return mRepFault;
}

// @brief 代表故障を返す．
inline
TpgFault*
TpgFault::_rep_fault()
{
  return mRepFault;
}

// @brief この故障の属しているFFRを返す．
inline
const TpgFFR&
TpgFault::ffr() const
{
  return *mFFR;
}

// @brief この故障の属しているMFFCを返す．
inline
const TpgMFFC&
TpgFault::mffc() const
{
  return *mMFFC;
}

END_NAMESPACE_YM_SATPG

BEGIN_NAMESPACE_YM
// TpgFault へのポインタをキーにしたハッシュ関数クラスの定義
template <>
struct HashFunc<nsSatpg::TpgFault*>
{
  SizeType
  operator()(nsSatpg::TpgFault* fault) const
  {
    return reinterpret_cast<ympuint>(fault)/sizeof(void*);
  }
};
END_NAMESPACE_YM

#endif // TPGFAULT_H
