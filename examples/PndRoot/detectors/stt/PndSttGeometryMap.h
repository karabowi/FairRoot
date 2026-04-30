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

#ifndef PNDSTTGEOMETRYMAP_H
#define PNDSTTGEOMETRYMAP_H

#include <iostream>
#include <map>
#include <vector>
#include "TVector3.h"
#include "TObject.h"
#include "TArrayI.h"

class PndSttTubeParameters;
class PndSttTube;
class PndGeoSttPar;
class PndSttHit;
class FairGeoNode;

class FairHit;

class TClonesArray;

using std::map;
using std::vector;

class PndSttGeometryMap : public TObject {
 public:
  PndSttGeometryMap();
  PndSttGeometryMap(TClonesArray *tubearray, Int_t geoType);
  ~PndSttGeometryMap();
  PndSttGeometryMap(const PndSttGeometryMap &) = delete;
  PndSttGeometryMap &operator=(const PndSttGeometryMap &) = delete;
  // ----------------------------
  void SetGeneralParameters();
  void GenerateStrawMap(Int_t map);
  Bool_t FillGeometryParameters();
  // ----------------------------

  // *** GEO TYPE 1 ***
  void SetGeneralParametersGeoType1();
  void GenerateStrawMapGeoType1(Int_t map);
  Bool_t FillGeometryParametersGeoType1();

  void GenerateStrawMapTubeIDGeoType1();
  void GenerateStrawMapAngleGeoType1();

  int GetRow(int strawindex) const { return fLayerOfStraw.at(strawindex); }
  int GetSector(int strawindex) const { return fSectorOfStraw.at(strawindex); }
  const vector<int> &GetStrawRow(int sector, int row) const { return (fStrawIndex.find(sector))->second.at(row); }
  const vector<vector<int>> &GetStrawSector(int sector) const { return (fStrawIndex.find(sector))->second; }

  bool IsEdgeStraw(int strawindex) const;
  int IsSectorBorderStraw(int strawindex) const;

  bool IsAxialStraw(int strawindex) const { return fAxialStraw.at(strawindex); }
  bool IsSkewedStraw(int strawindex) const { return !(fAxialStraw.at(strawindex)); }

  bool IsAxialRow(int rowindex) const;
  bool IsSkewedRow(int rowindex) const;

  TArrayI FindNeighborings(PndSttTube *tube);
  TArrayI FindNeighborings(int tubeId);

  TArrayI GetNeighboringsByMap(int tubeId);

  void FillStrawNeighborsMap();

  Double_t
  CalculateStrawPoca(PndSttHit *hit1, PndSttHit *hit2, TVector3 &poca); // returns smallest distance between two Stt Straws. poca is set to the point between the two straws.

  double GetAngleBetweenTubes(int tubeID1, int tubeID2) const;

  bool InStraightLine(int tube1, int tube2, int tube3) const;

  // *********************

  void SetVerbose(int ver) { fVerbose = ver; }

 private:
  Int_t fGeoType, fVerbose;

  void GenerateAngles();
  map<int, vector<vector<int>>> fStrawIndex;
  map<int, TArrayI> fStrawNeighbors;
  vector<int> fSectorOfStraw;
  vector<int> fLayerOfStraw;
  vector<bool> fAxialStraw;
  vector<double> fSectorStart;
  vector<double> fSectorEnd;
  bool fStrawMapInitialized;

  // ------------------ SECTORS/LAYERS
  Int_t fNLayers, fNSectors, fNTubes, fNTubes_inner_parallel, fNTubes_outer_parallel, fNTubes_fillup_parallel, fNTubes_skewed, fNLayers_inner_parallel, fNLayers_skew,
    fNLayers_outer_parallel, fNLayers_fillup_parallel;
  Int_t **fStartTube, **fEndTube, **fShift, **fShiftSkew;

  TClonesArray *fTubeArray;

 protected:
  ClassDef(PndSttGeometryMap, 2)
};

#endif
