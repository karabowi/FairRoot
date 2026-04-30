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
// -----                     PndGemSensor source file                  -----
// -----                  Created 12/02/2009  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "PndGemSensor.h"

#include "TMath.h"
#include "TRandom.h"
#include "FairLogger.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::flush;
using std::pair;
using std::vector;

// -----   Default constructor   -------------------------------------------
PndGemSensor::PndGemSensor()
{

  fDetectorId = 0;
  fType = 0;
  fPosition[0] = fPosition[1] = fPosition[2] = fRotation = fInnerRadius = fOuterRadius = fD = fStripAngle[0] = fStripAngle[1] = fPitch[0] = fPitch[1] = 0.;
  fNChannelsFront = fNChannelsBack = 0;
  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensor::PndGemSensor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad, Double_t outerRad, Double_t d,
                           Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1)
{

  fName = tempName.Data();
  fDetectorId = detId;
  fType = iType;
  fPosition[0] = x0;
  fPosition[1] = y0;
  fPosition[2] = z0; // z position of the station
  fRotation = rotation;
  fInnerRadius = innerRad;
  fOuterRadius = outerRad;
  fD = d;                       // thickness of the station
  fStripAngle[0] = stripAngle0; // strip angle
  fStripAngle[1] = stripAngle1; // strip angle
  fPitch[0] = pitch0;           // strip pitch
  fPitch[1] = pitch1;           // strip pitch

  if (fType == 0) { // r phi version
                    //     fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
    fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = 2 * (Int_t)((fOuterRadius - fInnerRadius) / fPitch[1] + 0.5);
  }
  if (fType == 1) { // tilted version, should not be used any more
                    //     fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
    fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[1] + 0.5);
  }
  if (fType == 2) { // x y version
                    //     fNChannelsFront =   (Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[0]))+
                    //                         (Int_t)(TMath::Ceil(2.*fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[1]));
    fNChannelsFront = (Int_t)(2. * fOuterRadius / fPitch[0] + 2. * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = 2 * (Int_t)(2. * fOuterRadius / fPitch[1] + 0.5);
  }
  if (fType == 3) { // r_tree phi version
    Int_t ifac = 1;
    if (1 < fOuterRadius / fInnerRadius && fOuterRadius / fInnerRadius < 100) {
      // extract highest bit number to get radial tree factor
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      //     fNChannelsFront = ifac *(Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
      fNChannelsFront = ifac * (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    } else {
      cout << tempName.Data() << "-W- !!! type " << fType << " with OuterRadius/InnerRadius " << fOuterRadius / fInnerRadius << " is currently not supported !!!" << endl;
      cout << " Please use type 0 instead !!!" << endl;
      //      fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
      fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    }
    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
    fNChannelsBack = 2 * (Int_t)((fOuterRadius - fInnerRadius) / fPitch[1] + 0.5);
  }

  // cout << tempName.Data() << " type " << fType << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensor::PndGemSensor(TString tempName, Int_t stationNr, Int_t sectorNr, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad,
                           Double_t outerRad, Double_t d, Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1)
{

  fName = tempName.Data();
  SetDetectorId(stationNr, sectorNr);
  fType = iType;
  fPosition[0] = x0;
  fPosition[1] = y0;
  fPosition[2] = z0; // z position of the station
  fRotation = rotation;
  fInnerRadius = innerRad;
  fOuterRadius = outerRad;
  fD = d;                       // thickness of the station
  fStripAngle[0] = stripAngle0; // strip angle
  fStripAngle[1] = stripAngle1; // strip angle
  fPitch[0] = pitch0;           // strip pitch
  fPitch[1] = pitch1;           // strip pitch

  if (fType == 0) { // r phi version
                    //     fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
    fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = 2 * (Int_t)((fOuterRadius - fInnerRadius) / fPitch[1] + 0.5);
  }
  if (fType == 1) { // tilted version, should not be used any more
                    //     fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[1]));
    fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[1] + 0.5);
  }
  if (fType == 2) { // x y version
                    //     fNChannelsFront = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]))+
                    //                       4*(Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
                    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil(2.*fOuterRadius/fPitch[1]));
    fNChannelsFront = (Int_t)(2 * (fOuterRadius - fInnerRadius) / fPitch[0] + 4 * fInnerRadius / fPitch[0] + 0.5);
    fNChannelsBack = 2 * (Int_t)(2. * fOuterRadius / fPitch[1] + 0.5);
  }
  if (fType == 3) { // r_tree phi version
    Int_t ifac = 1;
    if (1 < fOuterRadius / fInnerRadius && fOuterRadius / fInnerRadius < 100) {
      // extract highest bit number
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      //       fNChannelsFront = ifac *(Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
      fNChannelsFront = ifac * (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    } else {
      cout << tempName.Data() << "-W- !!! type " << fType << " with OuterRadius/InnerRadius " << fOuterRadius / fInnerRadius << " is currently not supported !!!" << endl;
      cout << " Please use type 0 instead !!!" << endl;
      //       fNChannelsFront = (Int_t)(TMath::Ceil(2.*TMath::Pi()*fInnerRadius/fPitch[0]));
      fNChannelsFront = (Int_t)(2. * TMath::Pi() * fInnerRadius / fPitch[0] + 0.5);
    }
    //     fNChannelsBack  = 2*(Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[1]));
    fNChannelsBack = 2 * (Int_t)((fOuterRadius - fInnerRadius) / fPitch[1] + 0.5);
  }

  // cout << tempName.Data() << " type " << fType << " has " << fNChannelsFront << " front and " << fNChannelsBack << " back channels" << endl;

  fSigmaX = fSigmaY = fSigmaXY = 0.;
}
// -------------------------------------------------------------------------

// -----   Copy constructor  -----------------------------------------------
PndGemSensor::PndGemSensor(const PndGemSensor &tempSensor)
  : TNamed(tempSensor), fDetectorId(tempSensor.fDetectorId), fType(tempSensor.fType),
    // fPosition      {tempSensor.fPosition[0],
    //   tempSensor.fPosition[1],
    //   tempSensor.fPosition[2]},
    fRotation(tempSensor.fRotation), fInnerRadius(tempSensor.fInnerRadius), fOuterRadius(tempSensor.fOuterRadius), fD(tempSensor.fD),
    // fStripAngle[0] (tempSensor.fStripAngle[0]),
    // fStripAngle[1] (tempSensor.fStripAngle[1]),
    // fPitch[0]      (tempSensor.fPitch[0]),
    // fPitch[1]      (tempSensor.fPitch[1]),
    fNChannelsFront(tempSensor.fNChannelsFront), fNChannelsBack(tempSensor.fNChannelsBack), fSigmaX(tempSensor.fSigmaX), fSigmaY(tempSensor.fSigmaY), fSigmaXY(tempSensor.fSigmaXY)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemSensor::~PndGemSensor(){};
// -------------------------------------------------------------------------

// -----   Public method Reset   -------------------------------------------
void PndGemSensor::Reset() {}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemSensor::Print()
{
  cout << fName.Data() << ": Sensor #";
  cout.width(3);
  cout << GetSensorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fPosition[0] << ", ";
  cout.width(6);
  cout << fPosition[1] << ", ";
  cout.width(6);
  cout << fPosition[2] << ") cm, rotation ";
  cout.width(6);
  cout << fRotation * 180. / TMath::Pi() << " deg, radius from ";
  cout.width(6);
  cout << fInnerRadius << " to ";
  cout.width(6);
  cout << fOuterRadius << ", thickness ";
  cout.width(6);
  cout << fD << " cm, strip angle (";
  cout.width(6);
  cout << fStripAngle[0] << ", ";
  cout.width(6);
  cout << fStripAngle[1] << ") cm, pitch (";
  cout.width(6);
  cout << fPitch[0] << ", ";
  cout.width(6);
  cout << fPitch[1] << ") cm. ";
  cout << endl;

  Int_t tempSId = fDetectorId;
  Int_t bc = 0;
  cout << "SENSOR :                                          " << flush;
  while (tempSId > 0) {
    bc++;
    cout << "\b" << tempSId % 2 << "\b" << flush;
    if (bc == 27 || bc == 21 || bc == 8 || bc == 6 || bc == 5)
      cout << "\b|\b" << flush;
    tempSId = tempSId / 2;
  }
  cout << endl;
}
// -------------------------------------------------------------------------

// -----   Public method GetChannel   --------------------------------------
Int_t PndGemSensor::GetChannel(Double_t x, Double_t y, Int_t iSide)
{

  if (iSide != 0 && iSide != 1) {
    LOG(warn) << " PndGemSensor::GetChannel: Illegal side number " << iSide;
    return -1;
  }

  if (!Inside(x, y))
    return -1;
  Double_t radius = TMath::Sqrt(x * x + y * y);

  if (fType == 1) {
    cout << "do not use this type anymore" << endl;
    return -1;
  }
  if (fType == 0) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return (Int_t)(TMath::Ceil((hitPhi * fInnerRadius) / fPitch[iSide]));
    }
    if (iSide == 1) {
      if (x < 0.)
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1]));
      else
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1])) + fNChannelsBack / 2;
    }
  }
  if (fType == 2) {   // x y strips
    if (iSide == 0) { // x information encoded
                      //       Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
                      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      if (x <= -fInnerRadius)
        return (Int_t)((x + fOuterRadius) / fPitch[0]);
      if (x < 0. && y >= 0.)
        return nlStrips + (Int_t)((x + fInnerRadius) / fPitch[0]);
      if (x < 0. && y < 0.)
        return nlStrips + nsStrips + (Int_t)((x + fInnerRadius) / fPitch[0]);
      // now x can't be smaller than 0.
      if (x >= fInnerRadius)
        return nlStrips + 4 * nsStrips + (Int_t)((x - fInnerRadius) / fPitch[0]);
      if (y >= 0.)
        return nlStrips + 2 * nsStrips + (Int_t)((x) / fPitch[0]);
      if (y < 0.)
        return nlStrips + 3 * nsStrips + (Int_t)((x) / fPitch[0]);
    }
    if (iSide == 1) { // y information encoded
      if (x < 0.)
        return (Int_t)((y + fOuterRadius) / fPitch[1]);
      if (x >= 0.)
        return (Int_t)((y + fOuterRadius) / fPitch[1]) + fNChannelsBack / 2;
    }
  }
  if (fType == 3) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      // extract radial tree factor at the OuterRadius
      Int_t ifac = 1;
      // extract highest bit number
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      // extract radial tree factor at the radius
      Int_t ifac_r = 1;
      ifac_r = (Int_t)(radius / fInnerRadius);
      ifac_r |= (ifac_r >> 1);
      ifac_r |= (ifac_r >> 2);
      ifac_r |= (ifac_r >> 4);
      ifac_r -= (ifac_r >> 1);
      Int_t ich_step;
      if (ifac_r == 0)
        return -1;
      else
        ich_step = (Int_t)(ifac / ifac_r);
      return (Int_t)(ich_step * TMath::Ceil(hitPhi * fNChannelsFront / ich_step / 2. / TMath::Pi()));
    }
    if (iSide == 1) {
      if (x < 0.)
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1]));
      else
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1])) + fNChannelsBack / 2;
    }
  }
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method GetNeighbours   -----------------------------------
Int_t PndGemSensor::GetNeighbours(Int_t iSide, Int_t iChan, Int_t &nChan1, Int_t &nChan2, Int_t &nChan3)
{
  nChan1 = -1;
  nChan2 = -1;
  nChan3 = -1;
  if (fType == 0) {
    nChan1 = iChan - 1;
    nChan2 = iChan + 1;
    return 2;
  }
  if (fType == 1) {
    nChan1 = iChan - 1;
    nChan2 = iChan + 1;
    return 2;
  }
  if (fType == 2) {
    if (iSide == 1) {
      nChan1 = iChan - 1;
      nChan2 = iChan + 1;
      return 2;
    }
    if (iSide == 0) {
      nChan1 = iChan - 1;
      nChan2 = iChan + 1;
      return 2;
    }
  }
  if (fType == 3) {
    if (iSide == 0) { // radial tree
      // need to decide what to return...(?)
      nChan1 = iChan - 1;
      nChan2 = iChan + 1;
      return 2;
    }
    if (iSide == 1) {
      nChan1 = iChan - 1;
      nChan2 = iChan + 1;
    }
  }
  return 0;
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorPart   --------------------------------------
Int_t PndGemSensor::GetSensorPart(Int_t iSide, Int_t chan)
{
  if (fType == 0) {
    return -1;
  }
  if (fType == 1) {
    return -1;
  }
  if (fType == 2) {
    if (iSide == 1) {
      return -1;
    }
    if (iSide == 0) {
      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      // if      ( TMath::Abs(chan-fNChannelsFront/2) > nsStrips*2 )
      if (TMath::Abs(chan - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        return 0;
      // else if ( TMath::Abs(chan-fNChannelsFront/2) < nsStrips   )
      else if (TMath::Abs(chan - fNChannelsFront / 2 + 0.5) < nsStrips)
        return 2;
      else
        return 1;
    }
  }
  if (fType == 3) {
    // should it be irregular??
    return -1;
  }
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method GetStripOrientation   --------------------------------------
Double_t PndGemSensor::GetStripOrientation(Double_t x, Double_t y, Int_t iSide)
{
  if (iSide != 0 && iSide != 1)
    return -1.;
  if (!Inside(x, y))
    return -1.;

  if (fType == 1) {
    return -1.;
  }
  if (fType == 0) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      Double_t radius = TMath::Sqrt(x * x + y * y);
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return hitPhi;
    }
    if (iSide == 1) {
      Double_t radius = TMath::Sqrt(x * x + y * y);
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return hitPhi + TMath::Pi() / 2.;
    }
  }
  if (fType == 2) {   // x y strips
    if (iSide == 0) { // x information encoded
      return 0.;
    }
    if (iSide == 1) { // y information encoded
      return TMath::Pi() / 2.;
    }
  }
  if (fType == 3) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      Double_t radius = TMath::Sqrt(x * x + y * y);
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return hitPhi;
    }
    if (iSide == 1) {
      Double_t radius = TMath::Sqrt(x * x + y * y);
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return hitPhi + TMath::Pi() / 2.;
    }
  }
  return -1.;
}
// -------------------------------------------------------------------------

// -----   Public method GetDistance   --------------------------------------
Double_t PndGemSensor::GetDistance(Int_t iSide, Double_t chan1, Double_t chan2)
{
  if (chan1 == -1 || chan2 == -1) {
    // cout<<"-W- !!! GetDistance(): channel number is -1 !!"<<endl;
    return -1.;
  }
  if (iSide == 0 && (chan1 - fNChannelsFront / 2. - 1) * (chan2 - fNChannelsFront / 2.) < 0.)
    return -1.;
  if (iSide == 1 && (chan1 - fNChannelsBack / 2. - 1) * (chan2 - fNChannelsBack / 2.) < 0.)
    return -1.;
  if (fType == 0) {
    return TMath::Abs(chan1 - chan2);
  }
  if (fType == 1) {
    return TMath::Abs(chan1 - chan2);
  }
  if (fType == 2) {
    if (iSide == 0) {
      // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      // Int_t nlStrips = (Int_t)((fOuterRadius-fInnerRadius)/fPitch[0] + 0.5); //[R.K. 01/2017] unused variable?
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      Int_t part1 = 666;
      Int_t part2 = 666;
      // if      ( TMath::Abs(chan1-fNChannelsFront/2) >= nsStrips*2 ) part1 =  0;
      if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part1 = 0;
      // else if ( TMath::Abs(chan1-fNChannelsFront/2) <  nsStrips   ) {
      else if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part1 = -1;
        if (chan1 < fNChannelsFront / 2)
          chan1 = chan1 - nsStrips;
        else
          chan1 = chan1 + nsStrips;
      } else
        part1 = 1;
      // if      ( TMath::Abs(chan2-fNChannelsFront/2) >= nsStrips*2 ) part2 =  0;
      if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part2 = 0;
      // else if ( TMath::Abs(chan2-fNChannelsFront/2) <  nsStrips   ) {
      else if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part2 = -1;
        if (chan2 < fNChannelsFront / 2)
          chan2 = chan2 - nsStrips;
        else
          chan2 = chan2 + nsStrips;
      } else
        part2 = 1;

      if (part1 * part2 == -1)
        return -1;
      return TMath::Abs(chan1 - chan2);
    }
    if (iSide == 1) {
      return TMath::Abs(chan1 - chan2);
    }
  }
  if (fType == 3) {
    if (iSide == 0) {
      // radial tree
      // it returns distance assuming the inner ring as possible minimum situation.
      Int_t ifac = 1;
      Double_t retval;
      // extract channel step at the inner radius as radial tree factor
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      retval = (Double_t)TMath::Abs((Int_t)(chan1 / ifac) - (Int_t)(chan2 / ifac));

      // cout << "-I- PndGemSensor::GetDistance("<<iSide<<","<<chan1<<","<<chan2<<") returns " << retval <<endl;

      return retval;
    }
    if (iSide == 1) {
      return TMath::Abs(chan1 - chan2);
    }
  }
  return -1; // ?ADDED (Stefano)
}
// -------------------------------------------------------------------------

// -----   Public method GetDistance   -------------------------------------
Int_t PndGemSensor::GetDistance(Int_t iSide, Int_t chanMin, Int_t chanMax, Int_t chanTest)
{
  if (chanMin == -1 || chanMax == -1 || chanTest == -1) {
    // cout<<"-W- !!! GetDistance(): channel number is -1 !!"<<endl;
    return -1.;
  }
  if (iSide == 0 && (chanMin - fNChannelsFront / 2) * (chanTest - fNChannelsFront / 2) < 0.)
    return -1;
  if (iSide == 1 && (chanMin - fNChannelsBack / 2) * (chanTest - fNChannelsBack / 2) < 0.)
    return -1;
  if (fType == 0) {
    if (chanTest > chanMin && chanTest < chanMax)
      return 0;
    return TMath::Min(TMath::Abs(chanTest - chanMax), TMath::Abs(chanTest - chanMin));
  }
  if (fType == 1) {
    if (chanTest > chanMin && chanTest < chanMax)
      return 0;
    return TMath::Min(TMath::Abs(chanTest - chanMax), TMath::Abs(chanTest - chanMin));
  }
  if (fType == 2) {
    if (iSide == 0) {
      // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      // Int_t nlStrips = (Int_t)((fOuterRadius-fInnerRadius)/fPitch[0] + 0.5); //[R.K. 01/2017] unused variable?
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      // THIS BROKEN VERTICAL STRIPS NEED SPECIAL ATTENTION
      // DO NOT YET CONSIDER ALL THE PROBLEMS...
      // if      ( TMath::Abs(chanMin-fNChannelsFront/2) < nsStrips ) {
      if (TMath::Abs(chanMin - fNChannelsFront / 2 + 0.5) < nsStrips) {
        //	cout << "chanMin " << chanMin << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
        if (chanMin < fNChannelsFront / 2)
          chanMin -= nsStrips;
        else
          chanMin += nsStrips;
        //	cout << " -----> " << chanMin << " ( "  << chanMax << " , " << chanTest << " )" << endl;
      }
      // if      ( TMath::Abs(chanMax-fNChannelsFront/2) < nsStrips ) {
      if (TMath::Abs(chanMax - fNChannelsFront / 2 + 0.5) < nsStrips) {
        //	cout << "chanMax " << chanMax << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
        if (chanMax < fNChannelsFront / 2)
          chanMax -= nsStrips;
        else
          chanMax += nsStrips;
        //	cout << " -----> " << chanMax << " ( "  << chanMin << " , " << chanTest << " )" << endl;
      }
      // if      ( TMath::Abs(chanTest-fNChannelsFront/2) < nsStrips ) {
      if (TMath::Abs(chanTest - fNChannelsFront / 2 + 0.5) < nsStrips) {
        //	cout << "chanTest " << chanTest << " is in bad position, consider " << fNChannelsFront << "/" << nlStrips << "/" << nsStrips << flush;
        if (chanTest < fNChannelsFront / 2)
          chanTest -= nsStrips;
        else
          chanTest += nsStrips;
        //	cout << " -----> " << chanTest << " ( "  << chanMin << " , " << chanMax << " )" << endl;
      }
      if (chanTest > chanMin && chanTest < chanMax)
        return 0;
      return TMath::Min(TMath::Abs(chanTest - chanMax), TMath::Abs(chanTest - chanMin));
    }
    if (iSide == 1) {
      if (chanTest > chanMin && chanTest < chanMax)
        return 0;
      return TMath::Min(TMath::Abs(chanTest - chanMax), TMath::Abs(chanTest - chanMin));
    }
  }
  if (fType == 3) {
    if (iSide == 0) {
      // radial tree
      if (chanTest > chanMin && chanTest < chanMax)
        return 0;
      // it returns distance assuming the inner ring as possible minimum situation.
      Int_t ifac = 1;
      Double_t disMin, disMax, retval;
      // extract channel step at the inner radius as radial tree factor
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      disMin = (Double_t)TMath::Abs((Int_t)(chanTest / ifac) - (Int_t)(chanMin / ifac));
      disMax = (Double_t)TMath::Abs((Int_t)(chanTest / ifac) - (Int_t)(chanMax / ifac));
      retval = TMath::Min(disMin, disMax);
      // cout << "-I- PndGemSensor::GetDistance("<<iSide<<","<<chanMin<<","<<chanMax<<","<<chanTest<<") returns " << retval <<endl;

      return retval;
    }
    if (iSide == 1) {
      if (chanTest > chanMin && chanTest < chanMax)
        return 0;
      return TMath::Min(TMath::Abs(chanTest - chanMax), TMath::Abs(chanTest - chanMin));
    }
  }
  return -1; // ? ADDED (Stefano)
}
// -------------------------------------------------------------------------

// -----   Public method   -------------------------------------------------
// Similar to GetChannel but for comparison of Digi and Digi.
// Type 3 considers r-info of r-strips in this method.
Double_t PndGemSensor::GetDistance2(Int_t iSide, Double_t chan1, Double_t chan2)
{
  if (chan1 == -1 || chan2 == -1)
    return -1.;
  if (iSide == 0 && (chan1 - fNChannelsFront / 2.) * (chan2 - fNChannelsFront / 2.) < 0.)
    return -1.;
  if (iSide == 1 && (chan1 - fNChannelsBack / 2.) * (chan2 - fNChannelsBack / 2.) < 0.)
    return -1.;
  if (fType == 0) {
    return TMath::Abs(chan1 - chan2);
  }
  if (fType == 1) {
    return TMath::Abs(chan1 - chan2);
  }
  if (fType == 2) {
    if (iSide == 0) {
      //       Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      // Int_t nlStrips = (Int_t)((fOuterRadius-fInnerRadius)/fPitch[0] + 0.5); //[R.K. 01/2017] unused variable?
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      Int_t part1 = 666;
      Int_t part2 = 666;
      // if      ( TMath::Abs(chan1-fNChannelsFront/2) > nsStrips*2 ) part1 =  0;
      if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part1 = 0;
      // else if ( TMath::Abs(chan1-fNChannelsFront/2) < nsStrips   ) {
      else if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part1 = -1;
        if (chan1 < fNChannelsFront / 2)
          chan1 = chan1 - nsStrips;
        else
          chan1 = chan1 + nsStrips;
      } else
        part1 = 1;
      // if      ( TMath::Abs(chan2-fNChannelsFront/2) > nsStrips*2 ) part2 =  0;
      if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part2 = 0;
      // else if ( TMath::Abs(chan2-fNChannelsFront/2) < nsStrips   ) {
      else if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part2 = -1;
        if (chan2 < fNChannelsFront / 2)
          chan2 = chan2 - nsStrips;
        else
          chan2 = chan2 + nsStrips;
      } else
        part2 = 1;

      if (part1 * part2 == -1)
        return -1;
      return TMath::Abs(chan1 - chan2);
    }
    if (iSide == 1) {
      return TMath::Abs(chan1 - chan2);
    }
  }
  // radial tree
  if (fType == 3) {
    if (iSide == 0) {
      Int_t ifac = 1, ifac_test = 1, ifac1 = 1, ifac2 = 1, icom_ifac = 0;
      Int_t ideno, iring_test = 0, iring1 = 0, iring2 = 0; //,icom_ring=0; //[R.K. 01/2017] unused variable?
      Int_t itest1, itest2, i1ok = 0, i2ok = 0;
      Int_t icom_chstep = 0;
      Double_t retval;
      // extract the radial tree factor of outer radius
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      // find minimum ring for ch1 and ch2
      itest1 = (Int_t)chan1;
      itest2 = (Int_t)chan2;
      for (ideno = ifac; ideno >= 1; ideno /= 2) {
        iring_test++;
        // cout << "ideno="<< ideno <<" iring_test="<<iring_test<<" ifac_test="<<ifac_test<<endl;
        itest1 %= ideno;
        if (itest1 == 0 && i1ok == 0) {
          // cout <<"Minimum ring of ch1 found at #"<<iring_test<<endl;
          iring1 = iring_test;
          ifac1 = ifac_test;
          i1ok = 1;
        }
        itest2 %= ideno;
        if (itest2 == 0 && i2ok == 0) {
          // cout <<"Minimum ring of ch2 found at #"<<iring_test<<endl;
          iring2 = iring_test;
          ifac2 = ifac_test;
          i2ok = 1;
        }
        ifac_test *= 2;
      }
      // take minimum common ring
      if (iring1 >= iring2) {
        // icom_ring=iring1; //[R.K. 01/2017] unused variable?
        icom_ifac = ifac1;
      } else {
        // icom_ring=iring2; //[R.K. 01/2017] unused variable?
        icom_ifac = ifac2;
      }
      // channel step at the common ring
      if (ifac % icom_ifac == 0) {
        icom_chstep = ifac / icom_ifac;
      } else {
        LOG(warn) << " !!! something wrong on ifac..in PndGemSensor::GetDistance2() !!!";
      }
      retval = TMath::Abs(chan1 - chan2) / (Double_t)icom_chstep;

      // cout << "-I- PndGemSensor::GetDistance2("<<iSide<<","<<chan1<<","<<chan2<<") returns "<< retval <<" for Common ring #"<<icom_ring<<" ifac="<<icom_ifac<<"
      // ch_step="<<icom_chstep<<endl;

      return retval;
    }
    if (iSide == 1) {
      return TMath::Abs(chan1 - chan2);
    }
  }
  return -1; // ? ADDED (Stefano)
}
// -------------------------------------------------------------------------

// -----   Public method GetMeanChannel   --------------------------------------
Double_t PndGemSensor::GetMeanChannel(Int_t iSide, Double_t chan1, Double_t weight1, Double_t chan2, Double_t weight2)
{
  if (chan1 == -1 || chan2 == -1)
    return -1.;
  if (iSide == 0 && (chan1 - fNChannelsFront / 2.) * (chan2 - fNChannelsFront / 2.) < 0.)
    return -1.;
  if (iSide == 1 && (chan1 - fNChannelsBack / 2.) * (chan2 - fNChannelsBack / 2.) < 0.)
    return -1.;
  if (fType == 0) {
    return (chan1 * weight1 + chan2 * weight2) / (weight1 + weight2);
  }
  if (fType == 1) {
    return (chan1 * weight1 + chan2 * weight2) / (weight1 + weight2);
  }
  if (fType == 2) {
    if (iSide == 0) {
      // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      //       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      Int_t part1 = 666;
      Int_t part2 = 666;
      // if      ( TMath::Abs(chan1-fNChannelsFront/2) >= nsStrips*2 ) part1 =  0;
      if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part1 = 0;
      // else if ( TMath::Abs(chan1-fNChannelsFront/2) <  nsStrips   ) {
      else if (TMath::Abs(chan1 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part1 = -1;
        if (chan1 < fNChannelsFront / 2)
          chan1 = chan1 - nsStrips;
        else
          chan1 = chan1 + nsStrips;
      } else
        part1 = 1;
      // if      ( TMath::Abs(chan2-fNChannelsFront/2) >= nsStrips*2 ) part2 =  0;
      if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) > nsStrips * 2)
        part2 = 0;
      // else if ( TMath::Abs(chan2-fNChannelsFront/2) <  nsStrips   ) {
      else if (TMath::Abs(chan2 - fNChannelsFront / 2 + 0.5) < nsStrips) {
        part2 = -1;
        if (chan2 < fNChannelsFront / 2)
          chan2 = chan2 - nsStrips;
        else
          chan2 = chan2 + nsStrips;
      } else
        part2 = 1;

      if (part1 * part2 == -1)
        return -1;

      Double_t meanCh = (chan1 * weight1 + chan2 * weight2) / (weight1 + weight2);

      if (part1 + part2 < 0) {
        if (meanCh > nlStrips && meanCh < nlStrips + nsStrips)
          meanCh += nsStrips;
        if (meanCh > fNChannelsFront / 2 + nsStrips && meanCh < fNChannelsFront - nlStrips)
          meanCh -= nsStrips;
      }

      return meanCh;
    }
    if (iSide == 1) {
      return (chan1 * weight1 + chan2 * weight2) / (weight1 + weight2);
    }
  }
  if (fType == 3) {
    return (chan1 * weight1 + chan2 * weight2) / (weight1 + weight2);
  }
  return -1; // ? ADDED (Stefano)
}
// -------------------------------------------------------------------------

// -----   Public method GetChannel2   --------------------------------------
Int_t PndGemSensor::GetChannel2(Double_t x, Double_t y, Int_t iSide, Double_t &feeDist)
{
  feeDist = -1.;

  if (iSide != 0 && iSide != 1) {
    LOG(warn) << " PndGemSensor::GetChannel: Illegal side number " << iSide;
    return -1;
  }

  if (!Inside(x, y))
    return -1;
  Double_t radius = TMath::Sqrt(x * x + y * y);

  if (fType == 1) {
    LOG(error) << " !!! do not use this type anymore";
    return -1;
  }
  if (fType == 0) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      feeDist = fOuterRadius - radius;

      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      return (Int_t)(TMath::Ceil((hitPhi * fInnerRadius) / fPitch[iSide]));
    }
    if (iSide == 1) {
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      //      feeDist = hitPhi;
      if (hitPhi < TMath::Pi() / 2.)
        feeDist = hitPhi * radius;
      else if (hitPhi < TMath::Pi())
        feeDist = (TMath::Pi() - hitPhi) * radius;
      else if (hitPhi < 3. * TMath::Pi() / 2.)
        feeDist = (hitPhi - TMath::Pi()) * radius;
      else
        feeDist = (2. * TMath::Pi() - hitPhi) * radius;
      feeDist -= fStripAngle[1] / 2.; // that's the width of middle bar

      if (x < 0.)
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1]));
      else
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1])) + fNChannelsBack / 2;
    }
  }
  if (fType == 2) {   // x y strips
    if (iSide == 0) { // x information encoded
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      Double_t feeY = TMath::Cos(hitPhi) * fOuterRadius;
      feeDist = TMath::Abs(feeY) - TMath::Abs(y);

      // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
      // Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
      Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
      Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
      if (x <= -fInnerRadius)
        return (Int_t)((x + fOuterRadius) / fPitch[0]);
      if (x < 0. && y >= 0.)
        return (Int_t)(nlStrips + (x + fInnerRadius) / fPitch[0]);
      if (x < 0. && y < 0.)
        return (Int_t)(nlStrips + nsStrips + (x + fInnerRadius) / fPitch[0]);
      // now x can't be smaller than 0.
      if (x >= fInnerRadius)
        return (Int_t)(nlStrips + 4 * nsStrips + (x - fInnerRadius) / fPitch[0]);
      if (y >= 0.)
        return (Int_t)(nlStrips + 2 * nsStrips + (x) / fPitch[0]);
      if (y < 0.)
        return (Int_t)(nlStrips + 3 * nsStrips + (x) / fPitch[0]);
    }
    if (iSide == 1) { // y information encoded
      feeDist = TMath::Abs(x) - fStripAngle[1] / 2.;

      if (x < 0.)
        return (Int_t)((y + fOuterRadius) / fPitch[1]);
      if (x >= 0.)
        return (Int_t)((y + fOuterRadius) / fPitch[1] + fNChannelsBack / 2);
    }
  }
  if (fType == 3) { // angle info for iSide 0, radius info for iSide 1
    if (iSide == 0) {
      feeDist = fOuterRadius - radius;

      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      // extract channel factor at the Outer radius
      Int_t ifac = 1;
      ifac = (Int_t)(fOuterRadius / fInnerRadius);
      ifac |= (ifac >> 1);
      ifac |= (ifac >> 2);
      ifac |= (ifac >> 4);
      ifac -= (ifac >> 1);
      // extract channel factor at the hit radius
      Int_t ifac_r = 1;
      ifac_r = (Int_t)(radius / fInnerRadius);
      ifac_r |= (ifac_r >> 1);
      ifac_r |= (ifac_r >> 2);
      ifac_r |= (ifac_r >> 4);
      ifac_r -= (ifac_r >> 1);
      // calculate channel step at the radius
      Int_t ich_step;
      if (ifac_r == 0)
        return -1;
      else
        ich_step = (Int_t)(ifac / ifac_r);
      // return channel number exists at the radius (use ceil as type0)
      return (Int_t)(ich_step * TMath::Ceil(hitPhi * fNChannelsFront / ich_step / 2. / TMath::Pi()));
    }
    if (iSide == 1) {
      Double_t hitPhi = TMath::ACos(y / radius);
      if (x < 0.)
        hitPhi = 2. * TMath::Pi() - hitPhi;
      hitPhi = 2. * TMath::Pi() - hitPhi;
      if (hitPhi < TMath::Pi() / 2.)
        feeDist = hitPhi * radius;
      else if (hitPhi < TMath::Pi())
        feeDist = (TMath::Pi() - hitPhi) * radius;
      else if (hitPhi < 3. * TMath::Pi() / 2.)
        feeDist = (hitPhi - TMath::Pi()) * radius;
      else
        feeDist = (2. * TMath::Pi() - hitPhi) * radius;
      feeDist -= fStripAngle[1] / 2.; // that's the width of middle bar
      if (x < 0.)
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1]));
      else
        return (Int_t)(TMath::Ceil((radius - fInnerRadius) / fPitch[1])) + fNChannelsBack / 2;
    }
  }
  return -1.;
}

// -------------------------------------------------------------------------
// COMMENTED to avoid confusion with Int_t GetChannel()
//
// -------------------------------------------------------------------------
// -----   Public method GetChannel   --------------------------------------
// Double_t PndGemSensor::GetChannel(Double_t x, Double_t y, Int_t iSide, Double_t& stripWidth) {
//   stripWidth = fPitch[iSide];

//   if (iSide !=0 && iSide != 1) {
//     cout << "-W- PndGemSensor::GetChannel: Illegal side number "
// 	 << iSide << endl;
//     return -1;
//   }

//   if ( !Inside(x,y) ) return -1;
//   Double_t radius = TMath::Sqrt(x*x+y*y);

//   if ( fType == 1 ) {
//     cout << "do not use this type anymore" << endl;
//     return -1;
//   }
//   if ( fType == 0 ) { // angle info for iSide 0, radius info for iSide 1
//     if ( iSide == 0 ) {
//       stripWidth = stripWidth*radius/fInnerRadius;
//       Double_t hitPhi = TMath::ACos(y/radius);
//       if ( x < 0. )
// 	hitPhi = 2.*TMath::Pi()-hitPhi;
//       hitPhi = 2.*TMath::Pi()-hitPhi;
//       return (hitPhi*fInnerRadius)/fPitch[iSide];
//     }
//     if ( iSide == 1 ) {
//       if ( x < 0. )
// 	return (radius-fInnerRadius)/fPitch[1];
//       else
// 	return (radius-fInnerRadius)/fPitch[1] + fNChannelsBack/2;
//     }
//   }
//   if ( fType == 2 ) { // x y strips
//     if ( iSide == 0 ) { // x information encoded
//       Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
//       Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
//       if ( x <= -fInnerRadius )
// 	return (x+fOuterRadius)/fPitch[0];
//       if ( x < 0. && y >= 0. )
// 	return nlStrips           +(x+fInnerRadius)/fPitch[0];
//       if ( x < 0. && y < 0. )
// 	return nlStrips+  nsStrips+(x+fInnerRadius)/fPitch[0];
//       // now x can't be smaller than 0.
//       if ( x >= fInnerRadius )
// 	return nlStrips+4*nsStrips+(x-fInnerRadius)/fPitch[0];
//       if ( y >= 0. )
// 	return nlStrips+2*nsStrips+(x)/fPitch[0];
//       if ( y <  0. )
// 	return nlStrips+3*nsStrips+(x)/fPitch[0];
//     }
//     if ( iSide == 1 ) { // y information encoded
//       if ( x <  0. )
// 	return (y+fOuterRadius)/fPitch[1];
//       if ( x >= 0. )
// 	return (y+fOuterRadius)/fPitch[1]+fNChannelsBack/2;
//     }
//   }
//   return -1.;
// }
// -------------------------------------------------------------------------

// -----   Public method Inside   ------------------------------------------
Bool_t PndGemSensor::Inside(Double_t x, Double_t y)
{
  if (TMath::Abs(x) < fStripAngle[1] / 2.)
    return kFALSE;
  Double_t radSq = x * x + y * y;
  if (radSq < fInnerRadius * fInnerRadius)
    return kFALSE;
  if (radSq > fOuterRadius * fOuterRadius)
    return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Inside   ------------------------------------------
Bool_t PndGemSensor::Inside(Double_t radius)
{
  if (radius < fInnerRadius)
    return kFALSE;
  if (radius > fOuterRadius)
    return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross)
{
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;

  if (fType == -1) {
    LOG(error) << " !!! not supported anymore";
    return -1;
  }
  // the hits are on different sides
  if (iFStrip < fNChannelsFront / 2 && iBStrip >= fNChannelsBack / 2)
    return -1;
  if (iFStrip >= fNChannelsFront / 2 && iBStrip < fNChannelsBack / 2)
    return -1;

  Double_t bs = iBStrip;
  if (bs >= fNChannelsBack / 2)
    bs -= fNChannelsBack / 2;
  if (fType == 0) { // r phi strips
    Double_t phi = fPitch[0] * ((Double_t)iFStrip - 0.5) / fInnerRadius;
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];
    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }
  if (fType == 2) { // x y strips
    // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    // Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
    Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
    // Double_t x = -666.; //[R.K. 01/2017] unused variable?
    yCross = -fOuterRadius + (Double_t(bs) + 0.5) * fPitch[1];
    zCross = fPosition[2];
    if (iFStrip < nlStrips) {
      xCross = -fOuterRadius + (Double_t(iFStrip) + 0.5) * fPitch[0];
      if (!Inside(xCross, yCross))
        return -1;
      return fDetectorId;
    }
    if (iFStrip < nlStrips + nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = -fOuterRadius + (Double_t(iFStrip) + 0.5) * fPitch[0];
      if (!Inside(xCross, yCross))
        return -1;
      return fDetectorId;
    }
    if (iFStrip < nlStrips + 2 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = -fOuterRadius + (Double_t(iFStrip - nsStrips) + 0.5) * fPitch[0];
      if (!Inside(xCross, yCross))
        return -1;
      return fDetectorId;
    }
    if (iFStrip < nlStrips + 3 * nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = -fOuterRadius + (Double_t(iFStrip - nsStrips) + 0.5) * fPitch[0];
      if (!Inside(xCross, yCross))
        return -1;
      return fDetectorId;
    }
    if (iFStrip < nlStrips + 4 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = -fOuterRadius + (Double_t(iFStrip - 3 * nsStrips) + 0.5) * fPitch[0];
      if (!Inside(xCross, yCross))
        return -1;
      return fDetectorId;
    }
  }
  if (fType == 3) {
    Double_t phi = 2. * TMath::Pi() * ((Double_t)iFStrip - 0.5) / (Double_t)fNChannelsFront;
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];
    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }
  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross, Double_t &dr, Double_t &dp)
{
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;

  if (fType == -1) {
    LOG(error) << " !!! not supported anymore";
    return -1;
  }
  // the hits are on different sides
  // cout << iFStrip << " of " << fNChannelsFront << " and " << iBStrip << " of " << fNChannelsBack << endl;
  if (iFStrip < fNChannelsFront / 2 && iBStrip >= fNChannelsBack / 2)
    return -1;
  if (iFStrip >= fNChannelsFront / 2 && iBStrip < fNChannelsBack / 2)
    return -1;
  Double_t bs = iBStrip;
  if (bs >= fNChannelsBack / 2)
    bs -= fNChannelsBack / 2;
  if (fType == 0) { // r phi strips
    Double_t phi = fPitch[0] * ((Double_t)iFStrip - 0.5) / fInnerRadius;
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];

    dp = fPitch[0] * radius / fInnerRadius / TMath::Sqrt(12.);
    dr = fPitch[1] / TMath::Sqrt(12.);

    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }
  if (fType == 2) { // x y strips
    // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    //     Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
    Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
    // Double_t x = -666.; //[R.K. 01/2017] unused variable?
    yCross = -fOuterRadius + (Double_t(bs) + 0.5) * fPitch[1];
    zCross = fPosition[2];

    if (iFStrip < nlStrips)
      xCross = -fOuterRadius + (Double_t(iFStrip) + 0.5) * fPitch[0];
    else if (iFStrip < nlStrips + nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 1 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 2 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 2 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 3 * nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 2 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 4 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 3 * nsStrips) + 0.5) * fPitch[0];
    } else
      xCross = (Double_t(iFStrip - nlStrips - 3 * nsStrips) + 0.5) * fPitch[0];

    if (!Inside(xCross, yCross))
      return -1;

    /*
      Double_t rMax = TMath::Sqrt( (TMath::Abs(xCross)+fPitch[0])*(TMath::Abs(xCross)+fPitch[0])+
      (TMath::Abs(yCross)+fPitch[1])*(TMath::Abs(yCross)+fPitch[1]) );
      Double_t rMin = TMath::Sqrt( (TMath::Abs(xCross)-fPitch[0])*(TMath::Abs(xCross)-fPitch[0])+
      (TMath::Abs(yCross)-fPitch[1])*(TMath::Abs(yCross)-fPitch[1]) );
`      Double_t phi1 = TMath::ATan( (TMath::Abs(yCross)-fPitch[1])/(TMath::Abs(xCross)+fPitch[0]) );
      Double_t phi2 = TMath::ATan( (TMath::Abs(yCross)+fPitch[1])/(TMath::Abs(xCross)-fPitch[0]) );
      dp = TMath::Abs(phi1-phi2);
      dp = TMath::Tan(dp)*(rMax+rMin)/2./TMath::Sqrt(12.);
      dr = (rMax-rMin)/TMath::Sqrt(12.);
      //     if ( dp > 350 )
      //       dp = TMath::Abs(360.-dp);
      */
    dr = fPitch[0] / TMath::Sqrt(12.);
    dp = fPitch[1] / TMath::Sqrt(12.);

    return fDetectorId;
  }
  if (fType == 3) { // r_a phi strips
    Double_t phi = 2. * TMath::Pi() * ((Double_t)iFStrip - 0.5) / (Double_t)fNChannelsFront;
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];
    // extract channel factor at the hit radius
    Int_t ifac_r = 1;
    ifac_r = (Int_t)(radius / fInnerRadius);
    ifac_r |= (ifac_r >> 1);
    ifac_r |= (ifac_r >> 2);
    ifac_r |= (ifac_r >> 4);
    ifac_r -= (ifac_r >> 1);
    dp = fPitch[0] * radius / fInnerRadius / ifac_r / TMath::Sqrt(12.);
    dr = fPitch[1] / TMath::Sqrt(12.);

    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }

  return -1;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t PndGemSensor::Intersect(Double_t iFStrip, Double_t iBStrip, Double_t &xCross, Double_t &yCross, Double_t &zCross, Double_t &dx, Double_t &dy, Double_t &dr, Double_t &dp)
{
  //  cout << "trying to find intersection of strip " << iFStrip << " and " << iBStrip << endl;
  if (fType == -1) {
    LOG(error) << " !!! not supported anymore";
    return -1;
  }
  // the hits are on different sides
  // cout << iFStrip << " of " << fNChannelsFront << " and " << iBStrip << " of " << fNChannelsBack << endl;
  if (iFStrip < fNChannelsFront / 2 && iBStrip >= fNChannelsBack / 2)
    return -1;
  if (iFStrip >= fNChannelsFront / 2 && iBStrip < fNChannelsBack / 2)
    return -1;
  Double_t bs = iBStrip;
  if (bs >= fNChannelsBack / 2)
    bs -= fNChannelsBack / 2;
  if (fType == 0) {                                                      // r phi strips
    Double_t phi = fPitch[0] * ((Double_t)iFStrip - 0.5) / fInnerRadius; // the angle is counted from Y axis
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];

    dp = fPitch[0] * radius / fInnerRadius / TMath::Sqrt(12.);
    dr = fPitch[1] / TMath::Sqrt(12.);
    dx = dr * TMath::Abs(TMath::Sin(phi)) + dp * TMath::Abs(TMath::Cos(phi));
    dy = dr * TMath::Abs(TMath::Cos(phi)) + dp * TMath::Abs(TMath::Sin(phi));

    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }
  if (fType == 2) { // x y strips
    // Int_t nlStrips = (Int_t)(TMath::Ceil((fOuterRadius-fInnerRadius)/fPitch[0]));
    // Int_t nsStrips = (Int_t)(TMath::Ceil(fInnerRadius/fPitch[0]));
    Int_t nlStrips = (Int_t)((fOuterRadius - fInnerRadius) / fPitch[0] + 0.5);
    Int_t nsStrips = (Int_t)(fInnerRadius / fPitch[0] + 0.5);
    // cout << " nlStrips="<<nlStrips<<" nsStrips="<<nsStrips<<endl;
    // Double_t x = -666.; //[R.K. 01/2017] unused variable?
    yCross = -fOuterRadius + (Double_t(bs) + 0.5) * fPitch[1];
    zCross = fPosition[2];

    if (iFStrip < nlStrips)
      xCross = -fOuterRadius + (Double_t(iFStrip) + 0.5) * fPitch[0];
    else if (iFStrip < nlStrips + nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 1 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 2 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 2 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 3 * nsStrips) {
      if (bs < fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 2 * nsStrips) + 0.5) * fPitch[0];
    } else if (iFStrip < nlStrips + 4 * nsStrips) {
      if (bs >= fNChannelsBack / 4)
        return -1;
      xCross = (Double_t(iFStrip - nlStrips - 3 * nsStrips) + 0.5) * fPitch[0];
    } else
      xCross = (Double_t(iFStrip - nlStrips - 3 * nsStrips) + 0.5) * fPitch[0];

    if (!Inside(xCross, yCross))
      return -1;

    /*
      Double_t rMax = TMath::Sqrt( (TMath::Abs(xCross)+fPitch[0])*(TMath::Abs(xCross)+fPitch[0])+
      (TMath::Abs(yCross)+fPitch[1])*(TMath::Abs(yCross)+fPitch[1]) );
      Double_t rMin = TMath::Sqrt( (TMath::Abs(xCross)-fPitch[0])*(TMath::Abs(xCross)-fPitch[0])+
      (TMath::Abs(yCross)-fPitch[1])*(TMath::Abs(yCross)-fPitch[1]) );
`      Double_t phi1 = TMath::ATan( (TMath::Abs(yCross)-fPitch[1])/(TMath::Abs(xCross)+fPitch[0]) );
      Double_t phi2 = TMath::ATan( (TMath::Abs(yCross)+fPitch[1])/(TMath::Abs(xCross)-fPitch[0]) );
      dp = TMath::Abs(phi1-phi2);
      dp = TMath::Tan(dp)*(rMax+rMin)/2./TMath::Sqrt(12.);
      dr = (rMax-rMin)/TMath::Sqrt(12.);
      //     if ( dp > 350 )
      //       dp = TMath::Abs(360.-dp);
      */
    dx = fPitch[0] / TMath::Sqrt(12.);
    dy = fPitch[1] / TMath::Sqrt(12.);
    dr = 0.;
    dp = 0.;

    return fDetectorId;
  }
  if (fType == 3) { // r_a phi strips
    Double_t radius = fPitch[1] * ((Double_t)bs - 0.5) + fInnerRadius;
    // extract channel factor at the Outer radius
    Int_t ifac = 1;
    ifac = (Int_t)(fOuterRadius / fInnerRadius);
    ifac |= (ifac >> 1);
    ifac |= (ifac >> 2);
    ifac |= (ifac >> 4);
    ifac -= (ifac >> 1);
    // extract channel factor at the hit radius
    Int_t ifac_r = 1;
    ifac_r = (Int_t)(radius / fInnerRadius);
    if (ifac_r == 0) {
      cout << "-W- PndGemSensor::Intersect() radius (" << radius << ") in smaller than InnerRadius (" << fInnerRadius << ").. return -1! (strip " << iFStrip << " / " << iBStrip
           << " )" << endl;
      return -1;
    }
    ifac_r |= (ifac_r >> 1);
    ifac_r |= (ifac_r >> 2);
    ifac_r |= (ifac_r >> 4);
    ifac_r -= (ifac_r >> 1);
    // calculate channel step at the radius
    Int_t ich_step;
    if (ifac_r == 0)
      return -1;
    else
      ich_step = (Int_t)(ifac / ifac_r);
    Double_t phi = 2. * TMath::Pi() * ((Double_t)iFStrip - 0.5 * (Double_t)ich_step) / (Double_t)fNChannelsFront;
    yCross = radius * TMath::Cos(phi);
    xCross = -radius * TMath::Sin(phi);
    zCross = fPosition[2];
    dp = fPitch[0] * radius / fInnerRadius / ifac_r / TMath::Sqrt(12.);
    dr = fPitch[1] / TMath::Sqrt(12.);
    dx = dr * TMath::Abs(TMath::Sin(phi)) + dp * TMath::Abs(TMath::Cos(phi));
    dy = dr * TMath::Abs(TMath::Cos(phi)) + dp * TMath::Abs(TMath::Sin(phi));

    if (Inside(xCross, yCross))
      return fDetectorId;
    else
      return -1;
  }

  return -1;
}
// -------------------------------------------------------------------------

ClassImp(PndGemSensor)
