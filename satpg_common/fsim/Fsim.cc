
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

namespace nsFsimSa2 {
  Fsim* new_Fsim(const TpgNetwork& network);
}

namespace nsFsimSa3 {
  Fsim* new_Fsim(const TpgNetwork& network);
}

namespace nsFsimTd2 {
  Fsim* new_Fsim(const TpgNetwork& network);
}

namespace nsFsimTd3 {
  Fsim* new_Fsim(const TpgNetwork& network);
}


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

std::unique_ptr<Fsim>
Fsim::new_Fsim2(const TpgNetwork& network,
		FaultType fault_type)
{
  if ( fault_type == FaultType::StuckAt ) {
    return static_cast<std::unique_ptr<Fsim>>(nsFsimSa2::new_Fsim(network));
  }
  else if ( fault_type == FaultType::TransitionDelay ) {
    return static_cast<std::unique_ptr<Fsim>>(nsFsimTd2::new_Fsim(network));
  }
  else {
    ASSERT_NOT_REACHED;
    return nullptr;
  }
}

std::unique_ptr<Fsim>
Fsim::new_Fsim3(const TpgNetwork& network,
		FaultType fault_type)
{
  if ( fault_type == FaultType::StuckAt ) {
    return static_cast<std::unique_ptr<Fsim>>(nsFsimSa3::new_Fsim(network));
  }
  else if ( fault_type == FaultType::TransitionDelay ) {
    return static_cast<std::unique_ptr<Fsim>>(nsFsimTd3::new_Fsim(network));
  }
  else {
    ASSERT_NOT_REACHED;
    return nullptr;
  }
}

// @brief 複数の故障にスキップマークをつける．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは消される．
void
Fsim::set_skip(const vector<const TpgFault*>& fault_list)
{
  clear_skip_all();
  for ( auto f: fault_list ) {
    set_skip(f);
  }
}

// @brief 複数の故障のスキップマークを消す．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは付けられる．
void
Fsim::clear_skip(const vector<const TpgFault*>& fault_list)
{
  set_skip_all();
  for ( auto f: fault_list ) {
    clear_skip(f);
  }
}

END_NAMESPACE_YM_SATPG
