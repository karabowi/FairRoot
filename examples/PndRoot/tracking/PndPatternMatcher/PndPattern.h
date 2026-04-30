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
 * PndPattern.h
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#ifndef PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERN_H_
#define PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERN_H_

#include <set>
#include <map>
#include <vector>
#include <TVector3.h>

class PndPattern : public TObject {
 public:
  PndPattern();
  virtual ~PndPattern();

  void Clear();

  void SetSectorID(int sectorID) { fsectorID = sectorID; }
  void AddMomentum(TVector3 momentum) { fmomenta.push_back(momentum); }
  void AddMomenta(std::vector<TVector3> momenta);
  void AddTubeID(int tubeID) { ftubeIDs.insert(tubeID); }
  void SetTubeIDs(std::set<int> tubeIDs) { ftubeIDs = tubeIDs; }
  void RaisePatternCount() { fcount++; }
  void SetPatternCount(int count) { fcount = count; }
  bool IsEmpty();

  std::set<int> GetTubeIDs() const { return ftubeIDs; }
  short GetSectorID() const { return fsectorID; }
  int GetCount() const { return fcount; }
  std::vector<TVector3> GetMomenta() const { return fmomenta; }

 private:
  std::set<int> ftubeIDs;
  short fpdgCode;
  short fsectorID;
  int fcount;
  std::vector<TVector3> fmomenta;

  ClassDef(PndPattern, 1);
};

#endif /* PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERN_H_ */
