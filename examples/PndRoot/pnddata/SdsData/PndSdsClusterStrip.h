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
 * PndSdsClusterStrip.h
 *
 *  Created on: Dec 15, 2009
 *      Author: stockmanns, kliemt, michel
 */

#ifndef PNDSDSCLUSTERSTRIP_H_
#define PNDSDSCLUSTERSTRIP_H_

#include "PndSdsCluster.h"

class PndSdsClusterStrip : public PndSdsCluster {

  friend std::ostream &operator<<(std::ostream &out, PndSdsClusterStrip &cl)
  {
    std::vector<Int_t> list = cl.GetClusterList();
    out << "Hits in Cluster:" << std::endl;
    for (UInt_t i = 0; i < list.size(); i++)
      out << list[i] << std::endl;
    out << std::endl;
    return out;
  }

 public:
  PndSdsClusterStrip() : fSide(SensorSide::kTOP){};
  PndSdsClusterStrip(Int_t DigiType, std::vector<Int_t> list) : fSide(SensorSide::kTOP) { SetClusterList(DigiType, list); };
  virtual ~PndSdsClusterStrip();

  void SetClusterList(Int_t DigiType, std::vector<Int_t> list, Int_t fileId = -1, Int_t eventId = -1)
  {
    fClusterList = list;
    SetLinks(FairMultiLinkedData(DigiType, list, fileId, eventId));
  }

  SensorSide GetSensorSide() const { return fSide; }
  void SetSensorSide(SensorSide s) { fSide = s; }

 private:
  SensorSide fSide;

  ClassDef(PndSdsClusterStrip, 1);
};

#endif /* PNDSDSCLUSTERSTRIP_H_ */
