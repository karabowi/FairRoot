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
//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	EMC Digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//	Xiaorong Shi            Lawrence Livermore National Lab
//	Stephen J. Gowdy        University of Edinburgh
// Dima Melnichuk - adaption for PANDA
//
// Copyright Information:
//	Copyright (C) 1996	Lawrence Livermore National Lab
//
///////////////////////////////////////////////////////////////
#include "BSEmcDigi.h"

#include "Rtypes.h"     // for ClassImp
#include "RtypesCore.h" // for Bool_t, kFALSE, kTRUE, Double_t, Float_t

#include "FairTimeStamp.h"     // for FairTimeStamp
#include "fairlogger/Logger.h" // for LOG

// -----   Default constructor   -----------------------------------
BSEmcDigi::BSEmcDigi() : FairTimeStamp(0), fEnergy(-1), fDetectorId(-1) {}
// -----------------------------------------------------------------

// -----   Constructor   -------------------------------------------
BSEmcDigi::BSEmcDigi(Int_t t_id, Double_t t_rawenergybin, Float_t t_time) : FairTimeStamp(t_time), fRawEnergyBin(t_rawenergybin), fEnergy(-1), fDetectorId(t_id) {}

// -----------------------------------------------------------------

// -----  Copy-Constructor   ---------------------------------------
BSEmcDigi::BSEmcDigi(const BSEmcDigi &t_other)
  : FairTimeStamp(t_other), fRawEnergyBin(t_other.fRawEnergyBin), fEnergy(t_other.fEnergy), fDetectorId(t_other.fDetectorId), fGain(t_other.fGain), fDCNumber(t_other.fDCNumber)
{
}
// -----------------------------------------------------------------
// -----   Destructor   --------------------------------------------
BSEmcDigi::~BSEmcDigi() {}
// -----------------------------------------------------------------

// -------------------------------------------------------------------------
Bool_t BSEmcDigi::equal(FairTimeStamp *t_data)
{
  BSEmcDigi *myDigi = dynamic_cast<BSEmcDigi *>(t_data);
  if (myDigi != nullptr) {
    if (fDetectorId == myDigi->GetDetectorId()) {
      return kTRUE;
    }
  }
  return kFALSE;
}

// -------------------------------------------------------------------------
Bool_t BSEmcDigi::operator==(const BSEmcDigi &t_otherDigi) const
{
  // 2 BSEmcDigis are equal is their relative energy difference whithin the folloowing tolerance
  Double_t energy_tolerance = 1e-5;

  if (GetDetectorId() == t_otherDigi.GetDetectorId() && (std::abs(this->GetEnergy() - t_otherDigi.GetEnergy()) / this->GetEnergy() < energy_tolerance)) {
    return kTRUE;
  }
  return kFALSE;
}

// -------------------------------------------------------------------------
Bool_t BSEmcDigi::operator!=(const BSEmcDigi &t_otherDigi) const
{
  return !(*this == t_otherDigi);
}

// -------------------------------------------------------------------------
Bool_t BSEmcDigi::operator<(const BSEmcDigi &t_otherDigi) const
{
  if (fEnergy < t_otherDigi.fEnergy) {
    return kTRUE;
  }

  return kFALSE;
}

// -----   Public method Print   -----------------------------------
void BSEmcDigi::Print(const Option_t * /*unused*/) const
{
  LOG(info) << "EMC digi: cellid=" << GetDetectorId() << ", RawEnergy=" << fRawEnergyBin << ", Energy=" << fEnergy << ", Time=" << GetTimeStamp();
}

// -------------------------------------------------------------------------
ClassImp(BSEmcDigi)
