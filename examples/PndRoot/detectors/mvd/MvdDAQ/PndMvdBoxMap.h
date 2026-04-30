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

//  Class PndMvdBoxApv
//  To be used when analyzing .root DAQ files

//  Simone Bianco

#ifndef PNDMVDMAPBOX_H
#define PNDMVDMAPBOX_H

#include <iostream>
#include <utility>
#include <string>
#include <vector>
#include <map>
#include "TString.h"
#include "TObject.h" // contains message printing methods

class PndMvdBoxMap : public TObject {
 public:
  // Constructor/destructor
  PndMvdBoxMap() : box(), fCh(), lCh(), fMap(), fFileName(""), nLines(0){};
  PndMvdBoxMap(TString nome) : box(), fCh(), lCh(), fMap(), fFileName(""), nLines(0) { SetMappingFile(nome); };

  ~PndMvdBoxMap(){};

  // Printing
  void Print();

  // Initialization & setup
  void SetMappingFile(TString nome) { fFileName = nome; };
  Bool_t Init();

  // Retrieveing information
  void DoMapping(Int_t nbox, Int_t chan, TString &detpath);

  void PrintMap();

  // Members
 private:
  // the lookup table: ( RW Id | SW Id | Detname )

  /*  Int_t *box;
    Int_t *fCh;
    Int_t *lCh;*/
  // TClonesArray *nameArr;
  // std::vector <TString> name;

  std::map<Int_t, Int_t> box; //!
  std::map<Int_t, Int_t> fCh; //!
  std::map<Int_t, Int_t> lCh; //!

  std::map<Int_t, TString> fMap; //!

  TString fFileName; //!
  Int_t nLines;      //!

  ClassDef(PndMvdBoxMap, 1);
};

#endif
