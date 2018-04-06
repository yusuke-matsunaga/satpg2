
/// @file dtpg_test.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgTest.h"
#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"
#include "NodeValList.h"
#include "Fsim.h"
#include "DetectOp.h"
#include "DopVerifyResult.h"
#include "DopList.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " ?--single|--mffc? --blif|--iscas89 <file>" << endl;
}

int
dtpg_test(int argc,
	  char** argv)
{
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;

  bool blif = false;
  bool iscas89 = false;

  bool sa_mode = false;
  bool td_mode = false;

  bool single = false;
  bool ffr = false;
  bool mffc = false;

  bool dump = false;

  bool verbose = false;

  int bt_mode = -1;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--single") == 0 ) {
	if ( ffr || mffc ) {
	  cerr << "--single, --ffr and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	single = true;
      }
      else if ( strcmp(argv[pos], "--ffr") == 0 ) {
	if ( single || mffc ) {
	  cerr << "--single, --ffr  and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	ffr = true;
      }
      else if ( strcmp(argv[pos], "--mffc") == 0 ) {
	if ( single || ffr ) {
	  cerr << "--single, --ffr and --mffc are mutually exclusive" << endl;
	  return -1;
	}
	mffc = true;
      }
      else if ( strcmp(argv[pos], "--blif") == 0 ) {
	if ( iscas89 ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	blif = true;
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	if ( blif ) {
	  cerr << "--blif and --iscas89 are mutually exclusive" << endl;
	  return -1;
	}
	iscas89 = true;
      }
      else if ( strcmp(argv[pos], "--stuck-at") == 0 ) {
	if ( td_mode ) {
	  cerr << "--stuck-at and --transition-delay are mutually exclusive" << endl;
	  return -1;
	}
	sa_mode = true;
      }
      else if ( strcmp(argv[pos], "--transition-delay") == 0 ) {
	if ( td_mode ) {
	  cerr << "--stuck-at and --transition-delay are mutually exclusive" << endl;
	  return -1;
	}
	td_mode = true;
      }
      else if ( strcmp(argv[pos], "--bt0") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 0;
      }
      else if ( strcmp(argv[pos], "--bt1") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 1;
      }
      else if ( strcmp(argv[pos], "--bt2") == 0 ) {
	if ( bt_mode != -1 ) {
	  cerr << "--bt0, --bt1, and --bt2 are mutually exclusive" << endl;
	  return -1;
	}
	bt_mode = 2;
      }
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
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

  if ( !single && !ffr && !mffc ) {
    // mffc をデフォルトにする．
    mffc = true;
  }

  if ( !sa_mode && !td_mode ) {
    // sa_mode をデフォルトにする．
    sa_mode = true;
  }

  if ( !blif && !iscas89 ) {
    // とりあえず blif をデフォルトにする．
    blif = true;
  }

  if ( bt_mode == -1 ) {
    // bt0 をデフォルトにする．
    bt_mode = 0;
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

  if ( td_mode && network.dff_num() == 0 ) {
    cerr << "Network is combinational, stuck-at mode is assumed" << endl;
    td_mode = false;
    sa_mode = true;
  }
  FaultType fault_type = sa_mode ? FaultType::StuckAt : FaultType::TransitionDelay;

  if ( dump ) {
    print_network(cout, network);
  }

  DtpgTest dtpgtest(sat_type, sat_option, sat_outp, fault_type, bt_mode, network);

  pair<ymuint, ymuint> num_pair;
  if ( single ) {
    num_pair = dtpgtest.single_test();
  }
  else if ( ffr ) {
    num_pair = dtpgtest.ffr_test();
  }
  else if ( mffc ) {
    num_pair = dtpgtest.mffc_test();
  }
  else {
    ASSERT_NOT_REACHED;
  }

  if ( verbose ) {
    ymuint detect_num = num_pair.first;
    ymuint untest_num = num_pair.second;
    dtpgtest.print_stats(detect_num, untest_num);
  }

  const DopVerifyResult& verify_result = dtpgtest.verify_result();
  ymuint n = verify_result.error_count();
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = verify_result.error_fault(i);
    const NodeValList& assign_list = verify_result.error_assign_list(i);
    cout << "Error: " << f->str() << " is not detected with "
	 << assign_list << endl;
  }

  return n;
}

END_NAMESPACE_YM_SATPG


int
main(int argc,
     char** argv)
{
  return nsYm::nsSatpg::dtpg_test(argc, argv);
}
