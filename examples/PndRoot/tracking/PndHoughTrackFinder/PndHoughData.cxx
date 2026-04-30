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
//  PndHoughData
//  Data structure for fast access to STT data
/////////////////////////////////////////////////////////////////

/** PndHoughData
 *@author Anna Alicke <a.alicke@fz-juelich.de>
 *@since 01.08.2020
 *@updated 27.04.2023
 *@version 3.1
 **
 ** PANDA class for fast access to STT data
 ** Task level RECO
 **/

/*
This class is similar to the data structure of the CellTrackFinder.
*/

#include "PndHoughData.h"
//#include "PndSttStrawMap.h"
//#include "PndSttGeometryMap.h"
#include "PndStt2GeoHandler.h"
#include "PndSttHit.h"

#include <FairRootManager.h>

#include "TClonesArray.h"

#include <stdio.h>

using std::cout;
using std::endl;

/**
 * @brief      Constructs a new instance of class PndHoughData.
 *
 * @param[in]  fTubeArray  TClonesArray pointer containing the STT tubes
 */
PndHoughData::PndHoughData(PndStt2GeoHandler *geoH) : fSttGeoH(geoH)
//PndHoughData::PndHoughData(TClonesArray *fTubeArray)
{
  //fStrawMap = new PndSttStrawMap(fTubeArray);
  //fGeometryMap = new PndSttGeometryMap(fTubeArray, 1);
  fPndHoughSpace = new PndHoughSpace();
}

/**
 * @brief      Adds hits to the used data structure for faster access. Data are stored in different maps.
 *
 * @param[in]  hits        TClonesArray pointer containing the hits
 * @param[in]  branchName  The branch name of the hits
 */
void PndHoughData::AddHits(TClonesArray *hits, TString branchName)
{
  FairHit *myHit = nullptr;
  PndSttHit *sttHit = nullptr;

  if (branchName.Contains("skewed", TString::kIgnoreCase)) {
  } else if (branchName == "STTHit") {
    for (int i = 0; i < hits->GetEntries(); i++) {
      myHit = (FairHit *)(hits->At(i));
      sttHit = (PndSttHit *)myHit;
      FairLink link(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), i);
      myHit->SetEntryNr(link);
      sttHit->SetEntryNr(link);
      fHits.push_back(myHit);
      fLinks.push_back(link);
      fMapFairLinktoFairHit[link] = myHit;
      fMapFairLinktoIsochrone[link] = sttHit->GetIsochrone();
      fMapFairLinktoIsochroneError[link] = sttHit->GetIsochroneError();
      if (std::find(fhittedTubes.begin(), fhittedTubes.end(), sttHit->GetTubeID()) != fhittedTubes.end())
        continue;
      fSTTLinks.push_back(link);
      fMapFairLinktoTubeId[link] = sttHit->GetTubeID();
      fMapTubetoHit[sttHit->GetTubeID()] = myHit;
      fMapTubetoLink[sttHit->GetTubeID()] = link;
      fhittedTubes.push_back(sttHit->GetTubeID());
    }
  } else if (branchName == "GEMHit") {
    for (int i = 0; i < hits->GetEntries(); i++) {
      myHit = (FairHit *)(hits->At(i));
      fHits.push_back(myHit);
      FairLink link(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), i);
      myHit->SetEntryNr(link);
      fLinks.push_back(link);
      fGEMLinks.push_back(link);
      fMapFairLinktoFairHit[link] = myHit;
      fMapFairLinktoIsochrone[link] = 0.;
      fMapFairLinktoIsochroneError[link] = 0.;
    }
  } else {
    for (int i = 0; i < hits->GetEntries(); i++) {
      myHit = (FairHit *)(hits->At(i));
      fHits.push_back(myHit);
      FairLink link(-1, FairRootManager::Instance()->GetEntryNr(), FairRootManager::Instance()->GetBranchId(branchName), i);
      myHit->SetEntryNr(link);
      fLinks.push_back(link);
      fMapFairLinktoFairHit[link] = myHit;
      fMapFairLinktoIsochrone[link] = 0.;
      fMapFairLinktoIsochroneError[link] = 0.;
      fMVDHits.push_back(link);
    }
  }
}

/**
 * @brief      Adds a single hit to the used data structure for faster access. Data are stored in different maps.
 *
 * @param[in]  link  The FairLink of the hit.
 */

void PndHoughData::AddHit(FairLink link)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TString branchName = ioman->GetBranchName(link.GetType());

  FairHit *myHit = (FairHit *)ioman->GetCloneOfLinkData(link);
  PndSttHit *sttHit = nullptr;
  myHit->SetEntryNr(link);

  fHits.push_back(myHit);
  fLinks.push_back(link);
  fMapFairLinktoFairHit[link] = myHit;

  if (branchName == "STTHit") {
    sttHit = (PndSttHit *)myHit;
    sttHit->SetEntryNr(link);
    fSTTLinks.push_back(link);
    fMapFairLinktoIsochrone[link] = sttHit->GetIsochrone();
    fMapFairLinktoIsochroneError[link] = sttHit->GetIsochroneError();
    fMapFairLinktoTubeId[link] = sttHit->GetTubeID();
    fMapTubetoHit[sttHit->GetTubeID()] = myHit;
    fhittedTubes.push_back(sttHit->GetTubeID());
  }

  else if (branchName == "GEMHit") {
    fGEMLinks.push_back(link);
    fMapFairLinktoIsochrone[link] = 0.;
    fMapFairLinktoIsochroneError[link] = 0.;
  }

  else {
    fMapFairLinktoIsochrone[link] = 0.;
    fMapFairLinktoIsochroneError[link] = 0.;
    fMVDHits.push_back(link);
  }
}

/**
 * @brief      For all GEM hits all other GEM hits in a certain distance (hier 1.5 cm) are counted and stored as neighbors of the GEM hit
 */
void PndHoughData::CreateGEMNeighborhoodData()
{
  for (int i = 0; i < fGEMLinks.size(); i++) {
    vector<FairLink> tmp;
    tmp.clear();
    FairLink GEMLinki = fGEMLinks[i];
    FairHit *GEMHiti = fMapFairLinktoFairHit[GEMLinki];

    for (int j = 0; j < fGEMLinks.size(); j++) {
      FairLink GEMLinkj = fGEMLinks[j];
      FairHit *GEMHitj = fMapFairLinktoFairHit[GEMLinkj];

      double d = sqrt((GEMHiti->GetX() - GEMHitj->GetX()) * (GEMHiti->GetX() - GEMHitj->GetX()) + (GEMHiti->GetY() - GEMHitj->GetY()) * (GEMHiti->GetY() - GEMHitj->GetY()));

      if (d <= 1.5)
        tmp.push_back(GEMLinkj);
    }

    fGEMNeighbors[GEMLinki] = tmp;
  }
}

/**
 * @brief      For all STT hits all other STT neighbors are counted and stored in a map.
 */
void PndHoughData::CreateSTTNeighborhoodData()
{
  for (int i = 0; i < fSTTLinks.size(); i++) {

    std::vector<int> fdoublehittedTubes;
    fdoublehittedTubes.clear();
    double counter = 0.;

    FairLink STTLinki = fSTTLinks[i];
    int tubeID = fMapFairLinktoTubeId[STTLinki];
    TArrayI neighbors = fSttGeoH->GetNeighborings(tubeID);
    //TArrayI neighbors = fGeometryMap->GetNeighboringsByMap(tubeID);

    for (int j = 0; j < neighbors.GetSize(); ++j) {
      std::vector<int>::iterator ishitted = find(fhittedTubes.begin(), fhittedTubes.end(), neighbors[j]);
      int index = std::distance(fhittedTubes.begin(), ishitted);

      if (find(fhittedTubes.begin(), fhittedTubes.end(), neighbors[j]) != fhittedTubes.end()) {
        if (fdoublehittedTubes.size() == 0 || find(fdoublehittedTubes.begin(), fdoublehittedTubes.end(), neighbors[j]) == fdoublehittedTubes.end()) {
          counter += 1.;
          fdoublehittedTubes.push_back(neighbors[j]);
        }
      }
    }

    fMapNumberhittedSTTNeighbors[STTLinki] = counter;
  }
}

ClassImp(PndHoughData);
