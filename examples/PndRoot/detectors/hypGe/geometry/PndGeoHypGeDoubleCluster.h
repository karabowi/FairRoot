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

// ******** Header of class to build HypGe Double Cluster Geometry *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGEDOUBLECLUSTER_H
#define PNDGEOHYPGEDOUBLECLUSTER_H

#include "PndGeoHypGeCrystal.h"
#include "PndGeoHypGeCluster.h"
#include "TGeoArb8.h"
#include "TGeoXtru.h"

class PndGeoHypGeDoubleCluster : public PndGeoHypGeCluster {
 private:
  Double_t DoubleCrystalYOffset;
  Double_t DoubleCrystalZOffset;

  Bool_t MakeTop;
  Int_t NumberOfFrontArb8;
  Int_t NumberOfTopArb8;
  Double_t CryostatFrontThickness;
  Double_t CryostatLowerArb8Heigt;
  Double_t CryostatMiddleArb8Heigt;
  Double_t CryostatUpperArb8Heigt;
  Double_t CryostatTopThickness;

  Double_t *CryostatFrontArb8XY_1;
  Double_t *CryostatFrontArb8XY_2;
  Double_t *CryostatFrontArb8XY_3;
  Double_t *CryostatFrontArb8XY_4;

  TGeoArb8 *CryostatFrontArb8_1;
  TGeoArb8 *CryostatFrontArb8_2;
  TGeoArb8 *CryostatFrontArb8_3;
  TGeoArb8 *CryostatFrontArb8_4;

  TGeoTranslation *CryostatFrontTranslation;

  Double_t *CryostatLowerArb8XY_1;
  Double_t *CryostatLowerArb8XY_2;
  Double_t *CryostatLowerArb8XY_3;
  Double_t *CryostatLowerArb8XY_4;
  Double_t *CryostatLowerArb8XY_5;
  Double_t *CryostatLowerArb8XY_6;
  Double_t *CryostatLowerArb8XY_7;
  Double_t *CryostatLowerArb8XY_8;
  Double_t *CryostatLowerArb8XY_9;
  Double_t *CryostatLowerArb8XY_10;

  TGeoArb8 *CryostatLowerArb8_1;
  TGeoArb8 *CryostatLowerArb8_2;
  TGeoArb8 *CryostatLowerArb8_3;
  TGeoArb8 *CryostatLowerArb8_4;
  TGeoArb8 *CryostatLowerArb8_5;
  TGeoArb8 *CryostatLowerArb8_6;
  TGeoArb8 *CryostatLowerArb8_7;
  TGeoArb8 *CryostatLowerArb8_8;
  TGeoArb8 *CryostatLowerArb8_9;
  TGeoArb8 *CryostatLowerArb8_10;

  TGeoTranslation *CryostatLowerTranslation;

  Double_t *CryostatMiddleArb8XY_1;
  Double_t *CryostatMiddleArb8XY_2;
  Double_t *CryostatMiddleArb8XY_3;
  Double_t *CryostatMiddleArb8XY_4;
  Double_t *CryostatMiddleArb8XY_5;
  Double_t *CryostatMiddleArb8XY_6;
  Double_t *CryostatMiddleArb8XY_7;
  Double_t *CryostatMiddleArb8XY_8;
  Double_t *CryostatMiddleArb8XY_9;
  Double_t *CryostatMiddleArb8XY_10;

  TGeoArb8 *CryostatMiddleArb8_1;
  TGeoArb8 *CryostatMiddleArb8_2;
  TGeoArb8 *CryostatMiddleArb8_3;
  TGeoArb8 *CryostatMiddleArb8_4;
  TGeoArb8 *CryostatMiddleArb8_5;
  TGeoArb8 *CryostatMiddleArb8_6;
  TGeoArb8 *CryostatMiddleArb8_7;
  TGeoArb8 *CryostatMiddleArb8_8;
  TGeoArb8 *CryostatMiddleArb8_9;
  TGeoArb8 *CryostatMiddleArb8_10;

  TGeoTranslation *CryostatMiddleTranslation;

  Double_t *CryostatUpperArb8XY_1;
  Double_t *CryostatUpperArb8XY_2;
  Double_t *CryostatUpperArb8XY_3;
  Double_t *CryostatUpperArb8XY_4;
  Double_t *CryostatUpperArb8XY_5;
  Double_t *CryostatUpperArb8XY_6;
  Double_t *CryostatUpperArb8XY_7;
  Double_t *CryostatUpperArb8XY_8;
  Double_t *CryostatUpperArb8XY_9;
  Double_t *CryostatUpperArb8XY_10;

  TGeoArb8 *CryostatUpperArb8_1;
  TGeoArb8 *CryostatUpperArb8_2;
  TGeoArb8 *CryostatUpperArb8_3;
  TGeoArb8 *CryostatUpperArb8_4;
  TGeoArb8 *CryostatUpperArb8_5;
  TGeoArb8 *CryostatUpperArb8_6;
  TGeoArb8 *CryostatUpperArb8_7;
  TGeoArb8 *CryostatUpperArb8_8;
  TGeoArb8 *CryostatUpperArb8_9;
  TGeoArb8 *CryostatUpperArb8_10;

  TGeoTranslation *CryostatUpperTranslation;

  Double_t *CryostatTopArb8XY_1;
  Double_t *CryostatTopArb8XY_2;
  Double_t *CryostatTopArb8XY_3;
  Double_t *CryostatTopArb8XY_4;

  TGeoArb8 *CryostatTopArb8_1;
  TGeoArb8 *CryostatTopArb8_2;
  TGeoArb8 *CryostatTopArb8_3;
  TGeoArb8 *CryostatTopArb8_4;

  TGeoTranslation *CryostatTopTranslation;

 public:
  PndGeoHypGeDoubleCluster(); // do not use!
  PndGeoHypGeDoubleCluster(TGeoMedium *ExtGe, TGeoMedium *ExtAl, Int_t ExtClusterNumber);
  ~PndGeoHypGeDoubleCluster();

  void BuildCrystals(Int_t *CrystalNumber);
  void BuildCryostat();

  // ---------------  Prints the nodes of a cluster -----------------
  void PrintNodes(Int_t nLevels);

  ClassDef(PndGeoHypGeDoubleCluster, 0) // Class for GeoHypGeDoubleCluster
};

#endif /* !PNDGEOHYPGEDOUBLECLUSTER_H */
