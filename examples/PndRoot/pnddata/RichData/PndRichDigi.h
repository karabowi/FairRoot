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

// -------------------------------------------------------------------------
// -----                     PndRichDigi header file                  -----
// -----               Created 05/04/17 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHDIGI_H
#define PNDRICHDIGI_H

#include "TVector3.h"
#include "FairHit.h"
#include "FairTimeStamp.h"

class PndRichDigi : public FairTimeStamp {

 public:
  friend std::ostream &operator<<(std::ostream &out, PndRichDigi &hit)
  {
    out << "PndRichDigi in: " << hit.GetSensorId() << ", from Point(s) ";
    std::vector<Int_t> indices = hit.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      out << indices[i] << "  ";
    }
    out << std::endl;

    return out;
  }

 public:
  /** Default constructor **/
  PndRichDigi();

  PndRichDigi(Int_t index, Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold, Double_t timeStamp);

  PndRichDigi(std::vector<Int_t> index, Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold, Double_t timeStamp);

  /** Copy constructor **/
  /*  PndRichDigi(const PndRichDigi& hit) : FairHit(hit),
    fSensorId(hit.fSensorId),
    fTime(hit.fTime),
    fTimeThreshold(hit.fTimeThreshold)
    { *this = hit; };
  */
  /** Destructor **/
  virtual ~PndRichDigi();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual Double_t GetTime() { return fTime; }
  virtual Double_t GetTimeThreshold() { return fTimeThreshold; }
  virtual Int_t GetRefIndex() { return fRefIndex; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  Int_t GetSensorId() const { return fSensorId; }
  std::vector<Int_t> GetIndices() const { return fIndex; }

  void AddIndex(int index)
  {
    fIndex.push_back(index);
    AddLink(FairLink("PndRichDigi", index));
  }

  void AddIndex(std::vector<Int_t> index)
  {
    fIndex = index;
    AddLinks(FairMultiLinkedData("PndRichDigi", index));
  }

  virtual bool equal(FairTimeStamp *data)
  {
    PndRichDigi *hit = dynamic_cast<PndRichDigi *>(data);
    if (hit != nullptr) {
      if (fSensorId == hit->GetSensorId())
        return true;
    }
    return false;
  }

  virtual bool operator<(const PndRichDigi &hit) const
  {
    if (fSensorId < hit.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator>(const PndRichDigi &hit) const
  {
    if (fSensorId > hit.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator==(const PndRichDigi &hit) const
  {
    if (fSensorId == hit.GetSensorId())
      return true;
    return false;
  }

 protected:
  std::vector<Int_t> fIndex; // indice of mc points contributing to this digi
  Int_t fSensorId;
  Int_t fRefIndex;
  Double_t fX, fY, fZ;
  Double_t fTime, fTimeThreshold;

  ClassDef(PndRichDigi, 1)
};

#endif // PNDRICHDIGI_H
