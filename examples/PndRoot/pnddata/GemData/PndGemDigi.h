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
// -----                      PndGemDigi header file                   -----
// -----                  Adapted in 2010  by R. Karabowicz            -----
// -------------------------------------------------------------------------

/** PndGemDigi.h
 **@author R.Karabowicz <r.karabowicz@gsi.de>
 **@since 2010
 **
 ** Data class for digital GEM information
 ** Data level: RAW
 **
 ** The index of the (first) MCPoint having activated this channel
 ** is accessible via GetMcPointIndex().
 **/

#ifndef PNDGEMDIGI_H
#define PNDGEMDIGI_H 1

#include <vector>

#include "FairTimeStamp.h"

#include "PndDetectorList.h"

#include "TObject.h"

class PndGemDigi : public FairTimeStamp {
  friend std::ostream &operator<<(std::ostream &out, PndGemDigi &digi)
  {
    out << "PndGemDigi in: " << digi.GetDetectorId() << "( system = " << digi.GetSystemId() << ", station = " << digi.GetStationNr() << ", sensor = " << digi.GetSensorNr()
        << ", side = " << digi.GetSide() << "), channelNr = " << digi.GetChannelNr() << ", charge = " << digi.GetCharge() << ", cor = " << digi.GetCor() << ", timestamp "
        << digi.GetTimeStamp() << ", from Point(s) ";
    std::vector<Int_t> indices = digi.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      out << indices[i] << "  ";
    }
    out << std::endl;

    return out;
  }

 public:
  /** Default constructor **/
  PndGemDigi();

  /** Constructor from station number, sector number,
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index);
  PndGemDigi(Int_t iDetectorId, Double_t iChannel, Int_t index, Double_t signal, Double_t time);

  /** Destructor **/
  virtual ~PndGemDigi();

  void SetCharge(Double_t iCharge) { fDigiCharge = iCharge; }
  void SetCor(Double_t iCor) { fDigiCor = iCor; }

  void AddCharge(Double_t iCharge) { fDigiCharge += iCharge; }

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

  Double_t GetCharge() const { return fDigiCharge; }
  Double_t GetCor() const { return fDigiCor; }

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

  void AddIndex(int index) { AddLink(FairLink("GEMPoint", index)); }
  void AddIndex(std::vector<Int_t> index) { SetLinks(FairMultiLinkedData("GEMPoint", index)); }

  virtual bool equal(FairTimeStamp *data)
  {
    PndGemDigi *myDigi = dynamic_cast<PndGemDigi *>(data);
    if (myDigi != nullptr) {
      if (fDetectorId == myDigi->GetDetectorId())
        return kTRUE;
    }
    return false;
  }

  virtual bool operator<(const PndGemDigi &myDigi) const
  {
    if (fDetectorId < myDigi.GetDetectorId())
      return true;
    else if (fDetectorId > myDigi.GetDetectorId())
      return false;
    if (fChannelNr < myDigi.GetChannelNr())
      return true;
    else if (fChannelNr > myDigi.GetChannelNr())
      return false;
    return false;
  }

  virtual bool operator>(const PndGemDigi &myDigi) const
  {
    if (fDetectorId > myDigi.GetDetectorId())
      return true;
    else if (fDetectorId < myDigi.GetDetectorId())
      return false;
    if (fChannelNr > myDigi.GetChannelNr())
      return true;
    else if (fChannelNr < myDigi.GetChannelNr())
      return false;
    return false;
  }

  virtual bool operator==(const PndGemDigi &myDigi) const
  {
    if (fDetectorId == myDigi.GetDetectorId())
      if (fChannelNr == myDigi.GetChannelNr())
        return true;
    return false;
  }

 private:
  Int_t fDetectorId;   // detectorId * 256 + stationId * 16 + sensorId
  Double_t fChannelNr; // channel number

  Double_t fDigiCharge; // charge in the digi
  Double_t fDigiCor;    // correlation between digis

  ClassDef(PndGemDigi, 2);
};

#endif
