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
// -----                PndSdsStripClusterTask source file             -----
// -------------------------------------------------------------------------

#include <cmath>

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoMatrix.h"
#include "TCanvas.h"
#include "TMatrixD.h"
#include "TCollection.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "FairContFact.h"
#include "FairLogger.h"

#include "PndStringSeparator.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsStripClusterTask.h"
#include "PndSdsMCPoint.h"
#include "PndSdsCalcStrip.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsClusterStrip.h"
#include "PndGeoHandling.h"

#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripAdvClusterFinder.h"
#include "PndSdsChargeWeightingAlgorithms.h"

#include <map>

// -----   Default constructor   -------------------------------------------
PndSdsStripClusterTask::PndSdsStripClusterTask()
  : PndSdsTask("SDS Strip Clustertisation Task"), fPath(), fDigiArray(nullptr), fClusterArray(nullptr), fHitArray(nullptr), fClustBranchName(""), fClusterType(0), fFEcolumns(0),
    fFErows(0), fChargeCut(1.e8), fRadChannel(0), fRadTime(0), fSingleStripChargeThreshold(0), fEventHeader(nullptr), fDigiParameterList(new TList()), fCurrentDigiPar(0),
    fSensorNamePar(nullptr), fChargeDigiParameterList(new TList()), fStripCalcTop(), fStripCalcBot(), fChargeConverter(), fCurrentStripCalcTop(0), fCurrentStripCalcBot(0),
    fChargeAlgos(nullptr), fCurrentChargeConverter(0), fDigiPar(0), fGeoH(nullptr), fCurrentClusterfinder(nullptr), fClusterFinderList(), fFunctor(nullptr), eta_rect(nullptr),
    eta_trap(nullptr), etahistofile(nullptr)
{
  SetPersistency(kTRUE);
}

// -----   Named constructor   -------------------------------------------
PndSdsStripClusterTask::PndSdsStripClusterTask(const char *name)
  : PndSdsTask(name), fPath(), fDigiArray(nullptr), fClusterArray(nullptr), fHitArray(nullptr), fClustBranchName(""), fClusterType(0), fFEcolumns(0), fFErows(0), fChargeCut(1.e8),
    fRadChannel(0), fRadTime(0), fSingleStripChargeThreshold(0), fEventHeader(nullptr), fDigiParameterList(new TList()), fCurrentDigiPar(0), fSensorNamePar(nullptr),
    fChargeDigiParameterList(new TList()), fStripCalcTop(), fStripCalcBot(), fChargeConverter(), fCurrentStripCalcTop(0), fCurrentStripCalcBot(0), fChargeAlgos(nullptr),
    fCurrentChargeConverter(0), fDigiPar(0), fGeoH(nullptr), fCurrentClusterfinder(nullptr), fClusterFinderList(), fFunctor(nullptr), eta_rect(nullptr), eta_trap(nullptr),
    etahistofile(nullptr)
{
  SetPersistency(kTRUE);
}

// -----   Destructor   ----------------------------------------------------
PndSdsStripClusterTask::~PndSdsStripClusterTask()
{
  if (0 != fDigiParameterList)
    delete fDigiParameterList;
  if (0 != fChargeDigiParameterList)
    delete fChargeDigiParameterList;
  if (0 != fFunctor)
    delete fFunctor;
  ClearCalculators();
}
// -------------------------------------------------------------------------

void PndSdsStripClusterTask::ClearCalculators()
{
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
  if (nullptr != fChargeAlgos)
    delete fChargeAlgos;
  for (std::map<const char *, PndSdsChargeConversion *>::iterator it = fChargeConverter.begin(); it != fChargeConverter.end(); it++) {
    if (0 != it->second)
      delete it->second;
    it->second = 0;
  }
  for (std::map<const char *, PndSdsStripClusterer *>::iterator it = fClusterFinderList.begin(); it != fClusterFinderList.end(); it++) {
    if (0 != it->second)
      delete it->second;
    it->second = 0;
  }

  fCurrentChargeConverter = 0;
  fCurrentStripCalcTop = 0;
  fCurrentStripCalcBot = 0;
  fCurrentClusterfinder = nullptr;
}

// -------------------------------------------------------------------------

void PndSdsStripClusterTask::SetParContainers()
{
  if (fGeoH == nullptr) {
    fGeoH = PndGeoHandling::Instance();
  }
  fGeoH->SetParContainers();
  return;
}

void PndSdsStripClusterTask::InitMQ(TList *tempList)
{
  SetBranchNames();
  fHitArray = new TClonesArray("PndSdsHit", 10000);
  fHitArray->SetName("MVDHitsStrip");
  fClusterArray = new TClonesArray("PndSdsClusterStrip", 10000);
  fClusterArray->SetName("MVDStripCluster");

  fSensorNamePar = (PndSensorNamePar *)tempList->FindObject("PndSensorNamePar");
  fGeoH = new PndGeoHandling(fSensorNamePar);
  fGeoH->FillSensorMap();

  SetParContainersMQ(tempList);
  SetCalculators();
}

void PndSdsStripClusterTask::ExecMQ(TList *inputList, TList *outputList)
{
  //	LOG(info) << "PndSdsStripClusterTask::ExecMQ inBranchName " << fInBranchName.Data() ;
  //	LOG(info) << "InputList: " << inputList->GetEntries() ;
  //	TIter next(inputList);
  //	while(TObject* obj = next()){
  //		LOG(info) << obj->GetName() ;
  //	}
  fDigiArray = (TClonesArray *)inputList->FindObject("PndSdsDigiStrips");
  fEventHeader = (FairEventHeader *)inputList->FindObject("EventHeader.");
  LOG(info) << "DigiArray: " << fDigiArray;
  LOG(info) << "DigiArray: " << fDigiArray->GetEntriesFast();

  outputList->Add(fClusterArray);
  outputList->Add(fHitArray);
  Exec("");
  return;
}

//
InitStatus PndSdsStripClusterTask::ReInit()
{
  SetParContainers();
  SetCalculators();
  return kSUCCESS;
}

void PndSdsStripClusterTask::SetCalculators()
{
  // called at the enf of Init()
  // TODO: Implement more clusterfinders
  if (fVerbose > 1)
    Info("SetCalculators", "sds part");
  ClearCalculators();
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
      digipar->Print();
    }
    fStripCalcTop[senstype] = new PndSdsCalcStrip(digipar, SensorSide::kTOP);
    fStripCalcTop[senstype]->SetVerboseLevel(fVerbose);
    fStripCalcBot[senstype] = new PndSdsCalcStrip(digipar, SensorSide::kBOTTOM);
    fStripCalcBot[senstype]->SetVerboseLevel(fVerbose);
  }
  fChargeAlgos = new PndSdsChargeWeightingAlgorithms(fDigiArray);
  fChargeAlgos->SetVerbose(fVerbose);
}

// -----   Public method Init   --------------------------------------------
InitStatus PndSdsStripClusterTask::Init()
{

  SetBranchNames();

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    std::cout << "-E- PndSdsStripClusterTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  fFunctor = new TimeGap();

  // Get input array
  fDigiArray = (TClonesArray *)ioman->GetObject(fInBranchName);
  //
  if (!fDigiArray) {
    std::cout << "-W- PndSdsPixelClusterTask::Init: "
              << "No SDSDigi array!" << std::endl;
    return kERROR;
  }
  // set output arrays

  //  fClusterArray = new TClonesArray("PndSdsClusterStrip");
  //  ioman->Register(fClustBranchName, fFolderName, fClusterArray, fPersistance);

  fClusterArray = ioman->Register(fClustBranchName, "PndSdsClusterStrip", fFolderName, GetPersistency());

  // fHitArray = new TClonesArray("PndSdsHit");
  fHitArray = ioman->Register(fOutBranchName, "PndSdsHit", fFolderName, GetPersistency());

  SetInBranchId();

  SetCalculators();

  fPath = getenv("VMCWORKDIR");
  fPath += "/macro/params/interstrippos_vs_eta_histos.root";

  etahistofile = new TFile(fPath, "READ");
  eta_rect = (TH2F *)etahistofile->Get("posvseta rect");
  eta_trap = (TH2F *)etahistofile->Get("posvseta trap");

  Info("Init", "Initialisation successfull");
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndSdsStripClusterTask::Exec(Option_t *)
{
  if (fVerbose > 2)
    std::cout << " **Starting PndSdsStripClusterTask::Exec()**" << std::endl;
  std::vector<PndSdsDigiStrip> digiStripArray;
  // Reset output array
  //  fClusterArray = FairRootManager::Instance()->GetTClonesArray(fClustBranchName);
  if (!fClusterArray)
    Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  //  fHitArray = FairRootManager::Instance()->GetTClonesArray(fOutBranchName);
  if (!fHitArray)
    Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  // Get input array

  if (FairRunAna::Instance() != 0 && FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray->Clear();
    fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fFunctor, (1. / 40.0) * 1000.); // FairRootManager::Instance()->GetEventTime() +
    if (fVerbose > 1)
      LOG(info) << " PndSdsStripClusterTask::Exec Digis: " << fDigiArray->GetEntries();
  }
  //	else
  //	  fDigiArray = (TClonesArray*)FairRootManager::Instance()->GetObject(fInBranchName);

  //	std::cout << "Requested Time: " << FairRootManager::Instance()->GetEventTime() + 10 << std::endl;
  //	for (int i = 0; i < fDigiArray->GetEntries(); i++){
  //		std::cout << i << ": " << ((PndSdsDigiStrip*)fDigiArray->At(i))->GetTimeStamp() << std::endl;
  //	}

  // std::cout << "-I- PndSdsStripClusterTask:: fDigiArray->Size(): " << fDigiArray->GetEntriesFast() << std::endl;
  // fDigiArray = (TClonesArray*) ioman->GetObject(fInBranchName);
  if (!fDigiArray) {
    std::cout << "-W- PndSdsStripClusterTask::Init: "
              << "No SDSDigi array!" << std::endl;
    return;
  }

  // when we have no digis, we can end the event here.
  if (fDigiArray->GetEntriesFast() == 0)
    return;
  fGeoH->SetVerbose(fVerbose);

  // Setup
  FillClusterFinders();

  std::vector<PndSdsClusterStrip *> clusters;
  std::vector<Int_t> topclusters; // contains index to fClusterArray
  std::vector<Int_t> botclusters; // contains index to fClusterArray
  std::vector<Int_t> oneclustertop;
  std::vector<Int_t> oneclusterbot;
  std::vector<Int_t> leftDigis;
  Int_t mcindex, clindex, botIndex, topIndex;
  // Int_t detID = FairRootManager::Instance()->GetBranchId(fInBranchName); //unused??
  Int_t clDetID = -1;
  if (FairRootManager::Instance() != 0)
    clDetID = FairRootManager::Instance()->GetBranchId(fClustBranchName);
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos, hitErr;
  TMatrixD hitCov(3, 3);
  Int_t clusterOffset = 0;
  PndSdsHit *tmphit;

  // -------   SEARCH  ------
  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) { // loop over all parameter sets and their filled finders (representing sensor types)
    SetCurrentCalculators(digipar);

    clusters = fCurrentClusterfinder->SearchClusters();
    // fetch ids in 'clusters' to the top and bot side
    topclusters = fCurrentClusterfinder->GetTopClusterIDs();
    botclusters = fCurrentClusterfinder->GetBotClusterIDs();
    if (fVerbose > 2) {
      leftDigis = fCurrentClusterfinder->GetLeftDigiIDs();
      if (0 < leftDigis.size()) {
        std::cout << "There are " << leftDigis.size() << " Digis not assigned to"
                  << " clusters:\n";
        for (unsigned int s = 0; s < leftDigis.size(); s++) {
          std::cout << leftDigis[s] << "|";
        }
        std::cout << std::endl;
      } else
        std::cout << "All Digis assigned to clusters" << std::endl;
    }
    // Fill the ClonesArray for output TODO: do this better, don't copy objects around
    // save array size before we fill
    clusterOffset = fClusterArray->GetEntriesFast();
    for (std::vector<PndSdsClusterStrip *>::iterator clit = clusters.begin(); clit != clusters.end(); ++clit) {
      clindex = fClusterArray->GetEntriesFast();
      PndSdsClusterStrip *myCluster = new ((*fClusterArray)[clindex]) PndSdsClusterStrip(*(*clit));

      if (FairRunAna::Instance() != 0 && FairRunAna::Instance()->IsTimeStamp()) {
        myCluster->ResetLinks();
        for (UInt_t i = 0; i < (UInt_t)myCluster->GetClusterSize(); i++) {
          PndSdsDigiStrip *tempDigi = (PndSdsDigiStrip *)fDigiArray->At(myCluster->GetDigiIndex(i));
          myCluster->AddLink(FairLink(tempDigi->GetEntryNr()));
        }
      }
    }

    // std::cout << "-I- PndSdsStripClusterTask:: fClusterArray size: " << fClusterArray->GetEntries() << std::endl;

    // printout for checking
    if (fVerbose > 2) {
      std::cout << "Check.. Offset: " << clusterOffset << std::endl;
      std::cout << "Top Clusters: ";
      for (std::vector<Int_t>::iterator itTop = topclusters.begin(); itTop != topclusters.end(); ++itTop) {
        std::cout << *itTop << " | ";
      }
      std::cout << std::endl;
      std::cout << "Bot Clusters: ";
      for (std::vector<Int_t>::iterator itBot = botclusters.begin(); itBot != botclusters.end(); ++itBot) {
        std::cout << *itBot << " | ";
      }
      std::cout << std::endl;
    }

    // begin the hit reconstruction
    // loop structure:
    //
    // top clusters
    // |-calculate chargeweighted mean
    // |-bot clusters
    // |-|-calc chargew. mean
    // |-|-calc the crossing strip points
    // |-|-fill hit array

    // -----  merge top/bot clusters to hits  -----
    // loop on clusters from the top side
    for (std::vector<Int_t>::iterator itTop = topclusters.begin(); itTop != topclusters.end(); ++itTop) {
      topIndex = *itTop + clusterOffset; // index in fClusterArray
      Double_t topcharge = 0., meantopstrip = 0., meantoperr = 0., timestamp = 0., timestampError = 0.;
      PndSdsClusterStrip *aTopCluster = clusters[*itTop];
      oneclustertop = aTopCluster->GetClusterList();
      if (oneclustertop.size() < 1)
        continue;
      PndSdsDigiStrip *atopDigi = ((PndSdsDigiStrip *)fDigiArray->At(oneclustertop[0]));
      Int_t sensorIDtop = atopDigi->GetSensorID();

      // if (kFALSE==SelectSensorParams(detName)) continue; // Invalid parameters, skip here.
      CalcMeanCharge(aTopCluster, meantopstrip, meantoperr, topcharge, timestamp, timestampError);

      if (oneclustertop.size() == 1 && topcharge < fSingleStripChargeThreshold) {
        std::cout << "-W- PndSdsClusterTask::Exec: Single strip charge (" << topcharge << " e-) falls below the threshold of " << fSingleStripChargeThreshold << "e- : skipping. "
                  << endl;
        continue;
      }
      if (topcharge <= 0) { // not a sane charge
        Error("Exec() - Hit combination", "Not a sane top charge (%f) calculated, skip cluster %i", topcharge, *itTop);
        continue;
      }
      if (meantopstrip < 0) { // not a sane strip number
        Error("Exec() - Hit combination", "Not a sane top mean (%f) calculated, skip cluster %i", meantopstrip, *itTop);
        continue;
      }
      fCurrentStripCalcTop->CalcStripPointOnLine(meantopstrip, meantopPoint);
      // loop on bottom side
      for (std::vector<Int_t>::iterator itBot = botclusters.begin(); itBot != botclusters.end(); ++itBot) {
        botIndex = *itBot + clusterOffset; // index in fClusterArray
        Double_t botcharge = 0., meanbotstrip = 0., meanboterr = 0.;
        PndSdsClusterStrip *aBotCluster = clusters[*itBot];
        oneclusterbot = aBotCluster->GetClusterList();
        if (oneclusterbot.size() < 1)
          continue;
        PndSdsDigiStrip *abotDigi = ((PndSdsDigiStrip *)fDigiArray->At(oneclusterbot[0]));
        Int_t sensorIDbot = abotDigi->GetSensorID();

        // go to the next cluster if we didn't hit the same sensor
        if (sensorIDbot != sensorIDtop)
          continue;

        CalcMeanCharge(aBotCluster, meanbotstrip, meanboterr, botcharge, timestamp, timestampError);

        if (oneclusterbot.size() == 1 && botcharge < fSingleStripChargeThreshold) {
          std::cout << "-W- PndSdsClusterTask::Exec: Single strip charge (" << botcharge << " e-) falls below the threshold of " << fSingleStripChargeThreshold << "e- : skipping. "
                    << endl;
          continue;
        }
        if (botcharge <= 0) { // not a sane charge
          Error("Exec() - Hit combination", "Not a sane bot charge (%f) calculated, skip cluster %i", botcharge, *itBot);
          continue;
        }
        if (meanbotstrip < 0) { // not a sane strip number
          Error("Exec() - Hit combination", "Not a sane bot mean (%f) calculated, skip cluster %i", meanbotstrip, *itBot);
          continue;
        }

        if (fVerbose > 2) {
          std::cout << "Charges: Ctop = " << topcharge << " | Cbot = " << botcharge << " | difference bot-top = " << botcharge - topcharge << " | Cut at " << fChargeCut
                    << std::endl;
        }

        if (fChargeCut > 0 && fabs(botcharge - topcharge) < fChargeCut) { // look if the charges are not too differently
          mycharge = (botcharge + topcharge) / 2.;
          fCurrentStripCalcBot->CalcStripPointOnLine(meanbotstrip, meanbotPoint);
          mcindex = -1; // reset
          for (Int_t mcI = 0; mcI < atopDigi->GetNIndices(); mcI++) {
            if (atopDigi->GetIndex(mcI) > -1) {
              for (Int_t mcIb = 0; mcIb < abotDigi->GetNIndices(); mcIb++) {
                if (abotDigi->GetIndex(mcIb) == atopDigi->GetIndex(mcI)) {
                  mcindex = abotDigi->GetIndex(mcIb);
                  break;
                }
              }
            }
          }
          Bool_t test = Backmap(meantopPoint, meantoperr, meanbotPoint, meanboterr, hitPos, hitCov, sensorIDtop);
          if (kFALSE == test)
            continue;

          // --- add hit to list ---
          Int_t i = fHitArray->GetEntriesFast();
          hitErr.SetXYZ(sqrt(hitCov[0][0]), sqrt(hitCov[1][1]), sqrt(hitCov[2][2]));
          tmphit = new ((*fHitArray)[i]) PndSdsHit(clDetID, sensorIDtop, hitPos, hitErr, topIndex, mycharge, oneclusterbot.size() + oneclustertop.size(), mcindex);
          tmphit->SetBotIndex(botIndex);
          if (FairRootManager::Instance() != 0) {
            tmphit->SetLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), fClusterType, topIndex));
            tmphit->AddLink(FairLink(-1, FairRootManager::Instance()->GetEntryNr(), fClusterType, botIndex));
          }
          tmphit->SetCov(hitCov);
          tmphit->SetTimeStamp(timestamp);
          tmphit->SetTimeStampError(timestampError);
          if (fVerbose > 1)
            tmphit->Print();
        } else if (fVerbose > 2)
          std::cout << "Strip charge contents too different" << std::endl;
      } // loop bot clusters
    }   // loop top clusters
    for (size_t i = 0; i < clusters.size(); i++) {
      delete (clusters[i]);
    }
    clusters.clear();
  } // loop finders
  fHitArray->Sort();
  if (fVerbose > 1)
    std::cout << "-I- PndSdsStripClusterTask: " << fClusterArray->GetEntriesFast() << " Mvd Clusters and " << fHitArray->GetEntriesFast() << " Hits calculated."
              << " out of " << fDigiArray->GetEntriesFast() << " Digis" << std::endl;
  return;
}

// -------------------------------------------------------------------------
Bool_t PndSdsStripClusterTask::SelectSensorParams(Int_t sensorID)
{
  TString detpath = fGeoH->GetPath(sensorID);
  if (!(detpath.Contains("Strip")))
    return kFALSE;
  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) {
    const char *sensortype = digipar->GetSensType();
    if (detpath.Contains(sensortype)) {
      SetCurrentCalculators(digipar);
      return kTRUE;
    }
  }
  // no suiting object found
  if (fVerbose > 1)
    std::cout << "detector name does not contain a valid parameter name." << std::endl;
  if (fVerbose > 2)
    std::cout << " DetName : " << detpath << std::endl;
  return kFALSE;
}
// -------------------------------------------------------------------------

void PndSdsStripClusterTask::SetCurrentCalculators(PndSdsStripDigiPar *digipar)
{
  const char *sensortype = digipar->GetSensType();
  fCurrentStripCalcTop = fStripCalcTop[sensortype];
  fCurrentStripCalcBot = fStripCalcBot[sensortype];
  fCurrentClusterfinder = fClusterFinderList[sensortype];
  fCurrentChargeConverter = fChargeConverter[sensortype];
  fCurrentDigiPar = digipar;
  fChargeAlgos->SetCalcStrip(fCurrentStripCalcTop);
  fChargeAlgos->SetChargeConverter(fCurrentChargeConverter);
  fChargeCut = digipar->GetChargeCut();
  fSingleStripChargeThreshold = digipar->GetSingleChargeCut();
  return;
}

// -------------------------------------------------------------------------
void PndSdsStripClusterTask::ResetClusterFinders()
{
  // recursively clean the digis in the clusterfinder objects each event
  for (std::map<const char *, PndSdsStripClusterer *>::iterator CFiter = fClusterFinderList.begin(); CFiter != fClusterFinderList.end(); CFiter++) {
    (CFiter->second)->Reinit();
  }
}
// -------------------------------------------------------------------------
void PndSdsStripClusterTask::FillClusterFinders()
{
  // Info("FillClusterFinders","Here! fCurrntClusterfinder:%p",fCurrentClusterfinder);
  ResetClusterFinders();
  // a std::map is a SORTED container, it is sorted by the identifier
  Int_t sensorID;
  Int_t strip;
  SensorSide side;
  PndSdsDigiStrip *myDigi = 0;
  Bool_t tester = kFALSE;
  // Sort Digi indice into the clusterfinder
  if (fDigiArray->GetEntriesFast() == 0)
    return;
  if (fVerbose > 2)
    Info("FillClusterFinders", "adding these digis to the finders:");
  for (Int_t iDigi = 0; iDigi < fDigiArray->GetEntriesFast(); iDigi++) { // sort digis by sensor name and stripnumber
    myDigi = (PndSdsDigiStrip *)(fDigiArray->At(iDigi));
    if (fVerbose > 2) {
      std::cout << "Digi " << iDigi << " ";
      myDigi->Print();
      std::cout << fGeoH->GetPath(myDigi->GetSensorID()) << std::endl;
    }
    sensorID = myDigi->GetSensorID();
    tester = SelectSensorParams(sensorID);
    if (kFALSE == tester)
      continue; // Invalid parameters, skip here.
    // we use the top side as "first" side
    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
    // Time stamp measured in ns, cropped to integer for clusterfinding
    // fCurrentClusterfinder->AddDigi(sensorID,side,(Int_t)myDigi->GetTimeStamp(),strip,iDigi);
    fCurrentClusterfinder->AddDigi(sensorID, side, 1, strip, iDigi);
  }
  // make sure the digi array is distributed well
  fChargeAlgos->SetDigiArray(fDigiArray);
}

// -------------------------------------------------------------------------
TVector2 PndSdsStripClusterTask::CalcLineCross(TVector2 point1, TVector2 dir1, TVector2 point2, TVector2 dir2) const
{
  Double_t dx, dy, s, M, x, y;
  dx = point2.X() - point1.X();
  dy = point2.Y() - point1.Y();

  M = dir1.X() * dir2.Y() - dir1.Y() * dir2.X();

  if (M != 0.) {
    s = dir1.Y() * dx / M - dir1.X() * dy / M;
    x = point2.X() + dir2.X() * s;
    y = point2.Y() + dir2.Y() * s;
  } else {
    std::cout << "Warning in PndSdsStripClusterTask::CalcLineCross(): M=0 setting (x,y) to 0" << std::endl;
    x = 0.;
    y = 0.;
  }

  TVector2 result(x, y);
  return result;
}

void PndSdsStripClusterTask::Finish() {}

void PndSdsStripClusterTask::CalcMeanCharge(PndSdsClusterStrip *onecluster, Double_t &meanstrip, Double_t &meanerr, Double_t &charge, Double_t &timestamp, Double_t &timestampError)
{
  meanstrip = 0;
  meanerr = 0;
  charge = 0;
  timestamp = 0;
  timestampError = 0;
  Int_t nDigis = 0;

  //	if (fCurrentDigiPar->GetClusterMean() == 0)
  //	{
  //  // Calculate mean position in position channels weighted by the charges
  //  Int_t strip;
  //  SensorSide side;
  //  Double_t tempcharge;
  //  std::vector<Int_t> oneclusterlist = onecluster->GetClusterList();
  //  for (std::vector<Int_t>::iterator itDigi = oneclusterlist.begin();
  //       itDigi != oneclusterlist.end(); ++itDigi)
  //  { // calculate the mean charge and stripnumber
  //    PndSdsDigiStrip* myDigi = (PndSdsDigiStrip*)fDigiArray->At(*itDigi);
  //    std::cout << "Digi: " << *myDigi << std::endl;
  //    fCurrentStripCalcTop->CalcFeChToStrip(myDigi->GetFE(), myDigi->GetChannel(), strip, side);
  //    tempcharge = fCurrentChargeConverter->DigiValueToCharge(*myDigi);
  //    std::cout << "TempCharge: " << tempcharge << std::endl;
  //    charge += tempcharge;
  //    meanstrip += tempcharge * strip;
  //    meanerr += tempcharge*tempcharge; // this is stupid, to be removed one day
  //    Double_t var = myDigi->GetTimeStampError() * myDigi->GetTimeStampError();
  //    timestamp += myDigi->GetTimeStamp()/var;
  //    timestampError += 1/var;
  //    nDigis++;
  //  }
  //  meanstrip = meanstrip/charge;
  //  std::cout << "Charge: " << charge << std::endl;
  //  // this error treatment is: dx = dpitch * sqrt(weigthsquares)
  //  meanerr = sqrt(meanerr/(charge*charge));
  //  if (nDigis > 0){
  //  	timestamp /= timestampError;
  //  	timestampError = sqrt(timestampError / nDigis);
  //  }
  //  return;
  //	} else {
  //  //	//TODO: Apply other clusterfinder mean & error algorithms
  //  //    if(onecluster->GetSensorSide()==SensorSide::kTOP) fCurrentChargeAlgos->SetCalcStrip(fCurrentStripCalcTop);
  //  //    else fCurrentChargeAlgos->SetCalcStrip(fCurrentStripCalcBot);
  //  //    fChargeAlgos->SetChargeConverter(fCurrentChargeConverter); // done somewhere else
  //  std::pair<Double_t,Double_t> result = fChargeAlgos->CenterOfGravity(onecluster);
  //  meanstrip=result.first;
  //  meanerr=result.second;
  //  std::vector<Int_t> oneclusterlist = onecluster->GetClusterList();
  //  for (std::vector<Int_t>::iterator itDigi = oneclusterlist.begin();
  //       itDigi != oneclusterlist.end(); ++itDigi)
  //  {
  //    PndSdsDigiStrip* myDigi = (PndSdsDigiStrip*)fDigiArray->At(*itDigi);
  //    std::cout << "Digi: " << * myDigi << std::endl;
  //    std::cout << "TempCharge: " << fCurrentChargeConverter->DigiValueToCharge(*myDigi) << std::endl;
  //    charge += fCurrentChargeConverter->DigiValueToCharge(*myDigi);
  //    Double_t var = myDigi->GetTimeStampError() * myDigi->GetTimeStampError();
  //    timestamp += myDigi->GetTimeStamp()/var;
  //    timestampError += 1/var;
  //    nDigis++;
  //  }
  //  if (nDigis > 0){
  //  	timestamp /= timestampError;
  //  	timestampError = sqrt(timestampError / nDigis);
  //  }
  //
  //  std::cout << "Charge: " << charge << std::endl;
  //  return;
  //	}

  Int_t centroidMod = fCurrentDigiPar->GetClusterMean();
  PndSdsDigiStrip *tmpdigi = 0;

  std::pair<Double_t, Double_t> result;

  switch (centroidMod) { // Select a method by the number of the CenteroidAlgorithm

  case 1:
    // Binary (hightest signal)
    result = fChargeAlgos->Binary(onecluster);
    break;

  case 2:
    // Center of Gravity, Binary for cluster size 1
    result = fChargeAlgos->CenterOfGravity(onecluster);
    break;

  case 3:
    // Eta Algorithm for cluster size 2, Binary for cluster size 1, else CoG
    tmpdigi = (PndSdsDigiStrip *)(fDigiArray->At(onecluster->GetDigiIndex(0)));
    if (fGeoH->GetPath(tmpdigi->GetSensorID()).Contains("Fwd"))
      result = fChargeAlgos->Eta(onecluster, eta_trap);
    else
      result = fChargeAlgos->Eta(onecluster, eta_rect);
    break;

  case 4:
    // Head-Tail, Binary for cluster size 1
    result = fChargeAlgos->HeadTail(onecluster);
    break;

    // Center of Gravity, Binary for cluster size 1
  default: result = fChargeAlgos->CenterOfGravity(onecluster); break;
  }

  meanstrip = result.first;
  meanerr = result.second;

  std::vector<Int_t> oneclusterlist = onecluster->GetClusterList();
  for (std::vector<Int_t>::iterator itDigi = oneclusterlist.begin(); itDigi != oneclusterlist.end(); ++itDigi) {
    tmpdigi = (PndSdsDigiStrip *)fDigiArray->At(*itDigi);
    if (fVerbose > 3)
      Info("CalcMeanCharge:", "Added charge of digi %i in cluster is %f", nDigis, fCurrentChargeConverter->DigiValueToCharge(*tmpdigi));
    charge += fCurrentChargeConverter->DigiValueToCharge(*tmpdigi);
    Double_t var = tmpdigi->GetTimeStampError() * tmpdigi->GetTimeStampError();
    timestamp += tmpdigi->GetTimeStamp() / var;
    timestampError += 1 / var;
    nDigis++;
  }
  if (nDigis > 0) {
    timestamp /= timestampError;
    timestampError = sqrt(timestampError / nDigis);
  }
  return;
}

Bool_t PndSdsStripClusterTask::Backmap(TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr, TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID)
{
  // BACKMAPPING
  // get the backmapped point

  TVector3 localpos;
  TMatrixD locCov(3, 3);
  Double_t t, b;

  Double_t errZ = 2. * fGeoH->GetSensorDimensionsShortId(sensorID).Z() / TMath::Sqrt(12.0);

  TVector2 onsensorPoint = CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection());
  // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ(onsensorPoint.X(), onsensorPoint.Y(), 0.);

  // let's see if we're still on the sensor (cut combinations with noise off)
  if (fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X()))
    return kFALSE;
  if (fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y()))
    return kFALSE;

  // do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterShortId(localpos, sensorID);

  // calculate the errors corresponding to a skewed system,
  // directions orthogonal to strip orientation
  t = meantoperr * fCurrentDigiPar->GetTopPitch() * cos(fCurrentDigiPar->GetOrient());
  b = meanboterr * fCurrentDigiPar->GetBotPitch() * cos(fCurrentDigiPar->GetOrient() + fCurrentDigiPar->GetSkew());
  locCov[0][0] = t * t + b * b;
  t = meantoperr * fCurrentDigiPar->GetTopPitch() * sin(fCurrentDigiPar->GetOrient());
  b = meanboterr * fCurrentDigiPar->GetBotPitch() * sin(fCurrentDigiPar->GetOrient() + fCurrentDigiPar->GetSkew());
  locCov[1][1] = t * t + b * b;
  locCov[2][2] = errZ * errZ;

  // do the transformation from sensor to lab frame
  hitCov = fGeoH->LocalToMasterErrorsShortId(locCov, sensorID);
  // hitCov = locCov;
  return kTRUE;
}

ClassImp(PndSdsStripClusterTask);
