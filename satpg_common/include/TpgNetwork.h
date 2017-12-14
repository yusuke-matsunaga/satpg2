#ifndef TPGNETWORK_H
#define TPGNETWORK_H

/// @file TpgNetwork.h
/// @brief TpgNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "Val3.h"
#include "TpgDff.h"
#include "TpgNode.h"

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/SimpleAlloc.h"


BEGIN_NAMESPACE_YM_SATPG

class TpgNodeInfo;
class AuxNodeInfo;

//////////////////////////////////////////////////////////////////////
/// @class TpgNetwork TpgNetwork.h "TpgNetwork.h"
/// @brief SATPG 用のネットワークを表すクラス
/// @sa TpgNode
/// @sa TpgDff
/// @sa TpgFault
///
/// 基本的には TpgNode のネットワーク(DAG)を表す．
/// ただし，順序回路を扱うために TpgDff というクラスを持つ．
/// TpgDff の入出力はそれぞれ疑似出力，疑似入力の TpgNode を持つ．<br>
/// 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．<br>
/// 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．<br>
/// クロック系の回路の情報も保持されるが，一般のノードとは区別される．
/// セット/リセット系の回路は通常の論理系の回路とみなす．
/// このクラスは const BnNetwork& から設定され，以降，一切変更されない．
/// 設定用の便利関数として blif フォーマットと isca89(.bench) フォーマットの
/// ファイルを読み込んで内容を設定する関数もある．<br>
/// 内容が設定されると同時に故障も定義される．
/// 構造的に等価な故障の中で一つ代表故障を決めて代表故障のリストを作る．
/// 代表故障はネットワーク全体，FFR，ノードごとにリスト化される．<br>
//////////////////////////////////////////////////////////////////////
class TpgNetwork
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgNetwork();

  /// @brief デストラクタ
  ~TpgNetwork();


public:
  //////////////////////////////////////////////////////////////////////
  // 通常の構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を得る．
  ymuint
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  ///
  /// @code
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(ymuint id) const;

  /// @brief 外部入力数を得る．
  ymuint
  input_num() const;

  /// @brief 外部入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(ymuint pos) const;

  /// @brief 外部出力数を得る．
  ymuint
  output_num() const;

  /// @brief 外部出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(ymuint pos) const;

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output2(ymuint pos) const;

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  ymuint
  ppi_num() const;

  /// @brief スキャン方式の擬似外部入力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppi_num() )
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(ymuint pos) const;

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  ymuint
  ppo_num() const;

  /// @brief スキャン方式の擬似外部出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppo_num() )
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(ymuint pos) const;

  /// @brief MFFC 数を返す．
  ymuint
  mffc_num() const;

  /// @brief MFFC を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
  const TpgMFFC*
  mffc(ymuint pos) const;

  /// @brief FFR 数を返す．
  ymuint
  ffr_num() const;

  /// @brief FFR を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
  const TpgFFR*
  ffr(ymuint pos) const;

  /// @brief DFF数を得る．
  ymuint
  dff_num() const;

  /// @brief DFF を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgDff*
  dff(ymuint pos) const;

  /// @brief 故障IDの最大値+1を返す．
  ymuint
  max_fault_id() const;

  /// @brief 全代表故障数を返す．
  ymuint
  rep_fault_num() const;

  /// @brief 代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
  const TpgFault*
  rep_fault(ymuint pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork から内容を設定する．
  /// @param[in] network 設定元のネットワーク
  void
  set(const BnNetwork& network);

  /// @brief blif ファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @return 読み込みが成功したら true を返す．
  bool
  read_blif(const string& filename);

  /// @brief blif ファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @return 読み込みが成功したら true を返す．
  bool
  read_blif(const string& filename,
	    const ClibCellLibrary& cell_library);

  /// @brief iscas89 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @return 読み込みが成功したら true を返す．
  bool
  read_iscas89(const string& filename);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 複合ゲートの入力の情報
  ///
  /// もとの入力が実際の TpgNode のどのファンインに
  /// 対応しているかを表す．
  struct InodeInfo
  {
    InodeInfo(TpgNode* node = nullptr,
	      ymuint pos = 0) :
      mNode(node),
      mPos(pos)
    {
    }

    void
    set(TpgNode* node,
	ymuint pos)
    {
      mNode = node;
      mPos = pos;
    }

    TpgNode* mNode;
    ymuint mPos;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 入力ノードを生成する．
  /// @param[in] iid 入力の番号
  /// @param[in] name ノード名
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_input_node(ymuint iid,
		  const string& name,
		  ymuint fanout_num);

  /// @brief 出力ノードを生成する．
  /// @param[in] oid 出力の番号
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_output_node(ymuint oid,
		   const string& name,
		   TpgNode* inode);

  /// @brief DFFの入力ノードを生成する．
  /// @param[in] oid 出力の番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_input_node(ymuint oid,
		      TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFの出力ノードを生成する．
  /// @param[in] iid 入力の番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_output_node(ymuint iid,
		       TpgDff* dff,
		       const string& name,
		       ymuint fanout_num);

  /// @brief DFFのクロック端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clock_node(TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFのクリア端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clear_node(TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFのプリセット端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_preset_node(TpgDff* dff,
		       const string& name,
		       TpgNode* inode);

  /// @brief 論理ノードを生成する．
  /// @param[in] name ノード名
  /// @param[in] node_info 論理関数の情報
  /// @param[in] fanin_list ファンインのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_logic_node(const string& name,
		  const TpgGateInfo* node_info,
		  const vector<TpgNode*>& fanin_list,
		  ymuint fanout_num);

  /// @brief 論理式から TpgNode の木を生成する．
  /// @param[in] name ノード名
  /// @param[in] expr 式
  /// @param[in] leaf_nodes 式のリテラルに対応するノードの配列
  /// @param[in] inode_array ファンインの対応関係を収める配列
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  ///
  /// leaf_nodes は 変数番号 * 2 + (0/1) に
  /// 該当する変数の肯定/否定のリテラルが入っている．
  TpgNode*
  make_cplx_node(const string& name,
		 const Expr& expr,
		 const vector<TpgNode*>& leaf_nodes,
		 vector<InodeInfo>& inode_array,
		 ymuint fanout_num);

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @param[in] name ノード名
  /// @param[in] type ゲートの型
  /// @param[in] fanin_list ファンインのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_prim_node(const string& name,
		 GateType type,
		 const vector<TpgNode*>& fanin_list,
		 ymuint fanout_num);

  /// @brief 出力の故障を作る．
  /// @param[in] name 故障位置のノード名
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] node 故障位置のノード
  void
  new_ofault(const char* name,
	     int val,
	     TpgNode* node);

  /// @brief 入力の故障を作る．
  /// @param[in] name 故障位置のノード名
  /// @param[in] ipos 故障位置のファンイン番号
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] inode_info TpgNode 上のノードの情報
  /// @param[in] rep 代表故障
  ///
  /// プリミティブ型の場合は ipos と inode_pos は同一だが
  /// 複合型の場合には異なる．
  void
  new_ifault(const char* name,
	     ymuint ipos,
	     int val,
	     const InodeInfo& inode_info,
	     TpgFault* rep);

  /// @brief 出力の故障を得る．
  /// @param[in] id ノードID ( 0 <= id < node_num() )
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFault*
  _node_output_fault(ymuint id,
		     int val);

  /// @brief 入力の故障を得る．
  /// @param[in] id ノードID ( 0 <= id < node_num() )
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] pos 入力の位置番号
  TpgFault*
  _node_input_fault(ymuint id,
		    int val,
		    ymuint pos);


  /// @brief 代表故障を設定する．
  /// @param[in] node 対象のノード
  void
  set_rep_faults(TpgNode* node);

  /// @brief FFR の情報を設定する．
  /// @param[in] root FFR の根のノード
  /// @param[in] ffr 対象の FFR
  void
  set_ffr(TpgNode* root,
	  TpgFFR* ffr);

  /// @brief MFFC の情報を設定する．
  /// @param[in] root MFFCの根のノード
  /// @param[in] mffc 対象のMFFC
  void
  set_mffc(TpgNode* root,
	   TpgMFFC* mffc);


private:
  //////////////////////////////////////////////////////////////////////
  // オブジェクトの生成はクラスメソッドのみが行なう．
  //////////////////////////////////////////////////////////////////////

  /// @brief コピーコンストラクタは実装しない．
  TpgNetwork(const TpgNetwork& src);

  /// @brief 代入演算子も実装しない．
  const TpgNetwork&
  operator=(const TpgNetwork& src);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNetwork 関係のメモリ確保を行なうオブジェクト
  SimpleAlloc mAlloc;

  // 外部入力数
  ymuint mInputNum;

  // 外部出力数
  ymuint mOutputNum;

  // DFF数
  ymuint mDffNum;

  // DFFの実体の配列
  TpgDff* mDffArray;

  // ノード数(mNodeArrayの要素数)
  ymuint mNodeNum;

  // ノードの配列
  TpgNode** mNodeArray;

  // ノードの付加情報の配列
  AuxNodeInfo* mAuxInfoArray;

  // PPIノードの配列
  TpgNode** mPPIArray;

  // PPOノードの配列
  TpgNode** mPPOArray;

  // TFI サイズの降順に整列したPPOノードの配列
  TpgNode** mPPOArray2;

  // MFFC 数
  ymuint mMffcNum;

  // MFFC の本体の配列
  TpgMFFC* mMffcArray;

  // FFR 数
  ymuint mFfrNum;

  // FFR の本体の配列
  TpgFFR* mFfrArray;

  // 全故障数
  ymuint mFaultNum;

  // 全代表故障数
  ymuint mRepFaultNum;

  // 代表故障の配列
  const TpgFault** mRepFaultArray;

};

/// @brief TpgNetwork の内容を出力する関数
/// @param[in] s 出力先のストリーム
/// @param[in] network 対象のネットワーク
void
print_network(ostream& s,
	      const TpgNetwork& network);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード数を得る．
inline
ymuint
TpgNetwork::node_num() const
{
  return mNodeNum;
}

// @brief 外部入力数を得る．
inline
ymuint
TpgNetwork::input_num() const
{
  return mInputNum;
}

// @brief 外部入力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
inline
const TpgNode*
TpgNetwork::input(ymuint pos) const
{
  ASSERT_COND( pos < input_num() );

  return mPPIArray[pos];
}

// @brief 外部出力数を得る．
inline
ymuint
TpgNetwork::output_num() const
{
  return mOutputNum;
}

// @brief 外部出力ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
inline
const TpgNode*
TpgNetwork::output(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );

  return mPPIArray[pos];
}

// @brief サイズの降順で整列した順番で外部出力ノードを取り出す．
inline
const TpgNode*
TpgNetwork::output2(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );

  return mPPOArray2[pos];
}

// @brief DFF数を得る．
inline
ymuint
TpgNetwork::dff_num() const
{
  return mDffNum;
}

// @brief DFF を得る．
// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
inline
const TpgDff*
TpgNetwork::dff(ymuint pos) const
{
  ASSERT_COND( pos < dff_num() );
  return &mDffArray[pos];
}

// @brief スキャン方式の擬似外部入力数を得る．
//
// = input_num() + dff_num()
inline
ymuint
TpgNetwork::ppi_num() const
{
  return mInputNum + mDffNum;
}

// @brief スキャン方式の擬似外部入力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < ppi_num() )
const inline
TpgNode*
TpgNetwork::ppi(ymuint pos) const
{
  ASSERT_COND( pos < ppi_num() );

  return mPPIArray[pos];
}

// @brief スキャン方式の擬似外部出力数を得る．
//
// = output_num() + dff_num()
inline
ymuint
TpgNetwork::ppo_num() const
{
  return mOutputNum + mDffNum;
}

// @brief スキャン方式の擬似外部出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < ppo_num() )
inline
const TpgNode*
TpgNetwork::ppo(ymuint pos) const
{
  ASSERT_COND( pos < ppo_num() );

  return mPPOArray[pos];
}

// @brief ノードを得る．
inline
const TpgNode*
TpgNetwork::node(ymuint pos) const
{
  ASSERT_COND( pos < mNodeNum );
  return mNodeArray[pos];
}

// @brief 故障IDの最大値+1を返す．
inline
ymuint
TpgNetwork::max_fault_id() const
{
  return mFaultNum;
}

// @brief MFFC 数を返す．
inline
ymuint
TpgNetwork::mffc_num() const
{
  return mMffcNum;
}

// @brief FFR 数を返す．
inline
ymuint
TpgNetwork::ffr_num() const
{
  return mFfrNum;
}

// @brief 全代表故障数を返す．
inline
ymuint
TpgNetwork::rep_fault_num() const
{
  return mRepFaultNum;
}

// @brief 代表故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
inline
const TpgFault*
TpgNetwork::rep_fault(ymuint pos) const
{
  ASSERT_COND( pos < rep_fault_num() );
  return mRepFaultArray[pos];
}

END_NAMESPACE_YM_SATPG

#endif // TPGNETWORK_H
