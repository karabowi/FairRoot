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

#ifndef PNDHYPCLUSTER_H
#define PNDHYPCLUSTER_H

#include "TObject.h"
#include <vector>
#include <iostream>

#include "PndHypHit.h" // contains SensorSide enumeration

//! PndHypClusterCand.h
//! @author T.Stockmanns <t.stockmanns@fz-juelich.de>
//!
//! \brief Class to store the Digis which belong to one cluster
//! This class holds the information which Digi belongs to the actual cluster.
//! The information is stored in a vector<Int_t> which contains the
//! position of the digi in the TClonesArray where it is stored.
class PndHypCluster : public TObject {
 public:
  PndHypCluster() : fClusterList(), fSide(SensorSide::kTOP){};
  ~PndHypCluster(){};
  PndHypCluster(std::vector<Int_t> list);

  void SetClusterList(std::vector<Int_t> list) { fClusterList = list; }
  std::vector<Int_t> GetClusterList() const { return fClusterList; };
  Int_t GetClusterSize() const { return fClusterList.size(); };
  Int_t GetDigiIndex(Int_t i) const { return fClusterList[i]; }

  SensorSide GetSensorSide() const { return fSide; };
  void SetSensorSide(SensorSide s) { fSide = s; }
  bool DigiBelongsToCluster(Int_t digiIndex);
  void Print();

  friend std::ostream &operator<<(std::ostream &out, PndHypCluster &cl)
  {
    std::vector<Int_t> list = cl.GetClusterList();
    out << "Hits in Cluster:" << std::endl;
    for (unsigned int i = 0; i < list.size(); i++)
      out << list[i] << std::endl;
    out << std::endl;

    return out;
  }

 private:
  std::vector<Int_t> fClusterList;
  SensorSide fSide;

  ClassDef(PndHypCluster, 1);
};

#endif
