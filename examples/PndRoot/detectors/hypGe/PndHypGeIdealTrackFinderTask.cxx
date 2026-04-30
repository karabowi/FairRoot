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
// -----                PndHypIdealTrackFinderTask source file         -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TROOT.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndMCTrack.h"
#include "PndHypGePoint.h"
#include "PndHypGeIdealTrackFinderTask.h"
#include <vector>
#include <map>

// -----   Default constructor   -------------------------------------------
PndHypGeIdealTrackFinderTask::PndHypGeIdealTrackFinderTask() : FairTask("HYP Ideal Track Finding Task"), fEventNr(0)
{

  fMcBranch = "HypGePoint";
  fTrackBranch = "MCTrack";
}

// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndHypGeIdealTrackFinderTask::~PndHypGeIdealTrackFinderTask()
{
  delete (hismom);
}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndHypGeIdealTrackFinderTask::SetParContainers()
{
  // Get Base Container
  /*
    FairRunAna* ana = FairRunAna::Instance();
    FairRuntimeDb* rtdb=ana->GetRuntimeDb();
    fGeoPar = (PndHypGeoPar*)(rtdb->getContainer("PndHypGeoPar"));
  */
}

InitStatus PndHypGeIdealTrackFinderTask::ReInit()
{

  InitStatus stat = kERROR;
  return stat;

  /*
  FairRunAna* ana = FairRunAna::Instance();
  FairRuntimeDb* rtdb=ana->GetRuntimeDb();
  fGeoPar=(PndHypGeoPar*)(rtdb->getContainer("PndHypGeoPar"));

  return kSUCCESS;
  */
}

// -----   Public method Init   --------------------------------------------
InitStatus PndHypGeIdealTrackFinderTask::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndHypGeIdealTrackFinderTask::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }

  // Get input array

  fMcArray = (TClonesArray *)ioman->GetObject(fMcBranch);
  if (!fMcArray) {
    std::cout << "-W- PndHypGeIdealTrackFinderTask::Init: "
              << "No mcArray!" << std::endl;
    return kERROR;
  }

  fTrackArray = (TClonesArray *)ioman->GetObject(fTrackBranch);
  if (!fTrackArray) {
    std::cout << "-W- PndHypGeIdealTrackFinderTask::Init: "
              << "No trackArray!" << std::endl;
    return kERROR;
  }

  hismom = new TH1F("hismom", "HYPGe MC Points, momentum", 100, 0., 180.);
  hismom->SetTitle("Kinetic energy GeV");

  LOG(info) << " PndHypGeIdealTrackFinderTask: Initialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndHypGeIdealTrackFinderTask::Exec(Option_t *)
{
  // Reset output array
  // if ( ! fTrackCandArray )
  //   Fatal("Exec", "No trackCandArray");
  // fTrackCandArray->Delete();

  // mcHitMap = AssignHitsToTracks();
  for (int i = 0; i < fMcArray->GetEntriesFast(); i++) {         // get all MC Hits
    PndHypGePoint *myPoint = (PndHypGePoint *)(fMcArray->At(i)); // sort MCHits with Tracks

    TVector3 mom;
    myPoint->Momentum(mom);

    PndMCTrack *myTrack = (PndMCTrack *)(fTrackArray->At(myPoint->GetTrackID()));
    mcHitMap[myPoint->GetTrackID()].push_back(i);
  }

  std::cout << "------------Event " << fEventNr << "-------------" << std::endl;
  fEventNr++;
  std::cout << " size map " << mcHitMap.size() << std::endl;

  for (std::map<int, std::vector<int>>::const_iterator it = mcHitMap.begin(); it != mcHitMap.end(); it++) { // go through all tracks

    PndMCTrack *myTrack = (PndMCTrack *)(fTrackArray->At(it->first));
    if (myTrack == 0)
      continue;

    std::vector<int> MChits = it->second;

    if (MChits.size() > 0) {

      PndHypGePoint *startPoint = (PndHypGePoint *)(fMcArray->At(MChits[0]));
      if (startPoint == 0)
        continue;

      TVector3 mome, momst;

      startPoint->Momentum(momst);

      Int_t MothId, Mothpdg;
      MothId = myTrack->GetMotherID();

      if (MothId == -1) {
        Mothpdg = myTrack->GetPdgCode();
        if (Mothpdg == 2112) {
          /*else {
            cout<<"mother id "<<MothId<<endl;
            PndMCTrack *mother =(PndMCTrack*)fMcTr->At(MothId);
            Mothpdg = mother->GetPdgCode();
            }
          */

          Double_t Theta;
          hismom->Fill(180 / 3.1416 * momst.Theta());
        }
      }
    }
  }

  fMcArray->Delete();
  fTrackArray->Delete();
}

/*std::map<int, std::vector<int> > PndHypGeIdealTrackFinder::AssignHitsToTracks()
{

  std::map<int, std::vector<int> > result;
  for (int i = 0; i < fMcArray->GetEntriesFast(); i++){											//get all MC Hits
    PndHypGePoint* myPoint = (PndHypGePoint*)(fMcArray->At(i));									//sort MCHits with Tracks

    TVector3 mom;
    myPoint->Momentum(mom);


    PndMCTrack* myTrack = (PndMCTrack*)(fTrackArray->At(myPoint->GetTrackID()));
    result[myPoint->GetTrackID()].push_back(i);


  }
  return result;
}
*/

void PndHypGeIdealTrackFinderTask::WriteHistograms()
{

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("neutron");
    outfile->cd("neutron");

    outfile->WriteTObject(hismom);
    delete (hismom);
    hismom = nullptr;

    outfile->cd("..");

    outfile->Close();
    delete (outfile);
  }

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("neutron");
  //  file->cd("neutron");
  //
  //  hismom->Write();
  //  delete hismom;
  //  hismom = nullptr;
}
ClassImp(PndHypGeIdealTrackFinderTask)
