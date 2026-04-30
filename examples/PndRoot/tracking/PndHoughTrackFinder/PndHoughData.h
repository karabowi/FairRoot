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
 *@since 29.10.2018
 *@updated 02.02.2021
 *@version 3.0
 **
 ** PANDA class for fast access to STT data
 ** Task level RECO
 **/

/*
This class is similar to the data structure of the CellTrackFinder.
*/

#ifndef PndHoughData_H_
#define PndHoughData_H_

#include <vector>
#include <map>

//#include "PndSttGeometryMap.h"
//#include "PndSttStrawMap.h"
#include "FairHit.h"
#include "PndHoughSpace.h"

class PndStt2GeoHandler;
class TClonesArray;

class PndHoughData {

 public:
  PndHoughData(PndStt2GeoHandler *geoH);
  //PndHoughData(TClonesArray *fTubeArray);

  virtual ~PndHoughData()
  {
    //delete fGeometryMap;
    //delete fPndHoughSpace;
    for (size_t i = 0; i < fHits.size(); ++i)
      delete fHits.at(i);
  }

  /** @brief Adds hits to the used data structure for faster access. Data are stored in different maps. */
  void AddHits(TClonesArray *hits, TString branchName);
  /** @brief Adds a sigle hit to the used data structure for faster access. Data are stored in different maps. */
  void AddHit(FairLink link);
  /** @brief For all GEM hits all other GEM hits in a certain distance (hier 1.5 cm) are counted and stored as neighbors of the GEM hit */
  void CreateGEMNeighborhoodData();
  /** @brief For all STT hits all other STT neighbors are counted and stored in a map. */
  void CreateSTTNeighborhoodData();
  /** @brief Initializes the Hough space */
  void Init(int NBins1 = 450, int NBins2 = 450)
  {
    fNbins1 = NBins1;
    fNbins2 = NBins2;
    fPndHoughSpace->SetNBins1(fNbins1);
    fPndHoughSpace->SetNBins2(fNbins2);
    fPndHoughSpace->Init();
  }
  /** @brief Clears all data maps */
  void clear()
  {
    fHits.clear();
    fLinks.clear();
    fSTTLinks.clear();
    fGEMLinks.clear();
    fhittedTubes.clear();
    fMVDHits.clear();

    fMapFairLinktoFairHit.clear();
    fMapFairLinktoIsochrone.clear();
    fMapFairLinktoIsochroneError.clear();
    fMapFairLinktoTubeId.clear();
    fMapTubetoHit.clear();
    fMapTubetoLink.clear();

    fGEMNeighbors.clear();
    fMapNumberhittedSTTNeighbors.clear();

    fPndHoughSpace->clear();
  }
  /** @brief Returns a vector of all hits in the event. */
  std::vector<FairHit *> GetHits() const { return fHits; }
  /** @brief Returns a vector of all FairLinks in the event. */
  std::vector<FairLink> GetLinks() const { return fLinks; }
  ///** @brief Returns the geometry map for a fast access. */
  //PndSttGeometryMap *GetGeometryMap() const { return fGeometryMap; }
  /** @brief Returns the straw map for a fast access. */
  //PndSttStrawMap *GetStrawMap() const { return fStrawMap; }
  PndStt2GeoHandler *GetSttGeoH() const { return fSttGeoH; }
  
  /** @brief Returns the map linking FairLinks to FairHits. */
  std::map<FairLink, FairHit *> GetMapFairLinktoFairHit() const { return fMapFairLinktoFairHit; }
  /** @brief Returns the map linking FairLinks to isochrone radii. */
  std::map<FairLink, Double_t> GetMapFairLinktoIsochrone() const { return fMapFairLinktoIsochrone; }
  /** @brief Returns the map linking FairLinks to isochrone radius errors. */
  std::map<FairLink, Double_t> GetMapFairLinktoIsochroneError() const { return fMapFairLinktoIsochroneError; }
  /** @brief Returns the map linking FairLinks to corresponding tubeId. */
  std::map<FairLink, Int_t> GetMapFairLinktoTubeId() const { return fMapFairLinktoTubeId; }
  /** @brief Returns the map linking a tube to a FairHit. */
  std::map<int, FairHit *> GetMapTubetoHit() const { return fMapTubetoHit; }
  /** @brief Returns the map linking a tube to a FairLink. */
  std::map<int, FairLink> GetMapTubetoLink() const { return fMapTubetoLink; }
  /** @brief Returns a vector of all hitted tubes */
  std::vector<int> GetHittedTubes() { return fhittedTubes; };
  /** @brief Returns a vector of all GEM hits in a distance d < 1.5 cm from a certain GEM hit */
  std::vector<FairLink> GetGEMNeighbors(FairLink link) { return fGEMNeighbors[link]; };
  /** @brief Returns the number of GEM hits in a distance d < 1.5 cm  */
  int GetNumOfGEMNeighbors(FairLink link) { return fGEMNeighbors[link].size(); };
  /** @brief Returns the number of neighbored STT hits. The return value is a double (not a int) to avoid a necessary type cast in the ghost reduction functon. */
  double GetNumOfSTTNeighbors(FairLink link) { return fMapNumberhittedSTTNeighbors[link]; };
  /** @brief Returns the Hough space. */
  PndHoughSpace *GetHoughSpace() { return fPndHoughSpace; };
  /** @brief Returns all MVD Hits. */
  std::vector<FairLink> GetMVDHits() { return fMVDHits; };
  /** @brief Returns all GEM Hits. */
  std::vector<FairLink> GetGEMHits() { return fGEMLinks; };
  /** @brief Returns all STT Hits. */
  std::vector<FairLink> GetSTTHits() { return fSTTLinks; };

 private:
  //PndSttStrawMap *fStrawMap = nullptr;       // for getting more information about the tubes
  //PndSttGeometryMap *fGeometryMap = nullptr; // for initializing the neighbors of each tube
  PndStt2GeoHandler *fSttGeoH = nullptr;
  PndHoughSpace *fPndHoughSpace = nullptr;

  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, Double_t> fMapFairLinktoIsochrone;
  std::map<FairLink, Double_t> fMapFairLinktoIsochroneError;
  std::map<FairLink, Int_t> fMapFairLinktoTubeId;
  std::map<FairLink, std::vector<FairLink>> fGEMNeighbors;
  std::map<FairLink, double> fMapNumberhittedSTTNeighbors;
  std::map<int, FairHit *> fMapTubetoHit; // maps hitted tube to FairHit
  std::map<int, FairLink> fMapTubetoLink; // maps hitted tube to FairLink

  std::vector<FairHit *> fHits;
  std::vector<FairLink> fGEMLinks;
  std::vector<FairLink> fSTTLinks;
  std::vector<FairLink> fLinks; // vector with selected hits of an event
  std::vector<int> fhittedTubes;
  std::vector<FairLink> fMVDHits;

  int fNbins1;
  int fNbins2;

  ClassDef(PndHoughData, 1);
};

#endif /* PndHoughData_H_ */
