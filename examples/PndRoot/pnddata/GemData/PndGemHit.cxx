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
// -----                    PndGemHit source file                -----
// -----                                 -----
// -------------------------------------------------------------------------

#include <iostream>
#include "PndGemHit.h"
#include "PndDetectorList.h"

#include "FairTimeStamp.h"

#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"

// -----   Default constructor   -------------------------------------------
PndGemHit::PndGemHit() : FairHit(), fDr(-1), fDp(-1), fCharge(-1), fNDigiHits(-1), fBotIndex(-1) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemHit::PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t index, Double_t charge, Int_t NDigiHits, TString fromStr)
  : FairHit(detID, pos, dpos, index), fDr(-1), fDp(-1), fCharge(charge), fNDigiHits(NDigiHits), fBotIndex(-1)
{
  fDigiNr[0] = -1;
  fDigiNr[1] = -1;
  AddLink(FairLink(fromStr, index));
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemHit::PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Int_t digi1, Int_t digi2, Double_t dr, Double_t dp, Int_t index, TString fromStr)
  : FairHit(detID, pos, dpos, index), fDr(dr), fDp(dp), fCharge(-1), fNDigiHits(-1), fBotIndex(-1)
{
  fDigiNr[0] = digi1;
  fDigiNr[1] = digi2;
  AddLink(FairLink(fromStr, digi1));
  AddLink(FairLink(fromStr, digi2));
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemHit::PndGemHit(Int_t detID, TVector3 &pos, TVector3 &dpos, Double_t charge, Double_t time, Int_t digi1, Int_t digi2, Double_t dr, Double_t dp, Int_t index, TString fromStr)
  : FairHit(detID, pos, dpos, index), fDr(dr), fDp(dp), fCharge(charge), fNDigiHits(-1), fBotIndex(-1)
{
  fTimeStamp = time;
  fDigiNr[0] = digi1;
  fDigiNr[1] = digi2;
  AddLink(FairLink(fromStr, digi1));
  AddLink(FairLink(fromStr, digi2));
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemHit::~PndGemHit() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemHit::Print(const Option_t *opt) const
{
  std::cout << *this << " opt = " << opt << std::endl;
}

// Double_t PndGemHit::GetD(Int_t i)
// {
//   if(!gGeoManager){
//     std::cout<<" -E- in PndGemHit::GetD(): No gGeoManager there. Please use "<<std::endl;
//     abort();
//   }
//
//   // Caution! if there is a point reconstructed OUTSIDE its volume,
//   // then the returned stuff is WRONG!
//   ((TGeoNode*)(gGeoManager->FindNode(fX,fY,fZ)))->cd();
//   TGeoHMatrix* currMatrix = gGeoManager->GetCurrentMatrix();
//
//   const Double_t* rotation = (currMatrix->Inverse()).GetRotationMatrix();
//   Double_t local[3]={fDx,fDy,fDz};
//
//   Double_t result = fabs(rotation[3*i+0]*local[0]) + fabs(rotation[3*i+1]*local[1]) + fabs(rotation[3*i+2]*local[2]);
// std::cout"PndGemHit::GetD("<<i<<"): dLocal={"<<fDx<<","<<fDy<<","<<fDz<<"} dLab("<<i<<"="<<result<<std::endl;
// return result;
// //   Double_t master[3];
// //   gGeoManager->LocalToMasterVect(local,master);
// //   return master[i];
//
// }

// -------------------------------------------------------------------------
ClassImp(PndGemHit);
