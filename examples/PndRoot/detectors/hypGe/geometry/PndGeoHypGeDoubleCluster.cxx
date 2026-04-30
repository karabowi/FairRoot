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

// ******** Implementation of class to build HypGe Double Cluster Geometry *************
//					by steinen@kph.uni-mainz.de
#include "PndGeoHypGeDoubleCluster.h"
#include "PndGeoHypGeCrystal.h"
#include <iostream>

ClassImp(PndGeoHypGeDoubleCluster)

  using namespace std;
// double Pi = TMath::Pi();
// -----   Default constructor   -------------------------------------------
PndGeoHypGeDoubleCluster::PndGeoHypGeDoubleCluster()
{
  // Constructor
}
// -----    constructor to use   -------------------------------------------
PndGeoHypGeDoubleCluster::PndGeoHypGeDoubleCluster(TGeoMedium *ExtGe, TGeoMedium *ExtAl, Int_t ExtClusterNumber)
{
  MakeTop = true;

  Ge = ExtGe;
  Al = ExtAl;
  ClusterNumber = ExtClusterNumber;
  StartClusterNumber = ExtClusterNumber;
  NumberOfCryostatEdges = 10;
  NumberOfDifferentZValues = 5;
  NumberOfXYInput = (NumberOfDifferentZValues * 2 - 1) * 2;
  NumberOfFrontArb8 = 4;
  NumberOfTopArb8 = NumberOfFrontArb8;
  // ClusterAssembly = new TGeoVolumeAssembly("ClusterAssembly");		//z = 0 only once, every other z value twice (outer and inner); always x and y coordinate

  // read coordinates from file
  char *InputTitleBuffer;
  ZValuesInputArray = new Double_t[NumberOfDifferentZValues];
  CoordinatesInputArray = new Double_t[NumberOfCryostatEdges * NumberOfXYInput];
  string Filename = getenv("VMCWORKDIR");
  Filename += "/hypGe/geometry/PndGeoHypGeDoubleClusterParfile.txt";

  ParametersInputFile.open(Filename.data());
  FillZValues(NumberOfDifferentZValues);
  FillInputArray(NumberOfCryostatEdges, NumberOfXYInput);
  ParametersInputFile.close();

  CryostatFrontThickness = ZValuesInputArray[1] - ZValuesInputArray[0];
  CryostatLowerArb8Heigt = ZValuesInputArray[2] - ZValuesInputArray[1];
  CryostatMiddleArb8Heigt = ZValuesInputArray[3] - ZValuesInputArray[2];
  CryostatUpperArb8Heigt = ZValuesInputArray[4] - ZValuesInputArray[3];
  CryostatTopThickness = 0.5;
}

// ------- destructor --------------
PndGeoHypGeDoubleCluster::~PndGeoHypGeDoubleCluster()
{
  cout << "Destrucor called" << endl;
};

// --------------- build the crystals ---------------------------

void PndGeoHypGeDoubleCluster::BuildCrystals(Int_t *CrystalNumber)
{
  // create 2 crystals. The copy number of a crystal is composed of the number of the cluster and a running number for every placed crystal like e.g.: cluster 4, crystal 7: 407 ;
  // cluster 11, crystal 21: 1121
  Crystal1Number = *CrystalNumber;
  *CrystalNumber += 1;
  Crystal2Number = *CrystalNumber;
  *CrystalNumber += 1;
  cout << "Cluster:" << ClusterNumber << "\tCrystal1Number: " << Crystal1Number << endl;
  cout << "Cluster:" << ClusterNumber << "\tCrystal2Number: " << Crystal2Number << endl;

  Crystal1 = new PndGeoHypGeCrystal(Ge, Al, 100 * ClusterNumber + Crystal1Number);
  Crystal2 = new PndGeoHypGeCrystal(Ge, Al, 100 * ClusterNumber + Crystal2Number);

  // set positions of the 2 crystals

  DoubleCrystalYOffset = 6.1141 / 2; // measured in CAD
  DoubleCrystalZOffset = 0.7704;     // measured in CAD

  Crystal1Rotation = new TGeoRotation("Crystal1Rotation", 0, -4.125, 0); // angles measured in CAD drawing
  Crystal1Rotation->RegisterYourself();
  Crystal2Rotation = new TGeoRotation("Crystal2Rotation", 0, 4.125, 0);
  Crystal2Rotation->RegisterYourself();

  // maybe some signs needs adaption
  Crystal1Position = new TGeoCombiTrans(0, -DoubleCrystalYOffset, -DoubleCrystalZOffset, Crystal1Rotation);
  Crystal1Position->RegisterYourself();
  Crystal2Position = new TGeoCombiTrans(0, +DoubleCrystalYOffset, -DoubleCrystalZOffset, Crystal2Rotation);
  Crystal2Position->RegisterYourself();

  Crystal1->PlaceCrystal(ClusterAssembly, Crystal1Position);
  Crystal2->PlaceCrystal(ClusterAssembly, Crystal2Position);
};

// ---------------	build the cryostat --------------------------
void PndGeoHypGeDoubleCluster::BuildCryostat()
{
  // the cryostat is a combination front, side and top parts. The front and the top is build of 4 TGeoArb8. The side parts are split into 10 lower, 10 middle and 10 upper parts.
  // All are TGeoArb8. This parts are all combined to a TGeoCompositeShape

  string CompositeShapeExpression;
  // the front of the cryostat

  CryostatFrontArb8XY_1 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_1, 1, 2, 3, 10, NumberOfXYInput);
  CryostatFrontArb8XY_2 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_2, 3, 4, 5, 6, NumberOfXYInput);
  CryostatFrontArb8XY_3 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_3, 3, 6, 7, 8, NumberOfXYInput);
  CryostatFrontArb8XY_4 = new Double_t[16];
  BuildTArb8FrontArray(CryostatFrontArb8XY_4, 3, 8, 9, 10, NumberOfXYInput);

  CryostatFrontArb8_1 = new TGeoArb8("CryostatDoubleFrontArb8_1", CryostatFrontThickness / 2, CryostatFrontArb8XY_1);
  CryostatFrontArb8_2 = new TGeoArb8("CryostatDoubleFrontArb8_2", CryostatFrontThickness / 2, CryostatFrontArb8XY_2);
  CryostatFrontArb8_3 = new TGeoArb8("CryostatDoubleFrontArb8_3", CryostatFrontThickness / 2, CryostatFrontArb8XY_3);
  CryostatFrontArb8_4 = new TGeoArb8("CryostatDoubleFrontArb8_4", CryostatFrontThickness / 2, CryostatFrontArb8XY_4);

  CryostatFrontTranslation = new TGeoTranslation("CryostatDoubleFrontTranslation", 0, 0, CryostatFrontThickness / 2);
  CryostatFrontTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfFrontArb8 + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatDoubleFrontArb8_%d : CryostatDoubleFrontTranslation", i);
    CompositeShapeExpression += buffer;
    // if (i<NumberOfFrontArb8)	//uncomment if last part of composite shape
    {
      CompositeShapeExpression += "+";
    }
  }

  // the lower part of the cryostat

  CryostatLowerArb8XY_1 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_1, 1, 2, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_2 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_2, 2, 3, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_3 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_3, 3, 4, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_4 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_4, 4, 5, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_5 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_5, 5, 6, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_6 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_6, 6, 7, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_7 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_7, 7, 8, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_8 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_8, 8, 9, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_9 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_9, 9, 10, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatLowerArb8XY_10 = new Double_t[16];
  BuildTArb8Array(CryostatLowerArb8XY_10, 10, 1, 1, 2, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatLowerArb8_1 = new TGeoArb8("CryostatDoubleLowerArb8_1", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_1);
  CryostatLowerArb8_2 = new TGeoArb8("CryostatDoubleLowerArb8_2", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_2);
  CryostatLowerArb8_3 = new TGeoArb8("CryostatDoubleLowerArb8_3", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_3);
  CryostatLowerArb8_4 = new TGeoArb8("CryostatDoubleLowerArb8_4", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_4);
  CryostatLowerArb8_5 = new TGeoArb8("CryostatDoubleLowerArb8_5", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_5);
  CryostatLowerArb8_6 = new TGeoArb8("CryostatDoubleLowerArb8_6", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_6);
  CryostatLowerArb8_7 = new TGeoArb8("CryostatDoubleLowerArb8_7", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_7);
  CryostatLowerArb8_8 = new TGeoArb8("CryostatDoubleLowerArb8_8", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_8);
  CryostatLowerArb8_9 = new TGeoArb8("CryostatDoubleLowerArb8_9", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_9);
  CryostatLowerArb8_10 = new TGeoArb8("CryostatDoubleLowerArb8_10", CryostatLowerArb8Heigt / 2, CryostatLowerArb8XY_10);

  CryostatLowerTranslation = new TGeoTranslation("CryostatDoubleLowerTranslation", 0, 0, CryostatFrontThickness + CryostatLowerArb8Heigt / 2);
  CryostatLowerTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfCryostatEdges + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatDoubleLowerArb8_%d : CryostatDoubleLowerTranslation", i);
    CompositeShapeExpression += buffer;
    // if (i<NumberOfCryostatEdges)		//uncomment if last part of composite shape
    {
      CompositeShapeExpression += "+";
    }
  }

  // the middle part of the cryostat

  CryostatMiddleArb8XY_1 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_1, 1, 2, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput); // from PndGeoHypGeBase
  CryostatMiddleArb8XY_2 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_2, 2, 3, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_3 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_3, 3, 4, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_4 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_4, 4, 5, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_5 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_5, 5, 6, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_6 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_6, 6, 7, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_7 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_7, 7, 8, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_8 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_8, 8, 9, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_9 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_9, 9, 10, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatMiddleArb8XY_10 = new Double_t[16];
  BuildTArb8Array(CryostatMiddleArb8XY_10, 10, 1, 2, 3, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatMiddleArb8_1 = new TGeoArb8("CryostatDoubleMiddleArb8_1", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_1);
  CryostatMiddleArb8_2 = new TGeoArb8("CryostatDoubleMiddleArb8_2", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_2);
  CryostatMiddleArb8_3 = new TGeoArb8("CryostatDoubleMiddleArb8_3", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_3);
  CryostatMiddleArb8_4 = new TGeoArb8("CryostatDoubleMiddleArb8_4", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_4);
  CryostatMiddleArb8_5 = new TGeoArb8("CryostatDoubleMiddleArb8_5", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_5);
  CryostatMiddleArb8_6 = new TGeoArb8("CryostatDoubleMiddleArb8_6", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_6);
  CryostatMiddleArb8_7 = new TGeoArb8("CryostatDoubleMiddleArb8_7", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_7);
  CryostatMiddleArb8_8 = new TGeoArb8("CryostatDoubleMiddleArb8_8", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_8);
  CryostatMiddleArb8_9 = new TGeoArb8("CryostatDoubleMiddleArb8_9", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_9);
  CryostatMiddleArb8_10 = new TGeoArb8("CryostatDoubleMiddleArb8_10", CryostatMiddleArb8Heigt / 2, CryostatMiddleArb8XY_10);

  CryostatMiddleTranslation = new TGeoTranslation("CryostatDoubleMiddleTranslation", 0, 0, CryostatFrontThickness + CryostatLowerArb8Heigt + CryostatMiddleArb8Heigt / 2);
  CryostatMiddleTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfCryostatEdges + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatDoubleMiddleArb8_%d : CryostatDoubleMiddleTranslation", i);
    CompositeShapeExpression += buffer;
    // if (i< NumberOfCryostatEdges)		//uncomment if last part of composite shape
    {
      CompositeShapeExpression += "+";
    }
  }

  // the upper part of the cryostat

  CryostatUpperArb8XY_1 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_1, 1, 2, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_2 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_2, 2, 3, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_3 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_3, 3, 4, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_4 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_4, 4, 5, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_5 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_5, 5, 6, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_6 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_6, 6, 7, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_7 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_7, 7, 8, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_8 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_8, 8, 9, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_9 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_9, 9, 10, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatUpperArb8XY_10 = new Double_t[16];
  BuildTArb8Array(CryostatUpperArb8XY_10, 10, 1, 3, 4, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatUpperArb8_1 = new TGeoArb8("CryostatDoubleUpperArb8_1", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_1);
  CryostatUpperArb8_2 = new TGeoArb8("CryostatDoubleUpperArb8_2", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_2);
  CryostatUpperArb8_3 = new TGeoArb8("CryostatDoubleUpperArb8_3", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_3);
  CryostatUpperArb8_4 = new TGeoArb8("CryostatDoubleUpperArb8_4", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_4);
  CryostatUpperArb8_5 = new TGeoArb8("CryostatDoubleUpperArb8_5", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_5);
  CryostatUpperArb8_6 = new TGeoArb8("CryostatDoubleUpperArb8_6", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_6);
  CryostatUpperArb8_7 = new TGeoArb8("CryostatDoubleUpperArb8_7", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_7);
  CryostatUpperArb8_8 = new TGeoArb8("CryostatDoubleUpperArb8_8", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_8);
  CryostatUpperArb8_9 = new TGeoArb8("CryostatDoubleUpperArb8_9", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_9);
  CryostatUpperArb8_10 = new TGeoArb8("CryostatDoubleUpperArb8_10", CryostatUpperArb8Heigt / 2, CryostatUpperArb8XY_10);

  CryostatUpperTranslation =
    new TGeoTranslation("CryostatDoubleUpperTranslation", 0, 0, CryostatFrontThickness + CryostatLowerArb8Heigt + CryostatMiddleArb8Heigt + CryostatUpperArb8Heigt / 2);
  CryostatUpperTranslation->RegisterYourself();

  for (Int_t i = 1; i < NumberOfCryostatEdges + 1; i++) {
    char buffer[100];
    sprintf(buffer, "CryostatDoubleUpperArb8_%d : CryostatDoubleUpperTranslation", i);
    CompositeShapeExpression += buffer;

    if (i < NumberOfCryostatEdges) {
      CompositeShapeExpression += "+";
    }
    if (i == NumberOfCryostatEdges && MakeTop) {
      CompositeShapeExpression += "+";
    }
  }

  // the top of the cryostat

  CryostatTopArb8XY_1 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_1, 1, 2, 3, 10, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_2 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_2, 3, 4, 5, 6, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_3 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_3, 3, 6, 7, 8, NumberOfDifferentZValues - 1, NumberOfXYInput);
  CryostatTopArb8XY_4 = new Double_t[16];
  BuildTArb8TopArray(CryostatTopArb8XY_4, 3, 8, 9, 10, NumberOfDifferentZValues - 1, NumberOfXYInput);

  CryostatTopArb8_1 = new TGeoArb8("CryostatDoubleTopArb8_1", CryostatTopThickness / 2, CryostatTopArb8XY_1);
  CryostatTopArb8_2 = new TGeoArb8("CryostatDoubleTopArb8_2", CryostatTopThickness / 2, CryostatTopArb8XY_2);
  CryostatTopArb8_3 = new TGeoArb8("CryostatDoubleTopArb8_3", CryostatTopThickness / 2, CryostatTopArb8XY_3);
  CryostatTopArb8_4 = new TGeoArb8("CryostatDoubleTopArb8_4", CryostatTopThickness / 2, CryostatTopArb8XY_4);

  CryostatTopTranslation = new TGeoTranslation("CryostatDoubleTopTranslation", 0, 0,
                                               CryostatFrontThickness + CryostatLowerArb8Heigt + CryostatMiddleArb8Heigt + CryostatUpperArb8Heigt + CryostatTopThickness / 2);
  CryostatTopTranslation->RegisterYourself();

  if (MakeTop) {
    for (Int_t i = 1; i < NumberOfTopArb8 + 1; i++) {
      char buffer[100];
      sprintf(buffer, "CryostatDoubleTopArb8_%d : CryostatDoubleTopTranslation", i);
      CompositeShapeExpression += buffer;
      if (i < NumberOfTopArb8) // uncomment if last part of composite shape
      {
        CompositeShapeExpression += "+";
      }
    }
  }

  // cout << CompositeShapeExpression << endl;

  // combine the cryostat
  CryostatShape = new TGeoCompositeShape("CryostatShape", CompositeShapeExpression.data());
  Cryostat = new TGeoVolume("Cryostat", CryostatShape, Al);
  Cryostat->SetLineColor(kGreen);
  Cryostat->SetFillColor(kGreen);
  // bring the Cryostat in the right position
  CryostatCombiTrans = new TGeoCombiTrans("CryostatCombiTrans", 0, 0, 0, new TGeoRotation("CryostatRotation", 0, 180, 0));
  CryostatCombiTrans->RegisterYourself();

  ClusterAssembly->AddNode(Cryostat, ClusterNumber, CryostatCombiTrans);
};

// ---------------  Prints the nodes of a cluster -----------------
void PndGeoHypGeDoubleCluster::PrintNodes(Int_t nLevels)
{
  if (nLevels == 0) {
    cout << "Nothing to Print, use higher number to go deeper in the tree." << endl;
  }
  if (nLevels > 0) {
    cout << "Printing the content of the cluster assembly." << endl;
    cout << endl << "Number of Nodes in cluster assembly: " << ClusterAssembly->GetNdaughters() << endl;
    ClusterAssembly->PrintNodes();
  }
  if (nLevels > 1) {
    cout << "Printing the content of the crystal assemblies." << endl;
    cout << endl << "Number of Nodes in crystal assembly 1: " << Crystal1->GetNdaughters() << endl;
    Crystal1->PrintNodes();
    cout << endl << "Number of Nodes in crystal assembly 2: " << Crystal2->GetNdaughters() << endl;
    Crystal2->PrintNodes();
  }
}
