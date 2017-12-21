
/// @file FoCone.cc
/// @brief FoCone の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/FoCone.h"
#include "td/StructEnc.h"
#include "TpgNode.h"
#include "GateLitMap_vid.h"
#include "ValMap.h"
#include "td/Extractor.h"
#include "NodeValList.h"

BEGIN_NAMESPACE_YM_SATPG_TD

// @brief コンストラクタ
// @param[in] struct_sat StructEnc ソルバ
// @param[in] fnode 故障位置のノード
// @param[in] bnode ブロックノード
// @param[in] detect 検出条件
//
// ブロックノードより先のノードは含めない．
// 通常 bnode は fnode の dominator
// となっているはず．
FoCone::FoCone(StructEnc& struct_sat,
	       const TpgNode* fnode,
	       const TpgNode* bnode,
	       Val3 detect) :
  ConeBase(struct_sat)
{
  if ( bnode != nullptr ) {
    set_end_mark(bnode);
  }

  mark_tfo_tfi(fnode);

  for (ymuint i = 0; i < tfo_num(); ++ i) {
    const TpgNode* node = tfo_node(i);
    if ( node != fnode ) {
      // 故障回路のゲートの入出力関係を表すCNFを作る．
      node->make_cnf(solver(), GateLitMap_vid(node, fvar_map()));
    }

    if ( detect == kVal1 ) {
      // D-Chain 制約を作る．
      make_dchain_cnf(node);
    }
  }

  ymuint npo = output_num();

  if ( detect == kVal0 ) {
    for (ymuint i = 0; i < npo; ++ i) {
      const TpgNode* node = output_node(i);
      SatLiteral glit(gvar(node), false);
      SatLiteral flit(fvar(node), false);
      solver().add_clause(~glit,  flit);
      solver().add_clause( glit, ~flit);
    }
  }
  else if ( detect == kVal1 ) {
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(npo);
    for (ymuint i = 0; i < npo; ++ i) {
      const TpgNode* node = output_node(i);
      SatLiteral dlit(dvar(node));
      tmp_lits.push_back(dlit);
    }
    solver().add_clause(tmp_lits);

    SatLiteral dlit(dvar(fnode));
    solver().add_clause(dlit);
  }
}

// @brief デストラクタ
FoCone::~FoCone()
{
}

// @brief 十分条件を得る．
// @param[in] sat_model SAT の割り当て結果
// @param[in] fault 故障
// @param[out] suf_list 十分条件の割り当てリスト
void
FoCone::get_suf_list(const vector<SatBool3>& sat_model,
		     const TpgFault* fault,
		     NodeValList& suf_list) const
{
  ValMap val_map(hvar_map(), gvar_map(), fvar_map(), sat_model);

  Extractor extractor(val_map);
  extractor(fault, suf_list);
  suf_list.sort();
}

END_NAMESPACE_YM_SATPG_TD
