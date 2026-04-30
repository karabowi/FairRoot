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
// -----                     PndRichPhoton header file                 -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHPHOTON_H
#define PNDRICHPHOTON_H

#include "FairHit.h"
#include "PndRichMirrorSegment.h"
#include "PndRichPDHit.h"
#include "PndRichBarPoint.h"
#include <vector>

class PndRichMirrorSegment;
class PndRichPDHit;

class PndRichPhoton {

 public:
  /** Default constructor **/
  PndRichPhoton();

  PndRichPhoton(Double_t theta, Double_t phi, UInt_t tmask);

  /** Copy constructor **/
  PndRichPhoton(const PndRichPhoton &photon)
    : fPDHit(photon.fPDHit), fDTime(photon.fDTime), fTime(photon.fTime), fHitTime(photon.fHitTime), fTheta(photon.fTheta), fPhi(photon.fPhi), fTMask(photon.fTMask),
      fMirrors(photon.fMirrors), fHitPosition(photon.fHitPosition), fTrackPosition(photon.fTrackPosition), fTrackDirection(photon.fTrackDirection),
      fTrackPositionOld(photon.fTrackPositionOld), fTrackDirectionOld(photon.fTrackDirectionOld), fMirrRefPosition(photon.fMirrRefPosition), fLength(photon.fLength),
      fTrack(photon.fTrack){};
  //{ *this = photon; };

  PndRichPhoton &operator=(const PndRichPhoton &photon)
  {
    if (this != &photon) {
      fPDHit = photon.fPDHit;
      fDTime = photon.fDTime;
      fTime = photon.fTime;
      fHitTime = photon.fHitTime;
      fTheta = photon.fTheta;
      fPhi = photon.fPhi;
      fTMask = photon.fTMask;
      fMirrors = photon.fMirrors;
      fHitPosition = photon.fHitPosition;
      fTrackPosition = photon.fTrackPosition;
      fTrackDirection = photon.fTrackDirection;
      fTrackPositionOld = photon.fTrackPositionOld;
      fTrackDirectionOld = photon.fTrackDirectionOld;
      fMirrRefPosition = photon.fMirrRefPosition;
      fLength = photon.fLength;
      fTrack = photon.fTrack;
    }
    return *this;
  };

  /** Destructor **/
  virtual ~PndRichPhoton();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual bool TrackCalc();
  virtual Double_t GetLength();
  virtual Double_t GetTime();
  virtual Double_t GetTheta();
  virtual Double_t GetPhi();
  virtual UInt_t GetTMask() { return fTMask; }
  virtual std::vector<PndRichMirrorSegment *> GetMirror() { return fMirrors; }
  virtual TVector3 GetHitPos() { return fHitPosition; }
  virtual std::vector<TVector3> GetMirrRefPos() { return fMirrRefPosition; }
  virtual PndRichBarPoint *GetTrack() { return fTrack; }

  void SetLength(Double_t length) { fLength = length; }
  void SetDTime(Double_t time) { fDTime = time; }
  void SetTime(Double_t time) { fTime = time; }
  void SetTheta(Double_t theta) { fTheta = theta; }
  void SetPhi(Double_t phi) { fPhi = phi; }
  void SetPDHit(PndRichPDHit *hit) { fPDHit = hit; }
  void SetHitTime(Double_t hitTime) { fHitTime = hitTime; }
  void SetHitPos(TVector3 hit) { fHitPosition = hit; }
  void SetTrackPos(TVector3 pos) { fTrackPosition = pos; }
  void SetTrackDir(TVector3 dir) { fTrackDirection = dir; }
  void SetTrack(PndRichBarPoint *track) { fTrack = track; }
  void SetMirrRefPos(std::vector<TVector3> pos) { fMirrRefPosition = pos; }
  void SetMirror(std::vector<PndRichMirrorSegment *> mirrors) { fMirrors = mirrors; }

 protected:
  PndRichPDHit *fPDHit;
  Double_t fDTime;
  Double_t fTime;
  Double_t fHitTime;
  Double_t fTheta, fPhi;
  UInt_t fTMask;
  std::vector<PndRichMirrorSegment *> fMirrors;
  TVector3 fHitPosition;
  TVector3 fTrackPosition;
  TVector3 fTrackDirection;
  TVector3 fTrackPositionOld;
  TVector3 fTrackDirectionOld;
  std::vector<TVector3> fMirrRefPosition;
  Double_t fLength;
  PndRichBarPoint *fTrack;

  ClassDef(PndRichPhoton, 1)
};

#endif // PNDRICHPHOTON_H
