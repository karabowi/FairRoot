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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// PndPidProbability	                                                  //
//                                                                      //
// Definition of the Panda pid probabilities .	                        //
//                                                                      //
// Author: Ralf Kliemt, Dresden/Turin/Bonn, 01.09.09                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "PndPidProbability.h"

PndPidProbability::PndPidProbability() : fElectronPdf(0.2), fMuonPdf(0.2), fPionPdf(0.2), fKaonPdf(0.2), fProtonPdf(0.2), fIndex(-1) {}

PndPidProbability::PndPidProbability(Double_t e, Double_t mu, Double_t pi, Double_t k, Double_t p, Int_t idx)
  : fElectronPdf(e), fMuonPdf(mu), fPionPdf(pi), fKaonPdf(k), fProtonPdf(p), fIndex(idx)
{
}

PndPidProbability::~PndPidProbability() {}

PndPidProbability PndPidProbability::operator*(const PndPidProbability &a)
{
  return PndPidProbability(a.GetElectronPdf() * fElectronPdf, a.GetMuonPdf() * fMuonPdf, a.GetPionPdf() * fPionPdf, a.GetKaonPdf() * fKaonPdf, a.GetProtonPdf() * fProtonPdf,
                           a.GetIndex());
}
PndPidProbability &PndPidProbability::operator*=(const PndPidProbability &a)
{
  fElectronPdf *= a.GetElectronPdf();
  fMuonPdf *= a.GetMuonPdf();
  fPionPdf *= a.GetPionPdf();
  fKaonPdf *= a.GetKaonPdf();
  fProtonPdf *= a.GetProtonPdf();
  return *this;
}

void PndPidProbability::Print()
{
  std::cout << "PndPidProbability pdf's:   "
            << "p(e)=" << fElectronPdf << "  "
            << "p(mu)=" << fMuonPdf << "  "
            << "p(pi)=" << fPionPdf << "  "
            << "p(K)=" << fKaonPdf << "  "
            << "p(P)=" << fProtonPdf << "  "
            << "index=" << fIndex << std::endl;
}

void PndPidProbability::Reset()
{
  fElectronPdf = 0.2;
  fMuonPdf = 0.2;
  fPionPdf = 0.2;
  fKaonPdf = 0.2;
  fProtonPdf = 0.2;
  fIndex = -1;
}

void PndPidProbability::NormalizeTo(Double_t N)
{
  Double_t scalefactor = GetSumProb();
  if (scalefactor == 0)
    return;
  scalefactor = N / scalefactor;
  fElectronPdf *= scalefactor;
  fMuonPdf *= scalefactor;
  fPionPdf *= scalefactor;
  fKaonPdf *= scalefactor;
  fProtonPdf *= scalefactor;
  return;
}

double PndPidProbability::GetPidProb(int pdgCode)
{
  int absPdg = abs(pdgCode);
  switch (absPdg) {
  case 11: return GetElectronPidProb(); break;
  case 13: return GetMuonPidProb(); break;
  case 211: return GetPionPidProb(); break;
  case 321: return GetKaonPidProb(); break;
  case 2212: return GetProtonPidProb(); break;
  default: return -1;
  }
}

double PndPidProbability::GetPdf(int pdgCode)
{
  int absPdg = abs(pdgCode);
  switch (absPdg) {
  case 11: return GetElectronPdf(); break;
  case 13: return GetMuonPdf(); break;
  case 211: return GetPionPdf(); break;
  case 321: return GetKaonPdf(); break;
  case 2212: return GetProtonPdf(); break;
  default: return -1;
  }
}

ClassImp(PndPidProbability)
