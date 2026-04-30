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

#include "PndMvdEventAna.h"
#include "PndSdsMCPoint.h"
#include "TVector3.h"
#include "TEventList.h"

ClassImp(PndMvdEventAna)

  PndMvdEventAna::PndMvdEventAna(TString fileName)
{
  fActiveEvent = 0;
  fAnaEvents = 0;
  fCan = 0;
  fCancol = 2;
}

void PndMvdEventAna::Init(TString fileName)
{
  InitHistos();
  InitBranch();
  fFile = new TFile(fileName.Data());
  fTree = (TTree *)(fFile->Get("pndsim"));
  fHitArray = new TClonesArray(fClassName.Data());
  fTree->SetBranchAddress(fBranchName.Data(), &fHitArray); // Branch names
  fActiveEvent = 0;
}

void PndMvdEventAna::AnaEventNr(Int_t Nr)
{
  TVector3 vecs;
  BeginOfEventAction();

  fAnaEvents = 1;
  InitHistos();
  ClearHistos();
  if (Nr > fTree->GetEntriesFast())
    return;

  fActiveEvent = Nr;
  AnaHits();
  EndOfEventAction();
  // InitCanvas();
  // DrawHistos();
}

void PndMvdEventAna::NextEvent()
{
  fAnaEvents = 1;
  if (fActiveEvent + 1 < fTree->GetEntriesFast()) {
    fActiveEvent++;
    AnaEventNr(fActiveEvent);
  }
}

void PndMvdEventAna::InitCanvas()
{
  if (fCan != 0)
    delete (fCan);
  fCan = new TCanvas("can1", "MCHit view in MVD", 0, 0, 1000, 1000);
  std::cout << "InitCanvas fHistos.size() " << fHistos.size() << std::endl;
  fCan->Divide(fCancol, (Int_t)(((Double_t)(fHistos.size()) / fCancol) + 0.5));
}

void PndMvdEventAna::DrawHistos()
{
  Int_t i = 0;
  for (std::map<TString, TH1 *>::const_iterator ki = fHistos.begin(); ki != fHistos.end(); ki++) {
    i++;
    fCan->cd(i);
    std::cout << ki->second << std::endl;
    ki->second->Draw(); //(fDrawOption[ci->second->GetName()]);
  }
}

void PndMvdEventAna::DrawHisto(TString path, TString opt)
{
  fHistos[path]->Draw(opt.Data());
}

void PndMvdEventAna::AnaEvents(Int_t count)
{
  fAnaEvents = count;
  InitHistos();
  if (count > fTree->GetEntriesFast())
    count = fTree->GetEntriesFast();
  for (Int_t i = 0; i < count; i++) {
    fActiveEvent = i;
    AnaHits();
  }
  // InitCanvas();
  // DrawHistos();
}

void PndMvdEventAna::AnaHits()
{
  for (Int_t i = 0; i < fHitArray->GetEntriesFast(); i++) {
    AnaHitNr(i);
  }
}

void PndMvdEventAna::FillVector()
{
  fHistoVector.clear();
  for (std::map<TString, TH1 *>::const_iterator ki = fHistos.begin(); ki != fHistos.end(); ki++) {
    fHistoVector.push_back(ki->second);
  }
}

void PndMvdEventAna::DrawVector(Int_t index, TString opt)
{
  fHistoVector[index]->Draw(opt.Data());
}

void PndMvdEventAna::SetDrawOption(TString histoName, TString option)
{
  fDrawOption[histoName] = option;
}
