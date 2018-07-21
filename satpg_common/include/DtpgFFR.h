#ifndef DTPGFFR_H
#define DTPGFFR_H

/// @file DtpgFFR.h
/// @brief DtpgFFR のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgEngine.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class DtpgFFR DtpgFFR.h "DtpgFFR.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgFFR :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] fault_type 故障の種類
  /// @param[in] just_type Justifier の種類を表す文字列
  /// @param[in] network 対象のネットワーク
  /// @param[in] ffr 故障伝搬の起点となる FFR
  DtpgFFR(const string& sat_type,
	  const string& sat_option,
	  ostream* sat_outp,
	  FaultType fault_type,
	  const string& just_type,
	  const TpgNetwork& network,
	  const TpgFFR& ffr);

  /// @brief デストラクタ
  ~DtpgFFR();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(const TpgFault* fault);

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[in] k 求めるベクタ数
  /// @param[out] tv_list ベクタを入れるリスト
  /// @return 結果を返す．
  ///
  /// * tv_list[0] は DtpgResult のベクタと同じ．
  /// * tv_list の要素数が k より少ない場合がある．
  DtpgResult
  gen_k_patterns(const TpgFault* fault,
		 int k,
		 vector<TestVector>& tv_list);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////


END_NAMESPACE_YM_SATPG

#endif // DTPGFFR_H
