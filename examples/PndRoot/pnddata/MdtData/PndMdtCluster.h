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

#ifndef PNDMDTCLUSTER_HH
#define PNDMDTCLUSTER_HH

#include "FairMultiLinkedData_Interface.h"
#include <vector>

class PndMdtCluster : public FairMultiLinkedData_Interface {

 public:
  PndMdtCluster();
  PndMdtCluster(std::vector<Int_t> list);

  virtual ~PndMdtCluster();

  void SetClusterList(std::vector<Int_t> list) { fDigiList = list; };
  std::vector<Int_t> GetDigiList() const { return fDigiList; }
  Int_t GetClusterSize() const { return fDigiList.size(); }
  Int_t GetDigiIndex(Int_t i) const { return fDigiList[i]; }

  Bool_t DigiBelongsToCluster(Int_t digiIndex);

  /** Output to screen **/
  virtual void Print(const Option_t *opt = nullptr);

  /** Accessors **/
  Short_t GetNDigis() const { return fDigiList.size(); }
  const std::vector<Int_t> &GetDigiList() { return fDigiList; }

 private:
  std::vector<Int_t> fDigiList; // List of MdtDigi indices

  ClassDef(PndMdtCluster, 1);
};

#endif
