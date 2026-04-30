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

#ifndef PndTrkSttConformalFilling_H
#define PndTrkSttConformalFilling_H 1

#include "PndTrkVectors.h"
// Root includes
#include "TROOT.h"

class PndTrkSttConformalFilling : public TObject {

 public:
  /** Default constructor **/
  PndTrkSttConformalFilling(){};

  /** Destructor **/
  ~PndTrkSttConformalFilling(){};

  void BoxConformalFilling(Vec<Short_t> &FiConformalIndex, Vec<Short_t> &HitsinBoxConformal, bool *InclusionListStt, Double_t infoparalConformal[][5], Short_t *ListSttParHits,
                           Short_t MAXHITSINCELL, Vec<Short_t> &nBoxConformal, Short_t NFIDIVCONFORMAL, Int_t Nparal, Short_t NRDIVCONFORMAL, Double_t *radiaConf,
                           Vec<Short_t> &RConformalIndex);

  void FromXYtoConformal(Double_t trajectory_vertex[2], Double_t info[][7], Short_t *ListSttParHits, Int_t Nparal, Double_t infoparalConformal[][5], Double_t STRAWRADIUS);

  ClassDef(PndTrkSttConformalFilling, 1);
};

#endif
