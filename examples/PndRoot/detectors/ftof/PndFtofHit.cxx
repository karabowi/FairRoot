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

// -------------------------------------------------------------------------
// -----                    FtofHit source file                -----
// -----                  Created by A. Sanchez               -----
// -------------------------------------------------------------------------

#include <iostream>
#include "PndFtofHit.h"
#include "FairLink.h"
#include "FairRootManager.h"

// -----   Default constructor   -------------------------------------------
PndFtofHit::PndFtofHit() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndFtofHit::PndFtofHit(Int_t trackId, Int_t detID, TString detName, Double_t time, Double_t dt, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge)
  : FairHit(detID, pos, dpos, index)
{
  fDetName = detName;
  fTrackID = trackId;
  fCharge = charge;
  ftime = time;
  fdt = dt;

  SetTimeStamp(time);
  SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId("FtofPoint"), index));
  // fNPixelHits = NPixelHits;
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndFtofHit::~PndFtofHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndFtofHit::Print(const Option_t *) const
{
  std::cout << "Forward tof hit in detector " << fDetName << " at (" << fX << ", " << fY << ", " << fZ << ") cm "
            << ", Point " << fRefIndex << std::endl;
}
// -------------------------------------------------------------------------
ClassImp(PndFtofHit)
