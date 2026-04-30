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

/////////////////////////////////////////////////////////////
// PndSttGeometryMap
//
// integration between PndSttStrawMap class and
// /development/lia/stt/PndSttMapCreator functions to create
// a map of the STT and retrieve layerID, sectorID, neighboring
// tubes.
//
// This class is called directly inside PndSttMapCreator and
// fills the PndSttTube with the relevant information.
//
// The user does not need to call this class but can retrieve all
// the info in the PndSttTube obj after having filled the
// fTubeArray via the PndSttMapCreator.
//
// created: May 2013
// authors: L. Lavezzi (PndSttMapCreator
//                      in development branch - Nov 2012)
//          M. Mertens (PndSttStrawMap - Apr 16, 2013)
// modified: Apr 2014 (S. Costanza)
//
/////////////////////////////////////////////////////////////

#include "PndSttGeometryMap.h"
#include "PndSttTubeParameters.h"
#include "PndSttTube.h"
#include "PndSttHit.h"
#include "PndGeoSttPar.h"

#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoRotation.h"
#include "FairGeoTube.h"
#include "FairRun.h"
#include "FairHit.h"

#include "TGeoTube.h"
#include "TVector3.h"
#include "TObjArray.h"
#include "TString.h"
#include "TGeoVolume.h"
#include "TGeoTube.h"
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TList.h"

#include <iostream>

using namespace std;

PndSttGeometryMap::PndSttGeometryMap() : fGeoType(-1), fVerbose(0), fTubeArray(nullptr) {}

PndSttGeometryMap::PndSttGeometryMap(TClonesArray *tubearray, Int_t geoType) : fGeoType(geoType), fVerbose(0), fTubeArray(tubearray)
{
  if (fGeoType != 1)
    cout << "ERROR PndSttGeometryMap: fGeoType " << fGeoType << " not supported" << endl;
  SetGeneralParameters();
  GenerateStrawMap(1);
}

PndSttGeometryMap::~PndSttGeometryMap() {}

// ========================================================================
// ********************* "INTERFACE" FUNCTIONS ****************************
void PndSttGeometryMap::SetGeneralParameters()
{
  if (fGeoType == 1)
    SetGeneralParametersGeoType1();
}

void PndSttGeometryMap::GenerateStrawMap(Int_t map)
{
  if (fGeoType == 1)
    GenerateStrawMapGeoType1(map);
}

Bool_t PndSttGeometryMap::FillGeometryParameters()
{
  if (fGeoType == 1)
    return FillGeometryParametersGeoType1();
  else
    return kFALSE;
}

// ************************ GEO TYPE 1 SPECIFIC ***************************
// ========================================================================
// development PndSttMapCreator
void PndSttGeometryMap::SetGeneralParametersGeoType1()
{ //  CHECK whether it depends on geometry or not

  //   fNLayers = 18;
  //   fNSectors = 6;
  //   const int size = 18* 6; // CHECK

  fNLayers = 26;
  fNSectors = 6;
  // const int size = 26* 6; // CHECK //[R.K. 01/2017] unused variable?

  fNTubes_inner_parallel = 1000;
  fNTubes_outer_parallel = 884;
  fNTubes_fillup_parallel = 914;
  fNTubes_skewed = 1744;
  fNTubes = fNTubes_inner_parallel + fNTubes_outer_parallel + fNTubes_fillup_parallel + fNTubes_skewed;

  fNLayers_inner_parallel = 8;
  fNLayers_skew = 8;
  fNLayers_outer_parallel = 4;
  fNLayers_fillup_parallel = 6;
}

Bool_t PndSttGeometryMap::FillGeometryParametersGeoType1()
{

  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(itube);
    tube->SetLayerLimitFlag(IsEdgeStraw(itube));
    tube->SetSectorLimitFlag(IsSectorBorderStraw(itube));

    bool found = true;
    tube->SetNeighborings(FindNeighborings(tube));
    if (!found)
      cout << "ERROR IN FINDIN NEIGHBORING " << endl;
  }
  return kTRUE;
}

TArrayI PndSttGeometryMap::FindNeighborings(PndSttTube *tube)
{
  double tolerance0 = 1.2; // parallel/parallel && inner parallel/skew
  double tolerance1 = 1.3; // skew/skew
  double tolerance2 = 1.5;
  ;                       // outer parallel/skew
  double tolerance = 1.5; // CHECK tolerance

  TArrayI neighboring;

  int isector = tube->GetSectorID();
  int ilayer = tube->GetLayerID();
  //  cout << "LAYER/SECTOR " << ilayer << " " << isector << endl;

  // possible lay/sec to check:
  // same layer/same sector, before, after
  // layer up/same sector, before, after
  // layer down/same sector, before, after
  int possible_lay_sec[9][2] = {{ilayer, isector},         {ilayer, isector + 1}, {ilayer, isector - 1},     {ilayer + 1, isector},    {ilayer + 1, isector + 1},
                                {ilayer + 1, isector - 1}, {ilayer - 1, isector}, {ilayer - 1, isector + 1}, {ilayer - 1, isector - 1}};

  std::vector<int> neigh_candidates;
  for (int itest = 0; itest < 9; itest++) {

    PndSttTube *tube2 = nullptr;
    int tubeid = -1;
    //    cout << "is " << possible_lay_sec[itest][0] << " " << possible_lay_sec[itest][1] << "good?" << endl;
    // check it is an existing layer/sec
    if (possible_lay_sec[itest][0] < 0 || possible_lay_sec[itest][0] >= fNLayers)
      continue;
    if (possible_lay_sec[itest][1] < 0 || possible_lay_sec[itest][1] >= fNSectors)
      continue;
    // skip che pipe
    if ((isector == 0 && possible_lay_sec[itest][1] == 5) || (isector == 5 && possible_lay_sec[itest][1] == 0) || (isector == 2 && possible_lay_sec[itest][1] == 3) ||
        (isector == 3 && possible_lay_sec[itest][1] == 2))
      continue;
    // if not @ limit of the sector
    //			if (tube->IsSectorLimit() == kFALSE && (possible_lay_sec[itest][1] == isector + 1 || possible_lay_sec[itest][1] == isector - 1))
    //					continue;

    //    cout << "CHECKING " << possible_lay_sec[itest][0] <<  " " << possible_lay_sec[itest][1] << endl;
    neigh_candidates = GetStrawRow(possible_lay_sec[itest][1], possible_lay_sec[itest][0]);

    for (size_t itube = 0; itube < neigh_candidates.size(); itube++) {
      tubeid = neigh_candidates.at(itube);
      if (tube->GetTubeID() == tubeid)
        continue;
      if (tubeid != -1)
        tube2 = (PndSttTube *)fTubeArray->At(tubeid);
      double distance = 1000;
      if (tube2)
        distance = tube->GetDistance(tube2);
      // pick the correct tolerance
      if (tube2->IsSkew() == kTRUE && tube->IsSkew() == kTRUE)
        tolerance = tolerance1;
      else if (tube2->IsParallel() == kTRUE && tube->IsParallel() == kTRUE)
        tolerance = tolerance0;
      else {
        if (ilayer < fNLayers_inner_parallel + 3)
          tolerance = tolerance1;
        else
          tolerance = tolerance2;
      }
      if (distance < tolerance) {
        int size = neighboring.GetSize();
        neighboring.Set(size + 1);
        neighboring.AddAt(tubeid, size);
        //      cout << "ADD " << tubeid << " " << distance << endl;
      }
    }
  }
  return neighboring;
}

TArrayI PndSttGeometryMap::FindNeighborings(int tubeId)
{

  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeId);
  return FindNeighborings(tube);
}

double PndSttGeometryMap::GetAngleBetweenTubes(int tubeID1, int tubeID2) const
{

  PndSttTube *tube1, *tube2;
  tube1 = (PndSttTube *)fTubeArray->At(tubeID1);
  tube2 = (PndSttTube *)fTubeArray->At(tubeID2);

  TVector3 pos1, pos2, direction;
  pos1 = tube1->GetPosition();
  pos2 = tube2->GetPosition();

  direction = pos2 - pos1;

  double phi = direction.Phi();
  if (phi < 0)
    phi = 2 * TMath::Pi() + phi;

  return phi;
}

bool PndSttGeometryMap::InStraightLine(int tube1, int tube2, int tube3) const
{

  double diff = abs(GetAngleBetweenTubes(tube1, tube2) - GetAngleBetweenTubes(tube1, tube3));

  // accept a deviation of 1 percent
  // if ((TMath::Pi() * 0.99) < diff & diff < (TMath::Pi() * 1.01)) {
  if (((TMath::Pi() * 0.99) < diff) && (diff < (TMath::Pi() * 1.01))) { //[R.K. 01/2017] more explicit parenthesis logic
    // tube1 is in the middle
    return true;
  } else if (diff < 0.01) {
    // tube1 is at the edge of the line
    return true;
  } else
    return false;
}

void PndSttGeometryMap::FillStrawNeighborsMap()
{
  for (int i = 1; i < fNTubes + 1; i++) {
    fStrawNeighbors[i] = FindNeighborings(i);
  }
}

TArrayI PndSttGeometryMap::GetNeighboringsByMap(int tubeId)
{
  if (fStrawNeighbors.size() == 0)
    FillStrawNeighborsMap();
  return fStrawNeighbors[tubeId];
}

Double_t PndSttGeometryMap::CalculateStrawPoca(PndSttHit *hit1, PndSttHit *hit2, TVector3 &poca)
{
  TVector3 p1(hit1->GetX(), hit1->GetY(), hit1->GetZ());
  TVector3 p2(hit2->GetX(), hit2->GetY(), hit2->GetZ());

  PndSttTube *tube1 = (PndSttTube *)fTubeArray->At(hit1->GetTubeID());
  PndSttTube *tube2 = (PndSttTube *)fTubeArray->At(hit2->GetTubeID());

  TVector3 u1(tube1->GetWireDirection());
  TVector3 u2(tube2->GetWireDirection());

  if (fVerbose > 1) {
    std::cout << "PndSttGeometryMap::CalculateStrawPoca Hit1 center: (" << hit1->GetX() << "/" << hit1->GetY() << "/" << hit1->GetZ() << ") Dir: (" << u1.x() << "/" << u1.y()
              << "/" << u1.z() << ") HL: " << tube1->GetHalfLength() << std::endl;

    std::cout << "PndSttGeometryMap::CalculateStrawPoca Hit2 center: (" << hit2->GetX() << "/" << hit2->GetY() << "/" << hit2->GetZ() << ") Dir: (" << u2.x() << "/" << u2.y()
              << "/" << u2.z() << ") HL: " << tube2->GetHalfLength() << std::endl;
  }
  TVector3 p21 = p2 - p1;
  if (p21.Mag() < 0.000001) {
    poca = p1;
    if (fVerbose > 1)
      std::cout << "Poca: " << poca.x() << "/" << poca.y() << "/" << poca.z() << std::endl;
    return 0;
  }
  //      p21.Print();
  TVector3 m = u2.Cross(u1);
  if (m.Mag() < 0.000001) {
    poca = p1 + 0.5 * p21;
    if (fVerbose > 1)
      std::cout << "Poca: " << poca.x() << "/" << poca.y() << "/" << poca.z() << std::endl;
    return p21.Mag();
  }
  //      m.Print();
  TVector3 mnorm = (1.0 / m.Mag2()) * m;
  TVector3 r = p21.Cross(mnorm);
  Double_t t1 = r.Dot(u2);
  Double_t t2 = r.Dot(u1);

  if (t1 > tube1->GetHalfLength()) {
    t1 = tube1->GetHalfLength();
  } else if (t1 < (-1) * tube1->GetHalfLength()) {
    t1 = (-1) * tube1->GetHalfLength();
  }

  if (t2 > tube2->GetHalfLength()) {
    t2 = tube2->GetHalfLength();
  } else if (t2 < (-1) * tube2->GetHalfLength()) {
    t2 = (-1) * tube1->GetHalfLength();
  }

  TVector3 q1 = p1 + t1 * u1;
  TVector3 q2 = p2 + t2 * u2;
  TVector3 q21 = q2 - q1;
  poca = q1 + 0.5 * q21;

  if (fVerbose > 1)
    std::cout << "q1: " << q1.x() << "/" << q1.y() << "/" << q1.z() << " t1: " << t1 << std::endl;
  if (fVerbose > 1)
    std::cout << "q2: " << q2.x() << "/" << q2.y() << "/" << q2.z() << " t2: " << t2 << std::endl;

  if (fVerbose > 1)
    std::cout << "Poca: " << poca.x() << "/" << poca.y() << "/" << poca.z() << std::endl;
  //      cout << "Crosspoints: " << endl;
  //      q1.Print();
  //      q2.Print();
  //      cout << "Poca: " << endl;
  //      poca.Print();
  // 	        return (TMath::Abs(p21.Dot(m))/m.Mag());

  return (TMath::Abs(p21.Dot(m)) / m.Mag());
}

void PndSttGeometryMap::GenerateStrawMapGeoType1(Int_t map)
{
  if (map == 0)
    GenerateStrawMapTubeIDGeoType1();
  else if (map == 1)
    GenerateStrawMapAngleGeoType1();
}

// ========================================================================
// PndSttStrawMap class functions
void PndSttGeometryMap::GenerateAngles()
{
  fSectorStart.push_back(1.57);
  fSectorEnd.push_back(2.62);
  fSectorStart.push_back(2.62);
  fSectorEnd.push_back(-2.62 + 2 * TMath::Pi());
  fSectorStart.push_back(-2.62 + 2 * TMath::Pi());
  fSectorEnd.push_back(-1.57 + 2 * TMath::Pi());
  fSectorStart.push_back(-1.57 + 2 * TMath::Pi());
  fSectorEnd.push_back(-0.52 + 2 * TMath::Pi());
  fSectorStart.push_back(-0.52 + 2 * TMath::Pi());
  fSectorEnd.push_back(0.52);
  fSectorStart.push_back(0.52);
  fSectorEnd.push_back(1.57);
}

void PndSttGeometryMap::GenerateStrawMapAngleGeoType1()
{
  GenerateAngles();

  vector<int> currentRow;
  int sector = 0;
  int lastsector = 0;
  int row = 0;
  fSectorOfStraw.push_back(-1);
  fLayerOfStraw.push_back(-1);
  fAxialStraw.push_back(false);
  if (fVerbose > 0)
    cout << "Generating straw map for " << fTubeArray->GetEntriesFast() << " straws." << endl;
  for (int i = 1; i < fTubeArray->GetEntriesFast(); i++) {
    if (fVerbose > 0)
      cout << "Finding tube." << endl;
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(i);
    if (fVerbose > 0)
      cout << "Tube address: " << tube << endl;
    bool isaxial = (tube->GetWireDirection().Theta() < 0.001);
    if (fVerbose > 0) {
      cout << "Axial Straw: " << isaxial << endl;
      cout << "Wire Direction: ";
      tube->GetWireDirection().Print();
    }
    double phi = tube->GetPosition().Phi();
    if (fVerbose > 0)
      cout << "Phi: " << phi << endl;
    if (phi < 0)
      phi += 2 * TMath::Pi();
    if (fVerbose > 0)
      cout << "Checking sector." << endl;
    while (!((phi > fSectorStart.at(sector)) && (phi < fSectorEnd.at(sector))) && !((sector == 4) && ((phi > fSectorStart.at(4)) || (phi < fSectorEnd.at(4))))) {
      sector++;
      sector %= 6;
    }
    if (sector != lastsector) {
      fStrawIndex[lastsector].push_back(currentRow);
      currentRow.clear();
      if (fVerbose > 0)
        cout << "SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;
    }
    if (sector < lastsector)
      row++;
    lastsector = sector;
    currentRow.push_back(i);
    fSectorOfStraw.push_back(sector);
    fLayerOfStraw.push_back(row);
    fAxialStraw.push_back(isaxial);
    if (fVerbose > 0)
      cout << "Straw " << i << " added to " << sector << ", " << row << endl;

    tube->SetLayerID(row);
    tube->SetSectorID(sector);
  }
  fStrawIndex[lastsector].push_back(currentRow);
  if (fVerbose > 0)
    cout << "STT COMPLETE. SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;

  fStartTube = (int **)malloc(sizeof(int *) * fNSectors);
  fEndTube = (int **)malloc(sizeof(int *) * fNSectors);
  fShift = (int **)malloc(sizeof(int *) * fNSectors);
  fShiftSkew = (int **)malloc(sizeof(int *) * fNSectors);
  for (int i = 0; i < fNSectors; i++) {
    fStartTube[i] = (int *)malloc(sizeof(int *) * fNLayers);
    fEndTube[i] = (int *)malloc(sizeof(int *) * fNLayers);
    fShift[i] = (int *)malloc(sizeof(int *) * fNLayers);
    fShiftSkew[i] = (int *)malloc(sizeof(int *) * fNLayers);
  }

  const int size = 26 * 6; // fNLayers * fNSectors
  int shift_list[size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 7, 11, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
                          0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,  0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 7, 11, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
                          0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 0, 0, 0, 0, 0, 0, 2, 5, 7, 10, 14, 19};
  const int sizeskew = 8 * 6; // fNLayers_skew * fNSectors;
  int shift_list_skew[sizeskew] = {16, 19, 16, 16, 19, 16, 17, 20, 17, 17, 20, 17, 19, 21, 19, 19, 21, 19, 20, 22, 20, 20, 22, 20,
                                   21, 24, 21, 21, 24, 21, 22, 25, 22, 22, 25, 22, 24, 26, 24, 24, 26, 24, 25, 27, 25, 25, 27, 25};

  int counter = 0;
  for (int i = 0; i < fNSectors; i++) {
    for (int j = 0; j < fNLayers; j++) {
      fStartTube[i][j] = GetStrawRow(i, j).front();
      fEndTube[i][j] = GetStrawRow(i, j).back();
      fShift[i][j] = shift_list[counter];
      if (j < 8 || j > 15)
        fShiftSkew[i][j] = 0;
      else
        fShiftSkew[i][j] = shift_list_skew[(j - 8) * 6 + i];
      counter++;
    }
  }

  if (fVerbose > 0) {
    // =============== PRINT
    for (int irow = 0; irow < fNSectors; irow++) {
      for (int icol = 0; icol < fNLayers; icol++) {
        cout << " " << fStartTube[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
    for (int irow = 0; irow < fNSectors; irow++) {
      for (int icol = 0; icol < fNLayers; icol++) {
        cout << " " << fEndTube[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
    for (int irow = 0; irow < fNSectors; irow++) {
      for (int icol = 0; icol < fNLayers; icol++) {
        cout << " " << fShift[irow][icol];
      }
      cout << endl;
    }
    cout << endl;
  }

  fStrawMapInitialized = true;
}

bool PndSttGeometryMap::IsEdgeStraw(int strawindex) const
{
  if (GetRow(strawindex) == 0)
    return true;
  int endstraw = 0; // there is no straw index 0
  switch (GetSector(strawindex)) {
  case 0: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front(); break;
  case 2: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back(); break;
  case 3: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front(); break;
  case 5: endstraw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back(); break;
  default: return false;
  };
  return (strawindex == endstraw);
}

int PndSttGeometryMap::IsSectorBorderStraw(int strawindex) const
{
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(strawindex);
  if (tube->IsParallel()) {
    int endstrawcw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
    int endstrawccw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back();
    if (strawindex == endstrawcw)
      return -1;
    if (strawindex == endstrawccw)
      return 1;
    return 0;
  } else {
    int endstrawcw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
    int endstrawscw = endstrawcw + fShiftSkew[GetSector(strawindex)][GetRow(strawindex)];
    int endstrawccw = endstrawscw - 1;
    if (tube->GetHalfLength() == 75) {
      if (strawindex == endstrawcw)
        return -1;
      if (strawindex == endstrawccw)
        return 1;
      return 0;
    } else {
      if (strawindex == endstrawccw + 1 || strawindex == endstrawccw + 3 || strawindex == endstrawccw + 5 || strawindex == endstrawccw + 7 || strawindex == endstrawccw + 9 ||
          strawindex == endstrawccw + 11 || strawindex == endstrawccw + 13 || strawindex == endstrawccw + 15)
        return 1;
      else if (strawindex == endstrawscw + 1 || strawindex == endstrawscw + 3 || strawindex == endstrawscw + 5 || strawindex == endstrawscw + 7 || strawindex == endstrawscw + 9 ||
               strawindex == endstrawscw + 11 || strawindex == endstrawscw + 13 || strawindex == endstrawscw + 15)
        return -1;
    }
  }
  return -1000;
}

bool PndSttGeometryMap::IsAxialRow(int rowindex) const
{
  // only works if all straws in one row are of the same type
  // which is the case for the current geometry
  return IsAxialStraw(GetStrawRow(0, rowindex).at(0));
}

bool PndSttGeometryMap::IsSkewedRow(int rowindex) const
{
  // only works if all straws in one row are of the same type
  // which is the case for the current geometry
  return IsSkewedStraw(GetStrawRow(0, rowindex).at(0));
}

// ========================================================================
// Mixed functions
void PndSttGeometryMap::GenerateStrawMapTubeIDGeoType1()
{
  vector<int> currentRow;
  int isector = 0, ilayer = 0;
  int lastsector = 0;
  fSectorOfStraw.push_back(-1);
  fLayerOfStraw.push_back(-1);

  if (fVerbose > 0)
    cout << "Generating straw map for " << fTubeArray->GetEntriesFast() << " straws." << endl;
  for (int itube = 1; itube < fTubeArray->GetEntriesFast(); itube++) {
    if (fVerbose > 0)
      cout << "Finding tube." << endl;
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(itube);
    bool isaxial = (tube->GetWireDirection().Theta() < 0.001);
    if (fVerbose > 0) {
      cout << "Axial Straw: " << isaxial << endl;
      cout << "Wire Direction: ";
      tube->GetWireDirection().Print();
    }

    while (!(itube >= fStartTube[0][ilayer] && itube <= fEndTube[5][ilayer])) {
      ilayer++;
      isector = 0;
    }

    while (!(itube >= fStartTube[isector][ilayer] && itube <= fEndTube[isector][ilayer])) {
      isector++;
      isector %= 6;
    }

    if (isector != lastsector) {
      fStrawIndex[lastsector].push_back(currentRow);
      currentRow.clear();
      if (fVerbose > 0)
        cout << "SECTOR COMPLETE: Row " << ilayer << " added to sector " << lastsector << endl;
    }
    // if (isector < lastsector) ilayer++;
    lastsector = isector;
    currentRow.push_back(itube);
    fSectorOfStraw.push_back(isector);
    fLayerOfStraw.push_back(ilayer);

    tube->SetLayerID(ilayer);
    tube->SetSectorID(isector);

    if (fVerbose > 0)
      cout << "Straw " << itube << " added to " << isector << ", " << ilayer << endl;
  }
  fStrawIndex[lastsector].push_back(currentRow);
  if (fVerbose > 0)
    cout << "SECTOR COMPLETE: Row " << ilayer << " added to sector " << lastsector << endl;
}

ClassImp(PndSttGeometryMap)
