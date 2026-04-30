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

//----------------------------------------------------------------------
// File and Version Information:
//      $Id: //
// Description:
//      Class PndEmcAnalysis. Module to take the ADC waveforms and produces digi.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother                  Original Author
// Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndEmcAnalysis.h"
#include "PndEmcWaveform.h"
#include "PndEmcDigi.h"
#include "PndEmcBump.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "FairEventHeader.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "PndMCTrack.h"
#include <iostream>
#include "PndEmcClusterCalibrator.h"
#include "PndPidCandidate.h"

using std::cout;
using std::endl;
using std::fstream;

static const Int_t NSubDetectors = 15;
static DetectorId detIds[NSubDetectors] = {DetectorId::kRICH, DetectorId::kDRC, DetectorId::kDSK, DetectorId::kEMC,  DetectorId::kGEM,
                                           DetectorId::kLUMI, DetectorId::kMDT, DetectorId::kMVD, DetectorId::kRPC,  DetectorId::kSTT,
                                           DetectorId::kFTOF, DetectorId::kTOF, DetectorId::kFTS, DetectorId::kHYPG, DetectorId::kHYP};

struct key {
  key(Int_t hit, Double_t e) : iHit(hit), dE(e) {}
  Int_t iHit;
  Double_t dE;
  bool operator<(const key &rhs) const { return dE < rhs.dE; }
  bool operator>(const key &rhs) const { return dE > rhs.dE; }
};
// struct IndexKey
//{
//	IndexKey(Int_t oIdx, Int_t newIdx) : GenIdx(oIdx), EnergyIdx(newIdx){}
//	Int_t GenIdx;
//	Int_t EnergyIdx;
//	bool operator < (const IndexKey& rhs) const {
//		return GenIdx < rhs.GenIdx;
//	}
//	bool operator > (const IndexKey& rhs) const {
//		return GenIdx > rhs.GenIdx;
//	}
//};
// bool operator< (const key& lhs, const key& rhs) {
//	return lhs.dE < rhs.dE;
//}
// bool operator> (const key& lhs, const key& rhs) {
//	return lhs.dE > rhs.dE;
//}
// ostream& operator<<(ostream& os, const TVector3& mom) {
//	os<<"("<<mom.Px()<<", "<<mom.Py()<<", "<<mom.Pz()<<")";
//	os<<std::endl;
//	return os;
//}

PndEmcAnalysis::PndEmcAnalysis(Int_t verbose, Bool_t storeFile)
  : fEmcHitsArray(nullptr), fWaveformArray(nullptr), fDigiArray(nullptr), fSharedDigiArray(nullptr), fClusterArray(nullptr), fBumpArray(nullptr), fMcTrackArray(nullptr),
    fEvtHeaderArray(nullptr), fVerbose(verbose), fStoreRooTFile(storeFile)
{
  fileName = "PndEmcAnalysis.root";
  fTimeOrderedDigi = kFALSE;
  fSaveWave = kFALSE;
  fSaveDigi = kFALSE;
  fSaveBump = kFALSE;
  fSaveTask = kFALSE;
  fSaveHits = kFALSE;
  fSaveMcTruth = kFALSE;
  fSaveReco = kFALSE;
  fSavePidCharged = kFALSE;
  fSavePidNeutral = kFALSE;
  fSaveJpsiTo3g = kFALSE;
}

//--------------
// Destructor --
//--------------
//#define TIMEBASEDSIM
PndEmcAnalysis::~PndEmcAnalysis() {}

InitStatus PndEmcAnalysis::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcAnalysis::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  fEmcHitsArray = (TClonesArray *)ioman->GetObject("EmcHit");
  if (!fEmcHitsArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No EmcHit array!" << endl;
    fSaveHits = kFALSE;
  }
  // Get input array
  if (fTimeOrderedDigi) {
    fWaveformArray = (TClonesArray *)ioman->GetObject("EmcSortedWaveform");
  } else {
    fWaveformArray = (TClonesArray *)ioman->GetObject("EmcWaveform");
  }
  // fEvtHeaderArray = (TClonesArray*) ioman->GetObject("EventHeader.");
  if (!fWaveformArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PndEmcWaveform array!" << endl;
    fSaveWave = kFALSE;
  }

  fDigiArray = (TClonesArray *)ioman->GetObject("EmcDigi");
  if (!fDigiArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PndEmcDigi array!" << endl;
    fSaveDigi = kFALSE;
  }
  fSharedDigiArray = (TClonesArray *)ioman->GetObject("EmcSharedDigi");
  if (!fSharedDigiArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PndEmcSharedDigi array!" << endl;
    fSaveDigi = kFALSE;
  }
  fMcTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMcTrackArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No MCTrack array!" << endl;
    fSaveMcTruth = kFALSE;
  }
  fClusterArray = (TClonesArray *)ioman->GetObject("EmcCluster");
  if (!fClusterArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PndEmcCluster array!" << endl;
    // fSaveBump = kFALSE;
  }
  fBumpArray = (TClonesArray *)ioman->GetObject("EmcBump");
  if (!fBumpArray) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PndEmcBump array!" << endl;
    fSaveBump = kFALSE;
  }
  fChargedCand = (TClonesArray *)ioman->GetObject("PidChargedCand");
  if (!fChargedCand) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PidChargedCand array!" << endl;
    fSavePidCharged = kFALSE;
  }
  fNeutralCand = (TClonesArray *)ioman->GetObject("PidNeutralCand");
  if (!fNeutralCand) {
    cout << "-W- PndEmcAnalysis::Init: "
         << "No PidNeutralCand array!" << endl;
    fSavePidNeutral = kFALSE;
  }

  fFunctor = new StopTime();
  fStoreRooTFile = fSaveDigi || fSaveWave || fSaveBump || fSaveTask || fTimeOrderedDigi || fSaveHits || fSaveMcTruth || fSaveReco;

  if (fStoreRooTFile)
    fRootFile = new TFile(fileName, "RECREATE");

  if (fSaveHits) {
    tHit = new TTree("hit", "hit data");
    tHit->Branch("ie", &fEventNo, "EventNO/I");
    tHit->Branch("ht", &fHitTime, "HitTime/D");
    tHit->Branch("mod", &fModule, "Module/I");
    tHit->Branch("he", &fHitEnergy, "HitEnergy/D");
    tHit->Branch("theta", &fTheta, "Theta/D");
    tHit->Branch("phi", &fPhi, "Phi/D");
    tHit->Branch("xcor", &fXcor, "XCoor/I");
    tHit->Branch("ycor", &fYcor, "XCoor/I");
  }

  if (fSaveWave) {
    tWave = new TTree("wave", "wave data");
    tWave->Branch("et", &fEvtTime, "EventTime/D");
    tWave->Branch("mod", &fModule, "Module/I");
    tWave->Branch("hitid", &fHitIndex, "HitIndex/I");
    tWave->Branch("wavet", &fWaveTime, "WaveTime/D");
    tWave->Branch("peak", &fWavePeak, "WavePeak/D");
    tWave->Branch("wl", &fWaveLen, "WaveLength/I");
    tWave->Branch("iw", &fWaveIdx, "WaveIndex/I");
    tWave->Branch("ie", &fEventNo, "EventNO/I");
    tWave->Branch("baseE", &fBaseline, "BaseLine/D");
    tWave->Branch("xcor", &fXcor, "XCoor/I");
    tWave->Branch("ycor", &fYcor, "YCoor/I");
    tWave->Branch("wpc", &fPileupCount, "Pileup/I");
    tWave->Branch("he", &fHitEnergy, "HitEnergy/D");
  }
  if (fSaveDigi) {
    tDigi = new TTree("digi", "digi data");
    tDigi->Branch("digit", &fDigiTime, "DigiTime/D");
    tDigi->Branch("et", &fEvtTime, "EventTime/D");
    tDigi->Branch("mod", &fModule, "Module/I");
    tDigi->Branch("wt", &fWaveTime, "WaveTime/D");
    tDigi->Branch("hite", &fHitEnergy, "HitEnergy/D");
    tDigi->Branch("e", &fDigiEnergy, "Energy/D");
    tDigi->Branch("r", &fDistance, "Path/D");
    tDigi->Branch("iw", &fWaveIdx, "WaveIndex/I");
    tDigi->Branch("ie", &fEventNo, "EventNO/I");
    tDigi->Branch("pos", &fPosition[0], "Position[3]/D");
  }
  if (fSaveBump) {
    tBump = new TTree("bump", "bump data");
    tBump->Branch("et", &fEvtTime, "EventTime/D");
    tBump->Branch("ie", &fEventNo, "EventNO/I");
    tBump->Branch("mod", &fModule, "Module/I");
    tBump->Branch("bt1", &fBumpTime1, "WeightedTimeByTimeError/D");
    tBump->Branch("bt2", &fBumpTime2, "WeightedTimeByTimeErrorAndEnergy/D");
    tBump->Branch("bt3", &fBumpTime3, "WeightedTimeByEnergy/D");
    tBump->Branch("seedt", &fSeedTime, "SeedTime/D");
    tBump->Branch("e", &fBumpEnergy, "BumpEnergy/D");
    tBump->Branch("pos", &fPosition[0], "Position[3]/D");
    tBump->Branch("seedpos", &fSeedPosition[0], "SeedPosition[3]/D");
    tBump->Branch("ndigi", &fNumDigi, "NumOfDigi/I");
    tBump->Branch("nbump", &fNumBump, "NumofBump/I");
    tBump->Branch("ic", &fClusterIdx, "ClusterIndex/I");
    tBump->Branch("nseed", &fNumOfSeed, "NumOfSeed/I");
    tBump->Branch("nmap", &fDigiMapSize, "NumDigiMap/I");
  }
  if (fSaveTask) {
    tTask = new TTree("task", "task data");
    tTask->Branch("ie", &fEventNo, "EventNO/I");
    tTask->Branch("nhits", &fNumOfHits, "NumOfEmcHit/I");
    tTask->Branch("nwave", &fNumWaveform, "NumOfWaveform/I");
    tTask->Branch("ndigi", &fNumDigi, "NumOfDigi/I");
    tTask->Branch("sdigi", &fNumSharedDigi, "NumOfSharedDigi/I");
    tTask->Branch("nclus", &fNumCluster, "NumOfCluster/I");
    tTask->Branch("nbump", &fNumBump, "NumOfBump/I");
    tTask->Branch("ntrack", &fNumMcTrack, "NumOfMcTrack/I");
    tTask->Branch("totde", &totDigiEnergy, "TotalDigiEnergy/D");
    tTask->Branch("totse", &totSharedDigiEnergy, "TotalSharedDigiEnergy/D");
  }
  if (fTimeOrderedDigi) {
    tEvtPileup = new TTree("evtpu", "event pileup data");
    tEvtPileup->Branch("evtNo", &fEventNo, "evtNo/I");
    tEvtPileup->Branch("totw", &fTotWave, "totw/I");
    tEvtPileup->Branch("pilew", &fPileupWave, "pilew/I");

    tTaskPileup = new TTree("taskpu", "task pileup data");
    tTaskPileup->Branch("nw", &totNumOfWave, "TotNumOfWave/I");
    tTaskPileup->Branch("nsw", &totNumOfSubWave, "TotNumOfSubWave/I");
    tTaskPileup->Branch("nd", &totNumOfDigi, "TotNumOfDigi/I");
    tTaskPileup->Branch("nsd", &totNumOfSharedDigi, "TotNumOfSharedDigi/I");
    tTaskPileup->Branch("nevt", &totNumOfEvents, "TotNumOfEvents/I");
    tTaskPileup->Branch("npevt", &totNumOfPileupEvents, "TotNumOfPileupEvents/I");
    tTaskPileup->Branch("match", &fNumOfMatch, "NumOfMatch/I");
    tTaskPileup->Branch("mismatch", &fNumOfDismatch, "NumOfDismatch/I");
  }
  if (fSaveReco) {
    tRec = new TTree("ResA", "Reconstruction");
    tRec->Branch("E1", &fEnergy[0], "E1[30]/D");
    tRec->Branch("E1C", &fEnergyC1[0], "E1C[30]/D");
    tRec->Branch("E2C", &fEnergyC2[0], "E2C[30]/D");
    tRec->Branch("p1", &fp4[0], "p1[120]/D");
    tRec->Branch("Mcp1", &fMcp4[0], "Mcp1[120]/D");
    tRec->Branch("pdg", &fPDGCode[0], "pdg[30]/I");
    tRec->Branch("Z201", &fZ201[0], "Z201[30]/D");
    tRec->Branch("Z531", &fZ531[0], "Z531[30]/D");
    tRec->Branch("Lat1", &fLat1[0], "Lat1[30]/D");
    tRec->Branch("pos", &fPositionV[0], "Position[30]/D");
    tRec->Branch("Mod1", &fModule1[0], "Mod1[30]/I");
    tRec->Branch("Theta1", &fTheta1[0], "Theta1[30]/D");
    tRec->Branch("Phi1", &fPhi1[0], "Phi1[30]/D");
    tRec->Branch("ExtE1", &fRestEnergy1, "ExtE1/D");
    tRec->Branch("ExtE2", &fRestEnergy2, "ExtE2/D");
    tRec->Branch("ExtE3", &fRestEnergy3, "ExtE3/D");
    tRec->Branch("NBump", &fNumBump, "NBump/I");
  }
  if (fSaveMcTruth) {
    tMcTruth = new TTree("Res1A", "Mc Truth");
    tMcTruth->Branch("Mcp1", &fMcp4[0], "Mcp1[120]/D");
    tMcTruth->Branch("Theta1", &fTheta1[0], "Theta1[30]/D");
    tMcTruth->Branch("Phi1", &fPhi1[0], "Phi1[30]/D");
    // tMcTruth->Branch("Det1"		,&fDet1[0]			,"Det1[30]/I");
    tMcTruth->Branch("isHit1", &fHitEmc1[0], "SizeOfMcTrackArray[30]/I");
    tMcTruth->Branch("isHit2", &fHitEmc2[0], "NumHitOfPrimaryTrack[30]/I");
    tMcTruth->Branch("isHit3", &fNPointV[0], "TotalNumHit[30]/I");
    tMcTruth->Branch("isHit4", &fNumGT, "NumOfSecondaryHit/I");
    // tMcTruth->Branch("X1"			,&fX1					,"X1/D");
    // tMcTruth->Branch("Y1"			,&fY1					,"Y1/D");
    // tMcTruth->Branch("Z1"			,&fZ1					,"Z1/D");
  }
  if (fSavePidCharged) {
    tCharged = new TTree("char", "charged cand");
    tCharged->Branch("char", &fchar[0], "char[30]/I");
    tCharged->Branch("p4", &fp4[0], "p4[120]/D");
    tCharged->Branch("pos", &fpos[0], "pos[90]/D");
    tCharged->Branch("mom", &fmom[0], "mom[30]/D");
    tCharged->Branch("mcIdx", &fmcidx[0], "mctrackid[30]/I");
  }
  if (fSavePidNeutral) {
    tNeutral = new TTree("neut", "neutral cand");
    tNeutral->Branch("char", &fchar[0], "char[30]/I");
    tNeutral->Branch("mcIdx", &fmcidx[0], "mctrackid[30]/I");
    tNeutral->Branch("p4", &fp4[0], "p4[120]/D");
    tNeutral->Branch("pos", &fpos[0], "pos[90]/D");
    tNeutral->Branch("rawe", &frawe[0], "rawe[30]/D");
    tNeutral->Branch("cale", &fcale[0], "cale[30]/D");
    tNeutral->Branch("ndigi", &fndigi[0], "ndigi[30]/I");
    tNeutral->Branch("mod", &fmod[0], "EmcModule[30]/I");
    tNeutral->Branch("cluIdx", &fcluIdx[0], "clusterIdx[30]/I");
    tNeutral->Branch("e25", &fe25[0], "e25[30]/D");
    tNeutral->Branch("e9", &fe9[0], "e9[30]/D");
    tNeutral->Branch("nbump", &fnbump[0], "nbump[30]/I");
  }
  if (fSaveJpsiTo3g) {
    tAna3g = new TTree("Ana", "Jpsi-->3photon");
    tAna3g->Branch("ng", &fNumberOfGoodPhoton, "GoodPhoton/I");
    tAna3g->Branch("eE", &fExternalEnergy, "ExtralEnergy/D");
    tAna3g->Branch("4p1", &f4p1[0], "Photon1[4]/D");
    tAna3g->Branch("mc4p1", &fMc4p1[0], "Truth1[4]/D");
    tAna3g->Branch("pos1", &fpos1[0], "pos1[3]/D");
    tAna3g->Branch("e1", &fenergy1, "e1/D");
    tAna3g->Branch("rawe1", &femcraw1, "rawe1/D");
    tAna3g->Branch("nd1", &fndigi1, "NumOfDigi1/I");
    tAna3g->Branch("mcTrk1", &fMcTrack1, "McTrackID1/I");
    tAna3g->Branch("mod1", &fmod1, "mod1/I");
    tAna3g->Branch("e251", &fe251, "e251/D");
    tAna3g->Branch("lat1", &flat1, "lat1/D");
    tAna3g->Branch("z201", &fz201, "z201/D");
    tAna3g->Branch("z531", &fz531, "z531/D");

    tAna3g->Branch("4p2", &f4p2[0], "Photon2[4]/D");
    tAna3g->Branch("mc4p2", &fMc4p2[0], "Truth2[4]/D");
    tAna3g->Branch("pos2", &fpos2[0], "pos2[3]/D");
    tAna3g->Branch("e2", &fenergy2, "e2/D");
    tAna3g->Branch("rawe2", &femcraw2, "rawe2/D");
    tAna3g->Branch("nd2", &fndigi2, "NumOfDigi2/I");
    tAna3g->Branch("mcTrk2", &fMcTrack2, "McTrackID2/I");
    tAna3g->Branch("mod2", &fmod2, "mod2/I");
    tAna3g->Branch("e252", &fe252, "e252/D");
    tAna3g->Branch("lat2", &flat2, "lat2/D");
    tAna3g->Branch("z202", &fz202, "z202/D");
    tAna3g->Branch("z532", &fz532, "z532/D");

    tAna3g->Branch("4p3", &f4p3[0], "Photon3[4]/D");
    tAna3g->Branch("mc4p3", &fMc4p3[0], "Truth3[4]/D");
    tAna3g->Branch("pos3", &fpos3[0], "pos3[3]/D");
    tAna3g->Branch("e3", &fenergy3, "e3/D");
    tAna3g->Branch("rawe3", &femcraw3, "rawe3/D");
    tAna3g->Branch("nd3", &fndigi3, "NumOfDigi3/I");
    tAna3g->Branch("mcTrk3", &fMcTrack3, "McTrackID3/I");
    tAna3g->Branch("mod3", &fmod3, "mod3/I");
    tAna3g->Branch("e253", &fe253, "e253/D");
    tAna3g->Branch("lat3", &flat3, "lat3/D");
    tAna3g->Branch("z203", &fz203, "z203/D");
    tAna3g->Branch("z533", &fz533, "z533/D");
  }

  evtset.clear();
  pevtset.clear();
  evtMap.clear();

  totNumOfWave = 0;
  totNumOfSubWave = 0;
  totNumOfDigi = 0;
  totNumOfSharedDigi = 0;
  totNumOfBump = 0;
  totNumOfEvents = 0;
  totNumOfPileupEvents = 0;
  fNumOfMatch = 0;
  fNumOfDismatch = 0;

  // vector resize
  //	const Int_t Num = 1;
  //	fEnergy.resize(Num);
  //	fEnergyC1.resize(Num);
  //	fEnergyC2.resize(Num);
  //	fPositionV.resize(3*Num);
  //	fp4.resize(4*Num);
  //	fMcp4.resize(4*Num);
  //	fZ201.resize(Num);
  //	fZ531.resize(Num);
  //	fLat1.resize(Num);
  //	fModule1.resize(Num);
  //	fTheta1.resize(Num);
  //	fPhi1.resize(Num);
  //	fDet1.resize(Num);
  //	fHitEmc1.resize(Num);
  //	fHitEmc2.resize(Num);
  //	fNPointV.resize(Num);

  LOG(info) << " PndEmcAnalysis: Intialization successfull";
  return kSUCCESS;
}

void PndEmcAnalysis::Exec(Option_t *)
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }

  fEvtTime = FairRootManager::Instance()->GetEventTime();
  cout << "=======================PndEmcAnalysis=======================" << endl;
  if (fSaveHits) {
    Int_t nHits = fEmcHitsArray->GetEntriesFast();
    for (Int_t iHit = 0; iHit < nHits; ++iHit) {
      PndEmcHit *theHit = (PndEmcHit *)fEmcHitsArray->At(iHit);
      fHitTime = theHit->GetTime();
      fModule = theHit->GetModule();
      fHitEnergy = theHit->GetEnergy();
      fTheta = theHit->GetTheta();
      fPhi = theHit->GetPhi();
      fXcor = theHit->GetXPad();
      fYcor = theHit->GetYPad();
      tHit->Fill();
    }
    if (fVerbose > 0)
      cout << "Save " << nHits << " PndEmcHit done" << endl;
  }
  if (fTimeOrderedDigi) {
    Double_t time_length = 40.; // 99.98%
    if (FairRunAna::Instance()->IsTimeStamp()) {
      fWaveformArray = FairRootManager::Instance()->GetData("EmcSortedWaveform", fFunctor, FairRootManager::Instance()->GetEventTime() + time_length);
    }
  }

  Int_t nWaveforms = fWaveformArray->GetEntriesFast();
  totNumOfWave += nWaveforms;
  if (fSaveWave) {
    for (Int_t iWaveform = 0; iWaveform < nWaveforms; iWaveform++) {
      PndEmcWaveform *theWaveform = (PndEmcWaveform *)fWaveformArray->At(iWaveform);
      if (fTimeOrderedDigi) {
        const std::vector<Int_t> &evtList = theWaveform->GetEvtList();
        for (size_t i = 0; i < evtList.size(); ++i) {
          evtset.insert(evtList[i]);
          ++evtMap[evtList[i]].first; // all waveforms counter
          if (evtList.size() > 1) {
            pevtset.insert(evtList[i]);
            ++evtMap[evtList[i]].second; // pileup waveforms counter
          }
        }
      }

      fPileupCount = theWaveform->GetPileupCount();
      totNumOfSubWave += (fPileupCount + 1);
      fHitIndex = theWaveform->GetHitIndex();
      fModule = theWaveform->GetModule();
      fWaveTime = theWaveform->GetTimeStamp();
      fWavePeak = theWaveform->Max();
      fWaveLen = theWaveform->GetWaveformLength();
      fWaveIdx = iWaveform;
      fBaseline = theWaveform->GetBaseline();
      fXcor = theWaveform->GetTCI()->XCoord();
      fYcor = theWaveform->GetTCI()->YCoord();
      if (!fTimeOrderedDigi)
        fHitEnergy = ((PndEmcHit *)fEmcHitsArray->At(fHitIndex))->GetEnergy();
      tWave->Fill();
    }
    if (fVerbose > 0)
      cout << "Save " << nWaveforms << " PndEmcWaveform done" << endl;
  }

  totDigiEnergy = 0.;
  totSharedDigiEnergy = 0.;
  Int_t nDigi = fDigiArray->GetEntriesFast();
  Int_t nSharedDigi = fSharedDigiArray->GetEntriesFast();
  if (!fTimeOrderedDigi) {
    totNumOfDigi += nDigi;
    totNumOfSharedDigi += nSharedDigi;
  }
  if (fSaveDigi) {
    for (Int_t iDigi = 0; iDigi < nDigi; iDigi++) {
      PndEmcDigi *theDigi = (PndEmcDigi *)fDigiArray->At(iDigi);
      totDigiEnergy += theDigi->GetEnergy();
      fDigiTime = theDigi->GetTimeStamp();
      fModule = theDigi->GetModule();
      fWaveIdx = theDigi->GetHitIndex();
      if (!fTimeOrderedDigi) {
        fWaveTime = ((PndEmcWaveform *)fWaveformArray->At(fWaveIdx))->GetTimeStamp();
        fHitEnergy = ((PndEmcHit *)fEmcHitsArray->At(fWaveIdx))->GetEnergy();
      }
      fDigiEnergy = theDigi->GetEnergy();
      fDistance = theDigi->where().Mag();
      fPosition = theDigi->where();
      tDigi->Fill();
    }
    for (Int_t iDigi = 0; iDigi < nSharedDigi; iDigi++) {
      PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(iDigi);
      totSharedDigiEnergy += theDigi->GetEnergy();
    }
    if (fVerbose > 0)
      cout << "Save " << nDigi << " PndEmcDigi done" << endl;
  }
  Int_t nBump = fBumpArray->GetEntriesFast();
  totNumOfBump += nBump;
  std::vector<key> goodHits;
  if (fSaveBump) {
    PndEmcAbsClusterCalibrator *calibrator1 = PndEmcClusterCalibrator::MakeEmcClusterCalibrator(1);
    PndEmcClusterCalibrator::MakeEmcClusterCalibrator(2); // PndEmcAbsClusterCalibrator * calibrator2= //[R.K.03/2017] unused variable
    for (Int_t iBump = 0; iBump < nBump; iBump++) {
      PndEmcBump *theBump = (PndEmcBump *)fBumpArray->At(iBump);
      if ((fEnergy1C = calibrator1->Energy(theBump)) > 0.050) { // 50MeV
        goodHits.push_back(key(iBump, fEnergy1C));
      }
      fModule = theBump->GetModule();
      fBumpEnergy = theBump->GetEnergy();
      fPosition = theBump->where();
      fNumDigi = theBump->NumberOfDigis();
      fNumBump = theBump->NBumps();
      fClusterIdx = theBump->GetClusterIndex();
      fNumOfSeed = theBump->LocalMaxMap().size();
      fDigiMapSize = theBump->MemberDigiMap().size();
      const std::vector<Int_t> &digis = theBump->DigiList();
      if (fTimeOrderedDigi) {
        totNumOfDigi += digis.size();
        totNumOfSharedDigi += digis.size();
      }
      PndEmcDigi *seedDigi(0);
      Double_t maxE(-1.);
      for (size_t id = 0; id < digis.size(); ++id) {
        PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(digis[id]);
        if (theDigi->GetEnergy() > maxE) {
          maxE = theDigi->GetEnergy();
          seedDigi = theDigi;
        }
        theDigi->fEvtNo == fEventNo ? fNumOfMatch++ : fNumOfDismatch++;
      }
      fSeedTime = seedDigi == 0 ? 0 : /* digiCalibrator.CalibrationEvtTimeByDigi(seedDigi, kFALSE);*/ seedDigi->GetTimeStamp();
      fBumpTime1 = theBump->GetTimeStamp();
      // fBumpTime2			= theBump->GetTimeStamp1();
      fSeedPosition = seedDigi->where();
      // fBumpTime3			= theBump->GetTimeStamp3();
      tBump->Fill();
    }
    if (fVerbose > 0)
      cout << "Save " << nBump << " PndEmcBump done" << endl;
  }
  if (fSaveReco) {
    std::vector<key> goodTrack;
    // std::vector<IndexKey> indicator;
    sort(goodHits.begin(), goodHits.end(), std::greater<key>());
    if (goodHits.size() >= 1) {
      // reset
      for (Int_t idx = 0; idx < NElement; ++idx) {
        fEnergy[idx] = fEnergyC1[idx] = fEnergyC2[idx] = fTheta1[idx] = fPhi1[idx] = fZ201[idx] = fZ531[idx] = fLat1[idx] = fModule1[idx] = 0;
        fPositionV[idx * 3] = fPositionV[idx * 3 + 1] = fPositionV[idx * 3 + 2] = 0;
        fp4[idx * 4] = fp4[idx * 4 + 1] = fp4[idx * 4 + 2] = fp4[idx * 4 + 3] = 0;
      }
      if (fMcTrackArray) {
        Int_t nTrack = fMcTrackArray->GetEntriesFast() > NElement ? NElement : fMcTrackArray->GetEntriesFast();
        for (Int_t i = 0; i < nTrack; ++i) {
          PndMCTrack *p1 = (PndMCTrack *)fMcTrackArray->At(i);
          if (p1->IsGeneratorCreated()) {
            goodTrack.push_back(key(i, p1->Get4Momentum().E()));
            // TLorentzVector p4(p1->GetMomentum(), p1->GetE());
            // memcpy(&fMcp4[i*4], &p4[0], 4*sizeof(Double_t));
          } else
            break;
        }
        sort(goodTrack.begin(), goodTrack.end(), std::greater<key>());
        for (size_t i = 0; i < goodTrack.size(); ++i) {
          PndMCTrack *p1 = (PndMCTrack *)fMcTrackArray->At(goodTrack[i].iHit);
          TLorentzVector p4(p1->GetMomentum(), p1->Get4Momentum().E());
          memcpy(&fMcp4[i * 4], &p4[0], 4 * sizeof(Double_t));
          fPDGCode[i] = p1->GetPdgCode();
        }
      }

      PndEmcAbsClusterCalibrator *calibrator1 = PndEmcClusterCalibrator::MakeEmcClusterCalibrator(1);
      PndEmcAbsClusterCalibrator *calibrator2 = PndEmcClusterCalibrator::MakeEmcClusterCalibrator(2);
      Int_t NEL = goodHits.size() > NElement ? NElement : goodHits.size();
      fRestEnergy1 = fRestEnergy2 = fRestEnergy3 = 0.;
      for (size_t i = NEL; i < goodHits.size(); ++i) {
        PndEmcBump *theBump = (PndEmcBump *)fBumpArray->At(goodHits[i].iHit);
        fRestEnergy1 += theBump->GetEnergy();
        fRestEnergy2 += calibrator1->Energy(theBump);
        fRestEnergy3 += calibrator1->Energy(theBump);
      }
      fNumBump = goodHits.size();
      Int_t locIdx = 0;

      for (Int_t idx = 0; idx < NEL; ++idx) {
        PndEmcBump *theBump = (PndEmcBump *)fBumpArray->At(goodHits[idx].iHit);
        // PndEmcBump* theBump = (PndEmcBump*)fBumpArray->At(index);
        // if(idx < goodTrack.size())
        //	locIdx = goodTrack[idx].iHit;
        // else
        //	locIdx = idx;
        locIdx = idx;
        fEnergy[locIdx] = theBump->GetEnergy();
        fEnergyC1[locIdx] = calibrator1->Energy(theBump) / 1.018;
        fEnergyC2[locIdx] = calibrator2->Energy(theBump);
        TVector3 pos(calibrator1->Where(theBump));
        memcpy(&fPositionV[locIdx * 3], &pos[0], 3 * sizeof(Double_t));
        fTheta1[locIdx] = pos.Theta();
        fPhi1[locIdx] = pos.Phi();
        fZ201[locIdx] = theBump->Z20();
        fZ531[locIdx] = theBump->Z53();
        fLat1[locIdx] = theBump->LatMom();
        fModule1[locIdx] = theBump->GetModule();
        TLorentzVector p4(fEnergyC1[locIdx] * TMath::Sin(fTheta1[locIdx]) * TMath::Cos(fPhi1[locIdx]), fEnergyC1[locIdx] * TMath::Sin(fTheta1[locIdx]) * TMath::Sin(fPhi1[locIdx]),
                          fEnergyC1[locIdx] * TMath::Cos(fTheta1[locIdx]), fEnergyC1[locIdx]);
        memcpy(&fp4[locIdx * 4], &p4[0], 4 * sizeof(Double_t));
      }
      tRec->Fill();
      if (fVerbose > 0)
        cout << "Save " << NEL << " reconstruction data done" << endl;
    }
  }
  if (fSaveMcTruth) {
    Bool_t isFill = kFALSE;
    Int_t nTrack = fMcTrackArray->GetEntriesFast(); // > NElement ? NElement : fMcTrackArray->GetEntriesFast();
    std::vector<std::map<Int_t, Int_t>> trackMap;   // top node, and its tree
    fNumGT = 0;
    for (Int_t idx = 0; idx < NElement; ++idx) {
      fTheta1[idx] = fPhi1[idx] = fDet1[idx] = fNPointV[idx] = fModule1[idx] = fHitEmc1[idx] = fHitEmc2[idx] = 0;
      fPositionV[idx * 3] = fPositionV[idx * 3 + 1] = fPositionV[idx * 3 + 2] = 0;
      fMcp4[idx * 4] = fMcp4[idx * 4 + 1] = fMcp4[idx * 4 + 2] = fMcp4[idx * 4 + 3] = 0;
    }
    for (Int_t itrack = 0; itrack < nTrack; ++itrack) {
      PndMCTrack *pt1 = ((PndMCTrack *)fMcTrackArray->At(itrack));
      if (!pt1)
        break;
      if (pt1->IsGeneratorCreated()) {
        pt1->Print(itrack);
        std::map<Int_t, Int_t> newMap;
        newMap.insert(std::pair<Int_t, Int_t>(itrack, pt1->GetMotherID()));
        trackMap.push_back(newMap);
        isFill = kTRUE;
        TLorentzVector p4(pt1->GetMomentum(), pt1->Get4Momentum().E());
        memcpy(&fMcp4[itrack * 4], &p4[0], 4 * sizeof(Double_t));
        fTheta1[itrack] = p4.Theta();
        fPhi1[itrack] = p4.Phi();
        fHitEmc1[itrack] = fMcTrackArray->GetEntriesFast();
        fHitEmc2[itrack] = pt1->GetNPoints(DetectorId::kEMC);
        fNPointV[itrack] = pt1->GetNPoints(DetectorId::kEMC);
        // fDet1[itrack] = -1;
        // for(Int_t i=0;i<15;++i){
        //	if(detIds[i] == kEMC){
        //		if(pt1->GetNPoints(detIds[i])>0)
        //		{fDet1[itrack] = detIds[i];break;}
        //	}else{
        //		if(pt1->GetNPoints(detIds[i])>0)
        //		{fDet1[itrack] = detIds[i];break;}
        //	}
        //}
      } else {
        if (itrack < 6) {
          pt1->Print(itrack);
          cout << "MC points #" << pt1->GetNPoints(DetectorId::kEMC) << ", #" << trackMap.size() << endl;
        }
        for (size_t im = 0; im < trackMap.size(); ++im) {
          std::map<Int_t, Int_t> &theMap = trackMap[im];
          if (theMap.find(pt1->GetMotherID()) != theMap.end()) {
            theMap.insert(std::pair<Int_t, Int_t>(itrack, pt1->GetMotherID()));
            fNPointV[im] += pt1->GetNPoints(DetectorId::kEMC);
            break;
          }
        }
        if (pt1->GetNPoints(DetectorId::kEMC) > 0)
          ++fNumGT;
      }
    }
    if (isFill)
      tMcTruth->Fill();
    if (fVerbose > 0)
      cout << "Save MCTruth data done" << endl;
  }
  if (fSavePidCharged) {
    for (Int_t idx = 0; idx < NElement; ++idx) {
      fchar[idx] = fmcidx[idx] = 0;
      fmom[idx] = 0.;
      fpos[idx * 3] = fpos[idx * 3 + 1] = fpos[idx * 3 + 2] = 0;
      fp4[idx * 4] = fp4[idx * 4 + 1] = fp4[idx * 4 + 2] = fp4[idx * 4 + 3] = 0;
    }
    Int_t nCand = fChargedCand->GetEntriesFast() > NElement ? NElement : fChargedCand->GetEntriesFast();
    for (Int_t ic = 0; ic < nCand; ++ic) {
      PndPidCandidate *theCand = (PndPidCandidate *)fChargedCand->At(ic);
      fchar[ic] = theCand->GetCharge();
      fmom[ic] = theCand->GetMomentum().Mag();
      fmcidx[ic] = theCand->GetMcIndex();
      TVector3 pos = theCand->GetPosition();
      memcpy(&fpos[ic * 3], &pos[0], 3 * sizeof(Double_t));
      TLorentzVector p4 = theCand->GetLorentzVector();
      memcpy(&fp4[ic * 4], &p4[0], 4 * sizeof(Double_t));
    }
    tCharged->Fill();
    if (fVerbose > 0)
      cout << "Save " << nCand << " charged candidates done" << endl;
  }
  if (fSavePidNeutral) {
    Int_t nCand = fNeutralCand->GetEntriesFast() > NElement ? NElement : fNeutralCand->GetEntriesFast();
    for (Int_t idx = 0; idx < NElement; ++idx) {
      fchar[idx] = fmcidx[idx] = fnbump[idx] = fndigi[idx] = fmod[idx] = fcluIdx[idx] = 0;
      frawe[idx] = fcale[idx] = fe25[idx] = fe9[idx] = 0;
      fpos[idx * 3] = fpos[idx * 3 + 1] = fpos[idx * 3 + 2] = 0;
      fp4[idx * 4] = fp4[idx * 4 + 1] = fp4[idx * 4 + 2] = fp4[idx * 4 + 3] = 0;
    }
    for (Int_t ic = 0; ic < nCand; ++ic) {
      PndPidCandidate *theCand = (PndPidCandidate *)fNeutralCand->At(ic);
      fchar[ic] = theCand->GetCharge();
      frawe[ic] = theCand->GetEmcRawEnergy();
      fcale[ic] = theCand->GetEmcCalEnergy();
      fndigi[ic] = theCand->GetEmcNumberOfCrystals();
      fnbump[ic] = theCand->GetEmcNumberOfBumps();
      fmod[ic] = theCand->GetEmcModule();
      fcluIdx[ic] = theCand->GetEmcIndex();
      fe25[ic] = theCand->GetEmcClusterE25();
      fe9[ic] = theCand->GetEmcClusterE9();
      TVector3 pos = theCand->GetPosition();
      memcpy(&fpos[ic * 3], &pos[0], 3 * sizeof(Double_t));
      TLorentzVector p4 = theCand->GetLorentzVector();
      memcpy(&fp4[ic * 4], &p4[0], 4 * sizeof(Double_t));
    }
    tNeutral->Fill();
    if (fVerbose > 0)
      cout << "Save " << nCand << " neutral candidates done" << endl;
  }
  if (fSaveJpsiTo3g && fChargedCand->GetEntriesFast() == 0) {
    Int_t nCand = fNeutralCand->GetEntriesFast();
    if (fVerbose > 0)
      cout << "begin write #" << nCand << " candidates." << endl;
    std::vector<key> goodCand;
    for (Int_t ic = 0; ic < nCand; ++ic) {
      PndPidCandidate *theCand = (PndPidCandidate *)fNeutralCand->At(ic);
      if (theCand->GetEnergy() > 0.02)
        goodCand.push_back(key(ic, theCand->GetEnergy()));
    }
    if (goodCand.size() >= 3) {
      sort(goodCand.begin(), goodCand.end(), std::greater<key>());
      PndPidCandidate *cand1 = (PndPidCandidate *)fNeutralCand->At(goodCand[0].iHit);
      fenergy1 = cand1->GetEnergy();
      femcraw1 = cand1->GetEmcRawEnergy();
      fndigi1 = cand1->GetEmcNumberOfCrystals();
      fmod1 = cand1->GetEmcModule();
      fMcTrack1 = cand1->GetMcIndex();
      fe251 = cand1->GetEmcClusterE25();
      flat1 = cand1->GetEmcClusterLat();
      fz201 = cand1->GetEmcClusterZ20();
      fz531 = cand1->GetEmcClusterZ53();
      TLorentzVector p4 = cand1->GetLorentzVector();
      TVector3 pos = cand1->GetPosition();
      memcpy(&f4p1[0], &p4[0], 4 * sizeof(Double_t));
      memcpy(&fpos1[0], &pos[0], 3 * sizeof(Double_t));
      // f4p1    = cand1->GetLorentzVector();
      // fpos1  = cand1->GetPosition();
      PndPidCandidate *cand2 = (PndPidCandidate *)fNeutralCand->At(goodCand[1].iHit);
      fenergy2 = cand2->GetEnergy();
      femcraw2 = cand2->GetEmcRawEnergy();
      fndigi2 = cand2->GetEmcNumberOfCrystals();
      fmod2 = cand2->GetEmcModule();
      fe252 = cand2->GetEmcClusterE25();
      flat2 = cand2->GetEmcClusterLat();
      fz202 = cand2->GetEmcClusterZ20();
      fz532 = cand2->GetEmcClusterZ53();
      fMcTrack2 = cand2->GetMcIndex();
      p4 = cand2->GetLorentzVector();
      pos = cand2->GetPosition();
      memcpy(&f4p2[0], &p4[0], 4 * sizeof(Double_t));
      memcpy(&fpos2[0], &pos[0], 3 * sizeof(Double_t));
      // f4p2    = cand2->GetLorentzVector();
      // fpos2  = cand2->GetPosition();
      PndPidCandidate *cand3 = (PndPidCandidate *)fNeutralCand->At(goodCand[2].iHit);
      fenergy3 = cand3->GetEnergy();
      femcraw3 = cand3->GetEmcRawEnergy();
      fndigi3 = cand3->GetEmcNumberOfCrystals();
      fmod3 = cand3->GetEmcModule();
      fe253 = cand3->GetEmcClusterE25();
      flat3 = cand3->GetEmcClusterLat();
      fz203 = cand3->GetEmcClusterZ20();
      fz533 = cand3->GetEmcClusterZ53();
      fMcTrack3 = cand3->GetMcIndex();
      p4 = cand3->GetLorentzVector();
      pos = cand3->GetPosition();
      memcpy(&f4p3[0], &p4[0], 4 * sizeof(Double_t));
      memcpy(&fpos3[0], &pos[0], 3 * sizeof(Double_t));
      // f4p3    = cand3->GetLorentzVector();
      // fpos3  = cand3->GetPosition();
      fNumberOfGoodPhoton = goodCand.size();
      fExternalEnergy = 0.;
      for (size_t jc = 3; jc < goodCand.size(); ++jc) {
        PndPidCandidate *cand = (PndPidCandidate *)fNeutralCand->At(goodCand[jc].iHit);
        fExternalEnergy += cand->GetEnergy();
      }
      memset(&fMc4p1[0], 0, 4 * sizeof(Double_t));
      memset(&fMc4p2[0], 0, 4 * sizeof(Double_t));
      memset(&fMc4p3[0], 0, 4 * sizeof(Double_t));
      if (fMcTrackArray) {
        if (fMcTrack1 >= 0) {
          PndMCTrack *p1 = (PndMCTrack *)fMcTrackArray->At(fMcTrack1);
          p4 = TLorentzVector(p1->GetMomentum(), p1->Get4Momentum().E());
          memcpy(&fMc4p1[0], &p4[0], 4 * sizeof(Double_t));
        }
        if (fMcTrack2 >= 0) {
          PndMCTrack *p2 = (PndMCTrack *)fMcTrackArray->At(fMcTrack2);
          p4 = TLorentzVector(p2->GetMomentum(), p2->Get4Momentum().E());
          memcpy(&fMc4p2[0], &p4[0], 4 * sizeof(Double_t));
        }
        if (fMcTrack3 >= 0) {
          PndMCTrack *p3 = (PndMCTrack *)fMcTrackArray->At(fMcTrack3);
          p4 = TLorentzVector(p3->GetMomentum(), p3->Get4Momentum().E());
          memcpy(&fMc4p3[0], &p4[0], 4 * sizeof(Double_t));
        }
      }
      tAna3g->Fill();
    }
  }

  if (fSaveTask) {
    fNumOfHits = fEmcHitsArray->GetEntriesFast();
    fNumWaveform = fWaveformArray->GetEntriesFast();
    fNumDigi = fDigiArray->GetEntriesFast();
    fNumSharedDigi = fSharedDigiArray->GetEntriesFast();
    fNumCluster = fClusterArray->GetEntriesFast();
    fNumBump = fBumpArray->GetEntriesFast();
    fNumMcTrack = fMcTrackArray->GetEntriesFast();
    tTask->Fill();
    if (fVerbose > 0)
      cout << "Save task data done" << endl;
  }

  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndEmcAnalysis, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
  // fEvtHeaderArray->Delete();
  fEventNo++;
  cout << "=================================================================" << endl;
}

void PndEmcAnalysis::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc digitisation parameter container
  // fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");

  // Get Emc reconstruction parameter container
  // fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");

  // Get Emc FPGA parameter container
  // fFpgaPar = (PndEmcFpgaPar*) db->getContainer("PndEmcFpgaPar");
}

void PndEmcAnalysis::FinishTask()
{

  std::cout << "*********************************************************" << std::endl;
  std::cout << "PndEmcAnalysis::FinishTask" << std::endl;
  std::cout << "*********************************************************" << std::endl;
  std::cout << "Total waveforms# " << totNumOfWave << std::endl;
  std::cout << "Total subwaveforms# " << totNumOfSubWave << std::endl;
  std::cout << "Total fpga digis# " << totNumOfDigi << std::endl;
  std::cout << "Total shared digis# " << totNumOfSharedDigi << std::endl;
  std::cout << "Total bumps# " << totNumOfBump << std::endl;
  // std::cout<<"Digis to events they belong to#"<<matchCount<<std::endl;
  // std::cout<<"Digis to events they dont belong to#"<<unmatchCount<<std::endl;
  std::cout << "*********************************************************" << std::endl;
  if (fTimeOrderedDigi) {
    std::map<Int_t, std::pair<Int_t, Int_t>>::iterator it = evtMap.begin();
    for (; it != evtMap.end(); ++it) {
      fEventNo = (*it).first;
      fTotWave = (*it).second.first;
      fPileupWave = (*it).second.second;
      tEvtPileup->Fill();
    }
    totNumOfEvents = evtset.size();
    totNumOfPileupEvents = pevtset.size();
    tTaskPileup->Fill();
  }

  if (fStoreRooTFile) {
    TFile *gOldFile = gFile;
    gFile = fRootFile;
    gFile->cd();
    if (fSaveHits) {
      cout << "write " << tHit->GetTitle() << endl;
      tHit->Write();
    }
    if (fSaveWave) {
      cout << "write " << tWave->GetTitle() << endl;
      tWave->Write();
    }
    if (fSaveDigi) {
      cout << "write " << tDigi->GetTitle() << endl;
      tDigi->Write();
    }
    if (fSaveBump) {
      cout << "write " << tBump->GetTitle() << endl;
      tBump->Write();
    }
    if (fSaveTask) {
      cout << "write " << tTask->GetTitle() << endl;
      tTask->Write();
    }
    if (fSaveReco) {
      cout << "write " << tRec->GetTitle() << endl;
      tRec->Write();
    }
    if (fSaveMcTruth) {
      cout << "write " << tMcTruth->GetTitle() << endl;
      tMcTruth->Write();
    }
    if (fTimeOrderedDigi) {
      cout << "write " << tTaskPileup->GetTitle() << endl;
      cout << "write " << tEvtPileup->GetTitle() << endl;
      tTaskPileup->Write();
      tEvtPileup->Write();
    }
    if (fSavePidCharged) {
      cout << "write " << tCharged->GetTitle() << endl;
      tCharged->Write();
    }
    if (fSavePidNeutral) {
      cout << "write " << tNeutral->GetTitle() << endl;
      tNeutral->Write();
    }
    if (fSaveJpsiTo3g) {
      cout << "write " << tAna3g->GetTitle() << endl;
      tAna3g->Write();
    }

    gFile = gOldFile;
  }

  evtset.clear();
  pevtset.clear();
  evtMap.clear();
}

ClassImp(PndEmcAnalysis)
