
/// @file DtpgImplM.cc
/// @brief DtpgImplM の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgImplM.h"

#include "TpgFault.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
//#include "../struct_sat/GateLitMap_vect.h"

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPGM
int debug_dtpgm = 1;
#else
const int debug_dtpgm = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の型
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] mffc 対象のMFFC
DtpgImplM::DtpgImplM(const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp,
		     FaultType fault_type,
		     BackTracer& bt,
		     const TpgNetwork& network,
		     const TpgMFFC* mffc) :
  DtpgImpl(sat_type, sat_option, sat_outp, fault_type, bt, network, mffc->root()),
{
}

// @brief デストラクタ
DtpgImplM::~DtpgImplM()
{
}

// @brief CNF 式を作る．
void
DtpgImplM::gen_cnf(DtpgStats& stats)
{
  cnf_begin();

  // root 以降の伝搬条件を作る．
  gen_cnf_base();

  cnf_end(stats);
}

// @briefテスト生成を行う．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgImplM::dtpg(const TpgFault* fault,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  ymuint ffr_id;
  bool stat = mMffcCone->get_ffr_id(fault, ffr_id);
  if ( !stat ) {
    cerr << "Error[DtpgImplM::dtpg()]: " << fault << " is not within the MFFC" << endl;
    return kB3X;
  }

  vector<SatLiteral> assumptions;
  ymuint ffr_num = mMffcCone->ffr_num();
  if ( ffr_num > 1 ) {
    // FFR の根の出力に故障を挿入する．
    assumptions.reserve(ffr_num);
    for (ymuint i = 0; i < ffr_num; ++ i) {
      SatVarId evar = mMffcCone->ffr_var(i);
      bool inv = (i != ffr_id);
      assumptions.push_back(SatLiteral(evar, inv));
    }
  }

  // 故障に対するテスト生成を行なう．
  SatBool3 ans = solve(fault, assumptions, nodeval_list, stats);

  return ans;
}

END_NAMESPACE_YM_SATPG
