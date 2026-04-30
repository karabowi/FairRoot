//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#ifndef BSEMCDATABRANCHNAMES_HH
#define BSEMCDATABRANCHNAMES_HH

#include <string>

/**
 * @brief Namespace containing all BranchNames
 * @ingroup EmcData
 */
namespace BSEmcDataBranchNames {
const std::string fgMCTrackBranchName{"MCTrack"};
const std::string fgMCPointBranchName{"EmcMCPoint"};
const std::string fgMCDepositBranchName{"EmcMCDeposit"};
const std::string fgWaveformDataBranchName{"EmcWaveformData"};
const std::string fgWaveformBranchName{"EmcWaveform"};
const std::string fgMultiWaveformBranchName{"EmcMultiWaveform"};
const std::string fgDigiBranchName{"EmcDigi"};
const std::string fgPreclusterBranchName{"EmcPrecluster"};
const std::string fgClusterBranchName{"EmcCluster"};
const std::string fgSubClusterBranchName{"EmcSubCluster"};
const std::string fgRecoHitBranchName{"EmcRecoHit"};
} // namespace BSEmcDataBranchNames

#endif /*BSEMCDATABRANCHNAMES_HH*/
