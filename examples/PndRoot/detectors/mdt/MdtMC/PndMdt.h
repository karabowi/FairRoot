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


#ifndef PNDMDT_H
#define PNDMDT_H 1

#include "TROOT.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TString.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"

#include "FairRootManager.h"
#include "FairDetector.h"
#include "FairVolume.h"

#include "PndGeoMdtPar.h"
#include "PndMdtPoint.h"

class PndMdt : public FairDetector {
 public:
  PndMdt();
  PndMdt(const char *name, Bool_t active);
  ~PndMdt();

  inline void SetMdtMagnet(bool opt = false)
  {
    mdtMagnet = opt;
    return;
  };
  inline void SetMdtCoil(bool opt = false)
  {
    mdtCoil = opt;
    return;
  };
  inline void SetMdtMFIron(bool opt = false)
  {
    mdtMFI = opt;
    return;
  };

  void SetBarrel(TString name) { fBarrel = name; };
  void SetEndcap(TString name) { fEndcap = name; };
  void SetMuonFilter(TString name) { fMuonFilter = name; };
  void SetForward(TString name) { fForward = name; };
  // hit
    inline void Register();

  void ResetParameters();
  void Print() const;
  void Reset();

  /** Accessors **/
  TClonesArray *GetCollection(Int_t iColl) const;

  // detector
  inline void SetVerbosity(Bool_t verbosity) { fVerboseLevel = verbosity; };

  void ConstructGeometry();
  void Initialize();
  void BeginEvent();
  Bool_t ProcessHits(FairVolume *vol);
  Bool_t ProcessHitsFast(FairVolume *vol);
  Bool_t ProcessHitsRoot(FairVolume *vol);
  void EndOfEvent();

        FairModule* CloneModule() const override;
    Bool_t IsSensitive(const std::string& name) override;

 private:
  Bool_t CheckIfSensitive(std::string name);
  Int_t PdgToIndex(Int_t pdg);

  void PndMdtMagnet();          //!
  void PndMdtMuonFilter();      //!
  void PndMdtMFIron();          //!
  void PndMdtForward();         //!
  void PndMdtCoil();            //!
  void ConstructGeometryTo();   //!
  void ConstructGeometryFast(); //!
  void ConstructGeometryRoot(); //!
  void ConstructGeometryDu();   //!

  // hit
    std::vector<PndMdtPoint>* fPointVector{new std::vector<PndMdtPoint>};
  Int_t fTrkIn;
  Float_t fELoss;
  Float_t fTime;
  TLorentzVector fPos_In;
  TLorentzVector fMom_In;

  // detector
  Bool_t fVerboseLevel;

  bool mdtMagnet; //!
  bool mdtCoil;   //!
  bool mdtMFI;    //!

  TString fBarrel;
  TString fEndcap;
  TString fMuonFilter;
  TString fForward;

        PndMdt(const PndMdt&);
    PndMdt& operator=(const PndMdt&);

  ClassDef(PndMdt, 1)
};

#endif /* !PNDMDT_H */
