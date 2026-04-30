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
// -----                     PndRichTSPDHit header file                  -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHTSPDHIT_H
#define PNDRICHTSPDHIT_H

#include "TVector3.h"
#include "FairTimeStamp.h"

class PndRichTSPDHit : public FairTimeStamp {

 public:
  friend std::ostream &operator<<(std::ostream &out, PndRichTSPDHit &hit)
  {
    out << "PndRichTSPDHi in: " << hit.GetSensorId() << ", from Point(s) ";
    std::vector<Int_t> indices = hit.GetIndices();
    for (unsigned int i = 0; i < indices.size(); i++) {
      out << indices[i] << "  ";
    }
    out << std::endl;

    return out;
  }

 public:
  /** Default constructor **/
  PndRichTSPDHit();

  PndRichTSPDHit(Int_t detID, Int_t sensorId, TVector3 pos, TVector3 dpos, Double_t time, Double_t timeThreshold);

  /** Destructor **/
  virtual ~PndRichTSPDHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual Double_t GetTime() { return fTime; }
  virtual Double_t GetTimeThreshold() { return fTimeThreshold; }
  // virtual Int_t GetRefIndex()  {return fRefIndex;}
  // TVector3 GetPosition()	  const { return TVector3(fX, fY, fZ);	  }
  Int_t GetSensorId() const { return fSensorId; }
  std::vector<Int_t> GetIndices() const { return fIndex; }

  void AddIndex(int index)
  {
    fIndex.push_back(index);
    AddLink(FairLink("PndRichPDHit", index));
  }

  void AddIndex(std::vector<Int_t> index)
  {
    fIndex = index;
    AddLinks(FairMultiLinkedData("PndRichPDHit", index));
  }

  virtual bool equal(FairTimeStamp *data)
  {
    PndRichTSPDHit *hit = dynamic_cast<PndRichTSPDHit *>(data);
    if (hit != nullptr) {
      if (fSensorId == hit->GetSensorId())
        return true;
    }
    return false;
  }

  virtual bool operator<(const PndRichTSPDHit &hit) const
  {
    if (fSensorId < hit.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator>(const PndRichTSPDHit &hit) const
  {
    if (fSensorId > hit.GetSensorId())
      return true;
    return false;
  }
  virtual bool operator==(const PndRichTSPDHit &hit) const
  {
    if (fSensorId == hit.GetSensorId())
      return true;
    return false;
  }

 protected:
  std::vector<Int_t> fIndex; // indice of mc points contributing to this digi
  Int_t fDetID;
  Int_t fSensorId;
  TVector3 fPos;
  TVector3 fdPos;
  Double_t fTime, fTimeThreshold;

  ClassDef(PndRichTSPDHit, 1)
};

#endif // PNDRICHTSPDHIT_H
