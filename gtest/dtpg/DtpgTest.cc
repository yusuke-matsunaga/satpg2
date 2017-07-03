
/// @file DtpgTest.cc
/// @brief DtpgTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"

#include "DtpgTest.h"


BEGIN_NAMESPACE_YM_SATPG

class DtpgTestWithParam :
public ::testing::TestWithParam<std::tuple<string, string, bool, int> >
{
public:

  /// @brief コンストラクタ
  DtpgTestWithParam();

  /// @brief テストを行う．
  void
  do_test();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  SetUp();

  /// @brief 終了処理を行う．
  void
  TearDown();

  /// @brief テストパラメータからファイル名を取り出す．
  string
  filename();

  /// @brief テストパラメータからテストモードを取り出す．
  string
  test_mode();

  /// @brief テストパラメータから td_mode を取り出す．
  bool
  td_mode();

  /// @brief テストパラメータから bt_mode を取り出す．
  int
  bt_mode();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  string mSatType;

  string mSatOption;

  ostream* mSatOutp;

  TpgNetwork mNetwork;

  DtpgTest* mDtpgTest;

};


DtpgTestWithParam::DtpgTestWithParam() :
  mSatType(""),
  mSatOption(""),
  mSatOutp(nullptr)
{
}

// @brief 初期化を行う．
void
DtpgTestWithParam::SetUp()
{
  bool stat = mNetwork.read_blif(filename());
  ASSERT_COND( stat );

  mDtpgTest = new DtpgTest(mSatType, mSatOption, mSatOutp, td_mode(), bt_mode(), mNetwork);
}

// @brief 終了処理を行う．
void
DtpgTestWithParam::TearDown()
{
  delete mDtpgTest;
}

void
DtpgTestWithParam::do_test()
{
  string mode = test_mode();
  pair<ymuint, ymuint> num_pair;
  if ( mode == "single" ) {
    num_pair = mDtpgTest->single_test();
  }
  else if ( mode == "ffr" ) {
    num_pair = mDtpgTest->ffr_test();
  }
  else if ( mode == "mffc" ) {
    num_pair = mDtpgTest->mffc_test();
  }
  else {
    ASSERT_NOT_REACHED;
  }

  const DopVerifyResult& result = mDtpgTest->verify_result();
  EXPECT_EQ( 0, result.error_count() );
}

// @brief テストパラメータからファイル名を取り出す．
string
DtpgTestWithParam::filename()
{
  string path("DATAPATH");
  return path + std::get<0>(GetParam());
}

// @brief テストパラメータからテストモードを取り出す．
string
DtpgTestWithParam::test_mode()
{
  return std::get<1>(GetParam());
}

// @brief テストパラメータから td_mode を取り出す．
bool
DtpgTestWithParam::td_mode()
{
  return std::get<2>(GetParam());
}

// @brief テストパラメータから bt_mode を取り出す．
int
DtpgTestWithParam::bt_mode()
{
  return std::get<3>(GetParam());
}

TEST_P(DtpgTestWithParam, test1)
{
  do_test();
}

INSTANTIATE_TEST_CASE_P(DtpgTest, DtpgTestWithParam,
			::testing::Combine(::testing::Values("s27.blif", "s35932.blif"),
					   ::testing::Values("single", "ffr", "mffc"),
					   ::testing::Values(false, true),
					   ::testing::Range(0, 3)));

END_NAMESPACE_YM_SATPG
