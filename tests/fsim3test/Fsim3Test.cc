
/// @file Fsim3Test.cc
/// @brief Fsim3Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgFault.h"
#include "sa/TestVector.h"
#include "sa/TvMgr.h"
#include "Fsim3.h"
#include "ym/RandGen.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

const char* argv0 = "";

// SPSFP のテスト
pair<ymuint, ymuint>
spsfp_test(const TpgNetwork& network,
	   Fsim3& fsim,
	   const vector<const TestVector*>& tv_list)
{
  vector<bool> mark(network.max_fault_id(), false);
  ymuint nf = network.rep_fault_num();
  ymuint det_num = 0;
  ymuint nepat = 0;
  ymuint nv = tv_list.size();
  for (ymuint i = 0; i < nv; ++ i) {
    const TestVector* tv = tv_list[i];
    bool detect = false;
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* f = network.rep_fault(j);
      if ( mark[f->id()] ) {
	continue;
      }
      if ( fsim.spsfp(tv, f) ) {
	++ det_num;
	detect = true;
	mark[f->id()] = true;
      }
    }
    if ( detect ) {
      ++ nepat;
    }
  }

  return make_pair(det_num, nepat);
}

// SPPFP のテスト
pair<ymuint, ymuint>
sppfp_test(Fsim3& fsim,
	   const vector<const TestVector*>& tv_list)
{
  ymuint det_num = 0;
  ymuint nepat = 0;
  ymuint nv = tv_list.size();
  for (ymuint i = 0; i < nv; ++ i) {
    const TestVector* tv = tv_list[i];
    ymuint n = fsim.sppfp(tv);
    if ( n > 0 ) {
      det_num += n;
      ++ nepat;
      for (ymuint j = 0; j < n; ++ j) {
	const TpgFault* f = fsim.det_fault(j);
	fsim.set_skip(f);
      }
    }
  }

  return make_pair(det_num, nepat);
}

// PPSFP のテスト
pair<ymuint, ymuint>
ppsfp_test(Fsim3& fsim,
	   const vector<const TestVector*>& tv_list)
{
  ymuint nv = tv_list.size();

  fsim.clear_patterns();
  ymuint wpos = 0;
  ymuint det_num = 0;
  ymuint nepat = 0;
  for (ymuint i = 0; i < nv; ++ i) {
    const TestVector* tv = tv_list[i];
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      ymuint n = fsim.ppsfp();

      ymuint nb = wpos;
      PackedVal dpat_all = 0ULL;
      det_num += n;
      for (ymuint j = 0; j < n; ++ j) {
	const TpgFault* f = fsim.det_fault(j);
	PackedVal dpat = fsim.det_fault_pat(j);
	fsim.set_skip(f);
	// dpat の最初の1のビットを求める．
	ymuint first = 0;
	for ( ; first < nb; ++ first) {
	  if ( dpat & (1ULL << first) ) {
	    break;
	  }
	}
	ASSERT_COND( first < nb );
	dpat_all |= (1ULL << first);
      }
      for (ymuint i = 0; i < nb; ++ i) {
	if ( dpat_all & (1ULL << i) ) {
	  ++ nepat;
	}
      }
      fsim.clear_patterns();
      wpos = 0;
    }
  }
  if ( wpos > 0 ) {
    ymuint n = fsim.ppsfp();

    ymuint nb = wpos;
    PackedVal dpat_all = 0ULL;
    det_num += n;
    for (ymuint j = 0; j < n; ++ j) {
      const TpgFault* f = fsim.det_fault(j);
      PackedVal dpat = fsim.det_fault_pat(j);
      fsim.set_skip(f);
      // dpat の最初の1のビットを求める．
      ymuint first = 0;
      for ( ; first < nb; ++ first) {
	if ( dpat & (1ULL << first) ) {
	  break;
	}
      }
      ASSERT_COND( first < nb );
      dpat_all |= (1ULL << first);
    }
    for (ymuint i = 0; i < nb; ++ i) {
      if ( dpat_all & (1ULL << i) ) {
	++ nepat;
      }
    }
  }

  return make_pair(det_num, nepat);
}

// ランダムにテストパタンを生成する．
// @param[in] rg 乱数発生器
// @param[in] tvmgr テストベクタを管理するオブジェクト
// @param[in] nv 生成するパタン数
// @param[out] tv_list 生成されたパタンを格納するベクタ
void
randgen(RandGen& rg,
	TvMgr& tvmgr,
	ymuint nv,
	vector<const TestVector*>& tv_list)
{
  tv_list.clear();
  tv_list.resize(nv);
  for (ymuint i = 0; i < nv; ++ i) {
    TestVector* tv = tvmgr.new_vector();
    tv->set_from_random(rg);
    tv_list[i] = tv;
  }
}

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?-n #pat? ?--ppspf|--sppfp? --blif|--iscas89 <file>" << endl;
}

int
fsim2test(int argc,
	  char** argv)
{
  ymuint npat = 0;
  bool blif = false;
  bool iscas89 = false;

  bool ppsfp = false;
  bool sppfp = false;

  argv0 = argv[0];

  ymuint pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "-n") == 0 ) {
	++ pos;
	if ( pos >= argc ) {
	  cerr << " -n option requires #pat" << endl;
	  return -1;
	}
	npat = atoi(argv[pos]);
	if ( npat == 0 ) {
	  cerr << " integer expected after -n" << endl;
	  return -1;
	}
      }
      else if ( strcmp(argv[pos], "--ppsfp") == 0 ) {
	if ( sppfp ) {
	  cerr << "--ppspf and --sppfp are mutual exclusive" << endl;
	  return -1;
	}
	ppsfp = true;
      }
      else if ( strcmp(argv[pos], "--sppfp") == 0 ) {
	if ( ppsfp ) {
	  cerr << "--ppspf and --sppfp are mutual exclusive" << endl;
	  return -1;
	}
	sppfp = true;
      }
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	if ( iscas89 ) {
	  cerr << "--blif and --iscas89 are mutual exclusive" << endl;
	  return -1;
	}
	blif = true;
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	if ( blif ) {
	  cerr << "--blif and --iscas89 are mutual exclusive" << endl;
	  return -1;
	}
	iscas89 = true;
      }
      else {
	cerr << argv[pos] << ": illegal option" << endl;
	usage();
	return -1;
      }
    }
    else {
      break;
    }
  }

  if ( pos != argc - 1 ) {
    usage();
    return -1;
  }

  if ( npat == 0 ) {
    // デフォルトのパタン数を設定する．
    npat = 100000;
  }

  if ( !blif && !iscas89 ) {
    // とりあえず blif をデフォルトにする．
    blif = true;
  }

  string filename = argv[pos];
  TpgNetwork network;
  if ( blif ) {
    if ( !network.read_blif(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else if ( iscas89 ) {
    if ( !network.read_iscas89(filename) ) {
      cerr << "Error in reading " << filename << endl;
      return -1;
    }
  }
  else {
    ASSERT_NOT_REACHED;
  }

  Fsim3 fsim;
  fsim.set_network(network);

  TvMgr tvmgr;
  tvmgr.init(network.ppi_num());

  RandGen rg;
  vector<const TestVector*> tv_list;

  randgen(rg, tvmgr, npat, tv_list);

  StopWatch timer;
  timer.start();

  pair<ymuint, ymuint> dpnum;
  if ( ppsfp ) {
    dpnum = ppsfp_test(fsim, tv_list);
  }
  else if ( sppfp ) {
    dpnum = sppfp_test(fsim, tv_list);
  }
  else {
    dpnum = spsfp_test(network, fsim, tv_list);
  }
  ymuint det_num = dpnum.first;
  ymuint nepat = dpnum.second;

  timer.stop();
  USTime time = timer.time();

  cout << "# of inputs             = " << network.ppi_num() << endl
       << "# of outputs            = " << network.ppo_num() << endl
       << "# of logic gates        = " << network.node_num() << endl
       << "# of MFFCs              = " << network.mffc_num() << endl
       << "# of FFRs               = " << network.ffr_num() << endl
       << "# of simulated patterns = " << npat << endl
       << "# of effective patterns = " << nepat << endl
       << "# of total faults       = " << network.rep_fault_num() << endl
       << "# of detected faults    = " << det_num << endl
       << "# of undetected faults  = " << network.rep_fault_num() - det_num << endl
       << "Total CPU time          = " << time << endl;

  return 0;
}

END_NAMESPACE_YM_SATPG_FSIM


int
main(int argc,
     char** argv)
{
  return nsYm::nsSatpg::nsSa::nsFsim3::fsim2test(argc, argv);
}