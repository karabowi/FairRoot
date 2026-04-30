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
// -----                PndGemFindHitsAna source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemFindHitsAna.h"

// FairRoot includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairTrackParam.h"
#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRootFileSink.h"

// Pnd includes
#include "PndGemDigiPar.h"
#include "PndGemHit.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TMatrixFSym.h"

// C++ includes
#include <iostream>
#include <iomanip>
#include <map>
#include <cmath>
using std::cout;
using std::endl;
using std::map;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemFindHitsAna::PndGemFindHitsAna()
  : FairTask("GEM Find Hits Ana", 1), fDigiPar(nullptr), fGemHitArray(nullptr), fNofEvents(0), fGridSize(10.),
    //    fStatBegHist(nullptr),
    //    fGridHalfLen(nullptr),
    fHistoList(nullptr), fhFrontBackDiff(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindHitsAna::PndGemFindHitsAna(Int_t iVerbose)
  : FairTask("GEM Find Hits Ana", iVerbose), fDigiPar(nullptr), fGemHitArray(nullptr), fNofEvents(0), fGridSize(10.),
    //    fStatBegHist(nullptr),
    //    fGridHalfLen(nullptr),
    fHistoList(nullptr), fhFrontBackDiff(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemFindHitsAna::PndGemFindHitsAna(TString taskName, Int_t iVerbose)
  : FairTask(taskName, iVerbose), fDigiPar(nullptr), fGemHitArray(nullptr), fNofEvents(0), fGridSize(10.),
    //    fStatBegHist(nullptr),
    //    fGridHalfLen(nullptr),
    fHistoList(nullptr), fhFrontBackDiff(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemFindHitsAna::~PndGemFindHitsAna() {}

// -----   Init  -----------------------------------------------------------
InitStatus PndGemFindHitsAna::Init()
{

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- " << GetName() << "::Init: "
         << "RootManager not instantised!" << endl;
    return kERROR;
  }

  // Get the pointer to the singleton FairRunAna object
  FairRunAna *ana = FairRunAna::Instance();
  if (nullptr == ana) {
    cout << "-E- " << GetName() << "::Init :"
         << " no FairRunAna object!" << endl;
    return kERROR;
  }
  // Get the pointer to run-time data base
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  if (nullptr == rtdb) {
    cout << "-E- " << GetName() << "::Init :"
         << " no runtime database!" << endl;
    return kERROR;
  }

  // Get Gem hit Array
  fGemHitArray = (TClonesArray *)ioman->GetObject("GEMHit");
  if (!fGemHitArray) {
    LOG(error) << " " << GetName() << "::Init: No PndGemHit array!";
    return kERROR;
  }

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)(rtdb->getContainer("PndGemDetectors"));

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  fGridSize = 5.;
  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemFindHitsAna::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemFindHitsAna::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)(db->getContainer("PndGemDetectors"));

  fNofEvents = 0;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemFindHitsAna::Exec(Option_t *)
{

  if (fVerbose) {
    cout << "IN EVENT GOT " << fGemHitArray->GetEntries() << " hits." << endl;
    cout << " - - - - - > " << fhFrontBackDiff->GetEntries() << " histograms" << endl;
  }

  Int_t nofGemHits = fGemHitArray->GetEntries();

  PndGemHit *gemHit1 = nullptr;
  PndGemHit *gemHit2 = nullptr;

  for (Int_t igem1 = 0; igem1 < nofGemHits; igem1++) {
    gemHit1 = (PndGemHit *)fGemHitArray->At(igem1);
    for (Int_t igem2 = 0; igem2 < nofGemHits; igem2++) {
      gemHit2 = (PndGemHit *)fGemHitArray->At(igem2);

      if (TMath::Abs(gemHit2->GetZ() - gemHit1->GetZ() - 2.) < 2.) {
        Int_t quartId = 0;
        if (gemHit1->GetY() < 0.)
          quartId += 2;
        if (gemHit1->GetX() < 0.)
          quartId += 1;
        Int_t histogramToFill = fStatBegHist[gemHit1->GetStationNr() - 1] + fGridHalfLen[gemHit1->GetStationNr() - 1] * fGridHalfLen[gemHit1->GetStationNr() - 1] * quartId +
                                fGridHalfLen[gemHit1->GetStationNr() - 1] * (Int_t(TMath::Abs(gemHit1->GetY()) / fGridSize)) + (Int_t(TMath::Abs(gemHit1->GetX()) / fGridSize));

        ((TH2F *)fhFrontBackDiff->At(histogramToFill))->Fill(gemHit2->GetX() - gemHit1->GetX(), gemHit2->GetY() - gemHit1->GetY());

        if (fVerbose > 1) {
          cout << "will compare " << gemHit1->GetX() << " " << gemHit1->GetY() << " " << gemHit1->GetZ() << endl
               << "        with " << gemHit2->GetX() << " " << gemHit2->GetY() << " " << gemHit2->GetZ() << endl;
          cout << " IN STATION " << gemHit1->GetStationNr() << endl;
          cout << " 1 : " << fStatBegHist[gemHit1->GetStationNr() - 1] << endl;
          cout << " 2 : " << fGridHalfLen[gemHit1->GetStationNr() - 1] * fGridHalfLen[gemHit1->GetStationNr() - 1] * quartId << endl;
          cout << " 3 : " << fGridHalfLen[gemHit1->GetStationNr() - 1] * (Int_t(gemHit1->GetY() / fGridSize)) << endl;
          cout << " 4 : " << (Int_t(gemHit1->GetX() / fGridSize)) << endl;
          cout << " histogram to fill " << histogramToFill << endl;
          cout << " - - - - - > " << fhFrontBackDiff->At(histogramToFill)->GetName() << " / \"" << fhFrontBackDiff->At(histogramToFill)->GetTitle() << "\"" << endl;
          cout << "====================================================================================" << endl;
        }
      }
    }
  }

  fNofEvents++;
}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemFindHitsAna::CreateHistos()
{
  fHistoList = new TList();

  if (fVerbose)
    cout << "ought to create histogram for " << fDigiPar->GetNStations() << " stations." << endl;

  Int_t nofStations = fDigiPar->GetNStations();
  Int_t nofHists = 0;

  fhFrontBackDiff = new TClonesArray("TH2F", 5000);

  for (Int_t istat = 0; istat < nofStations; istat++) {
    // PndGemStation* station = (PndGemStation*)fDigiPar->GetStation(istat);
    PndGemSensor *sensor = (PndGemSensor *)fDigiPar->GetSensor(istat + 1, 1);

    if (fVerbose)
      cout << "sensor @ z = " << sensor->GetZ0() << " has an outer radius of " << sensor->GetOuterRadius() << endl;

    Int_t gridHalfLen = TMath::Ceil(sensor->GetOuterRadius() / fGridSize);
    fStatBegHist.push_back(nofHists);
    fGridHalfLen.push_back(gridHalfLen);

    if (fVerbose)
      cout << "                           --> the grid of " << fGridSize << "x" << fGridSize << "cm^2 requires " << 4 * gridHalfLen * gridHalfLen << " histograms" << endl;

    for (Int_t iquart = 0; iquart < 4; iquart++) {
      for (Int_t igry = 0; igry < gridHalfLen; igry++) {
        for (Int_t igrx = 0; igrx < gridHalfLen; igrx++) {
          new ((*fhFrontBackDiff)[nofHists++])
            TH2F(Form("fhFrontBackCorr_s%d_q%d_x%d_y%d", istat, iquart, igrx, igry),
                 Form("front back corr on station %d, x(%s%.1f:%s%.1f), y(%s%.1f:%s%.1f)", istat, (iquart % 2 ? "-" : ""), igrx * fGridSize, (iquart % 2 ? "-" : ""),
                      (igrx + 1.) * fGridSize, (iquart / 2 ? "-" : ""), igry * fGridSize, (iquart / 2 ? "-" : ""), (igry + 1.) * fGridSize),
                 200, -1., 1., 200, -1., 1.);
          if (fVerbose)
            cout << nofHists - 1 << ": histogram " << fhFrontBackDiff->At(nofHists - 1)->GetName() << " / " << fhFrontBackDiff->At(nofHists - 1)->GetTitle() << endl;
        }
      }
    }

    new ((*fhFrontBackDiff)[nofHists++]) TH2F(Form("fhFrontBackDiffX_s%d", istat), Form("front back X difference on station %d", istat), 2 * gridHalfLen, -gridHalfLen * fGridSize,
                                              gridHalfLen * fGridSize, 2 * gridHalfLen, -gridHalfLen * fGridSize, gridHalfLen * fGridSize);
    new ((*fhFrontBackDiff)[nofHists++]) TH2F(Form("fhFrontBackDiffY_s%d", istat), Form("front back Y difference on station %d", istat), 2 * gridHalfLen, -gridHalfLen * fGridSize,
                                              gridHalfLen * fGridSize, 2 * gridHalfLen, -gridHalfLen * fGridSize, gridHalfLen * fGridSize);
  }

  if (fVerbose) {
    cout << "HM, seems that fhFrontBackDiff is filled with histograms, and there are " << nofHists << " of them" << endl;

    cout << "fStatBegHist has " << fStatBegHist.size() << " entries:" << endl;
    for (size_t isbh = 0; isbh < fStatBegHist.size(); isbh++)
      cout << " ---> " << fStatBegHist[isbh] << endl;
    cout << "fGridHalfLen has " << fGridHalfLen.size() << " entries:" << endl;
    for (size_t ighl = 0; ighl < fGridHalfLen.size(); ighl++)
      cout << " ---> " << fGridHalfLen[ighl] << endl;
  }
}
// ------------------------------------------------------------

// -----   Private method AnaHistos   --------------------------------------------
void PndGemFindHitsAna::AnaHistos()
{

  Int_t nofStations = fDigiPar->GetNStations();

  Double_t quartX[4] = {1., -1., 1., -1.};
  Double_t quartY[4] = {1., 1., -1., -1.};
  for (Int_t istat = 0; istat < nofStations; istat++) {

    Int_t histCounter = fStatBegHist[istat];

    Int_t histogramToFill = fStatBegHist[istat] + 4 * fGridHalfLen[istat] * fGridHalfLen[istat];

    for (Int_t iquart = 0; iquart < 4; iquart++) {
      for (Int_t igry = 0; igry < fGridHalfLen[istat]; igry++) {
        for (Int_t igrx = 0; igrx < fGridHalfLen[istat]; igrx++) {
          if (((TH2F *)(fhFrontBackDiff->At(histCounter)))->GetEntries() > 0) {
            ((TH2F *)fhFrontBackDiff->At(histogramToFill + 0))
              ->Fill(quartX[iquart] * (fGridSize / 2. + fGridSize * igrx), quartY[iquart] * (fGridSize / 2. + fGridSize * igry),
                     ((TH2F *)fhFrontBackDiff->At(histCounter))->GetMean(1));
            ((TH2F *)fhFrontBackDiff->At(histogramToFill + 1))
              ->Fill(quartX[iquart] * (fGridSize / 2. + fGridSize * igrx), quartY[iquart] * (fGridSize / 2. + fGridSize * igry),
                     ((TH2F *)fhFrontBackDiff->At(histCounter))->GetMean(2));
          } else {
            ((TH2F *)fhFrontBackDiff->At(histogramToFill + 0))
              ->Fill(quartX[iquart] * (fGridSize / 2. + fGridSize * igrx), quartY[iquart] * (fGridSize / 2. + fGridSize * igry), -1111.);
            ((TH2F *)fhFrontBackDiff->At(histogramToFill + 1))
              ->Fill(quartX[iquart] * (fGridSize / 2. + fGridSize * igrx), quartY[iquart] * (fGridSize / 2. + fGridSize * igry), -1111.);
          }
          histCounter++;
        }
      }
    }
  }
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemFindHitsAna::Finish()
{
  cout << "-------------------- PndGemFindHitsAna : Summary ------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << "---------------------------------------------------------------------" << endl;

  AnaHistos();

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("GemFindHitsAna");
    outfile->cd("GemFindHitsAna");
    TIter next(fHistoList); // this looks strange here
    for (Int_t ihist = 0; ihist < fhFrontBackDiff->GetEntries(); ihist++) {
      outfile->WriteTObject(fhFrontBackDiff->At(ihist));
    }
    outfile->cd("..");
  }

  //  TFile *temp = gFile;
  //  FairRootManager *ioman = FairRootManager::Instance();
  //  gFile = ioman->GetOutFile();
  //  gDirectory = (TDirectory *)gFile;
  //
  //  gDirectory->mkdir("GemFindHitsAna");
  //  gDirectory->cd("GemFindHitsAna");
  //  TIter next(fHistoList);
  //  for (Int_t ihist = 0; ihist < fhFrontBackDiff->GetEntries(); ihist++) {
  //    fhFrontBackDiff->At(ihist)->Write();
  //  }
  //  //  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  //  gDirectory->cd("..");
  //
  //  gFile = temp;
}
// ------------------------------------------------------------

ClassImp(PndGemFindHitsAna)
