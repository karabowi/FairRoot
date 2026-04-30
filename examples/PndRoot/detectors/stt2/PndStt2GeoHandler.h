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

//===============================================================
// PndStt2GeoHandler (singleton class)
//===============================================================
// Class to provide STT geometry info (for geoType 1 and 3):
// usage: add to your task Init function:
// PndStt2GeoHandler *geoH = PndStt2GeoHandler::Instance(par); // par [PndGeoSttPar*] will be used on first access only
// PndSttTube *tube = geoH->GetTube(id);
// map<int, PndSttTube*> fTubeMap = geoH->GetTubeMap();
// TClonesArray *fTubeArray = geohandler->GetTubeArray();
//===============================================================
// contact: K. Goetzen - GSI Darmstadt (k.goetzen@gsi.de)
//===============================================================

#ifndef PNDSTT2GEOHANDLER_H
#define PNDSTT2GEOHANDLER_H

#include <map>
#include "TString.h"
#include "PndGeoSttPar.h"
#include "PndSttTube.h"
#include "PndSttGeometryMap.h"
#include "PndSttStrawMap.h"

#include "TString.h"

typedef std::map<int, PndSttTube*> TmIntTubePtr;

class TClonesArray; 
class PndSttHit;
class PndSttStrawMap;

class PndStt2GeoHandler : public TObject {
public:
  typedef std::vector<int> TvInt;
  typedef std::map<std::pair<int, int>, TvInt> TmPIntVInt;
  typedef std::map<int, int> TmIntInt;
  typedef std::map<TString, int> TmStrInt;

  // fetch singleton instance
  static PndStt2GeoHandler* Instance(PndGeoSttPar *par=nullptr);

  // methods
  bool IsTube(int tubeid)      { return fTubeMap.find(tubeid)!=fTubeMap.end(); }  // check tube exists
  bool IsRow(int sec, int row) { return fTubesInLayer.find({sec,row})!=fTubesInLayer.end(); } // check layer exists
  int GetMaxTubeID()           { return fMaxTubeID; }         // return maximum tube ID
  int GetMaxRow()              { return GetRow(fMaxTubeID); } // return maximum layer 
  double GetDistance(int tubeid1, int tubeid2);  
  void CheckTubeMap(int nlay=1, int ndist=1); // do some printout for correctness checks
  int GetTubeAt(int sec, int lay, int idx);   // return tube ID at position (sec,lay,idx)

  // methods adapted from PndSttMapCreator
  int GetTubeIdFromStr(TString path);            // path/name -> tubeID
  TString GetPathFromTubeId(int id, bool copy);  // tubeID    -> path
  PndSttTube* GetTube(int tubeid) { return fTubeMap[tubeid]; }   // access single tube
  TmIntTubePtr GetTubeMap() { return fTubeMap; }                 // access tube map
  TClonesArray* GetTubeArray();                                  // return TClonesArray of tubes 
  
  // legacy methods adapted from PndSttGeometryMap/PndSttStrawMap
  int GetLayer(int tubeid) { return IsTube(tubeid) ? fTubeMap[tubeid]->GetLayerID() : -1; }
  int GetRow(int tubeid) { return (fGeoType==3) ? GetLayer(tubeid) : fStrawMap->GetRow(tubeid); }
  int GetSector(int tubeid) { return IsTube(tubeid) ? fTubeMap[tubeid]->GetSectorID() : -1; }
  int GetLeftSector(int sector) const  { return ((sector + 1) % 6); } 
  int GetRightSector(int sector) const { return ((sector + 5) % 6); } 
  TArrayI GetNeighborings(int tubeId)  { return (fGeoType==3) ? fTubeMap[tubeId]->GetNeighborings() : fGeoMap->GetNeighboringsByMap(tubeId); }

  TvInt GetStrawRow(int sec, int row);
  bool IsSkewedRow(int row);
  bool IsSkewedStraw(int tubeid) { return IsTube(tubeid) ? fTubeMap[tubeid]->IsSkew() : false; }
  bool IsAxialStraw(int tubeid) { return !IsSkewedStraw(tubeid); }
  double GetAngleBetweenTubes(int tube1, int tube2);
  bool InStraightLine(int tube1, int tube2, int tube3);
  bool IsEdgeStraw(int tubeId);
  double CalculateStrawPoca(int tubeid0, int tubeid1, TVector3 &poca);
  double CalculateStrawPoca(PndSttHit *hit0, PndSttHit *hit1, TVector3 &poca);
  double CalculateStrawPocaLegacy(PndSttHit *hit0, PndSttHit *hit1, TVector3 &poca);
  int FindPhiSector(double phi);
  bool ParallelTubes(int tubeid0, int tubeid1);

private:
  PndStt2GeoHandler(PndGeoSttPar *par=nullptr);
  ~PndStt2GeoHandler() {};

  PndSttTube* CreateTube(Int_t tubeid); // create a single tube
  void FillTubeMap();                   // fill complete tube map
  void FillTubeMapOld();                // fill tupe map (old) and setup PndSttStrawMap/PndSttGeometryMap
  int GetPosID(int sec, int lay, int idx) { return sec*10000 + lay*100 + idx; } // generate unique position ID for fTubeLookup                
  
  static PndStt2GeoHandler* fInstance;
  //int fIsInit;                //! flag whether geometry is initialized
  int fGeoType;               //! geometry type
  int fMaxTubeID;             //! maximum tube ID
  TmIntTubePtr fTubeMap;      //! maps tube ID to PndStt2Tube
  TmPIntVInt fTubesInLayer;   //! for given pair<sector,layer> returns the list of tube IDs
  TmIntInt fTubeLookup;       //! maps unique position id (sec*10000+lay*100+idx) to tube ID
  TmStrInt fPath2TubeID;      //! path to tubeID map
  
  PndGeoSttPar* fSttGeoPar = nullptr;   //! STT geo parameters 
  TClonesArray* fTubeArray = nullptr;   //! tube array as TClonesArray for backward compatibility
  PndSttStrawMap *fStrawMap = nullptr;  //! old STT straw map
  PndSttGeometryMap *fGeoMap = nullptr; //! old STT geometry map
  
  ClassDef(PndStt2GeoHandler, 1)
};

#endif
