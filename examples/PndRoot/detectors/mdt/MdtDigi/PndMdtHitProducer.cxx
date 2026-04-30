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
// -----                PndMdtHitProducer source file                  -----
// -----                  revised by  Jifeng Hu            -----
// author, Jifeng Hu, hu@to.infn.it
// -------------------------------------------------------------------------

#include "PndMdtHitProducer.h"

#include "PndMdtHit.h"
#include "PndMdtDigi.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "FairRunAna.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
bool PndMdtHitProducer::PndMdtDigiLess::operator()(const PndMdtDigi *lv, const PndMdtDigi *rv) const
{
  return lv->GetDetectorID() < rv->GetDetectorID();
}

// -----   Default constructor   -------------------------------------------
PndMdtHitProducer::PndMdtHitProducer() : FairTask("MDT Hit Producer"), fTimeOrderedDigi(kFALSE)
{
  fVerbose = 0;
  // Reset();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMdtHitProducer::~PndMdtHitProducer() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus PndMdtHitProducer::Init()
{

  cout << "-I- PndMdtHitProducer::Init: "
       << "INITIALIZATION *********************" << endl;

  // FairRun* sim = FairRun::Instance(); //[R.K. 01/2017] unused variable?
  // FairRuntimeDb* rtdb=sim->GetRuntimeDb(); //[R.K. 01/2017] unused variable?

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMdtHitProducer::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  if (fTimeOrderedDigi) {
    fDigiArray = (TClonesArray *)ioman->GetObject("MdgDigi");
    if (!fDigiArray) {
      cout << "-W- PndMdtHitProducer::Init: "
           << "No MdtDigi array!" << endl;
      return kERROR;
    }
    fBoxArray = new TClonesArray("PndMdtDigi");
    fStripArray = new TClonesArray("PndMdtDigi");
  } else {
    // Get input array
    fBoxArray = (TClonesArray *)ioman->GetObject("MdtDigiBox");
    if (!fBoxArray) {
      cout << "-W- PndMdtHitProducer::Init: "
           << "No MdtDigiBox array!" << endl;
      return kERROR;
    }

    fStripArray = (TClonesArray *)ioman->GetObject("MdtDigiStrip");
    if (!fStripArray) {
      cout << "-W- PndMdtHitProducer::Init: "
           << "No MdtDigiStrip array!" << endl;
      return kERROR;
    }
  }

  fBoxClusterArray = new TClonesArray("PndMdtDigi");
  fStripClusterArray = new TClonesArray("PndMdtDigi");

  // Create and register output array
  fHitArray = new TClonesArray("PndMdtHit");

  ioman->Register("MdtHit", "Mdt", fHitArray, kTRUE);

  fNumofBoxDigis = fNumofStripDigis = fNumofHits = 0;
  fNumofGroupedBoxDigis = fNumofGroupedStripDigis = 0;
  // nBoxDigi = nMatchStripDigi1 = nMatchStripDigi2 = 0;
  fFunctor = new StopTime();
  fEventCounter = 0;
  fTimeWindow = 0.;

  LOG(info) << " PndMdtHitProducer: Intialization successfull";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndMdtHitProducer::Exec(Option_t *)
{
  fHitArray->Delete();
  fBoxClusterArray->Clear();
  fStripClusterArray->Clear();

  if (fTimeOrderedDigi) {
    fDigiArray->Clear();
    static const double fixed_time_length = 100; // unit nano second
    Double_t fEvtTime = FairRootManager::Instance()->GetEventTime();
    fTimeWindow = fEvtTime + fixed_time_length; // one time interval
    // loading in digis in a time-window
    // fCurrentPosition = fDigiArray->GetEntriesFast();
    if (FairRunAna::Instance()->IsTimeStamp()) {
      fDigiArray = FairRootManager::Instance()->GetData("TSMdtDigi", fFunctor, fTimeWindow);
    }
    if (fVerbose > 1) {
      cout << "------------------------PndMdtHitProducer-------------------------------------------" << endl;
      cout << "|Event No. #" << fEventCounter << ", time-based simulation," << fDigiArray->GetEntriesFast() << " digis in time window [" << fEvtTime << ", " << fTimeWindow
           << "] (ns)  |" << endl;
      if (fVerbose >= 3) {
        Int_t fCurrentPosition = 0;
        for (Int_t idigi = fCurrentPosition; idigi < fDigiArray->GetEntriesFast(); ++idigi) {
          PndMdtDigi *theDigi = (PndMdtDigi *)fDigiArray->At(idigi);
          cout << "  ";
          theDigi->Print();
          std::cout << std::endl;
        }
      }
      cout << "------------------------PndMdtHitProducer-------------------------------------------" << endl;
    }
    fBoxArray->Delete();
    // diliver digis to two queue
    for (Int_t idigi = 0; idigi < fDigiArray->GetEntriesFast(); ++idigi) {
      PndMdtDigi *theDigi = (PndMdtDigi *)fDigiArray->At(idigi);
      if (theDigi->isWire())
        (*fBoxArray)[fBoxArray->GetEntriesFast()] = theDigi;
      else
        (*fStripArray)[fStripArray->GetEntriesFast()] = theDigi;
    }
  }

  Int_t nBox = fBoxArray->GetEntriesFast();
  Int_t nStrip = fStripArray->GetEntriesFast();
  if (nBox == 0 || nStrip == 0)
    return;

  fNumofBoxDigis += nBox;
  fNumofStripDigis += nStrip;

  ClustersCollection fStripClusters; // grouped strip digis
  for (Int_t istrip = 0; istrip < nStrip; ++istrip) {
    PndMdtDigi *stripDigi = (PndMdtDigi *)fStripArray->At(istrip);
    ClustersColIter cit = fStripClusters.begin();
    ClustersColIter cend = fStripClusters.end();
    Bool_t isFoundinClusters = kFALSE;
    for (; cit != cend; ++cit) { // loop each cluster
      SingleCluster &aClu = *cit;
      SingleDigiIter dit = aClu.begin();
      SingleDigiIter dend = aClu.end();
      Bool_t found = kFALSE;
      for (; dit != dend; ++dit) {
        PndMdtDigi *aDigi = *dit;
        if (abs(aDigi->GetDetectorID() - stripDigi->GetDetectorID()) == 1) // neighbour
        {
          found = kTRUE;
          break;
        }
      }
      if (found) {
        isFoundinClusters = kTRUE;
        aClu.push_back(stripDigi);
        break;
      }
    }
    if (!isFoundinClusters) {
      SingleCluster aClu;
      aClu.push_back(stripDigi);
      fStripClusters.push_back(aClu);
    }
  }
  // box digis group
  ClustersCollection fBoxClusters; // grouped strip digis
  for (Int_t ibox = 0; ibox < nBox; ++ibox) {
    PndMdtDigi *boxDigi = (PndMdtDigi *)fBoxArray->At(ibox);
    if (fVerbose > 0)
      cout << "pos 0, " << ibox << ", " << boxDigi->X() << ", " << boxDigi->Y() << ", " << boxDigi->Z() << endl;

    ClustersColIter cit = fBoxClusters.begin();
    ClustersColIter cend = fBoxClusters.end();
    Bool_t isFoundinClusters = kFALSE;
    for (; cit != cend; ++cit) { // loop each cluster
      SingleCluster &aClu = *cit;
      SingleDigiIter dit = aClu.begin();
      SingleDigiIter dend = aClu.end();
      Bool_t found = kFALSE;
      for (; dit != dend; ++dit) {
        PndMdtDigi *aDigi = *dit;
        if (abs(aDigi->GetDetectorID() - boxDigi->GetDetectorID()) == 1) // neighbour
        {
          found = kTRUE;
          break;
        }
      }
      if (found) {
        isFoundinClusters = kTRUE;
        aClu.push_back(boxDigi);
        break;
      }
    }
    if (!isFoundinClusters) {
      SingleCluster aClu;
      aClu.push_back(boxDigi);
      fBoxClusters.push_back(aClu);
    }
  }
  //
  // std::cout<<"read "<<nStrip<<" strip digis"<<endl;
  std::map<Int_t, Int_t> fLayerMapofBox;
  ClustersColIter cit = fBoxClusters.begin();
  ClustersColIter cend = fBoxClusters.end();
  for (; cit != cend; ++cit) {
    SingleCluster &aClu = *cit;
    sort(aClu.begin(), aClu.end(), PndMdtDigiLess());
    size_t idx = aClu.size() / 2;
    SingleDigiIter dit = aClu.begin();
    while (idx--)
      ++dit;
    if (fVerbose > 0)
      cout << "dit pos 1, " << fBoxClusterArray->GetEntriesFast() << ", " << (*dit)->X() << ", " << (*dit)->Y() << ", " << (*dit)->Z() << endl;
    (*fBoxClusterArray)[fBoxClusterArray->GetEntriesFast()] = *dit;

    ++fLayerMapofBox[PndMdtID::LayerID((*dit)->GetDetectorID())];
  }

  std::map<Int_t, Int_t> fLayerMapofStrip;
  cit = fStripClusters.begin();
  cend = fStripClusters.end();
  for (; cit != cend; ++cit) {
    SingleCluster &aClu = *cit;
    sort(aClu.begin(), aClu.end(), PndMdtDigiLess());
    size_t idx = aClu.size() / 2;
    SingleDigiIter dit = aClu.begin();
    while (idx--)
      ++dit;
    (*fStripClusterArray)[fStripClusterArray->GetEntriesFast()] = *dit;
    ++fLayerMapofStrip[PndMdtID::LayerID((*dit)->GetDetectorID())];
  }

  std::map<Int_t, Int_t>::iterator mit = fLayerMapofBox.begin();
  std::map<Int_t, Int_t>::iterator mend = fLayerMapofBox.end();
  for (; mit != mend; ++mit) {
    ++fBoxHitMapofLayer[mit->second];
  }
  mit = fLayerMapofStrip.begin();
  mend = fLayerMapofStrip.end();
  for (; mit != mend; ++mit) {
    ++fStripHitMapofLayer[mit->second];
  }
  if (fVerbose > 0)
    std::cout << "produce " << fStripClusterArray->GetEntriesFast() << " digis" << endl;

  fNumofGroupedBoxDigis += fBoxClusterArray->GetEntriesFast();
  fNumofGroupedStripDigis += fStripClusterArray->GetEntriesFast();

  // combine box cluster and strip cluster to Hit, 2D-->3D
  for (Int_t iBox = 0; iBox < fBoxClusterArray->GetEntriesFast(); iBox++) {
    PndMdtDigi *boxDigi = (PndMdtDigi *)fBoxClusterArray->At(iBox);
    if (fVerbose > 0)
      cout << "pos 2, " << iBox << ", " << boxDigi->X() << ", " << boxDigi->Y() << ", " << boxDigi->Z() << endl;
    Int_t numStripFound = 0;
    //++ nBoxDigi;
    for (Int_t iStrip = 0; iStrip < fStripClusterArray->GetEntriesFast(); iStrip++) {
      PndMdtDigi *stripDigi = (PndMdtDigi *)fStripClusterArray->At(iStrip);
      if (PndMdtID::LayerID(boxDigi->GetDetectorID()) != PndMdtID::LayerID(stripDigi->GetDetectorID()))
        continue;
      //++ nMatchStripDigi1;
      if (TMath::Abs(boxDigi->GetTimeStamp() - stripDigi->GetTimeStamp()) > 3. * 14.85)
        continue;
      //++ nMatchStripDigi2;

      if (boxDigi->GetModule() == 1) // barrel
      {                              // take x,y
        TVector3 pos;
        TVector3 dpos(0.5, 0.5, 0.5);
        pos.SetX(boxDigi->X());
        pos.SetY(boxDigi->Y());
        pos.SetZ(stripDigi->Z());
        PndMdtHit *aHit = AddHit(boxDigi->GetDetectorID(), stripDigi->GetDetectorID(), pos, dpos, iBox, iStrip);
        aHit->SetTimeStamp(boxDigi->GetTimeStamp());
        // aHit->SetEvtNumber(boxDigi->GetEvtNumber());
      } else if (boxDigi->GetModule() == 2 && boxDigi->GetLayer() < 3) // endcap
      {
        TVector3 pos;
        TVector3 dpos(0.5, 0.5, 0.5);
        pos.SetX(boxDigi->X());
        pos.SetZ(boxDigi->Z());
        pos.SetY(stripDigi->Y());
        PndMdtHit *aHit = AddHit(boxDigi->GetDetectorID(), stripDigi->GetDetectorID(), pos, dpos, iBox, iStrip);
        aHit->SetTimeStamp(boxDigi->GetTimeStamp());
        // aHit->SetEvtNumber(boxDigi->GetEvtNumber());
      } else {
        TVector3 pos;
        TVector3 dpos(0.5, 0.5, 0.5);
        pos.SetX(stripDigi->X());
        pos.SetY(boxDigi->Y());
        pos.SetZ(boxDigi->Z());
        PndMdtHit *aHit = AddHit(boxDigi->GetDetectorID(), stripDigi->GetDetectorID(), pos, dpos, iBox, iStrip);
        aHit->SetTimeStamp(boxDigi->GetTimeStamp());
        // aHit->SetEvtNumber(boxDigi->GetEvtNumber());
      }
      ++numStripFound;
    }
    ++fMatchMap[numStripFound];
  }
}
void PndMdtHitProducer::Exec_old(Option_t *)
{

  // Reset output array
  fHitArray->Delete();

  Int_t nBox = fBoxArray->GetEntriesFast();
  Int_t nStrip = fStripArray->GetEntriesFast();
  if (nBox == 0 || nStrip == 0)
    return;

  for (Int_t iBox = 0; iBox < nBox; iBox++) {
    PndMdtDigi *boxDigi = (PndMdtDigi *)fBoxArray->At(iBox);
    for (Int_t iStrip = 0; iStrip < nStrip; iStrip++) {
      PndMdtDigi *stripDigi = (PndMdtDigi *)fStripArray->At(iStrip);
      if ((boxDigi->GetModule() != stripDigi->GetModule()) || (boxDigi->GetSector() != stripDigi->GetSector()) || (boxDigi->GetLayerID() != stripDigi->GetLayerID()))
        continue;
      vector<Int_t> vecBox = boxDigi->GetPointList();
      vector<Int_t> vecStrip = stripDigi->GetPointList();
      for (size_t vBox = 0; vBox < (vecBox.size()); vBox++) {
        for (size_t vStrip = 0; vStrip < (vecStrip.size()); vStrip++) {
          if (vecBox[vBox] == vecStrip[vStrip]) {
            TVector3 pos = stripDigi->GetLabPosition();
            TVector3 dpos(0.5, 0.5, 0.5);

            AddHit(boxDigi->GetDetectorID(), (Int_t)stripDigi->GetStrip(), pos, dpos, vecBox[vBox], vecStrip[vStrip]);
          }
        }
      }
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method AddHit   --------------------------------------------
PndMdtHit *PndMdtHitProducer::AddHit(Int_t detID, Int_t stripID, TVector3 &pos, TVector3 &dpos, Int_t bIndex, Int_t sIndex)
{
  // It fills the PndMdtHit category
  ++fNumofHits;
  TClonesArray &clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new (clref[size]) PndMdtHit(detID, stripID, pos, dpos, bIndex, sIndex);
}
// ----
void PndMdtHitProducer::FinishTask()
{
  if (fVerbose) {
    std::cout << "===================================================" << std::endl;
    std::cout << "PndMdtHitProducer::FinishTask" << std::endl;
    std::cout << "***************************************************" << std::endl;
    std::cout << "Read box digis: " << fNumofBoxDigis << ", after group: " << fNumofGroupedBoxDigis << std::endl;
    std::cout << "Read strip digis: " << fNumofStripDigis << ", after group: " << fNumofGroupedStripDigis << std::endl;
    std::cout << "Produce hits# " << fNumofHits << std::endl;
    std::cout << "statistics: number of strip digis matching per box digi#" << std::endl;
    std::map<Int_t, Int_t>::const_iterator it = fMatchMap.begin();
    std::map<Int_t, Int_t>::const_iterator end = fMatchMap.end();
    cout << "Nmatch\t\t"
         << "Count\t" << endl;
    for (; it != end; ++it) {
      cout << it->first << "\t\t" << it->second << endl;
    }
    std::cout << "statistics, number of hits per layer" << std::endl;
    std::cout << "================== Box  ========================" << endl;
    std::cout << "NumHits\t\tCount\t" << std::endl;
    std::map<Int_t, Int_t>::iterator mit = fBoxHitMapofLayer.begin();
    std::map<Int_t, Int_t>::iterator mend = fBoxHitMapofLayer.end();
    for (; mit != mend; ++mit) {
      cout << mit->first << "\t\t" << mit->second << std::endl;
    }
    std::cout << "================== strip  ========================" << endl;
    mit = fStripHitMapofLayer.begin();
    mend = fStripHitMapofLayer.end();
    for (; mit != mend; ++mit) {
      cout << mit->first << "\t\t" << mit->second << std::endl;
    }
    std::cout << "***************************************************" << std::endl;
  }
}

ClassImp(PndMdtHitProducer)
