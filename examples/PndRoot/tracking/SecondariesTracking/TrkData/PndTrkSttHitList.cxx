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

//
// PndTrkSttHitList.cxx
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//

#include "PndTrkSttHitList.h"

#include "PndDetectorList.h"
#include "PndTrkParameters.h"
#include "PndSttTube.h"

#include "TClonesArray.h"

#include <iostream>

using namespace std;

// ------------------ instance ----------------------------------
PndTrkSttHitList *PndTrkSttHitList::fInstance = nullptr;

PndTrkSttHitList *PndTrkSttHitList::Instance()
{
  if (!fInstance)
    cout << "NO PndTrkSttHitList: you must fill it BEFORE doing what you are doing!" << endl;
  return fInstance;
}

PndTrkSttHitList *PndTrkSttHitList::Instanciate()
{
  if (fInstance)
    cout << "you are instanciating a PndTrkSttHitList, but you already did it, what are you doing?" << endl;
  fInstance = this;
  return fInstance;
}

PndTrkSttHitList::PndTrkSttHitList() : PndTrkHitList(), fTubeArray(nullptr)
{
  hitmap.clear();
  hitmap2.clear();
  hitmap3.clear();
}

PndTrkSttHitList::PndTrkSttHitList(TClonesArray *tubearray) : PndTrkHitList(), fTubeArray(tubearray)
{
  hitmap.clear();
  hitmap2.clear();
  hitmap3.clear();
}

PndTrkSttHitList::~PndTrkSttHitList()
{
  fInstance = nullptr;
}

// void PndTrkSttHitList::AddHit(Int_t hitid, Int_t detid, PndSttHit *hit) {
void PndTrkSttHitList::AddHit(Int_t hitid, Int_t detid, FairHit *hit)
{
  TVector3 position;
  //   hit->Position(position);

  int tubeID = ((PndSttHit *)hit)->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
  Int_t iregion = -1;
  position = tube->GetPosition();
  // parallel
  if (tube->GetWireDirection() == TVector3(0., 0., 1.)) {
    // x < 0
    if (tube->GetPosition().X() < 0) {
      if (tube->GetPosition().Perp() < SKEWLIMIT)
        iregion = INNER_LEFT; // 3
      else
        iregion = OUTER_LEFT; // 7
    }
    // x > 0
    else {
      if (tube->GetPosition().Perp() < SKEWLIMIT)
        iregion = INNER_RIGHT; // 2
      else
        iregion = OUTER_RIGHT; // 6
    }
  }
  // skewed tubes
  else {
    // x < 0
    if (tube->GetPosition().X() < 0)
      iregion = SKEW_LEFT; // 5
    // x > 0
    else
      iregion = SKEW_RIGHT; // 4

    //    cout << "tubeID " << tubeID << " " << iregion << endl;
    //    position.Print();
    //    tube->GetWireDirection().Print();
  }

  PndTrkHitList::AddHit(hitid, detid, 0, iregion, tubeID, position, ((PndSttHit *)hit)->GetIsochrone()); // CHECK iregion

  std::map<int, int>::iterator it;
  int isec = tube->GetSectorID();
  hitmap.insert(std::pair<int, int>(isec, hitlist.size() - 1));
  int ilay = tube->GetLayerID();
  hitmap2.insert(std::pair<int, int>(ilay, hitlist.size() - 1));
  //   cout << "ISEC " << isec << " ILAY " << ilay << endl;
  int iglo = FromSecLayToGlo(isec, ilay);
  hitmap3.insert(std::pair<int, int>(iglo, hitlist.size() - 1)); // CHECK
}

void PndTrkSttHitList::AddHit(PndTrkHit *hit)
{
  PndTrkHitList::AddHit(hit);

  std::map<int, int>::iterator it;
  int tubeID = hit->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
  int isec = tube->GetSectorID();
  hitmap.insert(std::pair<int, int>(isec, hitlist.size() - 1));
  int ilay = tube->GetLayerID();
  hitmap2.insert(std::pair<int, int>(ilay, hitlist.size() - 1));
  //   cout << "ISEC " << isec << " ILAY " << ilay << endl;
  int iglo = FromSecLayToGlo(isec, ilay);
  hitmap3.insert(std::pair<int, int>(iglo, hitlist.size() - 1)); // CHECK
}

// ---------------------- LAYERS ----------------------
void PndTrkSttHitList::PrintLayers()
{

  for (int ilay = 0; ilay < 24; ilay++) {
    cout << "LAYER " << ilay << ": ";

    std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
    ret = hitmap2.equal_range(ilay);
    for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
      std::cout << ' ' << it->second;
    cout << endl;
  }
}

void PndTrkSttHitList::PrintLayer(int ilay)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromLayer(ilay);
  cout << "Layer " << ilay << ": ";
  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    cout << " " << (listofhits.at(ihit))->GetHitID();
  cout << endl;
}

void PndTrkSttHitList::DrawLayer(int ilay, Color_t color)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromLayer(ilay);

  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    (listofhits.at(ihit))->Draw(color);
}

int PndTrkSttHitList::GetNofHitsInLayer(int ilay)
{
  //   cout << "GET NOF HITS IN LAYER" << endl;

  //   std::multimap< int, int >::iterator itr = hitmap2.begin();
  //   while(itr != hitmap2.end() ) {
  //     std::pair < int , int > thispair = *itr;
  //     cout <<   thispair.first <<  " " << thispair.second << endl;
  //     itr++;
  //   }

  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  ret = hitmap2.equal_range(ilay);
  int counter = 0;
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
    counter++; // CHECK
  return counter;
}

std::vector<PndTrkHit *> PndTrkSttHitList::GetHitListFromLayer(int ilay)
{

  std::vector<PndTrkHit *> thislayer;

  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  ret = hitmap2.equal_range(ilay);
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it) {
    PndTrkHit *hit = &hitlist[it->second];
    //   cout << "hit " << hit->GetHitID() << endl;
    thislayer.push_back(hit);
  }
  return thislayer;
}

PndTrkHit *PndTrkSttHitList::GetHitFromLayer(int ihit, int ilay)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromSector(ilay);
  return listofhits.at(ihit);
}

// ---------------------- SECTORS -------------------------
void PndTrkSttHitList::PrintSectors()
{

  for (int isec = 0; isec < 6; isec++) {
    cout << "SECTOR " << isec << ": ";

    std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
    ret = hitmap.equal_range(isec);
    for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
      std::cout << ' ' << it->second;
    cout << endl;
  }
}

void PndTrkSttHitList::PrintSector(int isec)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromSector(isec);
  cout << "Sector " << isec << ": ";
  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    cout << " " << (listofhits.at(ihit))->GetHitID();
  cout << endl;
}

void PndTrkSttHitList::DrawSector(int isec, Color_t color)
{

  std::vector<PndTrkHit *> listofhits = GetHitListFromSector(isec);
  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    (listofhits.at(ihit))->Draw(color);
}

int PndTrkSttHitList::GetNofHitsInSector(int isec)
{
  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  ret = hitmap.equal_range(isec);
  int counter = 0;
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
    counter++; // CHECK
  return counter;
}

std::vector<PndTrkHit *> PndTrkSttHitList::GetHitListFromSector(int isec)
{

  std::vector<PndTrkHit *> thissector;

  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  ret = hitmap.equal_range(isec);
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it) {
    PndTrkHit *hit = &hitlist[it->second];
    //   cout << "hit " << hit->GetHitID() << endl;
    thissector.push_back(hit);
  }
  return thissector;
}

PndTrkHit *PndTrkSttHitList::GetHitFromSector(int ihit, int isec)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromSector(isec);
  return listofhits.at(ihit);
}

// ---------------------- GLOBALS -------------------------

void PndTrkSttHitList::PrintSectorsAndLayers()
{

  int nmaxglo = FromSecLayToGlo(6, 25); // CHECK
  for (int iglo = 0; iglo < nmaxglo; iglo++) {
    int ilay, isec;

    FromGloToSecLay(iglo, isec, ilay);
    cout << "SECTOR " << isec << ", LAYER " << ilay << ": ";

    std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
    ret = hitmap3.equal_range(iglo);
    for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
      std::cout << ' ' << it->second;
    cout << endl;
  }
}

void PndTrkSttHitList::PrintSectorAndLayer(int isec, int ilay)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromSectorAndLayer(isec, ilay);
  cout << "Sector " << isec << ", Layer " << ilay << ": ";

  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    cout << " " << (listofhits.at(ihit))->GetHitID();
  cout << endl;
}

void PndTrkSttHitList::DrawSectorAndLayer(int isec, int ilay, Color_t color)
{

  std::vector<PndTrkHit *> listofhits = GetHitListFromSectorAndLayer(isec, ilay);
  for (size_t ihit = 0; ihit < listofhits.size(); ihit++)
    (listofhits.at(ihit))->Draw(color);
}

int PndTrkSttHitList::GetNofHitsInSectorAndLayer(int isec, int ilay)
{
  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  int iglo = FromSecLayToGlo(isec, ilay);
  ret = hitmap3.equal_range(iglo);
  int counter = 0;
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it)
    counter++; // CHECK
  return counter;
}

std::vector<PndTrkHit *> PndTrkSttHitList::GetHitListFromSectorAndLayer(int isec, int ilay)
{

  std::vector<PndTrkHit *> thissecandlay;

  std::pair<std::multimap<int, int>::iterator, std::multimap<int, int>::iterator> ret;
  int iglo = FromSecLayToGlo(isec, ilay);
  ret = hitmap3.equal_range(iglo);
  for (std::multimap<int, int>::iterator it = ret.first; it != ret.second; ++it) {
    PndTrkHit *hit = &hitlist[it->second];
    //   cout << "hit " << hit->GetHitID() << endl;
    thissecandlay.push_back(hit);
  }
  return thissecandlay;
}

PndTrkHit *PndTrkSttHitList::GetHitFromSectorAndLayer(int ihit, int isec, int ilay)
{
  std::vector<PndTrkHit *> listofhits = GetHitListFromSectorAndLayer(isec, ilay);
  return listofhits.at(ihit);
}

Int_t PndTrkSttHitList::FromSecLayToGlo(int isec, int ilay)
{

  int nofsectors = 6;
  return isec + ilay * nofsectors; // CHECK
}

void PndTrkSttHitList::FromGloToSecLay(int iglo, int &isec, int &ilay)
{
  int nofsectors = 6;
  ilay = iglo / nofsectors;
  isec = iglo - ilay * nofsectors;
}

std::vector<PndTrkHit *> PndTrkSttHitList::GetHitList()
{

  std::vector<PndTrkHit *> thislist;

  for (size_t ihit = 0; ihit < hitlist.size(); ihit++) {
    PndTrkHit *hit = &hitlist[ihit];
    thislist.push_back(hit);
  }
  return thislist;
}

ClassImp(PndTrkSttHitList)
