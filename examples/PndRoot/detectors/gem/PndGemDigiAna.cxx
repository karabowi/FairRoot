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
// -----                PndGemDigiAna source file             -----
// -----                Created 02.06.2009 by R. Karabowicz            -----
// -------------------------------------------------------------------------

#include "PndGemDigiAna.h"

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
#include "PndGemDigi.h"
// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"

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
PndGemDigiAna::PndGemDigiAna() : FairTask("GEM Digis Ana", 1), fDigiPar(nullptr), fGemDigiArray(nullptr), fNofEvents(0), fHistoList(nullptr), fHistoArray(nullptr) {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemDigiAna::PndGemDigiAna(Int_t iVerbose)
  : FairTask("GEM Digis Ana", iVerbose), fDigiPar(nullptr), fGemDigiArray(nullptr), fNofEvents(0), fHistoList(nullptr), fHistoArray(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemDigiAna::PndGemDigiAna(TString taskName, Int_t iVerbose)
  : FairTask(taskName, iVerbose), fDigiPar(nullptr), fGemDigiArray(nullptr), fNofEvents(0), fHistoList(nullptr), fHistoArray(nullptr)
{
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemDigiAna::~PndGemDigiAna() {}

// -----   Init  -----------------------------------------------------------
InitStatus PndGemDigiAna::Init()
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

  // Get Gem digi Array
  fGemDigiArray = (TClonesArray *)ioman->GetObject("GEMDigi");
  if (!fGemDigiArray) {
    LOG(error) << " " << GetName() << "::Init: No PndGemDigi array!";
    return kERROR;
  }

  // Get GEM digitisation parameter container
  fDigiPar = (PndGemDigiPar *)(rtdb->getContainer("PndGemDetectors"));

  LOG(info) << " " << fName.Data() << "::Init(). There are " << fDigiPar->GetNStations() << " GEM stations.";
  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  CreateHistos();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemDigiAna::ReInit()
{

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemDigiAna::SetParContainers()
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
void PndGemDigiAna::Exec(Option_t *)
{

  if (fVerbose) {
    cout << "IN EVENT GOT " << fGemDigiArray->GetEntries() << " hits." << endl;
    cout << " - - - - - > " << fHistoArray->GetEntries() << " histograms" << endl;
  }

  Int_t nofGemDigis = fGemDigiArray->GetEntries();

  PndGemDigi *gemDigi = nullptr;

  for (Int_t idigi = 0; idigi < nofGemDigis; idigi++) {
    gemDigi = (PndGemDigi *)fGemDigiArray->At(idigi);

    Int_t digiCode = 4 * (gemDigi->GetStationNr() - 1) + 2 * (gemDigi->GetSensorNr() - 1) + gemDigi->GetSide();

    if (digiCode < 0 || digiCode > 11)
      cout << "oops, wrong digiCode " << digiCode << " from station " << gemDigi->GetStationNr() << " sensor " << gemDigi->GetSensorNr() << " side " << gemDigi->GetSide() << endl;

    Double_t lastTime = ((TH1F *)fHistoArray->At(digiCode))->GetBinContent(gemDigi->GetChannelNr());

    if (lastTime > 0.) {
      // cout << " DIGI on station " << gemDigi->GetStationNr()
      // 	   << " sensor " << gemDigi->GetSensorNr()
      // 	   << " side " << gemDigi->GetSide()
      // 	   << " channel " << gemDigi->GetChannelNr()
      // 	   << " came again at " << gemDigi->GetTimeStamp()
      // 	   << " after " << lastTime
      // 	   << endl;
      ((TH1F *)fHistoArray->At(fDigiPar->GetNStations() * 4 + digiCode))->Fill(gemDigi->GetTimeStamp() - lastTime);
    }

    ((TH1F *)fHistoArray->At(digiCode))->SetBinContent(gemDigi->GetChannelNr(), gemDigi->GetTimeStamp());
  }

  fNofEvents++;
}
// ------------------------------------------------------------

// -----   Private method CreateHistos   --------------------------------------------
void PndGemDigiAna::CreateHistos()
{
  fHistoList = new TList();

  if (fVerbose)
    cout << "ought to create histogram for " << fDigiPar->GetNStations() << " stations." << endl;

  Int_t nofStations = fDigiPar->GetNStations();
  Int_t nofHists = 0;

  fHistoArray = new TClonesArray("TH1F", 5000);

  for (Int_t istat = 0; istat < nofStations; istat++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(istat);

    if (station->GetNSensors() != 2) {
      std::cout << "will have problems with different numbers of sensors per station (" << station->GetNSensors() << ")" << std::endl;
    }

    for (Int_t isens = 0; isens < station->GetNSensors(); isens++) {
      PndGemSensor *sensor = (PndGemSensor *)station->GetSensor(isens);

      for (Int_t iside = 0; iside < 2; iside++) {

        new ((*fHistoArray)[nofHists++])
          TH1F(Form("fhLDTS_s%d_s%d_s%d", istat, isens, iside), Form("Last digi time stamp, station %d, sensor %d, %s side", istat, isens, (iside ? "back" : "front")),
               sensor->GetSideChannels(iside) + 1, -0.5, sensor->GetSideChannels(iside) + 0.5);
      }
    }
  }

  for (Int_t istat = 0; istat < nofStations; istat++) {
    PndGemStation *station = (PndGemStation *)fDigiPar->GetStation(istat);

    for (Int_t isens = 0; isens < station->GetNSensors(); isens++) {
      // PndGemSensor* sensor = (PndGemSensor*)station->GetSensor(isens); //[R.K. 01/2017] unused variable?

      for (Int_t iside = 0; iside < 2; iside++) {

        new ((*fHistoArray)[nofHists++]) TH1F(Form("fhTimeBetweenDigis_s%d_s%d_s%d", istat, isens, iside),
                                              Form("Time between digis, station %d, sensor %d, %s side", istat, isens, (iside ? "back" : "front")), 100000, 0, 100000);
      }
    }
  }

  if (fVerbose) {
    cout << "HM, seems that fHistoArray is filled with histograms, and there are " << nofHists << " of them" << endl;
  }
}
// ------------------------------------------------------------

// -----   Private method AnaHistos   --------------------------------------------
void PndGemDigiAna::AnaHistos()
{

  // Int_t nofStations = fDigiPar->GetNStations(); //[R.K. 01/2017] unused variable?
}
// ------------------------------------------------------------

// -----   Private method Finish   --------------------------------------------
void PndGemDigiAna::Finish()
{
  cout << "-------------------- PndGemDigiAna : Summary ------------------" << endl;
  cout << " Events:        " << setw(10) << fNofEvents << endl;
  cout << "---------------------------------------------------------------------" << endl;

  AnaHistos();

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("GemDigiAna");
    outfile->cd("GemDigiAna");
    TIter next(fHistoList); // this looks strange here
    for (Int_t ihist = 0; ihist < fHistoArray->GetEntries(); ihist++) {
      outfile->WriteTObject(fHistoArray->At(ihist));
    }
    outfile->cd("..");
  }

  //  TFile *temp = gFile;
  //  FairRootManager *ioman = FairRootManager::Instance();
  //  gFile = ioman->GetOutFile();
  //  gDirectory = (TDirectory *)gFile;
  //
  //  gDirectory->mkdir("GemDigiAna");
  //  gDirectory->cd("GemDigiAna");
  //  TIter next(fHistoList);
  //  for (Int_t ihist = 0; ihist < fHistoArray->GetEntries(); ihist++) {
  //    fHistoArray->At(ihist)->Write();
  //  }
  //  gDirectory->cd("..");
  //
  //  gFile = temp;
}
// ------------------------------------------------------------

ClassImp(PndGemDigiAna)
