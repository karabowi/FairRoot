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
 * PndSttTubeIdMap.h
 *
 *  Created on: 02.08.2018
 *      Author: tstockmanns
 */

#ifndef DETECTORS_STT_PNDSTTTUBEIDMAP_H_
#define DETECTORS_STT_PNDSTTTUBEIDMAP_H_

#include <TObject.h>
#include <vector>
#include <map>
#include <algorithm>

using std::vector;

class PndSttTubeIdMap : public TObject {
 public:
  // Instanton: Constructor is in private
  virtual ~PndSttTubeIdMap();
  static PndSttTubeIdMap *Instance();

  vector<vector<int>> GetSector(int sector) { return fTubeMap[sector]; }
  vector<int> GetRowInSector(int sector, int row) { return fTubeMap[sector][row]; }
  int GetNTubesInRow(int sector, int row) { return fTubeMap[sector][row].size(); }
  int GetNTubes() { return fFlatTubeMap.size(); }
  int GetTube(int sector, int row, int posInRow) { return fTubeMap[sector][row][posInRow]; }
  bool IsSkewed(int row) { return std::find(fSkewedRows.begin(), fSkewedRows.end(), row) != fSkewedRows.end(); }
  void SetSkewedRows(std::vector<int> &values) { fSkewedRows = values; }
  //    vector<int> GetRowInAllSectors(int row);
  int GetTubeIdFlat(int index)
  {
    if (index < fFlatTubeMap.size())
      return fFlatTubeMap[index];
    else
      return -1;
  }

  std::pair<int, int> GetSectorRowFromTubeId(int tubeId)
  {
    if (fMapTubeIdSectorRow.count(tubeId) > 0)
      return fMapTubeIdSectorRow[tubeId];
    else {
      std::pair<int, int> noTubeId(-1, -1);
      return noTubeId;
    }
  }

  void AddTube(int sector, int row, int tubeId);

 private:
  PndSttTubeIdMap();
  static PndSttTubeIdMap *fInstance;
  vector<vector<vector<int>>> fTubeMap; //< [sector][row][posInRow] holds tubeId
  vector<int> fFlatTubeMap;
  std::map<int, std::pair<int, int>> fMapTubeIdSectorRow; //< key tubeId, value sector,row pair
  std::vector<int> fSkewedRows;
  ClassDef(PndSttTubeIdMap, 1);
};

#endif /* DETECTORS_STT_PNDSTTTUBEIDMAP_H_ */
