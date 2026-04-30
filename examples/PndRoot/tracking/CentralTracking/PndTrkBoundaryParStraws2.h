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

#ifndef PndTrkBoundaryParStraws2_H
#define PndTrkBoundaryParStraws2_H 1

// Root includes
#include "TROOT.h"

class PndTrkBoundaryParStraws2 : public TObject {

 public:
  /** Default constructor **/
  PndTrkBoundaryParStraws2(){};
  /** Destructor **/
  ~PndTrkBoundaryParStraws2(){};

  void CalculateSpecialRegion(Double_t RSTRAWDETECTORMIN, Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t VERTICALGAP, Double_t &x, Double_t &y);

  void Set(
    // inputs :
    Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Short_t NUMBER_STRAWS, Double_t RSTRAWDETECTORMIN, Double_t RSTRAWDETECTORMAX, bool stampa,
    TClonesArray *SttTubeArray, Double_t STRAWRADIUS, Double_t VERTICALGAP,
    // outputs :
    Short_t *StrawCode, // -1 = not a boundary straw;  >0 = boundary straw;
    Short_t *StrawCode2 // second Code; -1 = not a boundary straw;  >0 =  boundary straw;
  );

  void SttTubeList(
    // inputs :
    TClonesArray *SttTubeArray, Double_t RSTRAWDETECTORMIN, Double_t RSTRAWDETECTORMAX, Double_t APOTEMAMAXINNERPARSTRAW, Double_t APOTEMAMINOUTERPARSTRAW, Double_t VERTICALGAP,
    Double_t STRAWRADIUS, Short_t NUMBER_STRAWS, bool stampa,
    // outputs :
    Short_t *StrawCode, Short_t *StrawCode2);

  ClassDef(PndTrkBoundaryParStraws2, 1);
};

#endif
