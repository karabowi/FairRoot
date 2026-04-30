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

#include "PndMvdMCEventAna.h"
#include "PndSdsMCPoint.h"
#include "TVector3.h"
#include "TH3.h"

ClassImp(PndMvdMCEventAna)

  PndMvdMCEventAna::PndMvdMCEventAna(TString fileName)
  : PndMvdEventAna(fileName)
{
  Init(fileName);
  fMCTrackArray = new TClonesArray("PndMCTrack");
  fTree->SetBranchAddress("MCTrack", &fMCTrackArray);
  fGeoH = PndGeoHandling::Instance();
}

void PndMvdMCEventAna::InitBranch()
{
  fClassName = "PndSdsMCPoint";
  fBranchName = "MVDPoint";
}

void PndMvdMCEventAna::InitHistos()
{
  if (fHistos["hisxy"] != 0)
    delete (fHistos["hisxy"]);
  fHistos["hisxy"] = new TH2D("hisxy", "MVD MC Points, xy view", 400, -15., 15., 400, -15., 15.);
  fDrawOption["hisxy"] = "";
  if (fHistos["hisrz"] != 0)
    delete (fHistos["hisrz"]);
  fHistos["hisrz"] = new TH2D("hisrz", "MVD MC Points, rz view", 400, -20., 20., 400, 0., 40.);
  fDrawOption["hisrz"] = "";
  //  if (fHistos["hisxyz"]!= 0) delete (fHistos["hisxyz"]);
  //  fHistos["hisxyz"] = new TH3D("hisxyz","MVD MC Points xyz view",200,-15.,15.,200,-15.,15.,200,-20.,20.);
  //  fDrawOption["hisxyz"] = "";
  if (fHistos["hisde"] != 0)
    delete (fHistos["hisde"]);
  fHistos["hisde"] = new TH1D("hisde", "MVD MC Points, Energyloss", 500, 0., 0.002);
  fDrawOption["hisde"] = "B";
}

void PndMvdMCEventAna::AnaHitNr(Int_t Nr)
{
  TVector3 vecs;

  fHitArray->Delete();
  fTree->GetEntry(fActiveEvent);
  PndSdsMCPoint *hit = (PndSdsMCPoint *)fHitArray->At(Nr);
  if (hit->GetDetName().Contains(fSelection)) {
    vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
    fHistos["hisxy"]->Fill(vecs.x(), vecs.y());
    fHistos["hisrz"]->Fill(vecs.z(), vecs.Perp());
    fHistos["hisde"]->Fill(hit->GetEnergyLoss());
    // TH3D* histo3D = (TH3D*)(fHistos["hisxyz"]);
    // histo3D->Fill(hit->GetX(), hit->GetY(), hit->GetZ());
  }
}
