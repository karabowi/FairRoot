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

//---------------------------------------------------------------------
// File and Version Information:
// 	$Id: $
//
// Description:
//	This object constructs a list of clusters from a list of digis.
//      This module use a simple algorithm.
//      Digis which are in a Cluster are add to this cluster.
//      If a Digi is in more than one cluster, the clusters are merged.
//      Digis are added to the cluster if they have an energy above
//      _digiEnergyTreshold->value().
//
// Environment:
//	Software developed for the PANDA Detector at GSI.
//
// Author List:
//	Jan Zhong            //------------------------------------------------------------------------

#include "PndEmcCorrBump.h"
#include <assert.h>
#include "PndEmcXClMoments.h"
#include "PndEmcStructure.h"
#include "PndEmcMapper.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcCluster.h"
#include "PndEmcDigi.h"
#include "PndEmcBump.h"
#include "PndEmcSharedDigi.h"
#include "PndMCTrack.h"
#include "PndEmcClusterProperties.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"
#include "PndEmcDigiCalibrator.h"
#include "TClonesArray.h"
#include "TStopwatch.h"
#include "TROOT.h"

#include <iostream>
//#include <vector>
//#include <set>

using std::cout;
using std::endl;

Int_t PndEmcCorrBump::fEventCounter = 0;

// Double_t::PndEmcCorrBump::fTimeWindowOfShowerDigi[5][17]={
////(x-2)<0.5,.6-.8,.8-1,1-2, 2-3, 3-4, 4-5, 5-6, 6-7, 7-8, 8-9, 9-10, 10-20,20-30,30-40,40-50,50+
//	     22., 15., 9.8,  5.7, 3.0, 2.0, 1.6, 1.3, 1.0, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=1*/
//	     22., 15., 9.8,  5.7, 3.0, 2.0, 1.6, 1.3, 1.0, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=2*/
//	     7.0, 5.0, 3.6,  2.2, 1.2, 0.8, 0.7, 0.6, 0.5, 0.45,0.38,0.36, 0.27, 0.2,  0.20,0.20,  0.20,/*mod=3*/
//	     22., 15., 9.8,  5.7, 3.0, 2.0, 1.6, 1.3, 1.0, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=4*/
//	     2.3, 1.8, 1.3,  0.9, 0.6, 0.4, 0.3, 0.3, 0.3, 0.3, 0.25,0.25, 0.25, 0.25, 0.25, 0.25, 0.25/*mod=5*/
//};
// Double_t::PndEmcCorrBump::fTimeWindowOfSeedDigi[5][17]={
////(x-2)<0.5,.6-.8,.8-1,1-2, 2-3, 3-4, 4-5, 5-6, 6-7, 7-8, 8-9, 9-10, 10-20,20-30,30-40,40-50,50+
//	     14., 14., 9.4,  4.7, 3.0, 2.1, 1.6, 1.3, 1.1, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=1*/
//	     14., 14., 9.4,  4.7, 3.0, 2.1, 1.6, 1.3, 1.1, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=2*/
//	     7.0, 5.0, 3.6,  2.2, 1.2, 0.8, 0.7, 0.6, 0.5, 0.45,0.38,0.36, 0.27, 0.2,  0.20,0.20,  0.20,/*mod=3*/
//	     14., 14., 9.4,  4.7, 3.0, 2.1, 1.6, 1.3, 1.0, 0.9, 0.8, 0.7,  0.5,  0.3,  0.2, 0.16,  0.14,/*mod=4*/
//	     2.3, 1.8, 1.3,  0.9, 0.6, 0.4, 0.3, 0.3, 0.3, 0.3, 0.25,0.25, 0.25, 0.25, 0.25, 0.25, 0.25/*mod=5*/
//};

std::ostream &operator<<(std::ostream &os, const TVector3 &pos)
{
  os << "(" << pos[0] << ',' << pos[1] << ',' << pos[2] << ")";
  return os;
}

PndEmcCorrBump::PndEmcCorrBump(Int_t verbose, Bool_t storeclusters)
  : fDigiArray(nullptr), fSharedDigiArray(nullptr), fBumpArray(nullptr), fMapVersion(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()), fRecoPar(new PndEmcRecoPar()),
    fVerbose(verbose), fStoreClusters(storeclusters)
{
}

//--------------
// Destructor --
//--------------

PndEmcCorrBump::~PndEmcCorrBump() {}

// -----   Public method Init   -------------------------------
InitStatus PndEmcCorrBump::Init()
{
  PndEmcDigi::InitDigiArrayTBD();
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcCorrBump::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fDigiArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcDigi"));
  if (!fDigiArray) {
    cout << "-W- PndEmcCorrBump::Init: "
         << "No PndEmcDigi array!" << endl;
    return kERROR;
  }
  // Get input array
  fSharedDigiArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcSharedDigi"));
  if (!fSharedDigiArray) {
    cout << "-W- PndEmcCorrBump::Init: "
         << "No PndEmcSharedDigi array!" << endl;
    return kERROR;
  }

  // Get input array
  fBumpArray = dynamic_cast<TClonesArray *>(ioman->GetObject("EmcBump"));
  if (!fBumpArray) {
    cout << "-W- PndEmcCorrBump::Init: "
         << "No EmcBump array!" << endl;
    return kERROR;
  }
  fBumpArrayTBD = new TClonesArray("PndEmcBump");

  if (!strcmp(fRecoPar->GetEmcClusterPosMethod(), "lilo")) {
    cout << "Lilo cluster position method" << endl;
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmA());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmB());
    fClusterPosParam.push_back(fRecoPar->GetOffsetParmC());
  }

  HowManyDigi = 0;
  HowManyCluster = 0;

  LOG(info) << " PndEmcCorrBump: Intialization successfull";

  return kSUCCESS;
}

void PndEmcCorrBump::Exec(Option_t *)
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }
  // Reset output array
  if (!fBumpArray)
    Fatal("Exec", "No Bump Array");

  Double_t fEventTime = FairRootManager::Instance()->GetEventTime();
  if (fVerbose > 0) {
    Int_t nDigis = fDigiArray->GetEntriesFast();
    Int_t nSharedDigis = fSharedDigiArray->GetEntriesFast();
    cout << "*************************PndEmcCorrBump*****************************" << endl;
    cout << "Event NO. #" << fEventCounter << ", EventTime#" << fEventTime << ", Digis#" << nDigis << ", SharedDigis#" << nSharedDigis << endl;
    cout << "********************************************************************" << endl;
    cout << "Before correction, bumps#" << fBumpArray->GetEntriesFast() << ", BumptoBeDetermined #" << fBumpArrayTBD->GetEntriesFast() << ", DigitoBeDetermined #"
         << PndEmcDigi::fDigiArrayTBD->GetEntriesFast() << endl;
    cout << "********************************************************************" << endl;
    // cout<<"------------> fDigiArray#"<<fDigiArray->GetEntriesFast()<<endl;
    // for(Int_t iDigi=0; iDigi < fDigiArray->GetEntriesFast(); ++iDigi){
    //	PndEmcDigi* theDigi = (PndEmcDigi*)fDigiArray->At(iDigi);
    //	theDigi->Print();
    //	cout<<endl;
    //}
    // cout<<"------------> fSharedDigiArray#"<<fSharedDigiArray->GetEntriesFast()<<endl;
    // for(Int_t iDigi=0; iDigi < fSharedDigiArray->GetEntriesFast(); ++iDigi){
    //	PndEmcSharedDigi* theDigi = (PndEmcSharedDigi*)fSharedDigiArray->At(iDigi);
    //	theDigi->Print();
    //	cout<<endl;
    //}
    // if(fSharedDigiArray->GetEntriesFast() != fDigiArray->GetEntriesFast())
    //	cout<<"fSharedDigiArray.size() - fDigiArray.size() ##="<<(fDigiArray->GetEntriesFast()-fSharedDigiArray->GetEntriesFast())<<endl;
  }
  // merge buffer bumps into current fBumpArray
  // Int_t nBumpTBD = fBumpArrayTBD->GetEntriesFast();
  // for(Int_t ibump = 0; ibump < nBumpTBD; ++ibump ){
  //	PndEmcBump* copy = (PndEmcBump*)fBumpArrayTBD->At(ibump);
  //	new ((*fBumpArray)[nBump++]) PndEmcBump(*copy);
  //}
  // fBumpArrayTBD->Delete();

  // variabes definition
  std::vector<PndEmcBump *> taggedBumpofToBeDeleted;
  PndEmcDigi *seedDigi(nullptr);
  Double_t EnergyOfSeedDigi(-1.), fTimeError(0.), CalibTimeOfaDigi(0.), CalibTimeOfSeedDigi(0.);
  Int_t TheIndexOfModule; // TheIndexOfEnergy,  //[R.K. 01/2017] unused variable?
  Int_t iDigi = PndEmcDigi::fDigiArrayTBD->GetEntriesFast();
  Int_t iBump = 0;
  Bool_t isBumpOK = kTRUE;
  for (iBump = 0; iBump < fBumpArray->GetEntriesFast(); ++iBump) {
    EnergyOfSeedDigi = -1.;
    // isBumpOK = kTRUE;
    PndEmcBump *theBump = (PndEmcBump *)fBumpArray->At(iBump);
    // find the seed digi with maximum energy
    const std::vector<Int_t> &listOfDigi = theBump->DigiList();
    for (size_t id = 0; id < listOfDigi.size(); ++id) {
      PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(listOfDigi[id]);
      if (theDigi->GetEnergy() > EnergyOfSeedDigi) {
        seedDigi = theDigi;
        EnergyOfSeedDigi = theDigi->GetEnergy();
      }
      if (fVerbose > 1) {
        theDigi->Print();
        CalibTimeOfaDigi = digiCalibrator.CalibrationEvtTimeByDigi(theDigi, kFALSE);
        cout << ", CalibrationTimeOfEvent#" << CalibTimeOfaDigi << endl;
      }
    }
    theBump->SetEventNo(seedDigi->fEvtNo);
    // check current bump belongs to current event by event time
    fTimeError = digiCalibrator.GetTimeResolutionOfDigi(seedDigi);
    CalibTimeOfSeedDigi = digiCalibrator.CalibrationEvtTimeByDigi(seedDigi, kFALSE);
    if (CalibTimeOfSeedDigi - fEventTime < 5. * fTimeError) { // out of region
      isBumpOK = kTRUE;
    } else {
      taggedBumpofToBeDeleted.push_back(theBump);
      isBumpOK = kFALSE;
    }
    if (fVerbose > 0) {
      std::cout << "Bump[" << iBump << "], (Nd, E, Es, CalT, EvtT, TimeR, EvtNo)=(" << listOfDigi.size() << ", " << theBump->GetEnergy() << ", " << EnergyOfSeedDigi << ", "
                << CalibTimeOfSeedDigi << ", " << fEventTime << ", " << fTimeError << ", " << seedDigi->fEvtNo // theBump->where()
                << ") ... ";
      cout << (isBumpOK ? "[ OK ]" : "[ Veto ]") << endl;
    }

    // remove digis those time out of region compared to time of seed digi.
    if (isBumpOK) {
      Double_t WeightedFactor = 0.;
      Bool_t DigiStatus = kTRUE;
      for (size_t id = 0; id < listOfDigi.size(); ++id) {
        PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(listOfDigi[id]);
        CalibTimeOfaDigi = digiCalibrator.CalibrationEvtTimeByDigi(theDigi, kFALSE);
        fTimeError = digiCalibrator.GetTimeResolutionOfDigi(theDigi);
        if (CalibTimeOfaDigi - CalibTimeOfSeedDigi < 5. * fTimeError) { // time  ok
          if (TMath::Abs(CalibTimeOfaDigi - CalibTimeOfSeedDigi) < 3. * fTimeError) {
            WeightedFactor += 1. / fTimeError / fTimeError;
          }
          DigiStatus = kTRUE;
        } else {
          theBump->removeDigi(fSharedDigiArray, listOfDigi[id]);
          // save digis to a buffer for analysis in next event
          // if(CalibTimeOfaDigi - CalibTimeOfSeedDigi > 5.*fTimeError)
          new ((*PndEmcDigi::fDigiArrayTBD)[iDigi++]) PndEmcDigi(*theDigi);
          DigiStatus = kFALSE;
        }
        if (fVerbose > 0) {
          TheIndexOfModule = theDigi->GetModule();
          cout << "Digi[" << id << "] of bump[" << iBump << "], (E, Mod, CalT, EvtT, SigmaT, EvtNO) = (" << theDigi->GetEnergy() << ", " << TheIndexOfModule << ", "
               << CalibTimeOfaDigi << ", " << fEventTime << ", " << fTimeError << ", " << theDigi->fEvtNo << (DigiStatus == kTRUE ? ") ... [ OK ]" : ") ... [ Veto ]") << endl;
          // theDigi->Print();
          // cout<<endl;
        }
      }
    }
  }
  // remove tagged bumps
  for (size_t ix = 0; ix < taggedBumpofToBeDeleted.size(); ++ix) {
    const std::vector<Int_t> &listOfDigi = taggedBumpofToBeDeleted[ix]->DigiList();
    // copy all digis of this bump to the buffer PndEmcDigi::fDigiArrayTBD;
    for (size_t id = 0; id < listOfDigi.size(); ++id) {
      PndEmcDigi *theDigi = (PndEmcDigi *)fSharedDigiArray->At(listOfDigi[id]);
      new ((*PndEmcDigi::fDigiArrayTBD)[iDigi++]) PndEmcDigi(*theDigi);
    }
    fBumpArray->Remove(taggedBumpofToBeDeleted[ix]);
    fBumpArray->Compress();
  }

  if (fVerbose > 0) {
    cout << "******************************************************" << endl;
    cout << "After correction, bumps#" << fBumpArray->GetEntriesFast() << ", BumptoBeDetermined #" << fBumpArrayTBD->GetEntriesFast() << ", DigitoBeDetermined #"
         << PndEmcDigi::fDigiArrayTBD->GetEntriesFast() << endl;
    cout << "**********************PndEmcCorrBump******************" << endl;
  }
  // remake bump
  for (Int_t i = 0; i < fBumpArray->GetEntriesFast(); i++) {
    PndEmcBump *tmpbump = (PndEmcBump *)fBumpArray->At(i);
    PndEmcClusterProperties clusterProperties(*tmpbump, fSharedDigiArray);
    HowManyDigi += tmpbump->NumberOfDigis();

    if (!tmpbump->IsEnergyValid())
      tmpbump->SetEnergy(clusterProperties.Energy());
    if (!tmpbump->IsPositionValid())
      tmpbump->SetPosition(clusterProperties.Where(fRecoPar->GetEmcClusterPosMethod(), fClusterPosParam));
    PndEmcXClMoments xClMoments(*tmpbump, fSharedDigiArray);
    tmpbump->SetZ20(xClMoments.AbsZernikeMoment(2, 0, 15));
    tmpbump->SetZ53(xClMoments.AbsZernikeMoment(5, 3, 15));
    tmpbump->SetLatMom(xClMoments.Lat());
  }
  HowManyCluster += fBumpArray->GetEntriesFast();

  fEventCounter++;
  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndEmcCorrBump, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

// Helper function, does not depend on class, identical to the one in PndEmcHitProducer

void PndEmcCorrBump::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");

  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");

  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar *)db->getContainer("PndEmcRecoPar");
}

void PndEmcCorrBump::SetStorageOfData(Bool_t val)
{
  fStoreClusters = val;
  return;
}
void PndEmcCorrBump::FinishTask()
{
  std::cout << "*********************************************************" << std::endl;
  std::cout << "PndEmcCorrBump::FinishTask" << std::endl;
  std::cout << "*********************************************************" << std::endl;
  std::cout << "Read digis# " << HowManyDigi << std::endl;
  std::cout << "Produce cluster# " << HowManyCluster << std::endl;
  std::cout << "*********************************************************" << std::endl;
}

ClassImp(PndEmcCorrBump)
