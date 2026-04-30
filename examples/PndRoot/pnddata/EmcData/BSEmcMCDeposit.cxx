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

/////////////////////////////////////////////////////////////
//
//  BSEmcMCDeposit
//
//  BSEmc digitised hit
//
//  Created 14/08/06  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "BSEmcMCDeposit.h"

#include <FairMultiLinkedData.h>
#include <utility>

#include "Rtypes.h"     // for ClassImp
#include "RtypesCore.h" // for Double_t, Option_t

#include "FairHit.h"           // for FairHit
#include "fairlogger/Logger.h" // for LOG

// -----   Default constructor   -------------------------------------------
BSEmcMCDeposit::BSEmcMCDeposit() : FairHit(), fDepositedEnergy(), fMcList(0), fTrackEntering(), fTrackExiting(), fDetectorID(0)
{
  fMcList.clear();
}
// -------------------------------------------------------------------------

// -----   Constructor  ----------------------------------------------------
BSEmcMCDeposit::BSEmcMCDeposit(Int_t t_id, Float_t t_time, std::vector<Int_t> t_mcList, FairMultiLinkedData t_enteringTracks, FairMultiLinkedData t_exitingTracks)
  : FairHit(), fDepositedEnergy(), fMcList(t_mcList), fTrackEntering(t_enteringTracks), fTrackExiting(t_exitingTracks), fDetectorID(t_id)
{
  SetTime(t_time);
}

// -----  Copy Constructor  ------------------------------------------------

BSEmcMCDeposit::BSEmcMCDeposit(const BSEmcMCDeposit &t_copy)
  : FairHit(t_copy), fDepositedEnergy(t_copy.fDepositedEnergy), fMcList(t_copy.fMcList), fTrackEntering(t_copy.fTrackEntering), fTrackExiting(t_copy.fTrackExiting),
    fDetectorID(t_copy.fDetectorID)
{
}
// -----   Destructor   ----------------------------------------------------
BSEmcMCDeposit::~BSEmcMCDeposit() {}
// -------------------------------------------------------------------------
Double_t BSEmcMCDeposit::GetEnergy() const
{
  Double_t result = 0;
  for (const auto &pair : fDepositedEnergy) {
    result += pair.second;
  }
  return result;
}

// -------------------------------------------------------------------------
std::set<Int_t> BSEmcMCDeposit::GetClusterList() const
{
  std::set<Int_t> result;
  for (const auto &pair : fDepositedEnergy) {
    result.insert(pair.first);
  }
  return result;
}

// -----   Public method Print   -------------------------------------------
void BSEmcMCDeposit::Print(const Option_t * /*unused*/) const
{
  LOG(info) << "EMC hit: cellid=" << GetDetectorID() << ", Energy=" << GetEnergy();
  LOG(info) << "TrackEntering: " << fTrackEntering;
  LOG(info) << "TrackExiting: " << fTrackExiting;
}
// -------------------------------------------------------------------------

ClassImp(BSEmcMCDeposit)
