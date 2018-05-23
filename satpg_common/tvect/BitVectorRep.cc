
/// @file BitVectorRep.cc
/// @brief BitVectorRep の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "BitVectorRep.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] vlen ベクタ長
BitVectorRep::BitVectorRep(int vlen) :
  mVectLen(vlen),
  mPat(new PackedVal[block_num(vlen)])
{
  // X に初期化しておく．
  init();

  // マスクを設定する．
  int k = vect_len() % kPvBitLen;
  mMask = kPvAll1 << (kPvBitLen - k);
}

// @brief コピーコンストラクタ
// @param[in] src コピー元のソース
BitVectorRep::BitVectorRep(const BitVectorRep& src) :
  mVectLen(src.mVectLen),
  mMask(src.mMask),
  mPat(new PackedVal[block_num(mVectLen)])
{
  int n = block_num(mVectLen);
  for ( int i = 0; i < n; ++ i ) {
    mPat[i] = src.mPat[i];
  }
}

// @brief コピー代入演算子
// @param[in] src コピー元のソース
BitVectorRep&
BitVectorRep::operator=(const BitVectorRep& src)
{
  if ( &src != this ) {
    mVectLen = src.mVectLen;
    mMask = src.mMask;
    int n = block_num(mVectLen);
    mPat = new PackedVal[n];
    for ( int i = 0; i < n; ++ i ) {
      mPat[i] = src.mPat[i];
    }
  }

  return *this;
}

// @brief X の個数を得る．
int
BitVectorRep::x_count() const
{
  int nb = block_num(vect_len());
  int n = 0;
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    PackedVal v = mPat[i] | mPat[i + 1];
    // v 中の1の数を数える．
    n += count_ones(v);
  }
  return vect_len() - n;
}

// @brief 2つのビットベクタの等価比較を行う．
// @param[in] bv1, bv2 対象のビットベクタ
// @return 2つのビットベクタが等しい時 true を返す．
bool
BitVectorRep::is_eq(const BitVectorRep& bv1,
		    const BitVectorRep& bv2)
{
  ASSERT_COND( bv1.vect_len() == bv2.vect_len() );

  int nb = block_num(bv1.vect_len());
  for ( int i = 0; i < nb; ++ i ) {
    if ( bv1.mPat[i] != bv2.mPat[i] ) {
      return false;
    }
  }
  return true;
}

// @brief 2つのビットベクタの包含関係を調べる．
// @param[in] bv1, bv2 対象のビットベクタ
// @return bv1 が真に bv2 に含まれる時 true を返す．
bool
BitVectorRep::is_lt(const BitVectorRep& bv1,
		    const BitVectorRep& bv2)
{
  ASSERT_COND( bv1.vect_len() == bv2.vect_len() );

  int nb = block_num(bv1.vect_len());
  bool diff = false;
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    PackedVal val1_0 = bv1.mPat[i0];
    PackedVal val1_1 = bv1.mPat[i1];
    PackedVal val2_0 = bv2.mPat[i0];
    PackedVal val2_1 = bv2.mPat[i1];
    if ( (val1_0 & ~val2_0) != kPvAll0 ||
	 (val1_1 & ~val2_1) != kPvAll0 ) {
      return false;
    }
    if ( val1_0 != val2_0 || val1_1 != val2_1 ) {
      diff = true;
    }
  }
  return diff;
}

// @brief 2つのビットベクタの包含関係を調べる．
// @param[in] bv1, bv2 対象のビットベクタ
// @return bv1 が bv2 に含まれる時 true を返す．
//
// こちらは bv1 と bv2 が等しい場合も true を返す．
bool
BitVectorRep::is_le(const BitVectorRep& bv1,
		    const BitVectorRep& bv2)
{
  ASSERT_COND( bv1.vect_len() == bv2.vect_len() );

  int nb = block_num(bv1.vect_len());
  for ( int i = 0; i < nb; ++ i ) {
    int i0 = i;
    int i1 = i + 1;
    PackedVal val1_0 = bv1.mPat[i0];
    PackedVal val1_1 = bv1.mPat[i1];
    PackedVal val2_0 = bv2.mPat[i0];
    PackedVal val2_1 = bv2.mPat[i1];
    if ( (val1_0 & ~val2_0) != kPvAll0 ||
	 (val1_1 & ~val2_1) != kPvAll0 ) {
      return false;
    }
  }
  return true;
}

// @brief 2つのベクタが両立するとき true を返す．
bool
BitVectorRep::is_compat(const BitVectorRep& bv1,
			const BitVectorRep& bv2)
{
  ASSERT_COND( bv1.vect_len() == bv2.vect_len() );

  int nb = block_num(bv1.vect_len());
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    // 0 のビットと 1 のビットの両方が異なっていると
    // コンフリクトしている．
    PackedVal diff0 = (bv1.mPat[i0] ^ bv2.mPat[i0]);
    PackedVal diff1 = (bv1.mPat[i1] ^ bv2.mPat[i1]);
    if ( (diff0 & diff1) != kPvAll0 ) {
      return true;
    }
  }
  return false;
}

// @brief すべて未定(X) で初期化する．
void
BitVectorRep::init()
{
  int nb = block_num(vect_len());
  for ( int i = 0; i < nb; ++ i ) {
    mPat[i] = kPvAll1;
  }
}

// @brief HEX文字列から内容を設定する．
// @param[in] hex_string HEX 文字列
// @return hex_string に不適切な文字が含まれていたら false を返す．
// @note hex_string が短い時には残りは0で初期化される．
// @note hex_string が長い時には余りは捨てられる．
bool
BitVectorRep::set_from_hex(const string& hex_string)
{
  // よく問題になるが，ここでは最下位ビット側から入力する．
  int nl = hex_length(vect_len());
  int sft = 0;
  int blk = 0;
  PackedVal pat = kPvAll0;
  for ( int i = 0; i < nl; ++ i ) {
    char c = (i < hex_string.size()) ? hex_string[i] : '0';
    PackedVal pat1 = kPvAll0;
    if ( '0' <= c && c <= '9' ) {
      pat1 = static_cast<PackedVal>(c - '0');
    }
    else if ( 'a' <= c && c <= 'f' ) {
      pat1 = static_cast<PackedVal>(c - 'a' + 10);
    }
    else if ( 'A' <= c && c <= 'F' ) {
      pat1 = static_cast<PackedVal>(c - 'A' + 10);
    }
    else {
      return false;
    }
    pat |= (pat1 << sft);
    sft += 4;
    if ( sft == kPvBitLen ) {
      mPat[blk] = ~pat;
      mPat[blk + 1] = pat;
      sft = 0;
      blk += 2;
      pat = kPvAll0;
    }
  }
  if ( sft != 0 ) {
    mPat[blk + 0] = ~pat;
    mPat[blk + 1] = pat;
  }

  return true;
}

// @brief 乱数パタンを設定する．
// @param[in] randgen 乱数生成器
void
BitVectorRep::set_from_random(RandGen& randgen)
{
  int nb = block_num(vect_len());
  for ( int i = 0; i < nb; i += 2 ) {
    PackedVal v = randgen.uint64();
    int i0 = i;
    int i1 = i + 1;
    if ( i == nb - 2 ) {
      mPat[i0] = ~v & mMask;
      mPat[i1] =  v & mMask;
    }
    else {
      mPat[i0] = ~v;
      mPat[i1] =  v;
    }
  }
}

// @brief X の部分を乱数で 0/1 に設定する．
// @param[in] randgen 乱数生成器
void
BitVectorRep::fix_x_from_random(RandGen& randgen)
{
  int nb = block_num(vect_len());
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    // X のビットマスク
    PackedVal xmask = mPat[i0] & mPat[i1];
    if ( i == nb - 2 ) {
      xmask &= mMask;
    }
    if ( xmask == kPvAll0 ) {
      continue;
    }
    PackedVal v = randgen.uint64();
    mPat[i0] &= ~(~v & xmask);
    mPat[i1] &= ~( v & xmask);
  }
}

// @brief ビットベクタをコピーする．
// @param[in] src コピー元のビットベクタ
//
// - X の部分はコピーしない．
void
BitVectorRep::copy(const BitVectorRep& src)
{
  int nb = block_num(vect_len());
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    // X のビットマスク
    PackedVal xmask = src.mPat[i0] & src.mPat[i1];
    mPat[i0] = (src.mPat[i0] | xmask);
    mPat[i1] = (src.mPat[i1] | xmask);
  }
}

// @breif ビットベクタをマージする．
// @note X 以外で相異なるビットがあったら false を返す．
bool
BitVectorRep::merge(const BitVectorRep& src)
{
  ASSERT_COND( vect_len() == src.vect_len() );

  int nb = block_num(vect_len());

  // コンフリクトチェック
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    PackedVal diff0 = (mPat[i0] ^ src.mPat[i0]);
    PackedVal diff1 = (mPat[i1] ^ src.mPat[i1]);
    if ( (diff0 & diff1) != kPvAll0 ) {
      return false;
    }
  }

  // 実際のマージ
  for ( int i = 0; i < nb; i += 2 ) {
    int i0 = i;
    int i1 = i + 1;
    mPat[i0] |= src.mPat[i0];
    mPat[i1] |= src.mPat[i1];
  }
  return true;
}

// @brief 内容を BIN 形式で表す．
string
BitVectorRep::bin_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  string ans;
  for ( int i = 0; i < vect_len(); ++ i ) {
    switch ( val(i) ) {
    case Val3::_0: ans += '0'; break;
    case Val3::_1: ans += '1'; break;
    case Val3::_X: ans += 'X'; break;
    default:       ans += '-'; break; // ありえないけどバグで起こりうる．
    }
  }
  return ans;
}

// @brief 内容を HEX 形式で出力する．
string
BitVectorRep::hex_str() const
{
  // よく問題になるが，ここでは最下位ビット側から出力する．
  int tmp = 0U;
  int bit = 1U;
  string ans;
  for ( int i = 0; ; ++ i ) {
    if ( i < vect_len() ) {
      if ( val(i) == Val3::_1 ) {
	// 面倒くさいので Val3::X は Val3::_0 と同じとみなす．
	tmp += bit;
      }
      bit <<= 1;
      if ( bit != 16U ) {
	continue;
      }
    }
    else if ( bit == 1U ) {
      break;
    }

    if ( tmp <= 9 ) {
      ans += static_cast<char>('0' + tmp);
    }
    else {
      ans += static_cast<char>('A' + tmp - 10);
    }
    bit = 1U;
    tmp = 0U;
  }
  return ans;
}

END_NAMESPACE_YM_SATPG
