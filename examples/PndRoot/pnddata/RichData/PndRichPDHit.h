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
// -----                     PndRichPDHit header file                  -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHPDHIT_H
#define PNDRICHPDHIT_H

#include "TVector3.h"
#include "FairHit.h"

class PndRichPDHit : public FairHit {

 public:
  /** Default constructor **/
  PndRichPDHit();

  PndRichPDHit(Int_t index, Int_t detID, Int_t sensorId, TVector3 &pos, TVector3 &dpos, Double_t time, Double_t timeThreshold);

  /** Copy constructor **/
  /*  PndRichPDHit(const PndRichPDHit& hit) : FairHit(hit),
    fSensorId(hit.fSensorId),
    fTime(hit.fTime),
    fTimeThreshold(hit.fTimeThreshold)
    { *this = hit; };
  */
  /** Destructor **/
  virtual ~PndRichPDHit();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual Double_t GetTime() { return fTime; }
  virtual Double_t GetTimeThreshold() { return fTimeThreshold; }
  virtual Int_t GetRefIndex() { return fRefIndex; }
  TVector3 GetPosition() const { return TVector3(fX, fY, fZ); }
  Int_t GetSensorId() const { return fSensorId; }
  //  std::vector<Int_t> GetIndices() const { return fIndex;}

  /*  void AddIndex(int index)
    {
      fIndex.push_back(index);
      AddLink(FairLink("PndRichPDHit", index));
    }

    void AddIndex(std::vector<Int_t> index)
    {
      fIndex = index;
      AddLinks(FairMultiLinkedData("PndRichPDHit", index));
    }

    virtual bool equal(FairTimeStamp* data){
      PndRichPDHit* hit = dynamic_cast <PndRichPDHit*> (data);
      if (hit != 0){
        if (fSensorId == hit->GetSensorId()) return true;
      }
      return false;
    }

    virtual bool operator<(const PndRichPDHit& hit) const{
      if (fSensorId < hit.GetSensorId()) return true;
      return false;
    }
    virtual bool operator>(const PndRichPDHit& hit) const{
      if (fSensorId > hit.GetSensorId()) return true;
      return false;
    }
    virtual bool operator==(const PndRichPDHit& hit) const{
      if (fSensorId == hit.GetSensorId()) return true;
      return false;
    }
  */
 protected:
  //  std::vector<Int_t> fIndex;   // indice of mc points contributing to this digi
  Int_t fIndex;
  Int_t fSensorId;
  Int_t fRefIndex;
  Double_t fX, fY, fZ;
  Double_t fTime, fTimeThreshold;

  ClassDef(PndRichPDHit, 1)
};

#endif // PNDRICHPDHIT_H
