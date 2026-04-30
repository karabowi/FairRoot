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

//* $Id: */

// -------------------------------------------------------------------------
// -----                      PndGemCluster header file                -----
// -----                  Created 03/06/2013 by R.Karabowicz           -----
// -------------------------------------------------------------------------

/** PndGemCluster.h
 **@author R.Karabowicz <r.karabowicz@gsi.de>
 **@since 03.06.2013
 **@version 1.0
 **
 ** Data class for first reconstruction stage of the GEM Tracker
 ** Data level: cluster
 **
 **/

#ifndef PNDGEMCLUSTER_H
#define PNDGEMCLUSTER_H 1

#include <vector>

#include "FairTimeStamp.h"

#include "PndDetectorList.h"

#include "TObject.h"

class PndGemCluster : public FairTimeStamp {
  friend std::ostream &operator<<(std::ostream &out, PndGemCluster &cluster)
  {
    out << "PndGemCluster in: " << cluster.GetDetectorId() << "( system = " << cluster.GetSystemId() << ", station = " << cluster.GetStationNr()
        << ", sensor = " << cluster.GetSensorNr() << ", side = " << cluster.GetSide() << "), channelNr = " << cluster.GetChannelNr() << " < from " << cluster.GetClusterBeg()
        << " to " << cluster.GetClusterEnd() << " > charge = " << cluster.GetCharge() << ", cor = " << cluster.GetCor() << ", timestamp " << cluster.GetTimeStamp()
        << ", from Point(s) ";
    std::vector<Int_t> indices = cluster.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      out << indices[i] << "  ";
    }
    out << std::endl;

    return out;
  }

 public:
  /** Default constructor **/
  PndGemCluster();

  /** Constructor from station number, sector number,
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  PndGemCluster(Int_t iDetectorId, Double_t iChannel, Int_t bChannel, Int_t eChannel, std::vector<Int_t> index);
  PndGemCluster(Int_t iDetectorId, Double_t iChannel, Int_t bChannel, Int_t eChannel, Double_t signal, Double_t time, std::vector<Int_t> index);

  /** Destructor **/
  virtual ~PndGemCluster();

  void SetCharge(Double_t iCharge) { fClusterCharge = iCharge; }
  void SetCor(Double_t iCor) { fClusterCor = iCor; }

  void SetClusterBeg(Int_t bChan) { fClusterBeg = bChan; }
  void SetClusterEnd(Int_t eChan) { fClusterEnd = eChan; }

  void AddCharge(Double_t iCharge) { fClusterCharge += iCharge; }

  /** Accessors **/
  Int_t GetDetectorId() const { return fDetectorId; }
  Double_t GetChannelNr() const { return fChannelNr; }

  Int_t GetSystemId() const { return ((fDetectorId & (31 << 27)) >> 27); }
  Int_t GetStationNr() const { return ((fDetectorId & (8191 << 8)) >> 8); }
  Int_t GetSensorNr() const
  { // sensor number within station
    return ((fDetectorId & (3 << 6)) >> 6);
  }
  Int_t GetSide() const { return ((fDetectorId & (1 << 5)) >> 5); } // 0=front, 1=back

  Int_t GetClusterBeg() const { return fClusterBeg; }
  Int_t GetClusterEnd() const { return fClusterEnd; }
  Double_t GetCharge() const { return fClusterCharge; }
  Double_t GetCor() const { return fClusterCor; }

  std::vector<Int_t> GetIndices() const
  {
    std::vector<Int_t> result;
    std::set<FairLink> myLinks = GetLinks();
    for (std::set<FairLink>::iterator it = myLinks.begin(); it != myLinks.end(); it++) {
      result.push_back(it->GetIndex());
    }
    return result;
  }
  Int_t GetNIndices() { return GetNLinks(); }
  Int_t GetIndex(int i = 0) const { return GetLink(i).GetIndex(); }

  void AddIndex(int index) { AddLink(FairLink("GEMDigi", index)); }
  void AddIndex(std::vector<Int_t> index) { SetLinks(FairMultiLinkedData("GEMDigi", index)); }

  virtual bool equal(FairTimeStamp *data)
  {
    PndGemCluster *myCluster = dynamic_cast<PndGemCluster *>(data);
    if (myCluster != nullptr) {
      if (fDetectorId == myCluster->GetDetectorId())
        return kTRUE;
    }
    return false;
  }

  virtual bool operator<(const PndGemCluster &myCluster) const
  {
    if (fDetectorId < myCluster.GetDetectorId())
      return true;
    else if (fDetectorId > myCluster.GetDetectorId())
      return false;
    if (fChannelNr < myCluster.GetChannelNr())
      return true;
    else if (fChannelNr > myCluster.GetChannelNr())
      return false;
    return false;
  }

  virtual bool operator>(const PndGemCluster &myCluster) const
  {
    if (fDetectorId > myCluster.GetDetectorId())
      return true;
    else if (fDetectorId < myCluster.GetDetectorId())
      return false;
    if (fChannelNr > myCluster.GetChannelNr())
      return true;
    else if (fChannelNr < myCluster.GetChannelNr())
      return false;
    return false;
  }

  virtual bool operator==(const PndGemCluster &myCluster) const
  {
    if (fDetectorId == myCluster.GetDetectorId())
      if (fChannelNr == myCluster.GetChannelNr())
        return true;
    return false;
  }

 private:
  Int_t fDetectorId;   // detectorId * 256 + stationId * 16 + sensorId
  Double_t fChannelNr; // channel number

  Int_t fClusterBeg;
  Int_t fClusterEnd;

  Double_t fClusterCharge; // charge in the cluster
  Double_t fClusterCor;    // correlation between clusters

  ClassDef(PndGemCluster, 1);
};

#endif
