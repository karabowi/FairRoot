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

//
// C++ Implementation: MvdCalcStrip
//
// Description:
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
// modified by A. Sanchez for Hyp detector
//
#include <cmath>
#include "FairLogger.h"

#include "PndHypCalcStrip.h"
#include "TRandom.h"
//#include "FairGeoVector.h"

// static const int CH_PER_FE = 128;

PndHypCalcStrip::PndHypCalcStrip()
{
  fPitch = 0.;
  fOrient = 0.;
  fAnchor = TVector2(0., 0.);
  fNrStrips = 0;
  fThreshold = 0.;
  fNoise = 0.;
  fVerboseLevel = 1;
}

PndHypCalcStrip::PndHypCalcStrip(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise)
  : fPitch(pitch), fOrient(orient), fNrStrips(nrStrips), fNrFeChannels(nrFeChannels), fThreshold(threshold), fNoise(noise), fAnchor(firstStripAnchor)
{
  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  fVerboseLevel = 1;
  // Print();
}

PndHypCalcStrip::PndHypCalcStrip(const PndHypStripDigiPar *digipar, SensorSide side)
{
  if (side == SensorSide::kTOP) {
    fPitch = digipar->GetTopPitch();
    fOrient = digipar->GetOrient();
    fAnchor = digipar->GetTopAnchor();
    fNrStrips = digipar->GetNrTopFE() * digipar->GetNrFECh();
  } else if (side == SensorSide::kBOTTOM) {
    fPitch = digipar->GetBotPitch();
    fOrient = digipar->GetOrient() + digipar->GetSkew();
    fAnchor = digipar->GetBotAnchor();
    fNrStrips = digipar->GetNrBotFE() * digipar->GetNrFECh();
  }
  fNrFeChannels = digipar->GetNrFECh();
  fThreshold = digipar->GetThreshold();
  fNoise = digipar->GetNoise();

  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  fVerboseLevel = 1;
  // Print();
}

std::vector<PndHypStrip>
PndHypCalcStrip::GetStrips(Double_t inx, Double_t iny, Double_t, Double_t outx, Double_t outy, Double_t, Double_t eLoss, int) // inz outz id//[R.K.03/2017] unused variable(s)
{                                                                                                                             // 1
  if (fVerboseLevel > 2)
    LOG(info) << "  PndHypCalcStrip::GetStrips ";

  // 2d-Projection of trajectory
  // mvd local c.s(x,y);hyp local c.s(x-z);3.03.08 new hyp cs(xy)
  TVector2 in(inx, iny);    // TVector2 in(inx,inz);
  TVector2 out(outx, outy); // TVector2 out(outx,outz);
  TVector2 path = out - in;

  if (fVerboseLevel > 2) {
    std::cout << " InPoint: (" << in.X() << "," << in.Y() << std::endl;
    std::cout << " OutPoint: (" << out.X() << "," << out.Y() << std::endl;
  }

  std::vector<PndHypStrip> strips;
  Double_t SmearedQ;

  if (path.Mod() < 1E-18) {
    LOG(warn) << "  PndHypCalcStrip::GetStrips : No Trajectory inside Sensor!" << path.Mod();
    return strips;
  }

  if (fVerboseLevel > 1)
    std::cout << " pathlength: " << path.Mod() << std::endl;

  Double_t nuIn = CalcStripFromPoint(inx, iny);
  Double_t nuOut = CalcStripFromPoint(outx, outy);

  if (fVerboseLevel > 2)
    std::cout << " nuIn = " << nuIn << " ; nuOut = " << nuOut << std::endl;

  // Double_t dir = 0.;
  // if (nuOut>nuIn) dir=1.;
  // else dir = -1.;

  Double_t Q = ChargeFromEloss(eLoss); //*1E9/3.61; // 3.6 Electrons/eV in Silicon
  if (fVerboseLevel > 1)
    std::cout << " integral charge = " << Q << std::endl;

  // did we hit the active area ?
  // if ( (nuIn<0.5 && nuOut<0.5) ||
  //   (((nuIn+0.5) >Double_t(fNrStrips-1)) && ((nuOut+0.5) > Double_t(fNrStrips-1))))

  if ((nuIn < 0. && nuOut < 0.) || ((nuIn > Double_t(fNrStrips)) && (nuOut > Double_t(fNrStrips)))) {
    if (fVerboseLevel > 1)
      std::cout << " Hit outside active area." << std::endl;
    return strips;
  }

  // is the In-Point inside active area ?// only charge fraction inside active area taken
  if (nuIn < 0.) {
    Q *= (nuOut) / (nuOut - nuIn);
    nuIn = 0.;
  } else if (nuIn > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuOut) / (-nuOut + nuIn);
    nuIn = Double_t(fNrStrips);
  }

  // is the Out-Point inside active area ?
  if (nuOut < 0.) {
    Q *= (nuIn) / (-nuOut + nuIn);
    nuOut = 0.;

  } else if (nuOut > (Double_t(fNrStrips))) {
    Q *= ((Double_t)fNrStrips - nuIn) / (nuOut - nuIn);
    nuOut = Double_t(fNrStrips);
  }

  // only one strip hit ?
  if (Int_t(nuIn) == Int_t(nuOut)) {
    // this strip collected the entire charge
    SmearedQ = SmearCharge(Q);
    if (SmearedQ >= fThreshold)
      strips.push_back(PndHypStrip(Int_t(nuOut), SmearedQ));

    if (fVerboseLevel > 1)
      std::cout << " -> 1 strip hit." << std::endl;
    // return strips;
  } else {

    Int_t nrHits = 0;
    Double_t dQ = Q / std::fabs(nuOut - nuIn);
    Double_t dir = (nuOut > nuIn) ? 1. : -1.;
    // calculate portion of track in first strip
    Int_t nextIn = Int_t(nuIn + 0.5 + 0.5 * dir);
    Double_t Q1 = dQ * std::fabs(nextIn - nuIn);

    if (fVerboseLevel > 2) {
      std::cout << " part of first strip : " << nextIn - nuIn << std::endl;
      std::cout << " charge : " << Q1 << std::endl;
      std::cout << " next strip : " << nextIn << std::endl;
    }
    SmearedQ = SmearCharge(Q1);
    if (SmearedQ >= fThreshold)
      strips.push_back(PndHypStrip(Int_t(nuIn), SmearedQ));
    nrHits++;
    Q -= Q1;

    // calculate portion of track in last strip
    Int_t prevOut = Int_t(nuOut + 0.5 - 0.5 * dir);
    Double_t Q2 = dQ * std::fabs(nuOut - prevOut);

    if (fVerboseLevel > 2) {
      std::cout << " part of last strip : " << (nuOut - prevOut) << std::endl;
      std::cout << " charge : " << Q2 << std::endl;
      std::cout << " end of previous strip : " << prevOut << std::endl;
    }
    SmearedQ = SmearCharge(Q2);

    if (SmearedQ >= fThreshold)
      strips.push_back(PndHypStrip(Int_t(nuOut), SmearedQ));
    nrHits++;
    Q -= Q2;

    // Distribute the charge among the intermediate strips
    nextIn = Int_t(nextIn - 0.5 + 0.5 * dir);
    prevOut = Int_t(prevOut - 0.5 + 0.5 * dir);

    if (fVerboseLevel > 2) {
      std::cout << " dir=" << Int_t((dir)) << std::endl;
      std::cout << " begin=" << nextIn << " end=" << prevOut << std::endl;
    }

    for (Int_t n = nextIn; n != prevOut; n += Int_t(dir)) {

      if (fVerboseLevel > 2)
        std::cout << " n = " << n << std::endl;
      SmearedQ = SmearCharge(dQ);
      if (SmearedQ >= fThreshold)
        strips.push_back(PndHypStrip(n, SmearedQ));
      nrHits++;
      Q -= dQ; // std::cout<<" loop over MORE strips : charge "<<Q<<" dQ "<<dQ<<std::endl;
    }
    if (fVerboseLevel > 2)
      if (fabs(Q) > 1.)
        std::cout << " charge Q = " << Q << " not detected!" << std::endl;
    if (fVerboseLevel > 1)
      std::cout << " -> " << nrHits << " strips hit." << std::endl;
  }

  return strips;
}

Double_t PndHypCalcStrip::SmearCharge(Double_t charge)
{
  Double_t smeared = gRandom->Gaus(charge, fNoise);
  if (fVerboseLevel > 2)
    std::cout << " charge = " << charge << ", smeared = " << smeared << std::endl;
  return smeared; // fRNG->Gaus(charge,fNoise);//smeared;
}

Double_t PndHypCalcStrip::CalcStripFromPoint(Double_t x, Double_t y)
{
  return ((x - fAnchor.X()) * fOrthoDir.Y() - (y - fAnchor.Y()) * fOrthoDir.X()) / fPitch;
  // std::cout<<" nrstrip "
  //   <<( (x-fAnchor.X())*fStripDir.Y()-(y-fAnchor.Y())*fStripDir.X() )/fPitch<<std::endl;
}

Int_t PndHypCalcStrip::CalcFEfromStrip(Int_t stripNr) const
{
  return (stripNr / fNrFeChannels);
}

Int_t PndHypCalcStrip::CalcChannelfromStrip(Int_t stripNr) const
{
  return (stripNr % fNrFeChannels);
}

void PndHypCalcStrip::CalcFeChToStrip(Int_t fe, Int_t channel, Int_t &strip, enum SensorSide &side) const
{
  Int_t nr = fe * fNrFeChannels + channel;
  //  if (nr > fNrStrips) {
  //       nr -= fNrStrips;
  //       side = SensorSide::kBOTTOM;
  //     } else side = SensorSide::kTOP;

  if (nr < fNrStrips) {
    side = SensorSide::kTOP;
  } else {
    nr -= fNrStrips;
    side = SensorSide::kBOTTOM;
  }
  strip = nr;
  // std::cout<< "strip "<<strip<<"side "<<side<<" chan "<<channel<<std::endl;
}

void PndHypCalcStrip::CalcStripPointOnLine(Double_t strip, TVector2 &point) const
{
  point = fAnchor + (fPitch * (strip + 0.5) * fOrthoDir);
}

void PndHypCalcStrip::print() const
{
  LOG(info) << " PndHypCalcStrip Info :";
  std::cout << "     Pitch        = " << fPitch * 10000. << " um" << std::endl;
  std::cout << "     Orientation  = " << fOrient / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "     Nr Strips    = " << fNrStrips << std::endl;
  std::cout << "     nr of channels per FE  = " << fNrFeChannels << std::endl;
  std::cout << "     nr of frontends        = " << fNrStrips / fNrFeChannels << std::endl;
  std::cout << "     Anchor Point = " << fAnchor.X() << "," << fAnchor.Y() << ")" << std::endl;
  std::cout << "     Strip Vector = (" << fStripDir.X() << "," << fStripDir.Y() << ")" << std::endl;
}
