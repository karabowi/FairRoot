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
// -----                PndLmdNoiseProducer source file                -----
// -----                  Created 05.2015  by P. Jasinski                  -----
//-----                  updated 17/07/2015 by A.Karavdina         -----
// -------------------------------------------------------------------------

//#include <cmath>
/*
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


#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"

#include "PndDetectorList.h"*/
#include "PndLmdNoiseProducer.h"
#include "PndStringSeparator.h"
#include "PndLmdDim.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsTotChargeConversion.h"

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdNoiseProducer::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << " -E- PndMvdNoiseProducer::Init: RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  // call prior to base call the overloaded FillSensorMethod() to find sensors from geometry;
  FillSensorLists();
  // call the base implementation
  //	if ( PndMvdNoiseProducer::Init() == kFATAL ) return kFATAL;
  // override some lmd specific stuff

  // Get input array

  // fDigiStripBuffer = (PndSdsDigiStripWriteoutBuffer*)FairRootManager::Instance()->
  // 		RegisterWriteoutBuffer("LMDStripDigis", new PndSdsDigiStripWriteoutBuffer("LMDStripDigis", "LMD", kTRUE));

  fDigiPixelBuffer = new PndSdsDigiPixelWriteoutBuffer("LMDPixelDigis", "LMD", kTRUE);
  fDigiPixelBuffer = (PndSdsDigiPixelWriteoutBuffer *)FairRootManager::Instance()->RegisterWriteoutBuffer("LMDPixelDigis", fDigiPixelBuffer);
  fDigiPixelBuffer->ActivateBuffering(fTimeOrderedDigi);

  fMCEventheader = (FairMCEventHeader *)ioman->GetObject("MCEventHeader.");
  if (!fMCEventheader) {
    Warning("Init", "Did not find the MC event header, assume 50ns of noise clockticks per call of Exec().");
  }
  fPreviousTime = 0.;

  // FillSensorLists();

  //  fFEModel = new PndSdsFESimple();

  if (fVerbose > 0) {
    std::cout << " -I- PndLmdNoiseProducer: Registered Sensors: " << fStripRectLIds.size() + fStripRectSIds.size() << "xStripRect " << fStripTrapIds.size() << "xStripTrap "
              << fPixelIds2.size() << "xPixel" << std::endl;
  }
  std::cout << " -I- PndMvdNoiseProducer: Intialisation successfull" << std::endl;

  // if (fDigiParRect->GetChargeConvMethod() == 0){
  // 	if(fVerbose>0) Info("Init()","ideal charge conversion for rect. strips");
  // 	fStripRectChargeConv = new PndSdsIdealChargeConversion(fDigiParRect->GetNoise());
  // }
  // else if (fDigiParRect->GetChargeConvMethod() == 1){
  // 	if(fVerbose>0) Info("Init()","use TOT charge conversion for rect. strips");
  // 	fStripRectChargeConv = new PndSdsTotChargeConversion(
  // 							     fTotDigiParRect->GetChargingTime(),
  // 							     fTotDigiParRect->GetConstCurrent(),
  // 							     fDigiParRect->GetThreshold(),
  // 							     fTotDigiParRect->GetClockFrequency(),
  // 							     fVerbose);
  // }
  // else Fatal ("Init()","rect. strips: charge conversion method not defined!");

  // if (fDigiParTrap->GetChargeConvMethod() == 0){
  //   if(fVerbose>0) Info("Init()","ideal charge conversion for trap. strips");
  //   fStripTrapChargeConv = new PndSdsIdealChargeConversion(fDigiParTrap->GetNoise());
  // }
  // else if (fDigiParTrap->GetChargeConvMethod() == 1){
  //   if(fVerbose>0) Info("Init()","use TOT charge conversion for trap. strips");
  //   fStripTrapChargeConv = new PndSdsTotChargeConversion(
  // 							 fTotDigiParTrap->GetChargingTime(),
  // 							 fTotDigiParTrap->GetConstCurrent(),
  // 							 fDigiParTrap->GetThreshold(),
  // 							 fTotDigiParTrap->GetClockFrequency(),
  // 							 fVerbose);
  // }
  // else Fatal ("Init()","trap. strips: charge conversion method not defined!");

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

void PndLmdNoiseProducer::FillSensorLists()
{
  // std::cout << " ***** initializing sensor lists ******** " << std::endl;
  TObjArray *sensorNames = fGeoH->GetSensorNames();

  for (int i = 0; i < sensorNames->GetEntries(); i++) {
    TString volpath = ((TObjString *)(sensorNames->At(i)))->GetString();
    // std::cout << " volume path is " << volpath << std::endl;
    if (!volpath.Contains("Lum"))
      continue;
    PndStringSeparator sep(volpath.Data(), "/");
    std::vector<std::string> volvec = sep.GetStringVector();
    TString volname = volvec[volvec.size() - 1].c_str();
    if (fVerbose > 2)
      std::cout << "VolName: " << volname.Data();
    if (volname.Contains("Active"))
    // std::cout << " found sensor " << volname << std::endl;
    {
      //	if(volname.Contains("Trap")) {fStripTrapIds.push_back(i); if(fVerbose>2)std::cout << " \tAdded to StripTrap" << std::endl;}
      if (volname.Contains("Pixel")) {
        fPixelIds.push_back(i);
        if (fVerbose > 2)
          std::cout << " \tAdded to Pixel" << std::endl;
      }
    }
  }
}

void PndLmdNoiseProducer::SetParContainers()
{

  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  // fDigiParRect = (PndSdsStripDigiPar*)(rtdb->getContainer("SDSStripDigiParRect"));
  // fDigiParTrap = (PndSdsStripDigiPar*)(rtdb->getContainer("SDSStripDigiParTrap"));
  fDigiParPix = (PndSdsPixelDigiPar *)(rtdb->getContainer("LMDPixelDigiPar"));
  // fTotDigiParRect = (PndSdsTotDigiPar*)(rtdb->getContainer("SDSStripTotDigiParRect"));
  // fTotDigiParTrap = (PndSdsTotDigiPar*)(rtdb->getContainer("SDSStripTotDigiParTrap"));
  fTotDigiParPix = (PndSdsTotDigiPar *)(rtdb->getContainer("LMDPixelTotDigiPar"));

  if (fGeoH == nullptr) {
    fGeoH = PndGeoHandling::Instance();
  }

  fGeoH->SetParContainers();
}

void PndLmdNoiseProducer::Exec(Option_t *)
{
  //   TObjArray* activeSensors = fGeoPar->GetGeoSensitiveNodes();
  Int_t nrCh = 0, rnd = 0,
        nrFE = 0, // sens=0, //[R.K. 01/2017] unused variable?
    fe = 0,       // nrSensors=0, //[R.K. 01/2017] unused variable?
    chanmax = 0, chan = 0, col = 0, row = 0, chanwhite = 0, charge = 0, nNoisyStripRects = 0, nNoisyStripTraps = 0, nNoisyPixels = 0;
  Double_t xfrac = 0., cycles = 1.;
  Int_t did = -1;

  // // *** Strip Trapezoids ***
  // nrCh = fDigiParTrap->GetNrFECh();
  // nrFE = fDigiParTrap->GetNrBotFE() + fDigiParTrap->GetNrTopFE();
  // nrSensors = fStripTrapIds.size();
  // chanmax = nrCh * nrFE * nrSensors;
  // xfrac = CalcDistFraction(fDigiParTrap->GetNoise(),fDigiParTrap->GetThreshold());
  // cycles = CalcReadoutCycles(fDigiParTrap->GetFeBusClock());
  // chanwhite = gRandom->Poisson(xfrac*cycles*chanmax);
  // if(fVerbose>1) std::cout << " -I- PndLmdNoiseProducer: TRAP <N> = " << xfrac*cycles*chanmax
  //   << " leading to " << chanwhite << " noisy digis of " << chanmax
  //   << " total channels" << std::endl;

  // for(Int_t i = 0;i < chanwhite;i++)
  // {
  //   rnd = gRandom->Integer(chanmax);
  //   sens = rnd/(nrFE*nrCh);
  //   rnd = rnd % (nrFE*nrCh);
  //   fe = rnd/nrCh;
  //   chan = rnd % nrCh;
  //   charge = fDigiParTrap->GetThreshold()*6;//CalcChargeAboveThreshold(fDigiParTrap->GetNoise(),fDigiParTrap->GetThreshold());
  //   did = fStripTrapIds[sens];
  //   fCurrentChargeConv = fStripTrapChargeConv;
  //   AddDigiStrip(nNoisyStripTraps,-1,did,fe,chan,charge);
  // }

  PndLmdDim &lmd_dim = PndLmdDim::Get_instance();
  // *** Pixel Sensors ***
  nrCh = fDigiParPix->GetFECols() * fDigiParPix->GetFERows();
  Int_t pixx = fPixelIds.size();
  nrFE = pixx;
  chanmax = nrCh * nrFE;
  if (fVerbose > 2) {
    std::cout << " found " << nrFE << " sensors "
              << " with in total " << chanmax << " pixels " << std::endl;
  }
  // get the mean number of pixels fired
  xfrac = CalcDistFraction(fDigiParPix->GetNoise(), fDigiParPix->GetThreshold());
  if (fVerbose > 2) {
    std::cout << " with a noise of " << fDigiParPix->GetNoise() << " e and a threshold of " << fDigiParPix->GetThreshold() << " e " << xfrac * 100 << " % pixels should have fired "
              << std::endl;
  }
  cycles = CalcReadoutCycles(fDigiParPix->GetFeBusClock());
  chanwhite = gRandom->Poisson(xfrac * cycles * chanmax);
  // std::cout << " cycles " << cycles << " chanwhite " << chanwhite << std::endl;
  if (fVerbose > 1)
    std::cout << " -I- PndLmdNoiseProducer: PIXEL <N> = " << xfrac * cycles * chanmax << " leading to " << chanwhite << " noisy digis of " << chanmax << " total channels"
              << std::endl;
  for (Int_t i = 0; i < chanwhite; i++) {
    // prevent further steps in the software cutting those hits away by applying high charge
    charge = fDigiParPix->GetThreshold() * 6; // CalcChargeAboveThreshold(fDigiParPix->GetNoise(),fDigiParPix->GetThreshold());
    rnd = gRandom->Integer(chanmax);
    chan = rnd % nrCh;
    col = chan % fDigiParPix->GetFECols();
    row = chan / fDigiParPix->GetFECols();
    fe = rnd / nrCh;
    // if (fe < 0 || fe > 399) std::cout << "error! :" << fe << " does not match the total number of channels" << std::endl;
    did = fPixelIds[fe];
    /*
    if(fe >= (Int_t)(pixx) )
    {
      fe = fe - pixx;
      sens = fe/6;
      did = fPixelIds6[sens];
      fe = fe%6;
      //if(fe>6) fe=fe-6+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if( fe >= (Int_t)(pixx2 + pixx4) )
    {
      fe = fe - pixx2 - pixx4;
      sens = fe/5;
      did = fPixelIds5[sens];
      fe = fe%5;
      //if(fe>4) fe=fe-4+10; //0-9 one row of FE, 10-19 2nd row of FE
    } else if( fe >= (Int_t)(pixx2) )
    {
      fe = fe -pixx2;
      sens = fe/4;
      did = fPixelIds4[sens];
      fe = fe%4;
    } else
    {
      sens = fe/2;
      did = fPixelIds2[sens];
      fe = fe%2;
    }*/

    // std::cout << " adding  pixel " << i << " of " << chanwhite << std::endl;

    int ihalf, iplane, imodule, iside, idie, isensor;
    if (fe < 0 || fe > 399)
      std::cout << "error! :" << fe << " does not match the total number of channels" << std::endl;
    if (col > 255 || row > 255 || col < 0 || row < 0)
      std::cout << "error! :" << fe << " " << did << " " << col << " " << row << " " << charge << std::endl;
    lmd_dim.Get_sensor_by_id(fe, ihalf, iplane, imodule, iside, idie, isensor);
    // if (imodule != 0 || ihalf != 0) continue;
    AddDigiPixel(nNoisyPixels, -1, fe, 0, col, row, charge); // fe is abused here for sensID and real fe is set to 0
  }

  fPreviousTime = FairRootManager::Instance()->GetEventTime();

  // *** The End ***
  if (fVerbose > 0) {
    std::cout << " -I- PndLmdNoiseProducer: Noise produced\t" << nNoisyStripRects << "xStripRect\t" << nNoisyStripTraps << "xStripTrap\t" << nNoisyPixels << "xPixels" << std::endl;
  }
}

// TODO: for time-based simulation number of cycles should be calculated (e.g like in //CORRECT calculation!) and not fixed
Double_t PndLmdNoiseProducer::CalcReadoutCycles(Double_t clock)
{ // time [ns], clock [MHz]
  Double_t cycles = 1.;
  Double_t timewindow = 0.;
  if (clock > 0) {
    if (fMCEventheader != 0) {
      timewindow = FairRootManager::Instance()->GetEventTime();
      //  cout<<"ev time = "<<FairRootManager::Instance()->GetEventTime()<<" prev.ev time = "<<fPreviousTime<<endl;
      timewindow -= fPreviousTime;
      //   cout<<"timewindow = "<<timewindow<<endl;
    } else {
      timewindow = 50.; // 20 MHz
    }
  }
  // cycles = timewindow*clock/1000.;//CORRECT calculation!
  cycles = 3; // for event-based reconstruction we assume minimum 3 readout cycles are used in event construction
  if (fVerbose > 10)
    printf(" -I- PndLmdNoiseProducer::CalcReadoutCycles(): %g cycles (%gMHz,%gns)\n", cycles, clock, timewindow);
  return cycles;
}

// -------------------------------------------------------------------------
void PndLmdNoiseProducer::AddDigiPixel(Int_t &noisies, Int_t iPoint, Int_t sensorID, Int_t fe, Int_t col, Int_t row, Double_t charge)
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

  if (tempPixel->GetTimeStamp() < 0) { // ideal charge converter has fixed time stamp = -1 ns =/
    int timeSt = gRandom->Integer(CalcReadoutCycles(fDigiParPix->GetFeBusClock()) * 1000. / fDigiParPix->GetFeBusClock());
    tempPixel->SetTimeStamp(timeSt);
  }

  fDigiPixelBuffer->FillNewData(tempPixel, fPixChargeConv->ChargeToDigiValue(charge) * 6 + FairRootManager::Instance()->GetEventTime(),
                                FairRootManager::Instance()->GetEventTime());
  noisies++;
  delete (tempPixel);
  //  std::cout << "DataInBuffer: " << fDigiPixelBuffer->GetNData() << std::endl;
}

ClassImp(PndLmdNoiseProducer)
