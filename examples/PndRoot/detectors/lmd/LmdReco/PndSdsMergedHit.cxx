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
// -----                    PndSdsMergedHit source file                -----
// -----                                 -----
// -------------------------------------------------------------------------

#include "PndSdsMergedHit.h"
#include <iostream>
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "stdlib.h"

// -----   Default constructor   -------------------------------------------
PndSdsMergedHit::PndSdsMergedHit() {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSdsMergedHit::PndSdsMergedHit(Int_t detID, Int_t sensorID, TVector3 &pos, TVector3 &dpos, Int_t clindex, Double_t charge, Int_t NDigiHits, Int_t mcindex, Int_t secMC)
  : PndSdsHit(detID, sensorID, pos, dpos, clindex, charge, NDigiHits, mcindex)
{
  fsecMC = secMC;
}
// -------------------------------------------------------------------------

PndSdsMergedHit::PndSdsMergedHit(PndSdsHit &c, Int_t secMC) : PndSdsHit(c)
{
  fsecMC = secMC;
}

// -----   Destructor   ----------------------------------------------------
PndSdsMergedHit::~PndSdsMergedHit() {}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
ClassImp(PndSdsMergedHit);
