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
// -----                PndSdsStripHitProducer source file             -----
// -------------------------------------------------------------------------

// This Class
#include "PndSdsStripHitProducer.h"
// SDS
#include "PndSdsMCPoint.h"
#include "PndSdsCalcStrip.h"
#include "PndSdsDigiStrip.h"
// PANDA
#include "PndStringSeparator.h"
#include "PndDetectorList.h"
// FAIR
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairContFact.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairEventHeader.h"
#include "FairLogger.h"
// ROOT
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TVector2.h"
#include "TString.h"
#include "TObjString.h"
#include "TGeoManager.h"
#include "TList.h"
#include "TRandom.h"

// -----   Default constructor   -------------------------------------------
PndSdsStripHitProducer::PndSdsStripHitProducer()
  : PndSdsTask("SDS Strip Digi Producer(PndSdsStripHitProducer)"), fPointArray(nullptr), fStripArray(nullptr), fDataBuffer(0), fDigiParameterList(nullptr),
    fChargeDigiParameterList(nullptr), fCurrentDigiPar(nullptr), fCurrentChargeConverter(nullptr), fStripCalcTop(), fStripCalcBot(), fChargeConverter(),
    fCurrentStripCalcTop(nullptr), fCurrentStripCalcBot(nullptr), fMcEventHeader(nullptr), fGeoH(nullptr), fOverrideParams(kFALSE), fTimeOrderedDigi(kFALSE), fEventNr(0)
{
  fDigiParameterList = new TList();
  fChargeDigiParameterList = new TList();
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Default constructor   -------------------------------------------
PndSdsStripHitProducer::PndSdsStripHitProducer(const char *name)
  : PndSdsTask(name), fPointArray(nullptr), fStripArray(nullptr), fDataBuffer(0), fDigiParameterList(nullptr), fChargeDigiParameterList(nullptr), fCurrentDigiPar(nullptr),
    fCurrentChargeConverter(nullptr), fStripCalcTop(), fStripCalcBot(), fChargeConverter(), fCurrentStripCalcTop(nullptr), fCurrentStripCalcBot(nullptr), fMcEventHeader(nullptr),
    fGeoH(nullptr), fOverrideParams(kFALSE), fTimeOrderedDigi(kFALSE), fEventNr(0)
{
  fDigiParameterList = new TList();
  fChargeDigiParameterList = new TList();
  SetPersistency(kTRUE);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSdsStripHitProducer::~PndSdsStripHitProducer()
{
  if (0 != fDigiParameterList)
    delete fDigiParameterList;
  if (0 != fChargeDigiParameterList)
    delete fChargeDigiParameterList;
  if (0 != fDataBuffer)
    delete fDataBuffer;
  // TODO: needs check: now cleared correctly?
  for (std::map<const char *, PndSdsCalcStrip *>::iterator it = fStripCalcTop.begin(); it != fStripCalcTop.end(); it++) {
    if (0 != it->second)
      delete it->second;
    it->second = 0;
  }
  for (std::map<const char *, PndSdsCalcStrip *>::iterator it = fStripCalcBot.begin(); it != fStripCalcBot.end(); it++) {
    if (0 != it->second)
      delete it->second;
    it->second = 0;
  }
  for (std::map<const char *, PndSdsChargeConversion *>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++) {
    if (0 != it->second)
      delete it->second;
    it->second = 0;
  }
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
InitStatus PndSdsStripHitProducer::ReInit()
{
  SetParContainers();
  SetCalculators();
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
void PndSdsStripHitProducer::SetCalculators()
{
  // After the first start if the Init() tis can be set properly.

  TIter params(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)params()) {
    if (0 == digipar) {
      Error("SetCalculators()", "A Digi Parameter Set does not exist properly.");
      continue;
    }
    const char *senstype = digipar->GetSensType();
    if (fVerbose > 1) {
      Info("SetCalculators()", "Create a Parameter Set for %s sensors", senstype);
      std::cout << senstype << "#" << std::endl;
    }
    if (fVerbose > 2)
      digipar->Print();
    // TODO switch also with PndSdsCalcStripDif
    fStripCalcTop[senstype] = new PndSdsCalcStrip(digipar, SensorSide::kTOP);
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose);
    fStripCalcBot[senstype] = new PndSdsCalcStrip(digipar, SensorSide::kBOTTOM);
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose);
  }
}

// -------------------------------------------------------------------------

void PndSdsStripHitProducer::SetParContainers()
{
  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();

  fGeoH->SetParContainers();
  if (fVerbose > 1)
    Info("SetParContainers", "done.");
  return;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsStripHitProducer::Init()
{
  FairRootManager *ioman = FairRootManager::Instance();

  SetBranchNames();

  if (!ioman) {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fPointArray = (TClonesArray *)ioman->GetObject(fInBranchName);
  if (!fPointArray) {
    std::cout << "-E- PndSdsStripHitProducer::Init: "
              << "No " << fInBranchName << " array!" << std::endl;
    return kERROR;
  }

  fDataBuffer = new PndSdsDigiStripWriteoutBuffer(fOutBranchName, fFolderName, GetPersistency());
  fDataBuffer = (PndSdsDigiStripWriteoutBuffer *)ioman->RegisterWriteoutBuffer(fOutBranchName, fDataBuffer);

  fDataBuffer->ActivateBuffering(fTimeOrderedDigi);

  SetCalculators();

  if (fVerbose > 0) {
    LOG(info) << " PndSdsStripHitProducer: Initialisation successfull with these parameters:";
    TIter params(fDigiParameterList);
    while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)params()) {
      if (0 != digipar) {
        digipar->Print();
      }
    }
  }

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSdsStripHitProducer::Exec(Option_t *)
{
  // Reset output array
  fGeoH->SetVerbose(fVerbose);

  // std::cout << "EventTime: " << FairRootManager::Instance()->GetEventTime() << std::endl;

  for (std::map<const char *, PndSdsChargeConversion *>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++) {
    it->second->StartExecute();
  }

  // Declare some variables
  PndSdsMCPoint *point = nullptr;

  //  Int_t detID = 0;       // Detector ID
  //     Int_t trackID = 0;     // Track index

  fStripArray = FairRootManager::Instance()->GetTClonesArray(fOutBranchName);

  // Loop over PndSdsMCPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
  if (fVerbose > 0) {
    std::cout << " Nr of Points: " << nPoints << std::endl;
  }

  Int_t iStrip = 0;
  Bool_t selected = kFALSE;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    point = (PndSdsMCPoint *)fPointArray->At(iPoint);
    // std::cout << "Point " << iPoint << ": " << *point << std::endl;
    selected = SelectSensorParams(point->GetSensorID());
    if (!selected) {
      continue;
    }

    if (fVerbose > 2) {
      std::cout << "***** Strip Digi for " << fCurrentDigiPar->GetSensType() << " ******" << std::endl;
      std::cout << " DetName : " << fGeoH->GetPath(point->GetSensorID()) << std::endl;
    }
    if (!point) {
      std::cout << "No Point!" << std::endl;
      continue;
    }
    if (fVerbose > 2) {
      std::cout << "****Global Point: " << std::endl;
      point->Print("");
    }

    // transform to local sensor system... (mc point has the ID not the path to the volume)
    TVector3 posInL = fGeoH->MasterToLocalShortId(point->GetPosition(), point->GetSensorID());
    TVector3 posOutL = fGeoH->MasterToLocalShortId(point->GetPositionOut(), point->GetSensorID());
    if (fVerbose > 2) {
      posInL.Print();
      posOutL.Print();
      std::cout << "Energy: " << point->GetEnergyLoss() << std::endl;
    }
    //      detID   = point->GetDetectorID();

    // Top Side
    if (fVerbose > 2)
      std::cout << "Top Side: " << std::endl;
    // Calculate a cluster of Strips fired
    std::vector<PndSdsStrip> topStrips = fCurrentStripCalcTop->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss());

    if (topStrips.size() != 0) {
      if (fVerbose > 1)
        std::cout << "SensorStrips: " << std::endl;
      for (std::vector<PndSdsStrip>::const_iterator kit = topStrips.begin(); kit != topStrips.end(); ++kit) {

        AddDigi(iStrip, iPoint, FairRootManager::Instance()->GetBranchId(fInBranchName), point->GetSensorID(), fCurrentStripCalcTop->CalcFEfromStrip(kit->GetIndex()),
                fCurrentStripCalcTop->CalcChannelfromStrip(kit->GetIndex()), kit->GetCharge());

        if (fVerbose > 1)
          std::cout << *kit << std::endl;
      }
    } else if (fVerbose > 2)
      std::cout << "Top side empty" << std::endl;

    // Bottom Side
    if (fVerbose > 2)
      std::cout << "Bottom Side: " << std::endl;
    std::vector<PndSdsStrip> botStrips = fCurrentStripCalcBot->GetStrips(posInL.X(), posInL.Y(), posInL.Z(), posOutL.X(), posOutL.Y(), posOutL.Z(), point->GetEnergyLoss());
    if (botStrips.size() != 0) {
      if (fVerbose > 2)
        std::cout << " SensorStrips: " << std::endl;
      for (std::vector<PndSdsStrip>::const_iterator kit = botStrips.begin(); kit != botStrips.end(); ++kit) {

        AddDigi(iStrip, iPoint, FairRootManager::Instance()->GetBranchId(fInBranchName), point->GetSensorID(),
                fCurrentStripCalcBot->CalcFEfromStrip(kit->GetIndex()) + fCurrentDigiPar->GetNrTopFE(), fCurrentStripCalcBot->CalcChannelfromStrip(kit->GetIndex()),
                kit->GetCharge());

        //    	  }
        if (fVerbose > 2)
          std::cout << *kit << std::endl;
      }
    } else if (fVerbose > 2)
      std::cout << "Bottom side empty" << std::endl;

  } // Loop over MCPoints

  for (std::map<const char *, PndSdsChargeConversion *>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++) {
    it->second->EndExecute();
  }

  // Event summary
  if (fVerbose > 1)
    LOG(info) << " PndSdsStripHitProducer: EventNr " << fEventNr << " from " << nPoints << " PndSdsMCPoints, " << iStrip << " Digis created.";

  fEventNr++;
}
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::AddDigi(Int_t &iStrip, Int_t iPoint, Int_t, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge) // detID //[R.K.03/2017] unused variable(s)
{

  PndSdsMCPoint *point = (PndSdsMCPoint *)fPointArray->At(iPoint);
  SelectSensorParams(point->GetSensorID());
  Int_t smearedCharge = (Int_t)fCurrentChargeConverter->ChargeToDigiValue(charge);
  Double_t timeStamp = DigitizeTime(point->GetTime(), charge);

  Double_t smearedChargeInE = fCurrentChargeConverter->DigiValueToCharge(smearedCharge);
  Double_t timewalk = fCurrentChargeConverter->GetTimeWalk(smearedChargeInE);

  Double_t correctedTimeStamp = timeStamp - timewalk - fCurrentChargeConverter->GetTimeStep() / 2;

  //	std::cout << " charge: " << charge << " smeared DigiCharge: " << smearedCharge << " smeared charge in e " << smearedChargeInE << std::endl;
  //	std::cout << "MCTime: " << point->GetTime() << " TimeStamp: " << timeStamp << " timewalk " << timewalk << " corrected TimeStamp: " << correctedTimeStamp << " charge: " <<
  // charge << " smearedCharge in e: " << smearedChargeInE << std::endl;

  std::vector<Int_t> indices;
  indices.push_back(iPoint);

  PndSdsDigiStrip *tempStrip = new PndSdsDigiStrip(indices, FairRootManager::Instance()->GetBranchId(fInBranchName), sensorID, fe, chan, smearedCharge, correctedTimeStamp);

  if (fTimeOrderedDigi) {
    tempStrip->ResetLinks();
    FairEventHeader *evtHeader = (FairEventHeader *)FairRootManager::Instance()->GetObject("EventHeader.");
    //  for (int i = 0; i < indices.size(); i++)
    tempStrip->AddLink(FairLink(evtHeader->GetInputFileId(), evtHeader->GetMCEntryNumber() - 1, fInBranchId, iPoint));
    tempStrip->AddLink(FairLink(-1, fEventNr, "EventHeader.", -1));
    tempStrip->AddLinks(*(point->GetPointerToLinks()));
  }
  fDataBuffer->FillNewData(tempStrip, timeStamp + 100, point->GetTime() + FairRootManager::Instance()->GetEventTime());
  delete (tempStrip);

  //	  std::cout << "AddDigi called: " << *tempStrip << std::endl;

  //  // we're here when this channel didn't fire
  //  std::vector<Int_t>indices;
  //  indices.push_back(iPoint);
  //
  //  PndSdsDigiStrip* tempStrip = new ((*fStripArray)[iStrip]) PndSdsDigiStrip(indices,detID,sensorID,fe,chan,charge, FairRootManager::Instance()->GetEventTime());
  //  tempStrip->SetEntryNr(FairLink(FairRootManager::Instance()->GetBranchId(fOutBranchName), iStrip));
  //
  iStrip++;
  return;
}
// -------------------------------------------------------------------------

Bool_t PndSdsStripHitProducer::SelectSensorParams(Int_t sensorID)
{
  fCurrentDigiPar = nullptr;
  fCurrentStripCalcTop = nullptr;
  fCurrentStripCalcBot = nullptr;
  fCurrentChargeConverter = nullptr;

  TString detpath = fGeoH->GetPath(sensorID);
  // std::cout << "Detector: " << detpath.Data() << std::endl;
  if (!(detpath.Contains("Strip"))) { // filter from pixel points
    return kFALSE;
  }

  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) {
    const char *sensortype = digipar->GetSensType();
    if (detpath.Contains(sensortype)) {
      fCurrentStripCalcTop = fStripCalcTop[sensortype];
      fCurrentStripCalcBot = fStripCalcBot[sensortype];
      fCurrentChargeConverter = fChargeConverter[sensortype];
      fCurrentDigiPar = digipar;
      return kTRUE;
    }
  }
  // no suiting object found

  if (fVerbose > 1)
    Info("SelectSensorParams()", "No valid sensor parameters selected, skipping this point.");
  std::cout << "detector name does not contain a valid parameter name." << std::endl;
  std::cout << " DetName : " << detpath << std::endl;
  return kFALSE;
}

Double_t PndSdsStripHitProducer::DigitizeTime(Double_t time, Double_t charge)
{ // time [ns]
  Double_t eventTime = FairRootManager::Instance()->GetEventTime();
  return fCurrentChargeConverter->GetTimeStamp(time, charge, eventTime);
}

//______________________________________________________________________________
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::FinishEvent()
{
  // called after all Tasks did their Exex() and the data is copied to the file
  //  fStripArray->Delete();
  FinishEvents();
}
// -------------------------------------------------------------------------

void PndSdsStripHitProducer::FinishTask() {}

ClassImp(PndSdsStripHitProducer);
