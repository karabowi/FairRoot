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
// -----                PndMvdNoiseProducer source file                -----
// -----                  Created 01.07.08  by R.Kliemt                -----
// -------------------------------------------------------------------------

//#include <cmath>

#include "TClonesArray.h"
#include "TGeoNode.h"

#include "FairRootManager.h"
#include "FairGeoVolume.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"

#include "PndMvdNoiseProducer.h"
#include "PndSdsMCPoint.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsDigiPixel.h"
#include "PndStringSeparator.h"

#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"

#include "PndDetectorList.h"

// -----   Default constructor   -------------------------------------------
PndMvdNoiseProducer::PndMvdNoiseProducer()
  : PndPersistencyTask("Charge Noise Producer"), fTimeOrderedDigi(kFALSE), fBranchName("MVDStripDigis"), fDigiStripArray(nullptr), fDigiPixelArray(nullptr),
    fDigiPixelBuffer(nullptr), fDigiStripBuffer(nullptr), fDigiParRect(nullptr), fDigiParTrap(nullptr), fDigiParPix(nullptr), fTotDigiParRect(nullptr), fTotDigiParTrap(nullptr),
    fTotDigiParPix(nullptr), fGeoH(nullptr), fMCEventheader(nullptr), fPixelIds2(), fPixelIds4(), fPixelIds5(), fPixelIds6(), fStripRectLIds(), fStripRectSIds(), fStripTrapIds(),
    fStripRectChargeConv(nullptr), fStripTrapChargeConv(nullptr), fCurrentChargeConv(nullptr), fPixChargeConv(nullptr), fNoiseSpread(0), fThreshold(0), fPreviousTime(0)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdNoiseProducer::~PndMvdNoiseProducer()
{
  if (fStripRectChargeConv)
    delete fStripRectChargeConv;
  if (fStripTrapChargeConv)
    delete fStripTrapChargeConv;
  if (fPixChargeConv)
    delete fPixChargeConv;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdNoiseProducer::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << " -E- PndMvdNoiseProducer::Init: RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array

  fDigiStripBuffer =
    (PndSdsDigiStripWriteoutBuffer *)FairRootManager::Instance()->RegisterWriteoutBuffer("MVDStripDigis", new PndSdsDigiStripWriteoutBuffer("MVDStripDigis", "MVD", kTRUE));

  //  fDigiPixelArray = (TClonesArray*) ioman->GetObject("MVDPixelDigis");
  fDigiPixelBuffer = new PndSdsDigiPixelWriteoutBuffer("MVDPixelDigis", "MVD", kTRUE);
  fDigiPixelBuffer = (PndSdsDigiPixelWriteoutBuffer *)FairRootManager::Instance()->RegisterWriteoutBuffer("MVDPixelDigis", fDigiPixelBuffer);
  fDigiPixelBuffer->ActivateBuffering(fTimeOrderedDigi);

  fMCEventheader = (FairMCEventHeader *)ioman->GetObject("MCEventHeader.");
  if (!fMCEventheader) {
    Warning("Init", "Did not find the MC event header, assume 50ns of noise clockticks per call of Exec().");
  }
  fPreviousTime = 0.;

  FillSensorLists();

  //  fFEModel = new PndSdsFESimple();

  if (fVerbose > 0) {
    std::cout << " -I- PndMvdNoiseProducer: Registered Sensors: " << fStripRectLIds.size() + fStripRectSIds.size() << "xStripRect " << fStripTrapIds.size() << "xStripTrap "
              << fPixelIds2.size() << "xPixel" << std::endl;
  }
  std::cout << " -I- PndMvdNoiseProducer: Intialisation successfull" << std::endl;

  if (fDigiParRect->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("Init()", "ideal charge conversion for rect. strips");
    fStripRectChargeConv = new PndSdsIdealChargeConversion(fDigiParRect->GetNoise());
  } else if (fDigiParRect->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("Init()", "use TOT charge conversion for rect. strips");
    fStripRectChargeConv = new PndSdsTotChargeConversion(fTotDigiParRect->GetChargingTime(), fTotDigiParRect->GetConstCurrent(), fDigiParRect->GetThreshold(),
                                                         fTotDigiParRect->GetClockFrequency(), fVerbose);
  } else
    Fatal("Init()", "rect. strips: charge conversion method not defined!");

  if (fDigiParTrap->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("Init()", "ideal charge conversion for trap. strips");
    fStripTrapChargeConv = new PndSdsIdealChargeConversion(fDigiParTrap->GetNoise());
  } else if (fDigiParTrap->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("Init()", "use TOT charge conversion for trap. strips");
    fStripTrapChargeConv = new PndSdsTotChargeConversion(fTotDigiParTrap->GetChargingTime(), fTotDigiParTrap->GetConstCurrent(), fDigiParTrap->GetThreshold(),
                                                         fTotDigiParTrap->GetClockFrequency(), fVerbose);
  } else
    Fatal("Init()", "trap. strips: charge conversion method not defined!");

  if (fDigiParPix->GetChargeConvMethod() == 0) {
    if (fVerbose > 0)
      Info("Init()", "ideal charge conversion for pixel part");
    fPixChargeConv = new PndSdsIdealChargeConversion(fDigiParPix->GetNoise());
  } else if (fDigiParPix->GetChargeConvMethod() == 1) {
    if (fVerbose > 0)
      Info("Init()", "use TOT charge conversion for pixel part");
    fPixChargeConv = new PndSdsTotChargeConversion(fTotDigiParPix->GetChargingTime(), fTotDigiParPix->GetConstCurrent(), fDigiParPix->GetThreshold(),
                                                   fTotDigiParPix->GetClockFrequency(), fVerbose);
  } else
    Fatal("Init()", "pixel part: charge conversion method not defined!");

  return kSUCCESS;
}

void PndMvdNoiseProducer::FillSensorLists()
{
  TObjArray *sensorNames = fGeoH->GetSensorNames();

  for (int i = 0; i < sensorNames->GetEntries(); i++) {
    TString volpath = ((TObjString *)(sensorNames->At(i)))->GetString();
    if (!volpath.Contains("Mvd"))
      continue;
    PndStringSeparator sep(volpath.Data(), "/");
    std::vector<std::string> volvec = sep.GetStringVector();
    TString volname = volvec[volvec.size() - 1].c_str();
    if (fVerbose > 2)
      std::cout << "VolName: " << volname.Data();
    if (volname.Contains("Active")) {
      if (volname.Contains("RectL")) {
        fStripRectLIds.push_back(i);
        if (fVerbose > 2)
          std::cout << " \tAdded to StripRectL" << std::endl;
      }
      if (volname.Contains("RectS")) {
        fStripRectSIds.push_back(i);
        if (fVerbose > 2)
          std::cout << " \tAdded to StripRectS" << std::endl;
      }
      if (volname.Contains("Trap")) {
        fStripTrapIds.push_back(i);
        if (fVerbose > 2)
          std::cout << " \tAdded to StripTrap" << std::endl;
      }
      if (volname.Contains("Pixel")) {
        if (volname.Contains("2")) {
          fPixelIds2.push_back(i);
          if (fVerbose > 2)
            std::cout << " \tAdded to Pixel 2" << std::endl;
        }
        if (volname.Contains("4")) {
          fPixelIds4.push_back(i);
          if (fVerbose > 2)
            std::cout << " \tAdded to Pixel 4" << std::endl;
        }
        if (volname.Contains("5")) {
          fPixelIds5.push_back(i);
          if (fVerbose > 2)
            std::cout << " \tAdded to Pixel 5" << std::endl;
        }
        if (volname.Contains("6")) {
          fPixelIds6.push_back(i);
          if (fVerbose > 2)
            std::cout << " \tAdded to Pixel 6" << std::endl;
        }
      }
    }
  }
}

// -------------------------------------------------------------------------
void PndMvdNoiseProducer::SetParContainers()
{
  if (fGeoH == nullptr) {
    fGeoH = PndGeoHandling::Instance();
  }

  fGeoH->SetParContainers();
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiParRect = (PndSdsStripDigiPar *)(rtdb->getContainer("MVDStripDigiParRect"));
  fDigiParTrap = (PndSdsStripDigiPar *)(rtdb->getContainer("MVDStripDigiParTrap"));
  fDigiParPix = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
  fTotDigiParRect = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDStripTotDigiParRect"));
  fTotDigiParTrap = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDStripTotDigiParTrap"));
  fTotDigiParPix = (PndSdsTotDigiPar *)(rtdb->getContainer("MVDPixelTotDigiPar"));
}

// -----   Public method Exec   --------------------------------------------
void PndMvdNoiseProducer::Exec(Option_t *)
{
  //   TObjArray* activeSensors = fGeoPar->GetGeoSensitiveNodes();
  Int_t nrCh = 0, rnd = 0, nrFE = 0, sens = 0, nrSensors = 0, fe = 0, chanmax = 0, chan = 0, col = 0, row = 0, chanwhite = 0, charge = 0, nNoisyStripRects = 0,
        nNoisyStripTraps = 0, nNoisyPixels = 0;
  Double_t xfrac = 0., cycles = 1.;
  Int_t did = -1;

  // *** Strip Rect Long ***
  // how many channels left?
  nrCh = fDigiParRect->GetNrFECh();
  nrFE = fDigiParRect->GetNrBotFE() + fDigiParRect->GetNrTopFE();
  nrSensors = fStripRectLIds.size();
  chanmax = nrCh * nrFE * nrSensors;
  // Get Number of Channels fired from noise
  xfrac = CalcDistFraction(fDigiParRect->GetNoise(), fDigiParRect->GetThreshold());
  cycles = CalcReadoutCycles(fDigiParRect->GetFeBusClock());
  chanwhite = gRandom->Poisson(xfrac * chanmax);
  if (fVerbose > 1)
    std::cout << " -I- PndMvdNoiseProducer: RECT <N> = " << xfrac * cycles * chanmax << " leading to " << chanwhite << " noisy digis of " << chanmax << " total channels"
              << std::endl;
  for (Int_t i = 0; i < chanwhite; i++) {
    // randomize the channel numbers & sensors
    rnd = gRandom->Integer(chanmax);
    sens = rnd / (nrFE * nrCh);
    rnd = rnd % (nrFE * nrCh);
    fe = rnd / nrCh;
    chan = rnd % nrCh;
    // calculate a charge deposit above threshold
    charge = CalcChargeAboveThreshold(fDigiParRect->GetNoise(), fDigiParRect->GetThreshold());
    did = fStripRectLIds[sens];
    fCurrentChargeConv = fStripRectChargeConv;
    AddDigiStrip(nNoisyStripRects, -1, did, fe, chan, charge);
  }

  // *** Strip Rect Short ***
  // how many channels left?
  nrCh = fDigiParRect->GetNrFECh();
  nrFE = fDigiParRect->GetNrBotFE() + fDigiParRect->GetNrTopFE() / 2;
  nrSensors = fStripRectSIds.size();
  chanmax = nrCh * nrFE * nrSensors;
  // Get Number of Channels fired from noise
  xfrac = CalcDistFraction(fDigiParRect->GetNoise(), fDigiParRect->GetThreshold());
  chanwhite = gRandom->Poisson(xfrac * cycles * chanmax);
  if (fVerbose > 1)
    std::cout << " -I- PndMvdNoiseProducer: RECT <N> = " << xfrac * cycles * chanmax << " leading to " << chanwhite << " noisy digis of " << chanmax << " total channels"
              << std::endl;
  for (Int_t i = 0; i < chanwhite; i++) {
    // randomize the channel numbers & sensors
    rnd = gRandom->Integer(chanmax);
    sens = rnd / (nrFE * nrCh);
    rnd = rnd % (nrFE * nrCh);
    fe = rnd / nrCh;
    if (fe >= fDigiParRect->GetNrTopFE() / 2) {
      fe += fDigiParRect->GetNrTopFE() / 2; // shift to avoid the unused fe numbers.
    }
    chan = rnd % nrCh;
    // calculate a charge deposit above threshold
    charge = CalcChargeAboveThreshold(fDigiParRect->GetNoise(), fDigiParRect->GetThreshold());
    did = fStripRectSIds[sens];
    fCurrentChargeConv = fStripRectChargeConv;
    AddDigiStrip(nNoisyStripRects, -1, did, fe, chan, charge);
  }

  // *** Strip Trapezoids ***
  nrCh = fDigiParTrap->GetNrFECh();
  nrFE = fDigiParTrap->GetNrBotFE() + fDigiParTrap->GetNrTopFE();
  nrSensors = fStripTrapIds.size();
  chanmax = nrCh * nrFE * nrSensors;
  xfrac = CalcDistFraction(fDigiParTrap->GetNoise(), fDigiParTrap->GetThreshold());
  cycles = CalcReadoutCycles(fDigiParTrap->GetFeBusClock());
  chanwhite = gRandom->Poisson(xfrac * cycles * chanmax);
  if (fVerbose > 1)
    std::cout << " -I- PndMvdNoiseProducer: TRAP <N> = " << xfrac * cycles * chanmax << " leading to " << chanwhite << " noisy digis of " << chanmax << " total channels"
              << std::endl;
  for (Int_t i = 0; i < chanwhite; i++) {
    rnd = gRandom->Integer(chanmax);
    sens = rnd / (nrFE * nrCh);
    rnd = rnd % (nrFE * nrCh);
    fe = rnd / nrCh;
    chan = rnd % nrCh;
    charge = CalcChargeAboveThreshold(fDigiParTrap->GetNoise(), fDigiParTrap->GetThreshold());
    did = fStripTrapIds[sens];
    fCurrentChargeConv = fStripTrapChargeConv;
    AddDigiStrip(nNoisyStripTraps, -1, did, fe, chan, charge);
  }

  // *** Pixel Sensors ***
  nrCh = fDigiParPix->GetFECols() * fDigiParPix->GetFERows();
  Int_t pixx2 = 2 * fPixelIds2.size(), pixx4 = 4 * fPixelIds4.size(), pixx5 = 5 * fPixelIds5.size(), pixx6 = 6 * fPixelIds6.size();
  nrFE = pixx2 + pixx4 + pixx5 + pixx6;
  chanmax = nrCh * nrFE;
  xfrac = CalcDistFraction(fDigiParPix->GetNoise(), fDigiParPix->GetThreshold());
  cycles = CalcReadoutCycles(fDigiParPix->GetFeBusClock());
  chanwhite = gRandom->Poisson(xfrac * cycles * chanmax);
  if (fVerbose > 1)
    std::cout << " -I- PndMvdNoiseProducer: PIXEL <N> = " << xfrac * cycles * chanmax << " leading to " << chanwhite << " noisy digis of " << chanmax << " total channels"
              << std::endl;
  for (Int_t i = 0; i < chanwhite; i++) {
    charge = CalcChargeAboveThreshold(fDigiParPix->GetNoise(), fDigiParPix->GetThreshold());
    rnd = gRandom->Integer(chanmax);
    chan = rnd % nrCh;
    col = chan % fDigiParPix->GetFECols();
    row = chan / fDigiParPix->GetFECols();
    fe = rnd / nrCh;
    if (fe >= (Int_t)(pixx2 + pixx4 + pixx5)) {
      fe = fe - pixx2 - pixx4 - pixx5;
      sens = fe / 6;
      did = fPixelIds6[sens];
      fe = fe % 6;
      // if(fe>6) fe=fe-6+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if (fe >= (Int_t)(pixx2 + pixx4)) {
      fe = fe - pixx2 - pixx4;
      sens = fe / 5;
      did = fPixelIds5[sens];
      fe = fe % 5;
      // if(fe>4) fe=fe-4+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if (fe >= (Int_t)(pixx2)) {
      fe = fe - pixx2;
      sens = fe / 4;
      did = fPixelIds4[sens];
      fe = fe % 4;
    } else {
      sens = fe / 2;
      did = fPixelIds2[sens];
      fe = fe % 2;
    }

    AddDigiPixel(nNoisyPixels, -1, did, fe, col, row, charge);
  }

  fPreviousTime = FairRootManager::Instance()->GetEventTime();

  // *** The End ***
  if (fVerbose > 0) {
    std::cout << " -I- PndMvdNoiseProducer: Noise produced\t" << nNoisyStripRects << "xStripRect\t" << nNoisyStripTraps << "xStripTrap\t" << nNoisyPixels << "xPixels" << std::endl;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t PndMvdNoiseProducer::CalcDistFraction(Double_t spread, Double_t threshold)
{
  // mean fraction of fireing digis
  return TMath::Erfc(threshold / (TMath::Sqrt2() * spread));
}

Int_t PndMvdNoiseProducer::CalcChargeAboveThreshold(Double_t spread, Double_t threshold)
{
  // pick any charge from the gaussian tail above threshold
  Double_t temp = 0.;
  temp = TMath::Gaus(threshold, 0, spread); // maximum gauss at threshold
  temp = gRandom->Uniform(0., temp);        // random value in y
  temp = -2. * spread * spread * log(temp); // get x value (recalc charge)
  temp = sqrt(temp);
  return (Int_t)ceil(temp);
}

Double_t PndMvdNoiseProducer::CalcReadoutCycles(Double_t clock)
{ // time [ns], clock [MHz]
  Double_t cycles = 1.;
  Double_t timewindow = 0.;
  if (clock > 0) {
    if (fMCEventheader != 0) {
      timewindow = FairRootManager::Instance()->GetEventTime();
      timewindow -= fPreviousTime;
    } else {
      timewindow = 50.; // how many ns do we suppress readout of zeros?
    }
  }
  if (fVerbose > 1)
    printf(" -I- PndMvdNoiseProducer::CalcReadoutCycles(): %g cycles (%gMHz,%gns)\n", cycles, clock, timewindow);
  return cycles;
}

void PndMvdNoiseProducer::AddDigiStrip(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge)
{
  // Double_t tempcharge = 0.;
  // Bool_t found = kFALSE;
  Int_t detID = -1; // no source mc branch
  // PndSdsDigiStrip* aDigi = 0;
  //  FairMCEventHeader* MCevtHeader = (FairMCEventHeader*)FairRootManager::Instance()->GetObject("MCEventHeader.");

  //  if (fTimeOrderedDigi == kFALSE){
  //	  Int_t iStrip = fDigiStripArray->GetEntriesFast();
  //
  //	  for(Int_t kstr = 0; kstr < iStrip && found == kFALSE; kstr++)
  //	  {
  //		aDigi = (PndSdsDigiStrip*)fDigiStripArray->At(kstr);
  //		if (aDigi->GetSensorID() == sensorID &&
  //			aDigi->GetFE() == fe &&
  //			aDigi->GetChannel() == chan )
  //		{
  //		  tempcharge = fCurrentChargeConv->DigiValueToCharge(*aDigi);
  //		  aDigi->SetCharge( fCurrentChargeConv->ChargeToDigiValue(charge + tempcharge) );
  //		  aDigi->AddIndex(iPoint);
  //		  found = kTRUE;
  //		}
  //	  }
  //	  if(found == kFALSE){
  //		  //TODO: get a reasonable timestamp fake for the noise
  //		std::vector<Int_t> indices;
  //		indices.push_back(iPoint);
  //		new ((*fDigiStripArray)[iStrip]) PndSdsDigiStrip(indices,detID,sensorID,fe,chan,fCurrentChargeConv->ChargeToDigiValue(charge), FairRootManager::Instance()->GetEventTime())
  //; 		noisies++; 		if(fVerbose>2) std::cout
  //		  << " -I- PndSdsNoiseProducer: Added StripTrap Digi at: FE=" << fe
  //		  << ", channel=" << chan << ", charge=" << charge<< " e"
  //		  << ", in sensor \n" << sensorID <<std::endl;
  //	  }
  //  }
  //  else{
  std::vector<Int_t> indices;
  indices.push_back(iPoint);
  PndSdsDigiStrip *tempStrip = new PndSdsDigiStrip(indices, detID, sensorID, fe, chan, fCurrentChargeConv->ChargeToDigiValue(charge), FairRootManager::Instance()->GetEventTime());
  noisies++;
  fDigiStripBuffer->FillNewData(tempStrip, FairRootManager::Instance()->GetEventTime(), FairRootManager::Instance()->GetEventTime() + 10);
  delete (tempStrip);
  //  }
}
// -------------------------------------------------------------------------
void PndMvdNoiseProducer::AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge)
{
  // Double_t tempcharge = 0.;
  // Bool_t found = kFALSE;
  Int_t detID = -1; // no source mc branch

  std::vector<Int_t> indices;
  indices.push_back(iPoint);
  PndSdsDigiPixel *tempPixel =
    new PndSdsDigiPixel(indices, detID, sensorID, fe, col, row, fPixChargeConv->ChargeToDigiValue(charge),
                        fPixChargeConv->GetTimeStamp(0, charge, FairRootManager::Instance()->GetEventTime())); // FairRootManager::Instance()->GetEventTime()) ;
  if (fPixChargeConv->GetTimeWalk((Int_t)tempPixel->GetCharge()) < 1E5) {
    tempPixel->SetTimeStamp(tempPixel->GetTimeStamp() - fPixChargeConv->GetTimeWalk((Int_t)tempPixel->GetCharge()));
    tempPixel->SetTimeStampError(fPixChargeConv->GetTimeStampErrorAfterCorrection());
  }
  fDigiPixelBuffer->FillNewData(tempPixel, fPixChargeConv->ChargeToDigiValue(charge) * 6 + FairRootManager::Instance()->GetEventTime(),
                                FairRootManager::Instance()->GetEventTime());
  noisies++; // to count outside the noisy digis.
  delete (tempPixel);
  //  std::cout << "DataInBuffer: " << fDigiPixelBuffer->GetNData() << std::endl;
}

void PndMvdNoiseProducer::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file

  FinishEvents();
}
// -------------------------------------------------------------------------

ClassImp(PndMvdNoiseProducer)
