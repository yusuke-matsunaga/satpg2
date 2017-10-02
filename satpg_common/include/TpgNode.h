#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "GateType.h"
//#include "GateLitMap.h"
#include "Val3.h"
#include "ym/Alloc.h"
#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SATPG

class GateLitMap;

//////////////////////////////////////////////////////////////////////
/// @class TpgNode TpgNode.h "TpgNode.h"
/// @brief SATPG 用のノードを表すクラス
/// @sa TpgNetwork
/// @sa TpgFault
/// @sa TpgMFFC
/// @sa TpgFFR
///
/// 基本的には一つの BnNode に対応しているが，
/// もとのゲートが組み込み型でない場合には複数の TpgNode を組み合わ
/// せてもとのゲートを表す．
/// そのため，場合によってはファンインの故障を表すための仮想的な
/// ノードを挿入する場合もある．
//////////////////////////////////////////////////////////////////////
class TpgNode
{
public:
  //////////////////////////////////////////////////////////////////////
  // TpgNode の派生クラスのインスタンスを作るクラスメソッド
  // 別に個々のクラスのコンストラクタを直に呼んでもよいが，
  // こうすることで TpgNode の派生クラスのヘッダファイルを
  // TpgNode 以外のクラスの実装ファイルでインクルードする必要がなくなる．
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] iid 入力番号
  /// @param[in] fanout_num ファンアウト数
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_input(ymuint id,
	     const string& name,
	     ymuint iid,
	     ymuint fanout_num,
	     Alloc& alloc);

  /// @brief 出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] oid 出力番号
  /// @param[in] inode 入力ノード
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_output(ymuint id,
	      const string& name,
	      ymuint oid,
	      TpgNode* inode,
	      Alloc& alloc);

  /// @brief DFFの入力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] oid 出力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_input(ymuint id,
		 const string& name,
		 ymuint oid,
		 TpgDff* dff,
		 TpgNode* inode,
		 Alloc& alloc);

  /// @brief DFFの出力ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] iid 入力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanout_num ファンアウト数
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_output(ymuint id,
		  const string& name,
		  ymuint iid,
		  TpgDff* dff,
		  ymuint fanout_num,
		  Alloc& alloc);

  /// @brief DFFのクロック端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_clock(ymuint id,
		 const string& name,
		 TpgDff* dff,
		 TpgNode* inode,
		 Alloc& alloc);

  /// @brief DFFのクリア端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_clear(ymuint id,
		 const string& name,
		 TpgDff* dff,
		 TpgNode* inode,
		 Alloc& alloc);

  /// @brief DFFのプリセット端子を作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] dff 接続しているDFF
  /// @param[in] inode 入力ノード
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_dff_preset(ymuint id,
		  const string& name,
		  TpgDff* dff,
		  TpgNode* inode,
		  Alloc& alloc);

  /// @brief 論理ノードを作る．
  /// @param[in] id ノード番号
  /// @param[in] name ノード名
  /// @param[in] gate_type ゲートタイプ
  /// @param[in] inode_list 入力ノードのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @param[in] alloc メモリアロケータ
  /// @return 作成したノードを返す．
  static
  TpgNode*
  make_logic(ymuint id,
	     const string& name,
	     GateType gate_type,
	     const vector<TpgNode*>& inode_list,
	     ymuint fanout_num,
	     Alloc& alloc);


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  TpgNode(ymuint id);

  /// @brief デストラクタ
  virtual
  ~TpgNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  ymuint
  id() const;

  /// @brief ノード名を返す．
  const char*
  name() const;

  /// @brief 外部入力タイプの時 true を返す．
  virtual
  bool
  is_primary_input() const;

  /// @brief 外部出力タイプの時 true を返す．
  virtual
  bool
  is_primary_output() const;

  /// @brief DFF の入力に接続している出力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_output() でもある．
  virtual
  bool
  is_dff_input() const;

  /// @brief DFF の出力に接続している入力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_input() でもある．
  virtual
  bool
  is_dff_output() const;

  /// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clock() const;

  /// @brief DFF のクリア端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clear() const;

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_preset() const;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  virtual
  bool
  is_ppi() const;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  virtual
  bool
  is_ppo() const;

  /// @brief logic タイプの時 true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::ppi(node->input_id())
  /// の関係を満たす．
  /// is_ppi() が false の場合の返り値は不定
  virtual
  ymuint
  input_id() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::ppo(node->output_id())
  /// の関係を満たす．
  /// is_ppo() が false の場合の返り値は不定
  virtual
  ymuint
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  virtual
  ymuint
  output_id2() const;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  TpgDff*
  dff() const;

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  virtual
  GateType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は kValX を返す．
  virtual
  Val3
  noval() const;

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const = 0;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(ymuint pos) const = 0;

  /// @brief ファンアウト数を得る．
  ymuint
  fanout_num() const;

  /// @brief ファンアウトを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  TpgNode*
  fanout(ymuint pos) const;

  /// @brief FFR の根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  ffr_root() const;

  /// @brief FFRの根の場合にFFRを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgFFR*
  ffr() const;

  /// @brief MFFCの根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  mffc_root() const;

  /// @brief MFFCの根の場合にMFFCを返す．
  ///
  /// そうでなければ nullptr を返す．
  const TpgMFFC*
  mffc() const;

  /// @brief 直近の dominator を得る．
  ///
  /// これが nullptr の場合は MFFC の根のノードだということ．
  const TpgNode*
  imm_dom() const;

  /// @brief このノードに含まれる代表故障の数を返す．
  ymuint
  fault_num() const;

  /// @brief このノードに含まれる代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
  const TpgFault*
  fault(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号2をセットする．
  /// @param[in] id セットする番号
  ///
  /// 出力ノード以外では無効
  virtual
  void
  set_output_id2(ymuint id);

  /// @brief ファンアウトを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
  /// @param[in] fo_node ファンアウト先のノード
  void
  set_fanout(ymuint pos,
	     TpgNode* fo_node);

  /// @brief immediate dominator をセットする．
  /// @param[in] dom dominator ノード
  void
  set_imm_dom(const TpgNode* dom);

  /// @brief ノード名を設定する．
  /// @param[in] name ノード名
  /// @param[in] alloc メモリアロケータ
  void
  set_name(const string& name,
	   Alloc& alloc);

  /// @brief ファンインを設定する．
  /// @param[in] inode_list ファンインのリスト
  ///
  /// と同時にファンイン用の配列も確保する．
  /// 多入力ゲートのみ意味を持つ仮想関数
  virtual
  void
  set_fanin(const vector<TpgNode*>& inode_list,
	    Alloc& alloc);

  /// @brief ファンアウト数を設定する．
  /// @param[in] fanout_num
  /// @param[in] alloc メモリアロケータ
  ///
  /// 同時にファンアウト用の配列も確保する．
  void
  set_fanout_num(ymuint fanout_num,
		 Alloc& alloc);

  /// @brief 故障リストを設定する．
  void
  set_fault_list(const vector<TpgFault*>& fault_list,
		 Alloc& alloc);

  /// @brief MFFC を設定する．
  /// @param[in] mffc このノードを根とするMFFC
  void
  set_mffc(const TpgMFFC* mffc);

  /// @brief FFR を設定する．
  /// @param[in] ffr このノードを根とするFFR
  void
  set_ffr(TpgFFR* ffr);

  /// @brief このノードが持っている代表故障をリストに追加する．
  void
  add_to_fault_list(vector<TpgFault*>& fault_list);


public:
  //////////////////////////////////////////////////////////////////////
  // CNF の生成に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入出力の関係を表す CNF 式を生成する．
  /// @param[in] solver SAT ソルバ
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  virtual
  void
  make_cnf(SatSolver& solver,
	   const GateLitMap& lit_map) const;

  /// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
  /// @param[in] solver SAT ソルバ
  /// @param[in] fpos 故障のある入力位置
  /// @param[in] fval 故障値 ( 0 / 1 )
  /// @param[in] lit_map 入出力とリテラルの対応マップ
  ///
  /// こちらは入力に故障を仮定したバージョン
  virtual
  void
  make_faulty_cnf(SatSolver& solver,
		  ymuint fpos,
		  int fval,
		  const GateLitMap& lit_map) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DFS を行い FFR 内のノードと故障を求める．
  /// @param[out] fault_list 故障のリスト
  void
  dfs_ffr(vector<TpgFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  ymuint mId;

  // ノード名
  char* mName;

  // ファンアウト数
  ymuint mFanoutNum;

  // ファンアウトの配列
  TpgNode** mFanoutList;

  // immediate dominator
  const TpgNode* mImmDom;

  // FFR
  const TpgFFR* mFfr;

  // MFFC
  const TpgMFFC* mMffc;

  // 故障数
  ymuint mFaultNum;

  // 故障のリスト
  TpgFault** mFaultList;

};

/// @relates TpgNode
/// @brief ノード名を出力する
/// @param[in] s 出力先のストリーム
/// @param[in] node 対象のノード
void
print_node(ostream& s,
	   const TpgNode* node);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ID番号を得る．
inline
ymuint
TpgNode::id() const
{
  return mId;
}

// @brief ノード名を返す．
inline
const char*
TpgNode::name() const
{
  return mName;
}

// @brief ファンアウト数を得る．
inline
ymuint
TpgNode::fanout_num() const
{
  return mFanoutNum;
}

// @brief ファンアウトを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
inline
TpgNode*
TpgNode::fanout(ymuint pos) const
{
  ASSERT_COND( pos < fanout_num() );
  return mFanoutList[pos];
}

// @brief FFR の根のノードを得る．
//
// 自分が根の場合には自分自身を返す．
inline
const TpgNode*
TpgNode::ffr_root() const
{
  if ( fanout_num() == 0 || fanout_num() > 1 ) {
    return this;
  }
  return fanout(0)->ffr_root();
}

// @brief FFRの根の場合にFFRを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgFFR*
TpgNode::ffr() const
{
  return mFfr;
}

// @brief MFFCの根のノードを得る．
//
// 自分が根の場合には自分自身を返す．
inline
const TpgNode*
TpgNode::mffc_root() const
{
  if ( imm_dom() == nullptr ) {
    return this;
  }
  return imm_dom()->mffc_root();
}

// @brief MFFCの根の場合にMFFCを返す．
//
// そうでなければ nullptr を返す．
inline
const TpgMFFC*
TpgNode::mffc() const
{
  return mMffc;
}

// @brief 直近の dominator を得る．
inline
const TpgNode*
TpgNode::imm_dom() const
{
  return mImmDom;
}

// @brief このノードに含まれる代表故障の数を返す．
inline
ymuint
TpgNode::fault_num() const
{
  return mFaultNum;
}

// @brief このノードに含まれる代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < fault_num() )
inline
const TpgFault*
TpgNode::fault(ymuint pos) const
{
  ASSERT_COND( pos < fault_num() );

  return mFaultList[pos];
}

END_NAMESPACE_YM_SATPG

#endif // TPGNODE_H
