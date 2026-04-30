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

//______________________________________________________________________________
//
// C++ Implementation: PndSdsCalcStrip
//
// Description:
//
//
// Author: HG Zaunick <hg.zaunick@physik.tu-dresden.de>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <cmath>
#include <exception>

#include "PndSdsCalcStrip.h"
#include "TRandom.h"
#include "FairLogger.h"

//______________________________________________________________________________
PndSdsCalcStrip::PndSdsCalcStrip()
  : fPitch(0.), fOrient(0.), fNrStrips(0), fNrFeChannels(0), fAnchor(0., 0.), fThreshold(0.), fNoise(0.), fCSigma(0.), fStripDir(0., 0.), fOrthoDir(0., 0.), fVerboseLevel(0)
{
}

//______________________________________________________________________________
PndSdsCalcStrip::PndSdsCalcStrip(Double_t pitch, Double_t orient, Int_t nrStrips, Int_t nrFeChannels, const TVector2 &firstStripAnchor, Double_t threshold, Double_t noise,
                                 Double_t csigma = 0)
  : fPitch(pitch), fOrient(orient), fNrStrips(nrStrips), fNrFeChannels(nrFeChannels), fAnchor(firstStripAnchor), fThreshold(threshold), fNoise(noise), fCSigma(csigma),
    fStripDir(0., 0.), fOrthoDir(0., 0.), fVerboseLevel(0)
{
  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  // Print();
}

//______________________________________________________________________________
PndSdsCalcStrip::PndSdsCalcStrip(const PndSdsStripDigiPar *digipar, SensorSide side)
  : fPitch(0.), fOrient(0.), fNrStrips(0), fNrFeChannels(0), fAnchor(0., 0.), fThreshold(0.), fNoise(0.), fCSigma(0.), fStripDir(0., 0.), fOrthoDir(0., 0.), fVerboseLevel(0)
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
  fCSigma = digipar->GetQCloudSigma();

  fStripDir.Set(cos(fOrient), sin(fOrient));
  fOrthoDir.Set(sin(fOrient), -cos(fOrient));
  fVerboseLevel = 0;
  if (fVerboseLevel > 0)
    Print();
}

//______________________________________________________________________________
std::vector<PndSdsStrip> PndSdsCalcStrip::GetStrips(Double_t inx, Double_t iny, Double_t,   // inz //[R.K.03/2017] unused variable(s)
                                                    Double_t outx, Double_t outy, Double_t, //  outz //[R.K.03/2017] unused variable(s)
                                                    Double_t eLoss)
{
  if (fVerboseLevel > 2)
    LOG(info) << " PndSdsCalcStrip::GetStrips ";

  // 2d-Projection of trajectory
  TVector2 in(inx, iny);
  TVector2 out(outx, outy);
  TVector2 path = out - in;

  if (fVerboseLevel > 2) {
    std::cout << " InPoint: (" << in.X() << "," << in.Y() << ")" << std::endl;
    std::cout << " OutPoint: (" << out.X() << "," << out.Y() << ")" << std::endl;
  }

  // if (path.Mod()<1E-18) {
  // std::cout<<"-W- PndSdsCalcStrip::GetStrips : No Trajectory inside Sensor! (out-in).Mod() = "<<path.Mod()<<std::endl;
  // std::vector<PndSdsStrip> strips;
  // return strips;
  //}

  if (fVerboseLevel > 1)
    std::cout << " pathlength: " << path.Mod() << std::endl;

  Double_t nuIn = CalcStripFromPoint(inx, iny);
  Double_t nuOut = CalcStripFromPoint(outx, outy);

  if (fVerboseLevel > 2)
    std::cout << " nuIn = " << nuIn << " ; nuOut = " << nuOut << std::endl;

  Double_t Q = ChargeFromEloss(eLoss); //*1E9/3.61; // 3.6 eV/Electron in Silicon
  if (fVerboseLevel > 1)
    std::cout << " integral charge = " << Q << std::endl;
  if (fVerboseLevel > 1)
    std::cout << "Charge cloud sigma=" << fCSigma << std::endl;
  // Do charge distribution
  if (fCSigma > 0)
    return GetStripsDif(nuIn, nuOut, Q);
  else
    return GetStripsNoDif(nuIn, nuOut, Q);
}

//______________________________________________________________________________
std::vector<PndSdsStrip> PndSdsCalcStrip::GetStripsNoDif(Double_t nuIn, Double_t nuOut, Double_t Q)
{
  // Charge distributed equally along a path.

  if (fVerboseLevel > 2)
    LOG(info) << " PndSdsCalcStrip::GetStripsNoDif ";
  std::vector<PndSdsStrip> strips;

  if (fVerboseLevel > 2)
    std::cout << " nuIn = " << nuIn << " ; nuOut = " << nuOut << std::endl;
  if (fVerboseLevel > 1)
    std::cout << " integral charge = " << Q << std::endl;

  // did we hit the active area ?
  //     if ( (nuIn<0.5 && nuOut<0.5) || (((nuIn+0.5) > Double_t(fNrStrips-1)) && ((nuOut+0.5) > Double_t(fNrStrips-1)))){
  if ((nuIn < 0. && nuOut < 0.) || ((nuIn > Double_t(fNrStrips)) && (nuOut > Double_t(fNrStrips)))) {
    if (fVerboseLevel > 1)
      LOG(warn) << " PndSdsCalcStrip::GetStripsNoDif: Hit outside active area.";
    return strips;
  }

  // is the In-Point inside active area ?
  // only charge fraction inside active area taken
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
    InjectStripCharge(strips, (Int_t)nuOut, Q);
    return strips;

  } else { // more than one strip is hit
    Double_t dQ = Q / std::fabs(nuOut - nuIn);
    Double_t dir = (nuOut > nuIn) ? 1. : -1.;
    Int_t nrHits = 0;

    // calculate portion of track in first strip
    Int_t nextIn = Int_t(nuIn + 0.5 + 0.5 * dir);
    Double_t Q1 = dQ * std::fabs(nextIn - nuIn);
    if (fVerboseLevel > 2) {
      std::cout << " part of first strip : " << nextIn - nuIn << std::endl;
      std::cout << " charge : " << Q1 << std::endl;
      std::cout << " next strip : " << nextIn << std::endl;
    }
    InjectStripCharge(strips, (Int_t)nuIn, Q1);
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
    InjectStripCharge(strips, (Int_t)nuOut, Q2);
    nrHits++;
    Q -= Q2;

    // Distribute the charge amongst the intermediate strips
    nextIn = Int_t(nextIn - 0.5 + 0.5 * dir);
    prevOut = Int_t(prevOut - 0.5 + 0.5 * dir);
    if (fVerboseLevel > 2) {
      std::cout << " dir=" << Int_t(dir) << std::endl;
      std::cout << " begin=" << nextIn << " end=" << prevOut << std::endl;
    }

    for (Int_t n = nextIn; n != prevOut; n += Int_t(dir)) {
      if (fVerboseLevel > 2)
        std::cout << " n = " << n << std::endl;
      InjectStripCharge(strips, n, dQ);
      nrHits++;
      Q -= dQ;
    }
    if (fVerboseLevel > 2)
      if (fabs(Q) > 1.)
        std::cout << " charge Q = " << Q << " not detected!" << std::endl;
    if (fVerboseLevel > 1)
      std::cout << " -> " << nrHits << " strips hit." << std::endl;
  }

  return strips;
}

//______________________________________________________________________________
std::vector<PndSdsStrip> PndSdsCalcStrip::GetStripsDif(Double_t pathstart, Double_t pathend, Double_t Q)
{
  // Do charge diffusion integrated analytically over a path length
  // 0.5*(1+erf(x)) is the integral over a gauss from -inf to x
  // factor 0.5 is applied last, the +1 terms cancel in the difference

  if (pathend < pathstart) { // sort for direction
    Double_t tmp = pathstart;
    pathstart = pathend;
    pathend = tmp;
  }
  std::vector<PndSdsStrip> array;
  Double_t DQ = 0.;
  // sigma_str = sigma_um/pitch_str-per-um
  Double_t sigma_str = fCSigma / fPitch;
  // TODO how much extra bins to fill?, minimum 1...
  // how about 2sigma? shall be collected
  Int_t xtra = (Int_t)ceil(2. * sigma_str);
  for (Int_t i = (Int_t)pathstart - xtra; i < (Int_t)pathend + 1 + xtra; i++) {
    DQ = 0;
    if (fabs(pathstart - pathend) < 1e-6) { // too small path, don't integrate over path
      DQ += TMath::Erf(i + 1 - 0.5 * (pathstart + pathend)) / (sqrt(2) * sigma_str);
      DQ -= TMath::Erf(i - 0.5 * (pathstart + pathend)) / (sqrt(2) * sigma_str);
    } else {
      DQ += CalcFk(i, pathend, sigma_str);
      DQ -= CalcFk(i + 1, pathend, sigma_str);
      DQ -= CalcFk(i, pathstart, sigma_str);
      DQ += CalcFk(i + 1, pathstart, sigma_str);
      DQ /= (pathend - pathstart);
    }
    DQ *= 0.5 * Q;
    InjectStripCharge(array, i, DQ);
  }
  return array;
}

//______________________________________________________________________________
Double_t PndSdsCalcStrip::CalcFk(Double_t strip, Double_t x, Double_t sig)
{
  const Double_t t = (strip - x) / (sqrt(2) * sig);
  return ((strip - x) * TMath::Erf(t) + sqrt(2 / TMath::Pi()) * sig * exp(-t * t));
}

//______________________________________________________________________________
Int_t PndSdsCalcStrip::GetStripsAlternative(Double_t nuIn, Double_t nuOut, Double_t Q, Int_t mode, std::vector<Int_t> &indice, std::vector<Double_t> &charges)
{
  if (fVerboseLevel > 2)
    Info("GetStripsAlternative()", "begin with in=%f, out=%f, Q=%f", nuIn, nuOut, Q);
  std::vector<PndSdsStrip> strips;
  if (mode == 0)
    strips = GetStripsNoDif(nuIn, nuOut, Q);
  if (mode == 1)
    strips = GetStripsDif(nuIn, nuOut, Q);
  Int_t nstr = strips.size();
  for (Int_t i = 0; i < nstr; i++) {
    if (fVerboseLevel > 2)
      Info("GetStripsAlternative()", "pass this strip: i=%i, s=%i, q=%f", i, strips[i].GetIndex(), strips[i].GetCharge());
    indice.push_back(strips[i].GetIndex());
    charges.push_back(strips[i].GetCharge());
  }
  return nstr;
}

//______________________________________________________________________________
Double_t PndSdsCalcStrip::CalcStripFromPoint(Double_t x, Double_t y)
{
  // fOrthoDir is already set to magnitude == 1., makes it cheaper here.
  return ((x - fAnchor.X()) * fOrthoDir.X() + (y - fAnchor.Y()) * fOrthoDir.Y()) / fPitch;
}

//______________________________________________________________________________
void PndSdsCalcStrip::CalcStripPointOnLine(Double_t strip, TVector2 &point) const
{
  point = fPitch * (strip + 0.5) * fOrthoDir + fAnchor;
}

//______________________________________________________________________________
Int_t PndSdsCalcStrip::CalcFEfromStrip(Int_t stripNr) const
{
  return (stripNr / fNrFeChannels);
}

//______________________________________________________________________________
Int_t PndSdsCalcStrip::CalcChannelfromStrip(Int_t stripNr) const
{
  return (stripNr % fNrFeChannels);
}

//______________________________________________________________________________
void PndSdsCalcStrip::CalcFeChToStrip(Int_t fe, Int_t channel, Int_t &strip, enum SensorSide &side) const
{
  // Caution! The top side s always the reference side!
  Int_t nr = fe * fNrFeChannels + channel;
  if (nr < fNrStrips) {
    side = SensorSide::kTOP;
  } else {
    nr -= fNrStrips;
    side = SensorSide::kBOTTOM;
  }
  strip = nr;
}

//______________________________________________________________________________
void PndSdsCalcStrip::InjectStripCharge(std::vector<PndSdsStrip> &array, Int_t istrip, Double_t charge)
{
  if (istrip < 0) {
    if (fVerboseLevel > 2)
      Warning("InjectStripCharge", "Invalid strip number: %i < 0", istrip);
    return;
  }
  if (istrip > fNrStrips) {
    if (fVerboseLevel > 2)
      Warning("InjectStripCharge", "Invalid strip number: %i > %i", istrip, fNrStrips);
    return;
  }
  if (charge == 0)
    return; // cut zero electron charge now, real threshold later
  Double_t smearedQ = SmearCharge(charge);
  if (smearedQ < fThreshold) {
    if (fVerboseLevel > 3)
      Info("InjectStripCharge", "Strip %i, charge %f below threshold %f", istrip, smearedQ, fThreshold);
    return;
  }
  if (fVerboseLevel > 3)
    Info("InjectStripCharge", "istrip=%i,charge=%f,smearedCharge=%f", istrip, charge, smearedQ);
  array.push_back(PndSdsStrip(Int_t(istrip), smearedQ));
  return;
}

//______________________________________________________________________________
Double_t PndSdsCalcStrip::SmearCharge(Double_t charge)
{
  Double_t smeared = gRandom->Gaus(charge, fNoise);
  if (fVerboseLevel > 3)
    Info("SmearCharge:", " charge = %f, smeared = %f", charge, smeared);
  return smeared;
}

void PndSdsCalcStrip::Print() const
{
  LOG(info) << " PndSdsCalcStrip Info :";
  std::cout << "     pitch                  = " << fPitch * 10000. << " um" << std::endl;
  std::cout << "     orientation angle      = " << fOrient / TMath::Pi() * 180. << " deg" << std::endl;
  std::cout << "     nr of strips           = " << fNrStrips << std::endl;
  std::cout << "     nr of channels per FE  = " << fNrFeChannels << std::endl;
  std::cout << "     nr of frontends        = " << fNrStrips / fNrFeChannels << std::endl;
  std::cout << "     anchor point           = (" << fAnchor.X() << "," << fAnchor.Y() << ")" << std::endl;
  std::cout << "     strip-direction vector = (" << fStripDir.X() << "," << fStripDir.Y() << ")" << std::endl;
}
