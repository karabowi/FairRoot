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
// -----                     PndGemSensorMonitor source file           -----
// -----                  Created 25/03/2013  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "PndGemSensorMonitor.h"

#include "PndGemSensor.h"

#include "TMath.h"
#include "TRandom.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::flush;
using std::pair;
using std::vector;

// -----   Default constructor   -------------------------------------------
PndGemSensorMonitor::PndGemSensorMonitor()
  : PndGemSensor(), fFLATime(0), fFLACharge(0), fFLADigiNr(0), fFLAEventNr(0), fFMeanTimeD(0), fFNofDigis(0), fFSLChan(0), fBLATime(0), fBLACharge(0), fBLADigiNr(0),
    fBLAEventNr(0), fBMeanTimeD(0), fBNofDigis(0), fBSLChan(0), fSensorRecoveryTime(100.), fSensorFirstTime(0.), fSensorLastTime(0.)
{
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensorMonitor::PndGemSensorMonitor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation, Double_t innerRad, Double_t outerRad,
                                         Double_t d, Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1)
  : PndGemSensor(tempName, detId, iType, x0, y0, z0, rotation, innerRad, outerRad, d, stripAngle0, stripAngle1, pitch0, pitch1), fFLATime(0), fFLACharge(0), fFLADigiNr(0),
    fFLAEventNr(0), fFMeanTimeD(0), fFNofDigis(0), fFSLChan(0), fBLATime(0), fBLACharge(0), fBLADigiNr(0), fBLAEventNr(0), fBMeanTimeD(0), fBNofDigis(0), fBSLChan(0),
    fSensorRecoveryTime(100.), fSensorFirstTime(0.), fSensorLastTime(0.)
{
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensorMonitor::PndGemSensorMonitor(TString tempName, Int_t stationNr, Int_t sectorNr, Int_t iType, Double_t x0, Double_t y0, Double_t z0, Double_t rotation,
                                         Double_t innerRad, Double_t outerRad, Double_t d, Double_t stripAngle0, Double_t stripAngle1, Double_t pitch0, Double_t pitch1)
  : PndGemSensor(tempName, stationNr, sectorNr, iType, x0, y0, z0, rotation, innerRad, outerRad, d, stripAngle0, stripAngle1, pitch0, pitch1), fFLATime(0), fFLACharge(0),
    fFLADigiNr(0), fFLAEventNr(0), fFMeanTimeD(0), fFNofDigis(0), fFSLChan(0), fBLATime(0), fBLACharge(0), fBLADigiNr(0), fBLAEventNr(0), fBMeanTimeD(0), fBNofDigis(0),
    fBSLChan(0), fSensorRecoveryTime(100.), fSensorFirstTime(0.), fSensorLastTime(0.)
{
  fFLATime.resize(this->GetNChannelsFront(), -1.);
  fFLACharge.resize(this->GetNChannelsFront(), -1.);
  fFLADigiNr.resize(this->GetNChannelsFront(), -1);
  fFLAEventNr.resize(this->GetNChannelsFront(), -1);
  fFMeanTimeD.resize(this->GetNChannelsFront(), 0.);
  fFNofDigis.resize(this->GetNChannelsFront(), 0);
  fBLATime.resize(this->GetNChannelsBack(), -1.);
  fBLACharge.resize(this->GetNChannelsBack(), -1.);
  fBLADigiNr.resize(this->GetNChannelsBack(), -1);
  fBLAEventNr.resize(this->GetNChannelsBack(), -1);
  fBMeanTimeD.resize(this->GetNChannelsBack(), 0.);
  fBNofDigis.resize(this->GetNChannelsBack(), 0);
}
// -------------------------------------------------------------------------

// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
PndGemSensorMonitor::PndGemSensorMonitor(const PndGemSensor &tempSensor)
  : PndGemSensor(tempSensor), fFLATime(0), fFLACharge(0), fFLADigiNr(0), fFLAEventNr(0), fFMeanTimeD(0), fFNofDigis(0), fFSLChan(0), fBLATime(0), fBLACharge(0), fBLADigiNr(0),
    fBLAEventNr(0), fBMeanTimeD(0), fBNofDigis(0), fBSLChan(0), fSensorRecoveryTime(100.), fSensorFirstTime(0.), fSensorLastTime(0.)
{
  fFLATime.resize(this->GetNChannelsFront(), -1.);
  fFLACharge.resize(this->GetNChannelsFront(), -1.);
  fFLADigiNr.resize(this->GetNChannelsFront(), -1);
  fFLAEventNr.resize(this->GetNChannelsFront(), -1);
  fFMeanTimeD.resize(this->GetNChannelsFront(), 0.);
  fFNofDigis.resize(this->GetNChannelsFront(), 0);
  fBLATime.resize(this->GetNChannelsBack(), -1.);
  fBLACharge.resize(this->GetNChannelsBack(), -1.);
  fBLADigiNr.resize(this->GetNChannelsBack(), -1);
  fBLAEventNr.resize(this->GetNChannelsBack(), -1);
  fBMeanTimeD.resize(this->GetNChannelsBack(), 0.);
  fBNofDigis.resize(this->GetNChannelsBack(), 0);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemSensorMonitor::~PndGemSensorMonitor() {}
// -------------------------------------------------------------------------

// -----   Activate Channel   ----------------------------------------------------
void PndGemSensorMonitor::ActivateChannel(Int_t channelNr, Int_t sideId, Int_t eventNr, Int_t digiNr, Double_t channelTime, Double_t channelCharge)
{
  if (sideId == 0) {
    // had to comment it out. but be careful with stat options here: if this function is called from PndGemFindHits
    // while looping over clusters, it will cause problems, as digis may belong to different clusters....

    // if ( channelTime > fFLATime[fFSLChan] + 21 && fFLATime[fFSLChan] > -0.5 ) {
    //   cout << "SENSOR " << GetStationNr() << "." << GetSensorNr() << ".F not hitted for " << channelTime-fFLATime[fFSLChan] << " ns (from  "
    // 	   << fFLATime[fFSLChan] << " to " << channelTime << ")"
    // 	   << " EVENT " << fFLAEventNr[fFSLChan] << " vs " << eventNr << endl;
    // }
    // if ( fFLAEventNr[channelNr] == eventNr ) {
    //   cout << "****** " << GetStationNr() << "." << GetSensorNr() << ".F hitted twice after " << channelTime-fFLATime[channelNr] << " ns (at  "
    // 	   << fFLATime[channelNr] << " and " << channelTime << ")"
    // 	   << " IN EVENT " << eventNr << endl;
    // }
    if (fFNofDigis[channelNr] > 0) {
      fFMeanTimeD[channelNr] = fFMeanTimeD[channelNr] + (channelTime - fFLATime[channelNr] - fFMeanTimeD[channelNr]) / ((Double_t)fFNofDigis[channelNr]);
    }
    fFLATime[channelNr] = channelTime;
    fFLACharge[channelNr] = channelCharge;
    fFLADigiNr[channelNr] = digiNr;
    fFLAEventNr[channelNr] = eventNr;
    fFNofDigis[channelNr] += 1;
    fFSLChan = channelNr;
  } else {
    // if ( channelTime > fBLATime[fBSLChan] + 21 && fBLATime[fFSLChan] > -0.5  ) {
    //   cout << "SENSOR " << GetStationNr() << "." << GetSensorNr() << ".B not hitted for " << channelTime-fBLATime[fBSLChan] << " ns (from "
    // 	   << fBLATime[fBSLChan] << " to " << channelTime << ")"
    // 	   << " EVENT " << fBLAEventNr[fBSLChan] << " vs " << eventNr << endl;
    // }
    // if ( fBLAEventNr[channelNr] == eventNr ) {
    //   cout << "****** " << GetStationNr() << "." << GetSensorNr() << ".F hitted twice after " << channelTime-fBLATime[channelNr] << " ns (at  "
    // 	   << fBLATime[channelNr] << " and " << channelTime << ")"
    // 	   << " IN EVENT " << eventNr << endl;
    // }
    if (fBNofDigis[channelNr] > 0) {
      fBMeanTimeD[channelNr] = fBMeanTimeD[channelNr] + (channelTime - fBLATime[channelNr] - fBMeanTimeD[channelNr]) / ((Double_t)fBNofDigis[channelNr]);
    }
    fBLATime[channelNr] = channelTime;
    fBLACharge[channelNr] = channelCharge;
    fBLADigiNr[channelNr] = digiNr;
    fBLAEventNr[channelNr] = eventNr;
    fBNofDigis[channelNr] += 1;
    fBSLChan = channelNr;
  }
  CheckNeighbours(channelNr, sideId, eventNr, digiNr, channelTime, channelCharge);
}
// -------------------------------------------------------------------------

// -----   Check neighbours   ----------------------------------------------------
void PndGemSensorMonitor::CheckNeighbours(Int_t channelNr, Int_t sideId, Int_t, Int_t, Double_t, Double_t)
{ // eventNr digiNr channelTime channelCharge //[R.K.03/2017] unused variable(s)
  Int_t nChan[3] = {-1, -1, -1};
  Int_t nofChan = GetNeighbours(sideId, channelNr, nChan[0], nChan[1], nChan[2]);
  //  cout << "------ check " << nofChan << " neighbours of channel " << channelNr << " on sensor " << this->GetDetectorId() << "." << sideId << " : " << endl;
  for (Int_t ichan = 0; ichan < nofChan; ichan++) {
    //    cout << "            ---------------- channel " << nChan[ichan] << " at a distance " << GetDistance(sideId,channelNr,nChan[ichan]) << flush;
    // if ( GetDistance(sideId,channelNr,nChan[ichan]) > 1.1 ||
    // 	 GetDistance(sideId,channelNr,nChan[ichan]) < 0.9 ) cout << " OOOOOOOOOOOOOOOOOOOOOO" << flush;
    // cout << " // difference = " << channelNr-nChan[ichan] << flush;
    // cout << endl;
  }
}
// -------------------------------------------------------------------------

// -----   Is channel active?   ----------------------------------------------------
Bool_t PndGemSensorMonitor::ChannelIsActive(Int_t, Int_t, Double_t)
{ // channelNr sideId timeNow   //[R.K.03/2017] unused variable(s)
  // Empty function!!
  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   The last channel activation at...   ----------------------------------------------------
Double_t PndGemSensorMonitor::ChannelLastActiveAt(Int_t channelNr, Int_t sideId)
{
  if (sideId == 0 && fFLATime[channelNr] < -0.5)
    return -1.;
  if (sideId == 1 && fBLATime[channelNr] < -0.5)
    return -1.;

  if (sideId == 0) {
    // if ( eventNr == fFLAEventNr[channelNr] )
    //   cout << "channel " << channelNr << " in sensor " << GetStationNr() << "." << GetSensorNr() << ".F, two hits in ev " << eventNr
    // 	   << " 1st @ " << fFLATime[channelNr] << ", now @ " << timeNow << " (diffr=" << timeNow-fFLATime[channelNr] << ")" << endl;
    return fFLATime[channelNr];
  } else {
    // if ( eventNr == fBLAEventNr[channelNr] )
    //   cout << "channel " << channelNr << " in sensor " << GetStationNr() << "." << GetSensorNr() << ".B, two hits in ev " << eventNr
    // 	   << " 1st @ " << fBLATime[channelNr] << ", now @ " << timeNow << " (diffr=" << timeNow-fBLATime[channelNr] << ")" << endl;
    return fBLATime[channelNr];
  }
}
// -------------------------------------------------------------------------

// -----   How much time passed since last activation?   ----------------------------------------------------
Double_t PndGemSensorMonitor::ChannelLastActiveAt(Int_t channelNr, Int_t sideId, Double_t timeNow, Int_t)
{ // eventNr //[R.K.03/2017] unused variable(s)
  if (sideId == 0 && fFLATime[channelNr] < -0.5)
    return -1.;
  if (sideId == 1 && fBLATime[channelNr] < -0.5)
    return -1.;

  if (sideId == 0) {
    // if ( eventNr == fFLAEventNr[channelNr] )
    //   cout << "channel " << channelNr << " in sensor " << GetStationNr() << "." << GetSensorNr() << ".F, two hits in ev " << eventNr
    // 	   << " 1st @ " << fFLATime[channelNr] << ", now @ " << timeNow << " (diffr=" << timeNow-fFLATime[channelNr] << ")" << endl;
    return timeNow - fFLATime[channelNr];
  } else {
    // if ( eventNr == fBLAEventNr[channelNr] )
    //   cout << "channel " << channelNr << " in sensor " << GetStationNr() << "." << GetSensorNr() << ".B, two hits in ev " << eventNr
    // 	   << " 1st @ " << fBLATime[channelNr] << ", now @ " << timeNow << " (diffr=" << timeNow-fBLATime[channelNr] << ")" << endl;
    return timeNow - fBLATime[channelNr];
  }
}
// -------------------------------------------------------------------------

// -----   Last activation data   ----------------------------------------------------
Bool_t PndGemSensorMonitor::ChannelLastActivation(Int_t, Int_t, Int_t &, Int_t &, Double_t &, Double_t &)
{ // channelNr sideId eventNr digiNr channelTime channelCharge //[R.K.03/2017] unused variable(s)
  // Empty function
  return kFALSE;
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void PndGemSensorMonitor::Print()
{
  cout << "---> sensor has " << fFLATime.size() << " Front and " << fBLATime.size() << " Back strips" << endl;

  for (size_t ichan = 0; ichan < fFLATime.size(); ichan++) {
    cout << ichan << ":" << fFLATime[ichan] << " . " << flush;
  }
  cout << endl;

  // for ( Int_t ibin = 0 ; ibin < 10 ; ibin++ ) {
  //   cout << " | " << flush;
  //   for ( Int_t ichan = 0 ; ichan < fFLATime.size() ; ichan+=10 ) {
  //     if ( fFNofDigis[ichan] == 0 ) {cout << " " << flush;continue;}
  //     cout << fFNofDigis[ichan] % 2 << flush;
  //     fFNofDigis[ichan] = fFNofDigis[ichan]/2;
  //   }
  //   cout << " | " << endl;
  // }
}
// -------------------------------------------------------------------------

ClassImp(PndGemSensorMonitor)
