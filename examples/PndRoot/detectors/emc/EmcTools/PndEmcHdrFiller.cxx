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
//  PndEmcHdrFiller
//
//  Filler of PndEmcHeader
//
//  Created 21/04/07  by S.Spataro
//
///////////////////////////////////////////////////////////////

#include "PndEmcHdrFiller.h"

#include "PndEmcHeader.h"
#include "PndEmcHit.h"
#include "PndEmcCluster.h"

#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TLorentzVector.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndEmcHdrFiller::PndEmcHdrFiller() : FairTask("EMC Header Filler"), fHitArray(nullptr), fDigiArray(nullptr), fCluArray(nullptr), fHdrArray(nullptr) {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndEmcHdrFiller::~PndEmcHdrFiller() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndEmcHdrFiller::Init()
{

  cout << " INITIALIZATION *********************" << endl;

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHdrFiller::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcHit"));
  if (!fHitArray) {
    cout << "-W- PndEmcHdrFiller::Init: "
         << "No PndEmcHit array!" << endl;
  }

  fDigiArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcDigi"));
  if (!fDigiArray) {
    cout << "-W- PndEmcHdrFiller::Init: "
         << "No PndEmcDigi array!" << endl;
  }
  fCluArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcCluster"));
  if (!fCluArray) {
    cout << "-W- PndEmcHdrFiller::Init: "
         << "No PndEmcCluster array!" << endl;
  }

  // Create and register output array
  fHdrArray = new TClonesArray("PndEmcHeader");

  ioman->Register("EmcHeader", "Emc", fHdrArray, kTRUE);

  LOG(info) << " PndEmcHdrFiller: Intialization successfull";

  return kSUCCESS;
}

// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndEmcHdrFiller::Exec(Option_t *)
{
 
  // Initial reset of parameters
  Double32_t fHitEnergy = 0.;
  Double32_t fCluEnergy = 0.;
  Int_t nHitMult = 0;
  Int_t nDigiMult = 0;
  Int_t nCluMult = 0;

  PndEmcHit *hit;
  fHdrArray->Clear();

  if (fHitArray) // Loop over PndEmcHit points
  {
    for (Int_t iHit = 0; iHit < (fHitArray->GetEntriesFast()); iHit++) {
      hit = (PndEmcHit *)fHitArray->At(iHit);
      fHitEnergy += hit->GetEnergy();
      nHitMult++;
    }
  } // end of PndEmcHit loop

  if (fDigiArray) // Loop over PndEmcDigi points
  {
    nDigiMult = fDigiArray->GetEntriesFast();
  } // end of PndEmcDigi loop

  PndEmcCluster *cluster;
  TLorentzVector sum_clu(0., 0., 0., 0.), single_clu(0., 0., 0., 0.);
  TVector3 mom_clu(0., 0., 0.);
  if (fCluArray) // Loop over PndEmcCluster points
  {
    for (Int_t iClu = 0; iClu < (fCluArray->GetEntriesFast()); iClu++) {
      cluster = (PndEmcCluster *)fCluArray->At(iClu);
      fCluEnergy += cluster->GetEnergy();
      nCluMult++;
      mom_clu.SetMagThetaPhi(cluster->GetEnergy(), cluster->theta(), cluster->phi());
      single_clu.SetVectM(mom_clu, 0.);
      sum_clu = sum_clu + single_clu;
    }
  } // end of PndEmcCluster loop

  // Register EmcHeader

  AddHit(fHitEnergy, nHitMult, nDigiMult, fCluEnergy, nCluMult, sum_clu.Vect());
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
PndEmcHeader *PndEmcHdrFiller::AddHit(Double32_t ene_hit, Int_t mult_hit, Int_t mult_digi, Double32_t ene_clu, Int_t mult_clu, TVector3 p_clu)
{
  // It fills the PndEmcHeader category
  TClonesArray &clref = *fHdrArray;
  Int_t size = clref.GetEntriesFast();
  PndEmcHeader(ene_hit, mult_hit, mult_digi, ene_clu, mult_clu, p_clu).Print();
  return new (clref[size]) PndEmcHeader(ene_hit, mult_hit, mult_digi, ene_clu, mult_clu, p_clu);
}
// ----

ClassImp(PndEmcHdrFiller)
