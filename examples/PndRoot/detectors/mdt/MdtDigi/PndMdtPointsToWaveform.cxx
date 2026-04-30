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
//      Class PndMdtPointsToWaveform. Module to take the Mdt Points list for the
//      mdt and make induced current.
//
// Author List:
//      Jifeng Hu, hu@to.infn.it, Torino University
//
//----------------------------------------------------------------------

#include "PndMdtWaveformWriteoutBuffer.h"
#include "PndMdtPointsToWaveform.h"
#include "PndMdtPoint.h"
#include "PndMdtParamDigi.h"
#include "FairEventHeader.h"
#include "PndMdtWaveform.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "PndMCTrack.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TClonesArray.h"
#include "PndMdtIGeometry.h"
#include <iostream>
#include <cassert>

using std::cout;
using std::endl;
using std::fstream;

PndMdtPointsToWaveform::PndMdtPointsToWaveform(Int_t verbose, Bool_t storewaves)
  : // fDigiPar(new PndMdtDigiPar())
    //, fGeoPar(new PndMdtGeoPar())
    fWaveformArray(nullptr), fTimeOrderedWaveform(kFALSE), fDataBuffer(nullptr), fVerbose(verbose)
{
  SetPersistency(storewaves);
}
//--------------
// Destructor --
//--------------

PndMdtPointsToWaveform::~PndMdtPointsToWaveform() {}

InitStatus PndMdtPointsToWaveform::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtPointsToWaveform::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMcTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMcTrackArray) {
    cout << "-W- PndMdtPointsToWaveform::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }

  // Get input array
  fPointArray = (TClonesArray *)ioman->GetObject("MdtPoint");
  if (!fPointArray) {
    cout << "-W- PndMdtPointsToWaveform::Init: "
         << "No MdtHit array!" << endl;
    return kERROR;
  }
  if (fTimeOrderedWaveform) {
    fDataBuffer = new PndMdtWaveformWriteoutBuffer("MdtWaveform", "Mdt", GetPersistency());
    fDataBuffer->ActivateBuffering(kTRUE);
    fDataBuffer->SetVerbose(fVerbose);
    ioman->RegisterWriteoutBuffer("MdtWaveform", fDataBuffer);
  } else {
    fWaveformArray = ioman->Register("MdtWaveform", "PndMdtWaveform", "Mdt", GetPersistency());
  }

  // fDigiPar->printParams();

  nWaveformProduced = 0;
  HowManyPoint = 0;

  fParamDigiModel = new PndMdtParamDigi();
  fParamDigiModel->UseNoise(kTRUE);
  fParamDigiModel->SetNoiseWidth(0.05, 0.02);
  fParamDigiModel->UseDetailedSim(kFALSE);
  fParamDigiModel->UsePlot(kFALSE);
  fParamDigiModel->UseGaussianAmp(kFALSE);
  fParamDigiModel->SetOptimization(10);
  // fParamDigiModel->SetVerbose(2);
  fParamDigiModel->Init();

  fGeoIF = PndMdtIGeometry::Instance();
  fGeoIF->SetVerbose(0);
  fGeoIF->AddSensor("MDT");
  fGeoIF->AddSensor("GasCell");
  Bool_t fGoodGeo = fGeoIF->Init();
  if (fGoodGeo)
    fGeoIF->Print();

  fFile = new TFile("PndMdtPointsToWaveform.root", "RECREATE");
  tTree = new TTree("pt", "time-stamp diff");
  tTree->Branch("wt", &fWirpT, "wt/D");
  tTree->Branch("st", &fStripT, "st/D");
  tTree->Branch("et", &fEvtT, "et/D");
  tTree->Branch("len", &fLength, "len/D");
  tTree->Branch("dis", &fDis, "dis/D");
  tTree->Branch("mod", &fMod, "mod/I");
  tTree->Branch("pid", &fPid, "pid/I");
  // tTree->Branch("xs", &fxs, "xs/D");
  // tTree->Branch("ys", &fys, "ys/D");
  // tTree->Branch("zs", &fzs, "zs/D");
  // tTree->Branch("xe", &fxe, "xe/D");
  // tTree->Branch("ye", &fye, "ye/D");
  // tTree->Branch("ze", &fze, "ze/D");

  LOG(info) << " PndMdtPointsToWaveform: Intialization " << (fGoodGeo ? "successful." : "failed.");

  return fGoodGeo ? kSUCCESS : kERROR;
}
void PndMdtPointsToWaveform::Exec(Option_t *)
{
  if (fTimeOrderedWaveform)
    exec_t();
  else
    exec_e();
}
void PndMdtPointsToWaveform::exec_e()
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }

  fWaveformArray->Clear();

  Double_t fEventTime = FairRootManager::Instance()->GetEventTime(); // nano seconds
  Int_t nHits = fPointArray->GetEntriesFast();
  Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;
  if (fVerbose > 1) {
    cout << "================PndMdtPointsToWaveform===============================================" << endl;
    cout << "Event No.: " << evtNo << ", Event Time: " << fEventTime << ", Load in " << nHits << " PndMdtPoint." << endl;
  }
  std::map<key, Int_t> fMcTrackingInfo;
  PndMdtPoint *thePoint(0);
  Int_t TrackID;
  Int_t ParticleType;
  Int_t DetectorID;
  TVector3 EntrancePosition;
  TVector3 ExitingPosition;
  TVector3 Momentum;
  Double_t fWaveformTimeStamp;

  Int_t nPoints = fPointArray->GetEntriesFast();
  HowManyPoint += nPoints;
  std::map<Int_t, PndMdtWaveform *> listofWaveforms;

  Double_t fAmplitude;
  Double_t fTimeStamp;

  TVector3 fTubeCenter;
  for (Int_t ip = 0; ip < nPoints; ip++) {
    thePoint = (PndMdtPoint *)fPointArray->At(ip);
    if (thePoint->GetEnergyLoss() == 0)
      continue;
    key _TrkandDetID(thePoint->GetTrackID(), thePoint->GetDetectorID());
    // cout<<"TrackID = "<<thePoint->GetTrackID()<<", DetID = "<<thePoint->GetDetectorID()<<endl;
    std::map<key, Int_t>::iterator it = fMcTrackingInfo.find(_TrkandDetID);
    if (fMcTrackingInfo.end() == it) {
      fMcTrackingInfo.insert(std::pair<key, Int_t>(_TrkandDetID, 1));
      TrackID = thePoint->GetTrackID();
      DetectorID = thePoint->GetDetectorID();
      // ParticleType = thePoint->GetParticleType();
      EntrancePosition = thePoint->GetPosIn();
      ExitingPosition = thePoint->GetPosOut();
      Momentum = thePoint->GetMomIn();
      fWaveformTimeStamp = thePoint->GetTime() + fEventTime;

      // fxs = EntrancePosition.X();
      // fys = EntrancePosition.Y();
      // fzs = EntrancePosition.Z();
      fLength = thePoint->GetLength();
      fMod = thePoint->GetModule();

      //
      TVector3 fNewEntrance(0, 0, 0);
      TVector3 fNewExit(0, 0, 0);
      Bool_t fOK1 = fGeoIF->MasterToLocal(DetectorID, EntrancePosition, fNewEntrance);
      Bool_t fOK2 = fGeoIF->MasterToLocal(DetectorID, ExitingPosition, fNewExit);
      if (!(fOK1 && fOK2)) {
        cout << "PndMdtPointsToWaveform::exec_e, invliad detector index #" << DetectorID << endl;
        continue;
      }
      TVector2 fexit = fNewExit.XYvector();
      TVector2 fentr = fNewEntrance.XYvector();
      TVector2 fvd = fexit - fentr;

      fDis = TMath::Abs(fexit.Mod() * fentr.Mod() * TMath::Sin(fexit.DeltaPhi(fentr))) / fvd.Mod();
      if (fVerbose > 1) {
        cout << "================================================" << endl;
        cout << "Mod = " << thePoint->GetModule() << endl;
        cout << "Oct = " << thePoint->GetSector() << endl
             << "Entrance " << EntrancePosition << endl
             << "Exiting " << ExitingPosition << endl
             << "new Entrance " << fNewEntrance << endl
             << "new Exiting " << fNewExit << endl;
      }
      PndMCTrack *mcTrk = (PndMCTrack *)fMcTrackArray->At(thePoint->GetTrackID());
      ParticleType = PdgToIndex(mcTrk->GetPdgCode());
      fPid = ParticleType;

      fParamDigiModel->SetParams(ParticleType, Momentum, fNewEntrance, fNewExit);
      // cout<<"================================================"<<endl;
      fParamDigiModel->Compute();

      PndMdtWaveform *aWf = new PndMdtWaveform(TrackID, DetectorID, fWaveformTimeStamp, kTRUE);
      //++nWaveformProduced;
      aWf->SetSignal(fParamDigiModel->GetWireSignal());

      std::map<Int_t, PndMdtWaveform *>::iterator mit3 = listofWaveforms.find(DetectorID);
      if (mit3 == listofWaveforms.end()) { // the first waveform
        listofWaveforms.insert(std::pair<Int_t, PndMdtWaveform *>(DetectorID, aWf));
      } else {
        *(mit3->second) += (*aWf);
        // delete aWf;
      }

      Digitize(aWf, fTimeStamp, fAmplitude, kTRUE);
      fWirpT = fTimeStamp + fEventTime;

      const std::map<Int_t, std::vector<Double_t>> &fStripSignals = fParamDigiModel->GetStripSignals();

      std::map<Int_t, std::vector<Double_t>>::const_iterator mit = fStripSignals.begin();
      std::map<Int_t, std::vector<Double_t>>::const_iterator mend = fStripSignals.end();
      Int_t newDetID;
      for (; mit != mend; ++mit) {
        // re-make detector id;
        fGeoIF->MapWireToStrip(DetectorID, fNewEntrance, newDetID); // waiting for updates
        Int_t lid = mit->first;
        if (lid % 2 == 0)
          newDetID += lid / 2;
        else
          newDetID -= lid / 2;
        PndMdtWaveform *aSf = new PndMdtWaveform(TrackID, newDetID, fWaveformTimeStamp, kFALSE);
        //++nWaveformProduced;
        aSf->SetSignal(mit->second);

        Digitize(aSf, fTimeStamp, fAmplitude, kFALSE);
        fStripT = fTimeStamp + fEventTime;
        fEvtT = fEventTime;

        std::map<Int_t, PndMdtWaveform *>::iterator mit1 = listofWaveforms.find(newDetID);
        if (mit1 == listofWaveforms.end()) { // the first waveform
          listofWaveforms.insert(std::pair<Int_t, PndMdtWaveform *>(newDetID, aSf));
        } else {
          *(mit1->second) += (*aSf);
          // delete aSf;
        }
      }
      tTree->Fill();
    } else {
      ++it->second;
    }
  }

  std::map<Int_t, PndMdtWaveform *>::iterator mit2 = listofWaveforms.begin();
  for (; mit2 != listofWaveforms.end(); ++mit2) {
    (*fWaveformArray)[fWaveformArray->GetEntriesFast()] = mit2->second;
    ++nWaveformProduced;
  }

  if (fVerbose > 1) {
    std::map<key, Int_t>::iterator it = fMcTrackingInfo.begin();
    std::map<key, Int_t>::iterator end = fMcTrackingInfo.end();
    for (; it != end; ++it) {
      cout << "Track #" << it->first.TrkID << " contributes " << it->second << " points in tube " << it->first.DetID << endl;
    }
  }
  if (fVerbose > 1) {
    cout << "Number of produced waveforms: " << fWaveformArray->GetEntriesFast() << endl;
    cout << "================PndMdtPointsToWaveform===============================================" << endl;
  }

  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndMdtPointsToWaveform, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

void PndMdtPointsToWaveform::exec_t()
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }

  Double_t fEventTime = FairRootManager::Instance()->GetEventTime(); // nano seconds
  Int_t nHits = fPointArray->GetEntriesFast();
  Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;
  if (fVerbose > 1) {
    cout << "================PndMdtPointsToWaveform===============================================" << endl;
    cout << "Event No.: " << evtNo << ", Event Time: " << fEventTime << ", Load in " << nHits << " PndMdtHit." << endl;
    cout << "Size of PndMdtWaveformWriteoutBuffer: " << fDataBuffer->GetNData() << endl;
    cout << "================PndMdtPointsToWaveform===============================================" << endl;
  }
  std::map<key, Int_t> fMcTrackingInfo;
  PndMdtPoint *thePoint(0);
  Int_t TrackID;
  Int_t ParticleType;
  Long_t DetectorID;
  TVector3 EntrancePosition;
  TVector3 ExitingPosition;
  TVector3 Momentum;
  Double_t TimeStamp;

  Int_t nPoints = fPointArray->GetEntriesFast();
  HowManyPoint += nPoints;
  for (Int_t ip = 0; ip < nPoints; ip++) {
    thePoint = (PndMdtPoint *)fPointArray->At(ip);
    if (thePoint->GetEnergyLoss() == 0)
      continue;
    key _TrkandDetID(thePoint->GetTrackID(), thePoint->GetDetectorID());
    std::map<key, Int_t>::iterator it = fMcTrackingInfo.find(_TrkandDetID);
    if (fMcTrackingInfo.end() == it) {
      fMcTrackingInfo.insert(std::pair<key, Int_t>(_TrkandDetID, 1));
      TrackID = thePoint->GetTrackID();
      DetectorID = thePoint->GetDetectorID();
      // ParticleType = thePoint->GetParticleType();
      EntrancePosition = thePoint->GetPosIn();
      ExitingPosition = thePoint->GetPosOut();
      Momentum = thePoint->GetMomIn();
      TimeStamp = thePoint->GetTime() + fEventTime;

      TVector3 fNewEntrance(0, 0, 0);
      TVector3 fNewExit(0, 0, 0);
      Bool_t fOK1 = fGeoIF->MasterToLocal(DetectorID, EntrancePosition, fNewEntrance);
      Bool_t fOK2 = fGeoIF->MasterToLocal(DetectorID, ExitingPosition, fNewExit);
      if (!(fOK1 && fOK2)) {
        cout << "PndMdtPointsToWaveform::exec_t, invalid detector index #" << DetectorID << endl;
        continue;
      }
      TVector2 fexit = fNewExit.XYvector();
      TVector2 fentr = fNewEntrance.XYvector();
      TVector2 fvd = fexit - fentr;

      fDis = TMath::Abs(fexit.Mod() * fentr.Mod() * TMath::Sin(fexit.DeltaPhi(fentr))) / fvd.Mod();
      if (fVerbose > 1) {
        cout << "================================================" << endl;
        cout << "Mod = " << thePoint->GetModule() << endl;
        cout << "Oct = " << thePoint->GetSector() << endl
             << "Entrance " << EntrancePosition << endl
             << "Exiting " << ExitingPosition << endl
             << "new Entrance " << fNewEntrance << endl
             << "new Exiting " << fNewExit << endl;
      }
      PndMCTrack *mcTrk = (PndMCTrack *)fMcTrackArray->At(thePoint->GetTrackID());
      ParticleType = PdgToIndex(mcTrk->GetPdgCode());
      fPid = ParticleType;

      fParamDigiModel->SetParams(ParticleType, Momentum, fNewEntrance, fNewExit);
      fParamDigiModel->Compute();

      PndMdtWaveform *aWf = new PndMdtWaveform(TrackID, DetectorID, TimeStamp, kTRUE); // wire signal
      ++nWaveformProduced;
      aWf->SetSignal(fParamDigiModel->GetWireSignal());
      fDataBuffer->FillNewData(aWf, aWf->GetTimeStamp(), aWf->GetActiveTime());
      // aWf->Print();
      delete aWf;

      const std::map<Int_t, std::vector<Double_t>> &fStripSignals = fParamDigiModel->GetStripSignals();
      std::map<Int_t, std::vector<Double_t>>::const_iterator mit = fStripSignals.begin();
      std::map<Int_t, std::vector<Double_t>>::const_iterator mend = fStripSignals.end();
      Int_t detID;
      for (; mit != mend; ++mit, ++nWaveformProduced) {
        // re-make detector id;
        fGeoIF->MapWireToStrip(DetectorID, EntrancePosition, detID); // waiting for updates for new geometry versions
        Int_t lid = mit->first;
        if (lid % 2 == 0)
          detID += lid / 2;
        else
          detID -= lid / 2;
        PndMdtWaveform *aSf = new PndMdtWaveform(TrackID, detID, TimeStamp, kFALSE); // strip signal
        ++nWaveformProduced;
        aSf->SetSignal(mit->second);
        fDataBuffer->FillNewData(aSf, aSf->GetTimeStamp(), aSf->GetActiveTime());
        delete aSf;
      }
    } else {
      ++it->second;
    }
  }

  if (fVerbose > 1) {
    std::map<key, Int_t>::iterator it = fMcTrackingInfo.begin();
    std::map<key, Int_t>::iterator end = fMcTrackingInfo.end();
    for (; it != end; ++it) {
      cout << "Track #" << it->first.TrkID << " contributes " << it->second << " points in tube " << it->first.DetID << endl;
    }
  }

  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndMdtPointsToWaveform, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

void PndMdtPointsToWaveform::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Mdt geometry parameter container
  //  fGeoPar = (PndMdtGeoPar*) db->getContainer("PndMdtGeoPar");
  // Get Mdt digitisation parameter container
  //  fDigiPar = (PndMdtDigiPar*) db->getContainer("PndMdtDigiPar");
}

void PndMdtPointsToWaveform::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}
void PndMdtPointsToWaveform::FinishTask()
{
  if (fVerbose > 0) {
    std::cout << "===================================================" << std::endl;
    std::cout << "PndMdtPointsToWaveform::FinishTask" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "Read points# " << HowManyPoint << std::endl;
    std::cout << "Produce waveforms# " << nWaveformProduced << std::endl;
    std::cout << "***************************************************" << std::endl;
  }
  fFile->cd();
  tTree->Write();
  fFile->Close();
}
Bool_t PndMdtPointsToWaveform::Digitize(PndMdtWaveform *theWf, Double_t &time, Double_t &amp, Bool_t isWire)
{
  const Double_t fWireNoiseSigma = 0.05;
  const Double_t fStripNoiseSigma = 0.01;
  const Double_t fSamplingInterval = 10.; // nano seconds

  time = -1;
  amp = -9999;
  Double_t fPeak = 0.;
  Bool_t NotFound = kTRUE;
  // Double_t fNoiseSigma; //[R.K. 01/2017] unused variable?
  Double_t fThreshold(1e9);
  if (isWire)
    fThreshold = 5. * fWireNoiseSigma;
  else
    fThreshold = 5. * fStripNoiseSigma;

  const std::vector<Double_t> &fSignalData = theWf->GetSignal();
  for (size_t is = 0; is < fSignalData.size(); ++is) {
    if (TMath::Abs(fSignalData[is]) > TMath::Abs(fPeak))
      fPeak = fSignalData[is];
    if (TMath::Abs(fSignalData[is]) > fThreshold && NotFound) {
      time = is * fSamplingInterval; // nano seconds
      NotFound = kFALSE;
    }
  }
  amp = fPeak;
  return time > 0;
}
Int_t PndMdtPointsToWaveform::PdgToIndex(Int_t pdg)
{
  Int_t abspdg = abs(pdg);
  if (abspdg == 11)
    return 0;
  else if (abspdg == 211)
    return 2;
  else if (abspdg == 13)
    return 1;
  else if (abspdg == 2212)
    return 4;
  else if (abspdg == 321)
    return 3;
  else
    return 1;
}

ClassImp(PndMdtPointsToWaveform)
