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

// ******** Header of base class to support the building of HypGe cluster geometries *************
//					by steinen@kph.uni-mainz.de

#ifndef PNDGEOHYPGECLUSTER_H
#define PNDGEOHYPGECLUSTER_H

#include "PndGeoHypGeCrystal.h"
#include "PndGeoHypGeBase.h"

class PndGeoHypGeCluster : public PndGeoHypGeBase {
 protected:
  // Media
  TGeoMedium *Ge;
  TGeoMedium *Al;
  TGeoMedium *Cu;

  Int_t ClusterNumber;
  Int_t StartClusterNumber;
  TGeoVolume *ClusterAssembly;

  Int_t NumberOfCryostatEdges;
  Int_t NumberOfDifferentZValues;
  Int_t NumberOfXYInput;

  PndGeoHypGeCrystal *Crystal1;
  PndGeoHypGeCrystal *Crystal2;
  PndGeoHypGeCrystal *Crystal3;
  Int_t Crystal1Number;
  Int_t Crystal2Number;
  Int_t Crystal3Number;

  TGeoRotation *Crystal1Rotation;
  TGeoRotation *Crystal2Rotation;
  TGeoRotation *Crystal3Rotation;

  TGeoCombiTrans *Crystal1Position;
  TGeoCombiTrans *Crystal2Position;
  TGeoCombiTrans *Crystal3Position;

  // objects/variables to build the cryostat
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
  Double_t *CryostatFrontArb8XY_5;

  TGeoArb8 *CryostatFrontArb8_1;
  TGeoArb8 *CryostatFrontArb8_2;
  TGeoArb8 *CryostatFrontArb8_3;
  TGeoArb8 *CryostatFrontArb8_4;
  TGeoArb8 *CryostatFrontArb8_5;

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
  Double_t *CryostatLowerArb8XY_11;
  Double_t *CryostatLowerArb8XY_12;

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
  TGeoArb8 *CryostatLowerArb8_11;
  TGeoArb8 *CryostatLowerArb8_12;

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
  Double_t *CryostatMiddleArb8XY_11;
  Double_t *CryostatMiddleArb8XY_12;

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
  TGeoArb8 *CryostatMiddleArb8_11;
  TGeoArb8 *CryostatMiddleArb8_12;

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
  Double_t *CryostatUpperArb8XY_11;
  Double_t *CryostatUpperArb8XY_12;

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
  TGeoArb8 *CryostatUpperArb8_11;
  TGeoArb8 *CryostatUpperArb8_12;

  TGeoTranslation *CryostatUpperTranslation;

  Double_t *CryostatTopArb8XY_1;
  Double_t *CryostatTopArb8XY_2;
  Double_t *CryostatTopArb8XY_3;
  Double_t *CryostatTopArb8XY_4;
  Double_t *CryostatTopArb8XY_5;

  TGeoArb8 *CryostatTopArb8_1;
  TGeoArb8 *CryostatTopArb8_2;
  TGeoArb8 *CryostatTopArb8_3;
  TGeoArb8 *CryostatTopArb8_4;
  TGeoArb8 *CryostatTopArb8_5;

  TGeoTranslation *CryostatTopTranslation;

  // old!!!!!
  /*Double_t						CryostatHeight;
  Double_t						CryostatFrontThickness;
  Double_t						CryostatUpperPartHeight;
  // lots of storage for coordinates
  Double_t						*CryostatOuterArb8XY_1;
  Double_t						*CryostatOuterArb8XY_2;
  Double_t						*CryostatOuterArb8XY_3;
  Double_t						*CryostatOuterArb8XY_4;
  Double_t						*CryostatOuterArb8XY_5;
  Double_t						*CryostatOuterArb8XY_6;
  Double_t						*CryostatOuterArb8XY_7;
  Double_t						*CryostatOuterArb8XY_8;
  Double_t						*CryostatOuterArb8XY_9;
  Double_t						*CryostatOuterArb8XY_10;
  Double_t						*CryostatOuterArb8XY_11;
  Double_t						*CryostatOuterArb8XY_12;

  Double_t						*CryostatInnerArb8XY_1;
  Double_t						*CryostatInnerArb8XY_2;
  Double_t						*CryostatInnerArb8XY_3;
  Double_t						*CryostatInnerArb8XY_4;
  Double_t						*CryostatInnerArb8XY_5;

    //upper parts

  Double_t						*CryostatUpperArb8XY_1;
  Double_t						*CryostatUpperArb8XY_2;
  Double_t						*CryostatUpperArb8XY_3;
  Double_t						*CryostatUpperArb8XY_4;
  Double_t						*CryostatUpperArb8XY_5;
  Double_t						*CryostatUpperArb8XY_6;
  Double_t						*CryostatUpperArb8XY_7;
  Double_t						*CryostatUpperArb8XY_8;
  Double_t						*CryostatUpperArb8XY_9;
  Double_t						*CryostatUpperArb8XY_10;
  Double_t						*CryostatUpperArb8XY_11;
  Double_t						*CryostatUpperArb8XY_12;

    //top

  //the single parts of the cryostat
  TGeoArb8						*CryostatOuterArb8_1;
  TGeoArb8						*CryostatOuterArb8_2;
  TGeoArb8						*CryostatOuterArb8_3;
  TGeoArb8						*CryostatOuterArb8_4;
  TGeoArb8						*CryostatOuterArb8_5;
  TGeoArb8						*CryostatOuterArb8_6;
  TGeoArb8						*CryostatOuterArb8_7;
  TGeoArb8						*CryostatOuterArb8_8;
  TGeoArb8						*CryostatOuterArb8_9;
  TGeoArb8						*CryostatOuterArb8_10;

  TGeoArb8						*CryostatInnerArb8_1;
  TGeoArb8						*CryostatInnerArb8_2;
  TGeoArb8						*CryostatInnerArb8_3;
  TGeoArb8						*CryostatInnerArb8_4;
  TGeoArb8						*CryostatInnerArb8_5;

    //upper
  TGeoArb8						*CryostatUpperArb8_1;
  TGeoArb8						*CryostatUpperArb8_2;
  TGeoArb8						*CryostatUpperArb8_3;
  TGeoArb8						*CryostatUpperArb8_4;
  TGeoArb8						*CryostatUpperArb8_5;
  TGeoArb8						*CryostatUpperArb8_6;
  TGeoArb8						*CryostatUpperArb8_7;
  TGeoArb8						*CryostatUpperArb8_8;
  TGeoArb8						*CryostatUpperArb8_9;
  TGeoArb8						*CryostatUpperArb8_10;
  TGeoArb8						*CryostatUpperArb8_11;
  TGeoArb8						*CryostatUpperArb8_12;

    //top

  Double_t 						*CryostatTopXCoordinates;
  Double_t 						*CryostatTopYCoordinates;
  Double_t						CryostatTopThickness;
  TGeoXtru						*CryostatTopShape;
  // combination
  */
  // TGeoTranslation 		*CryostatInnerZTranslation;
  // TGeoTranslation 		*CryostatUpperPartsZTranslation;
  TGeoCompositeShape *CryostatShape;
  TGeoCombiTrans *CryostatCombiTrans;
  TGeoVolume *Cryostat;
  // TGeoCombiTrans			*CryostatTopCombiTrans;
  // TGeoVolume					*CryostatTop;

  TGeoCompositeShape *ColdFingerShape;
  TGeoVolume *ColdFinger;

  Int_t CryostatTransparency;

  Double_t ClusterPositionX;
  Double_t ClusterPositionY;
  Double_t ClusterPositionZ;
  Double_t ClusterEulerAnglePsi;   // first angle aroud z-axis
  Double_t ClusterEulerAngleTheta; // second angle around new x-axis	(x')
  Double_t ClusterEulerAnglePhi;   // third  angle around new z-axis 	(z'')

 public:
  PndGeoHypGeCluster(){}; // do not use!

  ~PndGeoHypGeCluster(){};

  // -------- gives the copy number of the last placed cluster. If no cluster placed yet, returns the starting cluster number ----
  Int_t GetClusterNumber();

  // -------- gives the starting cluster number (number giving @ construction) ------
  Int_t GetStartClusterNumber();

  // -------- gives the number of placed clusters -----
  Int_t GetNumberOfCopies();

  void GetExternalParameters(Int_t NumberOfValues, Double_t *DataArray);
  void TrashCommentaryLineInFile();
  virtual void BuildCrystals(Int_t *CrystalNumber) = 0;
  virtual void BuildCryostat() = 0;

  virtual void PrintNodes(Int_t nLevels) = 0;

  void SetCryostatTransparency(Int_t ExtTransparency);
  void SetCryostatColor(Color_t ExtColor);

  void PlaceCluster(TGeoVolume *top, TGeoMatrix *ClusterPlaceAndDirectionTranslation, Int_t *CrystalNumber); //		places the triple cluster
  void PlaceCluster(TGeoVolume *top, Double_t x, Double_t y, Double_t z, Double_t GlobalZOffset, Double_t phi, Double_t theta, Double_t psi, Int_t *CrystalNumber);
  void PlaceCluster(TGeoVolume *top, Double_t GlobalZOffset, Double_t Radius, Double_t phi, Double_t theta, Double_t psi,
                    Int_t *CrystalNumber); // can be used for radial-symmetric arrangements

  ClassDef(PndGeoHypGeCluster, 0); // Class for GeoHypGeCluster
};

#endif /* !PNDGEOHYPGECLUSTER_H */
