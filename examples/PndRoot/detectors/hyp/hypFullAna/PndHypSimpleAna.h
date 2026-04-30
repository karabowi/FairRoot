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

#ifndef PNDHYPSIMPLEANA_H
#define PNDHYPSIMPLEANA_H 1

#include "FairTask.h"
#include <map>
#include <string>
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;

class RhoChargedParticleSelector;
class RhoNeutralParticleSelector;
class RhoPlusParticleSelector;
class RhoMinusParticleSelector;
class RhoMassParticleSelector;
class RhoSimpleKaonSelector;
class RhoSimplePionSelector;
class RhoSimpleProtonSelector;

class PndHypSimpleAna : public FairTask {

 public:
  typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/
  PndHypSimpleAna();

  /** Destructor **/
  ~PndHypSimpleAna();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void Finish(TString cat);

  /*void SetEnergySpectra(int event,int cluster);
    void SetTotESpectra(int clus);*/
  Int_t GetIonCharge(Int_t Z, Int_t &mass, Int_t &str);

  // void CreateStructure();

 protected:
  int evcount;

  // **** some basic selectors
  //
  // RhoChargedParticleSelector *chargedSel;
  RhoNeutralParticleSelector *neutralSel;
  RhoPlusParticleSelector *plusSel;
  RhoMinusParticleSelector *minusSel;

  // **** mass selectors for the resonances/composites
  //
  RhoMassParticleSelector *phiMSel;
  RhoMassParticleSelector *pi0MSel;
  RhoMassParticleSelector *dsMSel;
  RhoMassParticleSelector *LambMSel;
  RhoSimpleKaonSelector *kSel;
  RhoSimplePionSelector *piSel;
  RhoSimpleProtonSelector *pSel;

  /**book all the histograms**/
  TH2F *hvtx2[10];
  // TH1F *spectra[10];

  TH1F *ds0mass;
  TH1F *ximass;
  TH1F *Lamb;
  TH1F *ppi2mass;
  TH1F *ppi2;
  TH1F *e;
  TH2F *pid;
  TH2F *pidh;

  TH1F *nmult;

 private:
  /** Input array of TpcLheTrack **/

  TClonesArray *fChargedArray;
  TClonesArray *fMcTr;
  TClonesArray *fMicroArray;
  TClonesArray *fMcCands;
  TClonesArray *fMc;
  // TClonesArray* fGe;

  /** Geo file to use **/

  /** Get parameter containers **/
  virtual void SetParContainers();

  ClassDef(PndHypSimpleAna, 1);
};

#endif
