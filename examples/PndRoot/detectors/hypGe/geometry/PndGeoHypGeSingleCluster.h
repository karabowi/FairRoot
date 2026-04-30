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

// ******** Header of class to build HypGe Single Cluster Geometry *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGESINGLECLUSTER_H
#define PNDGEOHYPGESINGLECLUSTER_H

#include "PndGeoHypGeCrystal.h"
#include "PndGeoHypGeCluster.h"
#include "TGeoArb8.h"
#include "TGeoXtru.h"

class PndGeoHypGeSingleCluster : public PndGeoHypGeCluster {
 private:
  Double_t SingleCrystalZOffset;

  Bool_t MakeTop;
  Int_t NumberOfFrontArb8;
  Int_t NumberOfTopArb8;
  Double_t CryostatFrontThickness;
  Double_t CryostatSideHeight;

  Double_t CryostatBack1Height;
  Double_t CryostatBack2Height;
  Double_t ColdFinger1Height;
  Double_t ColdFinger2Height;

  TGeoTube *CryostatFront;
  TGeoTube *CryostatSide;
  TGeoTube *CryostatBack1;
  TGeoTube *CryostatBack2;
  TGeoTube *ColdFinger1;
  TGeoTube *ColdFinger2;

  TGeoTranslation *CryostatFrontTranslation;
  TGeoTranslation *CryostatSideTranslation;
  TGeoTranslation *CryostatBack1Translation;
  TGeoTranslation *CryostatBack2Translation;
  TGeoTranslation *ColdFinger1Translation;
  TGeoTranslation *ColdFinger2Translation;

 public:
  PndGeoHypGeSingleCluster(); // do not use!
  PndGeoHypGeSingleCluster(TGeoMedium *ExtGe, TGeoMedium *ExtAl, TGeoMedium *ExtCu,
                           Int_t ExtClusterNumber); // the extCu variable must be put into the inherited function later (30.07.14)
  ~PndGeoHypGeSingleCluster();

  void BuildCrystals(Int_t *CrystalNumber);
  void BuildCryostat();

  // ---------------  Prints the nodes of a cluster -----------------
  void PrintNodes(Int_t nLevels);

  ClassDef(PndGeoHypGeSingleCluster, 0) // Class for GeoHypGeSingleCluster
};

#endif /* !PNDGEOHYPGESINGLECLUSTER_H */
