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

#include "PndMvdDigiAna.h"

#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "PndSdsMCPoint.h"

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairGeoNode.h"
#include "FairGeoVector.h"
#include "PndStringSeparator.h"
#include "FairRootManager.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TArrayD.h"
#include "TGeoManager.h"
#include "TCanvas.h"

using std::string;

PndMvdDigiAna::PndMvdDigiAna() : FairTask("PndMvd Digi Ana")
{
  fBranchName = "MVDPixelDigis";
  //	fBranchName 	= "MVDDigiStrip";
  fPixelPos = new TH3F("pPos", "pPos", 200, -15, 15, 200, -15, 15, 200, -20, 20);
  fStripPos = new TH3F("sPos", "sPos", 128, 0, 127, 16, 0, 15, 200, 0, 20);

  //	fHitArray	= new TClonesArray("PndSdsDigiPixel");
  //	fPixelArray	= new TClonesArray("PndSdsDigiPixel");
}
// -------------------------------------------------------------------------

PndMvdDigiAna::PndMvdDigiAna(TString DetName) : FairTask("MVD Digi Ana")
{
  fBranchName = "MVDPixelDigis";
  //	fBranchName 	= "MVDDigiStrip";
  fDetName = DetName;
  fPixelPos = new TH3F("pPos", "pPos", 200, -15, 15, 200, -15, 15, 200, -20, 20);
  fStripPos = new TH3F("sPos", "sPos", 128, 0, 127, 16, 0, 15, 200, 0, 20);
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMvdDigiAna::~PndMvdDigiAna() {}
// -------------------------------------------------------------------------

// -----   Initialization  of Parameter Containers -------------------------
void PndMvdDigiAna::SetParContainers()
{
  // Get Base Container
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  fDigiPar = (PndSdsPixelDigiPar *)(rtdb->getContainer("MVDPixelDigiPar"));
}

InitStatus PndMvdDigiAna::ReInit()
{
  SetParContainers();
  return kSUCCESS;
}

// -----   Public method Init   --------------------------------------------
InitStatus PndMvdDigiAna::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    std::cout << "-E- PndMvdDigiAna::Init: "
              << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  std::cout << "within init" << std::endl;

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject(fBranchName);

  if (!fHitArray) {
    std::cout << "-W- PndMvdDigiAna::Init: "
              << "No MVDHit array!" << std::endl;
    return kERROR;
  }

  // register output histograms
  ioman->Register("MVDHist1", "MVD", fStripPos, kTRUE);
  ioman->Register("MVDHist2", "MVD", fPixelPos, kTRUE);

  LOG(info) << " PndMvdDigiAna: Intialisation successfull";
  return kSUCCESS;
}
// -------------------------------------------------------------------------

void PndMvdDigiAna::Exec(Option_t *)
{
  // Reset output array
  std::cout << "Within Exec! " << fHitArray << " " << fPixelArray << std::endl;
  if (!fHitArray)
    Fatal("Exec", "No HitArray");

  // Loop over PndMvdMCPoints
  Int_t nPoints = fHitArray->GetEntriesFast();

  PndSdsDigiStrip *hit = 0;

  for (Int_t iPoint = 0; iPoint < nPoints; iPoint++) {
    //      hit = (PndSdsDigiPixel*) fHitArray->At(iPoint);
    hit = (PndSdsDigiStrip *)fHitArray->At(iPoint);

    if (!hit) {
      std::cout << "No Hit!" << std::endl;
      continue;
    }

    // std::cout << "****Hit Point: " << std::endl;
    // hit->Print("");

    std::string det = hit->GetDetName().Data();

    PndStringSeparator sAna(det, "/");
    std::vector<std::string> sVector = sAna.GetStringVector();
    if (sVector[sVector.size() - 1].find("Strip") != string::npos) {
      fStripPos->Fill(hit->GetChannel(), hit->GetFE(), hit->GetCharge());
      std::cout << "Strip Found" << std::endl;
      //	  	std::cout<<"col "<<hit->GetPixelColumn()<<"\trow "
      //               <<hit->GetPixelRow()<<" charge "<<hit->GetCharge()<<std::endl;
    } else {
      /*
      fPixelPos->Fill(hit->GetPixelColumn(), hit->GetPixelRow(), hit->GetCharge());
      std::cout << "Pixel Found" << std::endl;
      std::cout<<"col "<<hit->GetPixelColumn()<<"\trow "
               <<hit->GetPixelRow()<<" charge "<<hit->GetCharge()<<std::endl;
      */
    }
  } // Loop over HitPoints

  // Event summary
  LOG(info) << " PndMvdDigiAna: " << nPoints << " PndMvdHits";
  PrintHistograms("PndMvdDigiAna.ps");
}

void PndMvdDigiAna::WriteHistograms(const TString &filename)
{
  TFile *file = new TFile(filename, "UPDATE");
  file->mkdir("PndMvdDigiAna");
  file->cd("PndMvdDigiAna");

  fStripPos->Write();
  delete fStripPos;
  fStripPos = nullptr;

  fPixelPos->Write();
  delete fPixelPos;
  fPixelPos = nullptr;

  file->Close();
  delete file;
}

void PndMvdDigiAna::PrintHistograms(const TString &outpsfile)
{
  TCanvas *can1 = new TCanvas("can1", "MVD digitization analysis", 0, 0, 600, 800);
  Int_t a = 2, b = 4, zaehl = 1;
  can1->Divide(a, b);
  can1->Print(outpsfile + "["); // opens the ps file, no writing

  if (zaehl > (a * b)) // new page
  {
    zaehl = 1;
    can1->Print(outpsfile);
    can1->Clear("D");
  }
  can1->cd(zaehl);
  fStripPos->DrawCopy();
  delete fStripPos;
  fStripPos = nullptr;

  if (zaehl > (a * b)) // new page
  {
    zaehl = 1;
    can1->Print(outpsfile);
    can1->Clear("D");
  }
  can1->cd(zaehl);
  fPixelPos->DrawCopy();
  delete fPixelPos;
  fPixelPos = nullptr;

  // finish plotting
  can1->Print(outpsfile);       // writes in file
  can1->Print(outpsfile + "]"); // closes the file
  delete can1;
}

ClassImp(PndMvdDigiAna)
