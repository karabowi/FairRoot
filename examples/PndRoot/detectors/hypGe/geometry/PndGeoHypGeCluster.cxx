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

// ******** Implementation of class to build HypGe Triple Cluster Geometry *************
//					by steinen@kph.uni-mainz.de
#include "PndGeoHypGeCluster.h"
//#include "PndGeoHypGeCrystal.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

ClassImp(PndGeoHypGeCluster)

  // -------- gives the copy number of the last placed cluster. If no cluster placed yet, returns the starting cluster number ----
  Int_t PndGeoHypGeCluster::GetClusterNumber()
{
  if (ClusterNumber == StartClusterNumber) {
    return ClusterNumber;
    cout << "No Cluster placed yet!" << endl;
  } else {
    return ClusterNumber - 1;
  }
};

// -------- gives the starting cluster number (number giving @ construction) ------
Int_t PndGeoHypGeCluster::GetStartClusterNumber()
{
  return StartClusterNumber;
}

// -------- gives the number of placed clusters -----
Int_t PndGeoHypGeCluster::GetNumberOfCopies()
{
  return ClusterNumber - StartClusterNumber;
}

void PndGeoHypGeCluster::GetExternalParameters(Int_t NumberOfValues, Double_t *DataArray)
{
  if (!ParametersInputFile) {
    cout << "Inputfile does not exist!" << endl;
  } else if (ParametersInputFile.good()) {
    string InputBuffer;
    char *pEnd;
    getline(ParametersInputFile, InputBuffer);
    // cout << InputBuffer << endl;

    DataArray[0] = strtod(InputBuffer.data(), &pEnd);
    for (Int_t iValues = 1; iValues < NumberOfValues; iValues++) {
      DataArray[iValues] = strtod(pEnd, &pEnd);
      // cout << "Value_" << iValues <<": " <<DataArray[iValues] << endl;
    }
  }
}
void PndGeoHypGeCluster::TrashCommentaryLineInFile()
{
  if (!ParametersInputFile) {
    cout << "Inputfile does not exist!" << endl;
  } else if (ParametersInputFile.good()) {
    string InputBuffer;
    getline(ParametersInputFile, InputBuffer);
  }
}
void PndGeoHypGeCluster::SetCryostatTransparency(Int_t ExtTransparency)
{
  // some make-up ;D
  CryostatTransparency = ExtTransparency;

  Cryostat->SetTransparency(CryostatTransparency);
}

void PndGeoHypGeCluster::SetCryostatColor(Color_t ExtColor)
{
  // some make-up ;D

  Cryostat->SetLineColor(ExtColor);
}

// place the cluster with external TGeoMatrix
void PndGeoHypGeCluster::PlaceCluster(TGeoVolume *top, TGeoMatrix *ClusterPlaceAndDirectionTranslation, Int_t *CrystalNumber)
{
  ClusterAssembly = new TGeoVolumeAssembly("ClusterAssembly");
  BuildCrystals(CrystalNumber);
  BuildCryostat();
  SetCryostatColor(kGreen);
  SetCryostatTransparency(0);
  top->AddNode(ClusterAssembly, ClusterNumber, ClusterPlaceAndDirectionTranslation);
  ClusterNumber++;
};

// place the cluster by giving just the coordinates and euler-angles
void PndGeoHypGeCluster::PlaceCluster(TGeoVolume *top, Double_t x, Double_t y, Double_t z, Double_t GlobalZOffset, Double_t psi, Double_t theta, Double_t phi, Int_t *CrystalNumber)
{
  ClusterAssembly = new TGeoVolumeAssembly("ClusterAssembly");
  BuildCrystals(CrystalNumber);
  BuildCryostat();
  SetCryostatColor(kGreen);
  SetCryostatTransparency(0);

  ClusterPositionX = x;
  ClusterPositionY = y;
  ClusterPositionZ = z + GlobalZOffset;
  ClusterEulerAnglePsi = psi;
  ClusterEulerAngleTheta = theta;
  ClusterEulerAnglePhi = phi;
  TGeoRotation *TempClusterRotation = new TGeoRotation("TempClusterRotation", ClusterEulerAnglePsi, ClusterEulerAngleTheta, ClusterEulerAnglePhi);
  TempClusterRotation->RegisterYourself();
  top->AddNode(ClusterAssembly, ClusterNumber, new TGeoCombiTrans(ClusterPositionX, ClusterPositionY, ClusterPositionZ, TempClusterRotation));
  ClusterNumber++;
};

void PndGeoHypGeCluster::PlaceCluster(TGeoVolume *top, Double_t GlobalZOffset, Double_t Radius, Double_t psi, Double_t theta, Double_t phi, Int_t *CrystalNumber)
{
  ClusterAssembly = new TGeoVolumeAssembly("ClusterAssembly");
  BuildCrystals(CrystalNumber);
  BuildCryostat();
  SetCryostatColor(kGreen);
  SetCryostatTransparency(0);

  ClusterPositionX = -Radius * TMath::Sin(TMath::Pi() / 180 * psi) * TMath::Sin(TMath::Pi() / 180 * theta);
  ClusterPositionY = Radius * TMath::Cos(TMath::Pi() / 180 * psi) * TMath::Sin(TMath::Pi() / 180 * theta);
  ClusterPositionZ = -Radius * TMath::Cos(TMath::Pi() / 180 * theta) + GlobalZOffset;
  // cout << ClusterPositionZ << endl << endl;
  ClusterEulerAnglePsi = psi;
  ClusterEulerAngleTheta = theta;
  ClusterEulerAnglePhi = phi;
  TGeoRotation *TempClusterRotation = new TGeoRotation("TempClusterRotation", ClusterEulerAnglePsi, ClusterEulerAngleTheta, ClusterEulerAnglePhi);
  TempClusterRotation->RegisterYourself();
  top->AddNode(ClusterAssembly, ClusterNumber, new TGeoCombiTrans(ClusterPositionX, ClusterPositionY, ClusterPositionZ, TempClusterRotation));
  ClusterNumber++;
};
