#ifndef NODEMAP_H
#define NODEMAP_H

/// @file NodeMap.h
/// @brief NodeMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "satpg.h"
#include "ym/HashMap.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class NodeMap NodeMap.h "NodeMap.h"
/// @brief BnNode と TpgNode の対応を記録するクラス
//////////////////////////////////////////////////////////////////////
class NodeMap
{
public:

  /// @brief コンストラクタ
  NodeMap();

  /// @brief デストラクタ
  ~NodeMap();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録する．
  /// @param[in] bnnode_id BnNode のID番号
  /// @param[in] tpgnode TpgNode
  void
  reg(int bnnode_id,
      TpgNode* tpgnode);

  /// @brief 対応するノードを得る．
  TpgNode*
  get(int bnnode_id) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BnNode::id() をキーにした配列
  HashMap<int, TpgNode*> mNodeMap;

};

END_NAMESPACE_YM_SATPG

#endif // NODEMAP_H
