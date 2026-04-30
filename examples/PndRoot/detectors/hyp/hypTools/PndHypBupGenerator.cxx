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
// -----            PndHypBupGenerator source file                 -----
// -----                Created by A. Sanchez              -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TClonesArray.h"

#include "TFile.h"
#include "TF1.h"
#include "TRandom.h"
#include "TLorentzVector.h"
#include "TTree.h"
#include "TVector3.h"
#include "THParticle.h"
#include "PndHypBupGenerator.h"
#include "FairPrimaryGenerator.h"
#include "FairLogger.h"

using namespace std;

// -----   Default constructor   ------------------------------------------
PndHypBupGenerator::PndHypBupGenerator() : fPMin(0), fPMax(0), fThetaMin(0), fThetaMax(0), fPhiMin(0), fPhiMax(0), fThetaRangeIsSet(0), fPRangeIsSet(0), fGamOn(0)
{
  iEvent = 0;
  fInputFile = nullptr;
  fInputTree = nullptr;
}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
PndHypBupGenerator::PndHypBupGenerator(const char *fileName)
  : fPMin(0), fPMax(0), fThetaMin(0), fThetaMax(0), fPhiMin(0), fPhiMax(0), fThetaRangeIsSet(0), fPRangeIsSet(0), fGamOn(0)
{

  fFileName = fileName;
  iEvent = 0;
  fInputFile = new TFile(fFileName);
  fInputTree = (TTree *)fInputFile->Get("data");
  fParticles = new TClonesArray("THParticle", 100);
  fInputTree->SetBranchAddress("Particles", &fParticles);
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
PndHypBupGenerator::~PndHypBupGenerator()
{
  CloseInput();
}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
Bool_t PndHypBupGenerator::ReadEvent(FairPrimaryGenerator *primGen)
{

  // Check for input file
  if (!fInputFile) {
    LOG(error) << " PndHypBupGenerator: Input file not open!";
    return kFALSE;
  }
  // Check for number of events in input file
  if (iEvent > fInputTree->GetEntries()) {
    cout << "-E PndDpmGenerator: No more events in input file!" << endl;
    CloseInput();
    return kFALSE;
  }

  // Defining kinematics variables, angle is expressed in degrees.
  Double_t phi, p = 0.0, theta = 0.0;

  TFile *g = gFile;
  fInputFile->cd();
  fInputTree->GetEntry(iEvent++);
  g->cd();

  // Get number of particle in TClonesrray
  Int_t nParts = fParticles->GetEntriesFast();

  // Loop over particles in TClonesArray

  for (Int_t iPart = 0; iPart < nParts; iPart++) {
    THParticle *part = (THParticle *)fParticles->At(iPart);
    Int_t pdgType = part->GetPdgCode();

    // Check if particle type is known to database
    if (!pdgType) {
      cout << "-W PndDpmGenerator: Unknown type " << part->GetPdgCode() << ", skipping particle." << endl;
      continue;
    }

    Double_t px = 0.;
    Double_t py = 0.;
    Double_t pz = 0.;

    Double_t vx;
    Double_t vy;
    Double_t vz;

    if (fGamOn) {

      phi = gRandom->Uniform(0, 360) * TMath::DegToRad();
      p = gRandom->Uniform(fPMin, fPMax);
      theta = acos(gRandom->Uniform(cos(fThetaMin * TMath::DegToRad()), cos(fThetaMax * TMath::DegToRad())));

      pz = p * TMath::Cos(theta);
      py = p * TMath::Sin(theta) * TMath::Sin(phi);
      px = p * TMath::Sin(theta) * TMath::Cos(phi);

      vx = part->Vx();
      vy = part->Vy();
      vz = part->Vz();

      cout << " Secondary vertex for stopping Xi - gammas " << vx << " " << vy << " " << vz << endl;

      // Give track to PrimaryGenerator

      primGen->AddTrack(22, px, py, pz, vx, vy, vz);

    } else {
      px = part->Px();
      py = part->Py();
      pz = part->Pz();

      vx = part->Vx();
      vy = part->Vy();
      vz = part->Vz();

      cout << " Secondary vertex for stopping Xi - " << pdgType << " " << vx << " " << vy << " " << vz << endl;

      // Give track to PrimaryGenerator

      primGen->AddTrack(pdgType, px, py, pz, vx, vy, vz);
    }
  }

  return kTRUE;
}
// ------------------------------------------------------------------------

// -----   Private method CloseInput   ------------------------------------
void PndHypBupGenerator::CloseInput()
{

  if (fInputFile) {
    LOG(info) << " CbmHypBupGenerator: Closing input file " << fFileName;
    fInputFile->Close();

    delete fInputFile;
  }

  fInputFile = nullptr;
}

// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
ClassImp(PndHypBupGenerator)
