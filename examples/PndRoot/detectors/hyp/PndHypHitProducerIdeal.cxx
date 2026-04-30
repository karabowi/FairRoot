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
// -----                PndHypHitProducerIdeal source file             -----
// -------------------------------------------------------------------------

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "PndHypHitProducerIdeal.h"
#include "PndHypHit.h"
#include "PndHypHitInfo.h"
#include "PndHypPoint.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

// -----   Default constructor   -------------------------------------------
PndHypHitProducerIdeal::PndHypHitProducerIdeal() : FairTask("Ideal PndHyp Hit Producer")
{
  fBranchName = "HypPoint";
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypHitProducerIdeal::~PndHypHitProducerIdeal() {}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypHitProducerIdeal::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndHypHitProducerIdeal::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject(fBranchName);

  if (!fPointArray) {
    std::cout << "-W- PndHypHitProducerIdeal::Init: "
              << "No HypPoint array!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("PndHypHit");
  ioman->Register("HypHit", "Hyp", fHitArray, kTRUE);

  LOG(info) << " PndHypHitProducerIdeal: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------
void PndHypHitProducerIdeal::SetParContainers()
{
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fGeoPar = (PndGeoHypPar *)(rtdb->getContainer("PndGeoHypPar"));
}

// -----   Public method Exec   --------------------------------------------
void PndHypHitProducerIdeal::Exec(Option_t *)
{
  // Reset output array
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  fHitArray->Delete();

  // Declare some variables
  PndHypPoint *point = 0;

  Int_t detID = 0; // Detector ID
  // trackID = 0;     // Track index //[R.K.03/2017] unused variable

  TVector3 dpos; // Position and error vectors

  // Loop over PndHypPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndHypPoint *)fPointArray->At(iPoint);
    //	std::cout << " Ideal Hit Producer -Point-: " << point << std::endl;
    if (!point)
      continue;

    // Detector ID
    detID = point->GetVolumeID();

    // MCTrack ID
    // trackID = point->GetTrackID(); //[R.K.03/2017] unused variable

    TVector3 position(point->GetXin(), point->GetYin(), point->GetZin());
    // point->Position(pos);
    dpos.SetXYZ(0.2, 0.2, 0.2);
    smear(position, dpos);

    // Create new hit
    new ((*fHitArray)[iPoint]) PndHypHit(detID, point->GetDetName(), position, dpos, iPoint, point->GetEnergyLoss(), 1);
    std::cout << "Hit created for module: " << point->GetDetName() << std::endl;

  } // Loop over MCPoints

  // Event summary
  LOG(info) << " PndHypHitProducerIdeal: " << nPoints << " HypPoints, " << nPoints << " Hits created.";
}
// -------------------------------------------------------------------------
void PndHypHitProducerIdeal::smear(TVector3 &pos, TVector3 &dpos)
{
  /// smear a 3d vector

  double x = pos.x(), y = pos.y(), z = pos.z(), sigx = dpos.x(), sigy = dpos.y(), sigz = dpos.z();

  sigx = gRandom->Gaus(0, sigx);
  sigy = gRandom->Gaus(0, sigy);
  sigz = gRandom->Gaus(0, sigz);

  x += sigx;
  y += sigy;
  z += sigz;

  pos.SetXYZ(x, y, z);
  return;
}

ClassImp(PndHypHitProducerIdeal)
