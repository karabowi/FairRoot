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

#ifndef PndTrkMergeSort_H
#define PndTrkMergeSort_H 1

// Root includes
#include "TROOT.h"

class PndTrkMergeSort : public TObject {

 public:
  /** Default constructor **/
  PndTrkMergeSort(){};
  /** Destructor **/
  ~PndTrkMergeSort(){};

  void Merge(Short_t nl, Double_t *left, Int_t *ind_left, Short_t nr, Double_t *right, Int_t *ind_right, Double_t *result, Int_t *ind);

  // the only difference with Merge is that in Merge2 only Short_t
  // are used (NO Int_t);
  void Merge2(Short_t nl, Double_t *left, Short_t *ind_left, Short_t nr, Double_t *right, Short_t *ind_right, Double_t *result, Short_t *ind);

  void Merge3(Short_t nl, Int_t *left, Short_t *ind_left, Short_t nr, Int_t *right, Short_t *ind_right, Int_t *result, Short_t *ind);

  void Merge_Sort(Short_t n_ele, Double_t *array, Int_t *ind);

  // the only difference with Merge_Sort is that   *ind is a Sort_t
  // (NO Int_t);
  void Merge_Sort2(Short_t n_ele, Double_t *array, Short_t *ind);

  void Merge_Sort3(Short_t n_ele,
                   Int_t *array, // the array to be ordered;
                   Short_t *ind);

  ClassDef(PndTrkMergeSort, 1);
};

#endif
