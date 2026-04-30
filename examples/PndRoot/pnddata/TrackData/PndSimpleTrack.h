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
 * PndSimpleTrack.h
 *
 *  Created on: 05.03.2009
 *      Author: everybody
 */

#ifndef PNDSIMPLETRACK_H_
#define PNDSIMPLETRACK_H_

#include "TObject.h"
#include "PndTrackCand.h"
#include "FairTrackPar.h"
#include "FairTimeStamp.h"
#include "TRef.h"
#include "Rtypes.h"

#ifndef __CINT__
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#endif //__CINT__

class PndSimpleTrack : public FairTimeStamp {
 public:
  PndSimpleTrack();
  PndSimpleTrack(const FairTrackPar &par, const PndTrackCand &cand, const double &chi2);

  void Print();

  void SetTrackCand(const PndTrackCand &cand) { fTrackCand = cand; };

  PndTrackCand GetTrackCand() { return fTrackCand; }
  PndTrackCand *GetTrackCandPtr() { return &fTrackCand; }
  FairTrackPar GetParam() { return fTrackParam; }

  virtual bool equal(FairTimeStamp *data) const
  {
    if (!data)
      return false; // dummy
    return false;
  }

  virtual bool operator<(const PndSimpleTrack &myTrack) const
  {
    PndTrackCand myCand = ((PndSimpleTrack)myTrack).GetTrackCand();
    if (fTimeStamp < myCand.GetTimeStamp())
      return true;
    else if (fTimeStamp > myCand.GetTimeStamp())
      return false;
    return false;
  }

  virtual bool operator>(const PndSimpleTrack &myTrack) const
  {
    PndTrackCand myCand = ((PndSimpleTrack)myTrack).GetTrackCand();
    if (fTimeStamp > myCand.GetTimeStamp())
      return true;
    else if (fTimeStamp < myCand.GetTimeStamp())
      return false;
    return false;
  }

  virtual bool operator==(const PndSimpleTrack &myTrack) const
  {
    if (myTrack.fChi2 != fChi2)
      return false; // dummy
    return false;
  }

#ifndef __CINT__ // for BOOST serialization
  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &boost::serialization::base_object<FairTimeStamp>(*this);
    ar &fTrackParam;
    ar &fTrackCand;
    ar &fChi2;
  }
#endif // for BOOST serialization

 private:
  FairTrackPar fTrackParam;
  PndTrackCand fTrackCand;
  Double_t fChi2;

  ClassDef(PndSimpleTrack, 2);
};

#endif /* PNDTRACK_H_ */
