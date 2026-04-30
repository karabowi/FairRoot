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
//  PndHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////

#include <iostream>
#include "FairLogger.h"
using std::cout;
using std::endl;
#include "PndHypPoint.h"

// -----   Default constructor   -------------------------------------------
PndHypPoint::PndHypPoint()
  : FairMCPoint(), fEventID(-1), fPLin(0), fPLout(0), fmass(0), fcharge(0), fXin(0.), fYin(0.), fZin(0.), fPxin(0.), fPyin(0.), fPzin(0.), fXout(0.), fYout(0.), fZout(0.),
    fPxout(0.), fPyout(0.), fPzout(0.), fdist(0.), fDetName(""), fVolumeID(0), fpdgCode(0)
{
  fTime = 0.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndHypPoint::PndHypPoint(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof, Double_t length,
                         Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout)
  : FairMCPoint(trackID, detID, posin, momin, tof, length, eLoss), fEventID(evtID), fPLin(PLin), fPLout(PLout), fmass(mass), fcharge(charge), fXin(posin.X()), fYin(posin.Y()),
    fZin(posin.Z()), fPxin(momin.X()), fPyin(momin.Py()), fPzin(momin.Pz()), fXout(posout.X()), fYout(posout.Y()), fZout(posout.Z()), fPxout(momout.Px()), fPyout(momout.Py()),
    fPzout(momout.Pz()), fdist(dist), fDetName(detName), fVolumeID(detID), fpdgCode(pdgCode)
{
}

// -----   Destructor   ----------------------------------------------------
PndHypPoint::~PndHypPoint() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndHypPoint::Print(const Option_t *opt) const
{
  LOG(info) << " PndHypPoint: HYP Point for track " << fTrackID << " in detector " << fVolumeID;
  cout << "    Position (" << fXin << ", " << fYin << ", " << fZin << ") cm" << endl;
  cout << "    Momentum (" << fPxin << ", " << fPyin << ", " << fPzin << ") GeV" << endl;
  cout << "    Time " << fTime << " ns,  Length " << fLength << " cm,  Energy loss " << fELoss * 1.0e06 << " keV" << endl;
  cout << " opt = " << opt << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndHypPoint)
