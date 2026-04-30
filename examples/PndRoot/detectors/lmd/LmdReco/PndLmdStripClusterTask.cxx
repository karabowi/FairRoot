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
// -----                PndLmdStripClusterTask source file             -----
// -----           modified for Lmd by M. Michel & A.Karavdina         -----
// -------------------------------------------------------------------------

// LUMI
#include "PndLmdStripClusterTask.h"
#include "PndLmdAlignPar.h"
#include "PndLmdContFact.h"
// PANDA
#include "PndSdsChargeWeightingAlgorithms.h"
#include "PndSdsIdealChargeConversion.h"
#include "PndSdsSimpleStripClusterFinder.h"
#include "PndSdsStripAdvClusterFinder.h"
#include "PndSdsTotChargeConversion.h"
#include "PndSdsTotDigiPar.h"
// FAIR
#include "FairBaseParSet.h"
#include "FairRun.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
// ROOT
#include "TDatabasePDG.h"
#include "TList.h"
#include "TLorentzVector.h"
// -----   Default constructor   -------------------------------------------

PndLmdStripClusterTask::PndLmdStripClusterTask() : PndSdsStripClusterTask("LMD Strip Clusterisation Task")
{
  fGeoH = PndGeoHandling::Instance();
  // fAlignParamList = new TList();
  flagMS = true;
}

// -----   Destructor   ----------------------------------------------------
PndLmdStripClusterTask::~PndLmdStripClusterTask()
{
  // if(0!=fGeoH)  delete fGeoH;
  // if(0!=fChargeAlgos) delete fChargeAlgos;
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndLmdStripClusterTask::Init()
{
  FairBaseParSet *par = (FairBaseParSet *)(rtdb->findContainer("FairBaseParSet"));
  fPbeam = par->GetBeamMom();

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
    std::cout << "-W- PndSdsStripClusterTask::Init: "
              << "No SDSDigi array!" << std::endl;
    return kERROR;
  }
  // set output arrays

  fClusterArray = ioman->Register(fClustBranchName, "PndSdsClusterStrip", fFolderName, GetPersistency());

  // fHitArray = new TClonesArray("PndSdsHit");
  fHitArray = ioman->Register(fOutBranchName, "PndSdsHit", fFolderName, GetPersistency());

  SetInBranchId();

  SetCalculators();

  // fPath = getenv("VMCWORKDIR");
  // fPath += "/macro/params/interstrippos_vs_eta_histos.root";

  // etahistofile = new TFile(fPath,"READ");
  // eta_rect = (TH1F*)etahistofile->Get("posvseta rect");
  // eta_trap = (TH1F*)etahistofile->Get("posvseta trap");

  Info("Init", "Initialisation successfull");
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndLmdStripClusterTask::SetBranchNames(TString inBranchname, TString outHitBranchname, TString outClustBranchname, TString folderName)
{
  fInBranchName = inBranchname;
  fOutBranchName = outHitBranchname;
  fClustBranchName = outClustBranchname;
  fFolderName = folderName;
}

void PndLmdStripClusterTask::SetBranchNames()
{
  fInBranchName = "LMDStripDigis";
  fOutBranchName = "LMDHitsStrip";
  fClustBranchName = "LMDStripClusterCand";
  fFolderName = "PndLmd";
}

// -----   Initialization  of Parameter Containers -------------------------
void PndLmdStripClusterTask::SetParContainers()
{
  // called from the FairRun::Init()
  // Caution: The Parameter Set is not filled from the DB IO, yet.
  // This will be done just before this Tasks Init() is called.
  ana = FairRun::Instance();
  rtdb = ana->GetRuntimeDb();

  PndLmdContFact *themvdcontfact = (PndLmdContFact *)rtdb->getContFactory("PndLmdContFact");
  TList *theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString *contname = (TObjString *)cfIter()) {
    TString parsetname = contname->String();
    Info("SetParContainers()", "%s", parsetname.Data());
    if (parsetname.BeginsWith("SDSStripDigiPar")) {
      PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)(rtdb->getContainer(parsetname.Data()));
      if (!digipar)
        Fatal("SetParContainers", "No DIGI parameter found: %s", parsetname.Data());
      fDigiParameterList->Add(digipar);
    }
    if (parsetname.BeginsWith("SDSStripTotDigiPar")) {
      PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)(rtdb->getContainer(parsetname.Data()));
      if (!totdigipar)
        Fatal("SetParContainers", "No TOT parameter found: %s", parsetname.Data());
      fChargeDigiParameterList->Add(totdigipar);
    }
  } // while

  // //read params for lumi alignment
  // TList* theAlignLMDContNames = themvdcontfact->GetAlignParNames();
  // Info("SetParContainers()","AlignLMD The container names list contains %i
  // entries",theAlignLMDContNames->GetEntries());
  // TIter cfAlIter(theAlignLMDContNames);
  // while (TObjString* contname = (TObjString*)cfAlIter()) {
  //   TString parsetname = contname->String();
  //   Info("SetParContainers()",parsetname.Data());
  //   PndLmdAlignPar *lmdalignpar =
  //   (PndLmdAlignPar*)(rtdb->getContainer(parsetname.Data()));
  //   if(!lmdalignpar) Fatal("SetParContainers","No ALIGN parameter found:
  //   %s",parsetname.Data());
  //   fAlignParamList->Add(lmdalignpar);
  // }
  PndSdsStripClusterTask::SetParContainers();
}

void PndLmdStripClusterTask::SetCalculators()
{
  Info("SetCalculators", "lmd");
  PndSdsStripClusterTask::SetCalculators();
  TIter params(fDigiParameterList);
  TIter totparams(fChargeDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)params()) {
    PndSdsTotDigiPar *totdigipar = (PndSdsTotDigiPar *)totparams();
    if (0 == digipar)
      continue;
    const char *senstype = digipar->GetSensType();
    if (digipar->GetChargeConvMethod() == 1) {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Tot charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] =
        new PndSdsTotChargeConversion(totdigipar->GetChargingTime(), totdigipar->GetConstCurrent(), digipar->GetThreshold(), totdigipar->GetClockFrequency(), fVerbose);
    } else {
      if (fVerbose > 0)
        Info("SetCalculators()", "Use Ideal charge conversion for %s sensors", senstype);
      fChargeConverter[senstype] = new PndSdsIdealChargeConversion();
    }

    Int_t ClusterMod = digipar->GetClusterMod();
    Int_t RadChannel = digipar->GetRadChannel();
    Int_t RadTime = digipar->GetRadTime();
    if (0 == ClusterMod) {
      fClusterFinderList[senstype] = new PndSdsSimpleStripClusterFinder(fInBranchId, RadChannel); // search radius in channel no.
    } else if (1 == ClusterMod) {
      fClusterFinderList[senstype] = new PndSdsStripAdvClusterFinder(fInBranchId, RadChannel, RadTime);
    }
  }

  // TIter alignparams(fAlignParamList);
  // PndLmdAlignPar* lmdalignpar=(PndLmdAlignPar*)alignparams();
  // if(0==lmdalignpar) {
  //   Error("PndLmdStripClusterTask::SetCalculators()","A ALIGN Parameter Set
  //   does not exist properly.");
  // }
  // else{
  //   //   lmdalignpar->Print();
  //   for(int ik=0;ik<32;ik++){
  //     fShiftX[ik] = lmdalignpar->GetShiftX(ik);
  //     fShiftY[ik] = lmdalignpar->GetShiftY(ik);
  //     fShiftZ[ik] = lmdalignpar->GetShiftZ(ik);
  //     fRotateX[ik] = lmdalignpar->GetRotateX(ik);
  //     fRotateY[ik] = lmdalignpar->GetRotateY(ik);
  //     fRotateZ[ik] = lmdalignpar->GetRotateZ(ik);
  //     if (fVerbose > 2) cout<<"fShiftX["<<ik<<"]="<<fShiftX[ik]<<"
  //     fRotateX["<<ik<<"]="<<fRotateX[ik]
  // 			    <<" fRotateY["<<ik<<"]="<<fRotateY[ik]<<"
  // fRotateZ["<<ik<<"]="<<fRotateZ[ik]<<endl;
  //   }
  // }
  // lmdalignpar->Print();
}

TVector3 PndLmdStripClusterTask::AddMSErr(TVector3 hpos, TVector3 hposerr)
{
  if (fVerbose > 0)
    Info("AddMSErr", "calculation additional errors due to multiple scaterring");

  // Calculation of ThetaMS -------------------------------------
  // Charge & mass of particle
  Int_t PDGCode = -2212;
  TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
  TParticlePDG *fParticle = fdbPDG->GetParticle(PDGCode);
  Double_t fMass = fParticle->Mass();

  Double_t Ebeam = TMath::Hypot(fPbeam, fMass);
  TLorentzVector LorMom(0, 0, fPbeam, Ebeam);
  Double_t beta = LorMom.Beta();
  Double_t X = 0.015;
  Double_t X0 = 9.37;
  // Double_t thetaMS =
  // 13.6*1e-3*TMath::Sqrt(X/X0)*(1+0.038*TMath::Log(X/X0))/(beta*fPbeam);
  Double_t thetaMS = 13.6 * 1e-3 * TMath::Sqrt(X / X0) / (beta * fPbeam);
  // cout<<"thetaMS = "<<thetaMS<<" fPbeam = "<<fPbeam<<endl;
  //-----------------------------------------------------------

  // TO DO: use parameters from geometry info for LUMI
  Double_t d = 10.;

  double xerr, yerr;
  double zhit = hpos.Z();
  //  const double Z0 = 1100.;
  const double Z0 = 1099.;
  // const double Z0 = 0.;
  int num = (zhit - Z0) / d;
  //  double numd = (zhit-Z0)/10.;
  // cout<<"num = "<<num<<endl;
  xerr = hposerr.X();
  yerr = hposerr.Y();
  // cout<<"Plane #"<<num<<" before: zhit="<<zhit<<" xerr = "<<xerr<<" yerr =
  // "<<yerr<<endl;
  // Double_t sigmaMSplane = 0.5*X*thetaMS; //TEST
  Double_t sigmaMSplane = X * thetaMS;
  //  cout<<"sigmaMSplane = "<<sigmaMSplane<<" um"<<endl;
  if (num == 0) {
    xerr = TMath::Hypot(xerr, sigmaMSplane);
    yerr = TMath::Hypot(yerr, sigmaMSplane);
  }
  // double xhit = hpos.X(); //[R.K. 01/2017] unused variable

  //  Double_t l = 10./cos(2.326*TMath::Pi()/180.);
  double sigmaMS;
  for (int j = 0; j < num; j++) {
    // sigmaMS = 2*(j+1)*d*thetaMS;
    sigmaMS = (j + 1) * d * thetaMS;
    // cout<<"sigmaMS = "<<sigmaMS<<" xerr="<<xerr<<" yerr="<<yerr<<endl;
    // sigmaMS = j*d*thetaMS;
    xerr = TMath::Hypot(xerr, sigmaMS);
    yerr = TMath::Hypot(yerr, sigmaMS);
  }
  if (fVerbose > 2)
    cout << " num:" << num << "(Z=" << zhit << ") xerr=" << xerr << " yerr=" << yerr << endl;
  TVector3 res(xerr, yerr, hposerr.Z());
  return res;
};

// -----   Public method Exec   --------------------------------------------
void PndLmdStripClusterTask::Exec(Option_t *)
{
  if (fVerbose > 2)
    std::cout << " **Starting PndLmdStripClusterTask::Exec()**" << std::endl;
  std::vector<PndSdsDigiStrip> digiStripArray;
  // Reset output array
  fClusterArray = FairRootManager::Instance()->GetTClonesArray(fClustBranchName);
  if (!fClusterArray)
    Fatal("Exec", "No ClusterArray");
  fClusterArray->Delete();

  fHitArray = FairRootManager::Instance()->GetTClonesArray(fOutBranchName);
  if (!fHitArray)
    Fatal("Exec", "No HitArray");
  fHitArray->Delete();

  // Get input array

  if (FairRunAna::Instance()->IsTimeStamp()) {
    fDigiArray->Clear();
    fDigiArray = FairRootManager::Instance()->GetData(fInBranchName, fFunctor,
                                                      FairRootManager::Instance()->GetEventTime() + 10); // FairRootManager::Instance()->GetEventTime() +
    if (fVerbose > 1)
      LOG(info) << " PndLmdStripClusterTask::Exec Digis: " << fDigiArray->GetEntries();
  } else
    fDigiArray = (TClonesArray *)FairRootManager::Instance()->GetObject(fInBranchName);

  if (!fDigiArray) {
    std::cout << "-W- PndLmdStripClusterTask::Init: "
              << "No LMDDigi array!" << std::endl;
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
  // Int_t detID = FairRootManager::Instance()->GetBranchId(fInBranchName);
  // //[R.K. 01/2017] unused variable
  Int_t clDetID = FairRootManager::Instance()->GetBranchId(fClustBranchName);
  Double_t mycharge;
  TVector2 meantopPoint, meanbotPoint, onsensorPoint;
  TVector3 hitPos, hitErr;
  TMatrixD hitCov(3, 3);
  Int_t clusterOffset = 0;
  PndSdsHit *tmphit;

  // -------   SEARCH  ------
  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) { // loop over all parameter sets and their filled
                                                                             // finders (representing sensor types)
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
    // Fill the ClonesArray for output TODO: do this better, don't copy objects
    // around
    // save array size before we fill
    clusterOffset = fClusterArray->GetEntriesFast();
    for (std::vector<PndSdsClusterStrip *>::iterator clit = clusters.begin(); clit != clusters.end(); ++clit) {
      clindex = fClusterArray->GetEntriesFast();
      PndSdsClusterStrip *myCluster = new ((*fClusterArray)[clindex]) PndSdsClusterStrip(*(*clit));

      if (FairRunAna::Instance()->IsTimeStamp()) {
        myCluster->ResetLinks();
        for (Int_t i = 0; i < myCluster->GetClusterSize(); i++) {
          PndSdsDigiStrip *tempDigi = (PndSdsDigiStrip *)fDigiArray->At(myCluster->GetDigiIndex(i));
          myCluster->AddLink(FairLink(tempDigi->GetEntryNr()));
        }
      }
    }

    // printout for checking
    if (fVerbose > 2) {
      std::cout << "Check.. Offset: " << clusterOffset << "Top Clusters: ";
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

      // if (kFALSE==SelectSensorParams(detName)) continue; // Invalid
      // parameters, skip here.
      CalcMeanCharge(aTopCluster, meantopstrip, meantoperr, topcharge, timestamp, timestampError);

      if (oneclustertop.size() == 1 && topcharge < fSingleStripChargeThreshold) {
        std::cout << "-W- PndLmdStripClusterTask::Exec: Single strip charge (" << topcharge << " e-) falls below the threshold of " << fSingleStripChargeThreshold
                  << "e- : skipping. " << endl;
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
          std::cout << "-W- PndLmdStripClusterTask::Exec: Single strip charge (" << botcharge << " e-) falls below the threshold of " << fSingleStripChargeThreshold
                    << "e- : skipping. " << endl;
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
          tmphit->SetLink(FairLink(fClusterType, topIndex));
          tmphit->AddLink(FairLink(fClusterType, botIndex));
          tmphit->SetCov(hitCov);
          tmphit->SetTimeStamp(timestamp);
          tmphit->SetTimeStampError(timestampError);
          if (fVerbose > 1)
            tmphit->Print();
        } else if (fVerbose > 2)
          std::cout << "Strip charge contents too different" << std::endl;
      } // loop bot clusters
    }   // loop top clusters
  }     // loop finders

  if (fVerbose > 1)
    std::cout << "-I- PndLmdStripClusterTask: " << fClusterArray->GetEntriesFast() << " Lmd Clusters and " << fHitArray->GetEntriesFast() << " Hits calculated."
              << " out of " << fDigiArray->GetEntriesFast() << " Digis" << std::endl;
  return;
}
void PndLmdStripClusterTask::combitransToLumiFrame(TVector3 &hitPos)
{
  // do the transformation from lab frame to LUMI frame (with z-axis perp. to
  // lumi planes)
  // const Double_t  kHalfFoilThickness  = 0.0075; // Thickness of sensitive
  // foil (cm) //[R.K. 01/2017] unused variable
  const Double_t kTransZ = 1100.;  //(cm) //move at z-position
  const Double_t kRotUmZ = 476.03; //(cm) //z-point to rotate
  // const Double_t  kTransX = 25; //(cm) //move at x-position //[R.K. 01/2017]
  // unused variable
  const Double_t kRot = 0.040596358401388; // 2.326 degree  = 4.05963584013881024e-02 rad
  TVector3 LumiTrans(0, 0, kRotUmZ);
  hitPos -= LumiTrans;
  hitPos.RotateY(-kRot);
  LumiTrans = TVector3(0, 0, kTransZ - kRotUmZ);
  hitPos -= LumiTrans;
  // cout<<"!!! NEW HIT position in LUMI frame!!! "<<endl;
  //  hitPos.Print();
}

void PndLmdStripClusterTask::rotateToLumiFrame(TVector3 &hitPos)
{
  TMatrixD hitMtx(3, 3);
  hitMtx[0][0] = hitPos[0];
  hitMtx[1][0] = hitPos[1];
  hitMtx[2][0] = hitPos[2];
  TMatrixD res = rotateToLumiFrame(hitMtx);
  hitPos = TVector3(hitMtx(0, 0), hitMtx(1, 0), hitMtx(2, 0));
}
TMatrixD PndLmdStripClusterTask::rotateToLumiFrame(TMatrixD &hitCov)
{
  Double_t theta = 2.326;
  Double_t degrad = TMath::Pi() / 180.;
  Double_t sintheta = TMath::Sin(degrad * theta);
  Double_t costheta = TMath::Cos(degrad * theta);
  TMatrixD rot(3, 3); // Rotation around Y axis
  rot[0][0] = costheta;
  rot[0][1] = 0;
  rot[0][2] = sintheta;
  rot[1][0] = 0;
  rot[1][1] = 1;
  rot[1][2] = 0;
  rot[2][0] = -sintheta;
  rot[2][1] = 0;
  rot[2][2] = costheta;
  TMatrixD result = rot;
  result.T();
  result *= hitCov;
  hitCov = result;
  result *= rot;
  return result;
}

// //Correction to hit position due to misalignment of sensor
// //TO DO: find a way do it in global and not hit by hit.
// void PndLmdStripClusterTask::alignmentCorr(TVector3& hitPos, int sensID){
//   TVector3 hitPos_loc(hitPos.X(),hitPos.Y(),0.);
//   hitPos_loc -=TVector3(fShiftX[sensID],fShiftY[sensID],fShiftZ[sensID]);
//   double xnew =
//   hitPos_loc.X()+fRotateZ[sensID]*hitPos_loc.Y()-fRotateY[sensID]*hitPos_loc.Z();
//   double ynew =
//   hitPos_loc.Y()-fRotateZ[sensID]*hitPos_loc.X()+fRotateX[sensID]*hitPos_loc.Z();
//   double znew =
//   hitPos_loc.Z()-fRotateY[sensID]*hitPos_loc.X()-fRotateX[sensID]*hitPos_loc.Y();
//   hitPos = TVector3(xnew,ynew,hitPos.Z()+znew);
// }

Bool_t PndLmdStripClusterTask::Backmap(TVector2 meantopPoint, Double_t meantoperr, TVector2 meanbotPoint, Double_t meanboterr, TVector3 &hitPos, TMatrixD &hitCov, Int_t &sensorID)
{
  // BACKMAPPING
  // get the backmapped point
  //  cout<<"PndLmdStripClusterTask::BACKMAP"<<endl;
  // Info("Backmap","Sensor ID is %s",sensorID);
  TVector3 localpos;
  TMatrixD locCov(3, 3);
  Double_t t, b;
  //  cout<<"sensorID = "<<sensorID<<endl;
  //  cout<<"fGeoH = "<<fGeoH<<endl;
  Double_t errZ = 2. * fGeoH->GetSensorDimensionsShortId(sensorID).Z() / TMath::Sqrt(12.0);
  // cout<<"fGeoH->GetSensorDimensionsShortId(sensorID).Z() =
  // "<<fGeoH->GetSensorDimensionsShortId(sensorID).Z()<<endl;
  //  Double_t errZ =
  //  fGeoH->GetSensorDimensionsShortId(sensorID).Z()/TMath::Sqrt(12.0);//TEST!!!
  // cout<<"errZ = "<<errZ<<endl;

  TVector2 onsensorPoint = CalcLineCross(meantopPoint, fCurrentStripCalcTop->GetStripDirection(), meanbotPoint, fCurrentStripCalcBot->GetStripDirection());
  // // here we assume the sensor system to be in the _Middle_ of the volume
  localpos.SetXYZ(onsensorPoint.X(), onsensorPoint.Y(), 0.);
  //  here we assume the sensor system to be in the _surface_ of the volume
  //  localpos.SetXYZ( onsensorPoint.X(),
  //  onsensorPoint.Y(),-(fGeoH->GetSensorDimensionsShortId(sensorID).Z()));
  // let's see if we're still on the sensor (cut combinations with noise off)
  if (fabs(localpos.X()) > fabs(fCurrentDigiPar->GetTopAnchor().X()))
    return kFALSE;
  if (fabs(localpos.Y()) > fabs(fCurrentDigiPar->GetTopAnchor().Y()))
    return kFALSE;

  // do the transformation from sensor to lab frame
  hitPos = fGeoH->LocalToMasterShortId(localpos, sensorID);

  /// TODO: think how to make alignment with using Kalman fillter
  // // //do the transformation from lab frame to LUMI frame (with z-axis perp.
  // to lumi planes)
  // combitransToLumiFrame(hitPos);

  // //do correction due to misalignemt of sensor
  // alignmentCorr(hitPos,sensorID);

  // calculate the errors corresponding to a skewed system!
  t = meantoperr * fCurrentDigiPar->GetTopPitch() * cos(fCurrentDigiPar->GetOrient());
  b = meanboterr * fCurrentDigiPar->GetBotPitch() * cos(fCurrentDigiPar->GetOrient() + fCurrentDigiPar->GetSkew());
  locCov[0][0] = t * t + b * b + 2 * fabs(t * b * cos(fCurrentDigiPar->GetSkew())); // TEST
  t = meantoperr * fCurrentDigiPar->GetTopPitch() * sin(fCurrentDigiPar->GetOrient());
  b = meanboterr * fCurrentDigiPar->GetBotPitch() * sin(fCurrentDigiPar->GetOrient() + fCurrentDigiPar->GetSkew());
  locCov[1][1] = t * t + b * b + 2 * fabs(t * b * cos(fCurrentDigiPar->GetSkew())); // TEST
  locCov[2][2] = errZ * errZ;

  if (flagMS) {
    // //Add uncertanty due to multiple scattering
    TVector3 hitErr(sqrt(locCov[0][0]), sqrt(locCov[1][1]), sqrt(locCov[2][2]));
    TVector3 hitErrMSadd = AddMSErr(hitPos, hitErr);
    locCov[0][0] = TMath::Power(hitErrMSadd.X(), 2);
    locCov[1][1] = TMath::Power(hitErrMSadd.Y(), 2);
    locCov[2][2] = TMath::Power(hitErrMSadd.Z(), 2);
  }

  // do the transformation from sensor to lab frame
  hitCov = fGeoH->LocalToMasterErrorsShortId(locCov, sensorID);

  /// TODO: think how to make alignment with using Kalman fillter
  // //transformation to LUMI frame
  // hitCov = rotateToLumiFrame(hitCov);

  return kTRUE;
}

ClassImp(PndLmdStripClusterTask);
