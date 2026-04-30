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

/*
 * PndSTTStrawMap.cxx
 *
 *  Created on: Apr 16, 2013
 *      Author: mertens
 */

#include "PndSttStrawMap.h"
#include "TClonesArray.h"
#include "PndSttTube.h"
#include "TMath.h"

#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;

PndSttStrawMap::PndSttStrawMap() : fStrawMapInitialized(false), fTubeArray(0)
{
  GenerateAngles();
}

PndSttStrawMap::PndSttStrawMap(TClonesArray *const stt_tube_array) : fStrawMapInitialized(false), fTubeArray(0)
{
  GenerateAngles();
  GenerateStrawMap(stt_tube_array);
}

void PndSttStrawMap::GenerateAngles()
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

void PndSttStrawMap::GenerateStrawMap(TClonesArray *const stt_tube_array)
{
  fTubeArray = stt_tube_array;

  vector<int> currentRow;
  int sector = 0;
  int lastsector = 0;
  int row = 0;
  fSectorOfStraw.push_back(-1);
  fRowOfStraw.push_back(-1);
  fAxialStraw.push_back(false);
  // cout << "Generating straw map for " << fTubeArray->GetEntriesFast() << " straws." << endl;
  for (int i = 1; i < fTubeArray->GetEntriesFast(); i++) {
    // cout << "Finding tube." << endl;
    PndSttTube *tube = GetTube(i);
    // cout << "Tube address: " << tube << endl;
    bool isaxial = (tube->GetWireDirection().Theta() < 0.001);
    // cout << "Axial Straw: " << isaxial << endl;
    // cout << "Wire Direction: ";
    //		tube->GetWireDirection().Print();
    double phi = tube->GetPosition().Phi();
    // cout << "Phi: " << phi << endl;
    if (phi < 0)
      phi += 2 * TMath::Pi();
    // cout << "Checking sector." << endl;
    while (!((phi > fSectorStart.at(sector)) && (phi < fSectorEnd.at(sector))) && !((sector == 4) && ((phi > fSectorStart.at(4)) || (phi < fSectorEnd.at(4))))) {
      sector++;
      sector %= 6;
    }
    if (sector != lastsector) {
      fStrawIndex[lastsector].push_back(currentRow);
      currentRow.clear();
      // cout << "SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;
    }
    if (sector < lastsector)
      row++;
    lastsector = sector;
    currentRow.push_back(i);
    fSectorOfStraw.push_back(sector);
    fRowOfStraw.push_back(row);
    fAxialStraw.push_back(isaxial);
    // cout << "Straw " << i << " added to " << sector << ", " << row << endl;
  }
  fStrawIndex[lastsector].push_back(currentRow);
  // cout << "STT COMPLETE. SECTOR COMPLETE: Row " << row << " added to sector " << lastsector << endl;

  fStrawMapInitialized = true;
}

int PndSttStrawMap::FindPhiSector(double phi) const
{
  int sector = 0;
  if (phi < 0)
    phi += 2 * TMath::Pi();
  while (!((phi > fSectorStart.at(sector)) && (phi < fSectorEnd.at(sector))) && !((sector == 4) && ((phi > fSectorStart.at(4)) || (phi < fSectorEnd.at(4))))) {
    sector++;
    sector %= 6;
  }
  return sector;
}

bool PndSttStrawMap::IsEdgeStraw(int strawindex) const
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

int PndSttStrawMap::IsSectorBorderStraw(int strawindex) const
{
  int endstrawcw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).front();
  int endstrawccw = GetStrawRow(GetSector(strawindex), GetRow(strawindex)).back();
  if (strawindex == endstrawcw)
    return -1;
  if (strawindex == endstrawccw)
    return 1;
  return 0;
}

bool PndSttStrawMap::IsAxialRow(int rowindex) const
{
  // only works if all straws in one row are of the same type
  // which is the case for the current geometry
  return IsAxialStraw(GetStrawRow(0, rowindex).at(0));
}

bool PndSttStrawMap::IsSkewedRow(int rowindex) const
{
  // only works if all straws in one row are of the same type
  // which is the case for the current geometry
  return IsSkewedStraw(GetStrawRow(0, rowindex).at(0));
}

ClassImp(PndSttStrawMap)
