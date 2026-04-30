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
// -----                     PndRichMirrorSegment header file          -----
// -----               Created 22/07/16 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------

#ifndef PNDRICHMIRRORSEGMENT_H
#define PNDRICHMIRRORSEGMENT_H

#include "FairHit.h"
#include <vector>

class PndRichMirrorSegment {

 public:
  /** Default constructor **/
  PndRichMirrorSegment();

  PndRichMirrorSegment(TVector3 point, TVector3 dims, TVector3 normal);

  /** Copy constructor **/
  PndRichMirrorSegment(const PndRichMirrorSegment &seg)
    : fMiddleFlatMirrorPoint(seg.fMiddleFlatMirrorPoint), fSizeOfFlatMirror(seg.fSizeOfFlatMirror), fNormalOfFlatMirror(seg.fNormalOfFlatMirror)
  {
    *this = seg;
  };

  /** Destructor **/
  virtual ~PndRichMirrorSegment();

  /** Output to screen **/
  virtual void Print(const Option_t *opt = "") const;

  /** Modifiers **/

  /** Accessors **/
  virtual TVector3 GetPoint() { return fMiddleFlatMirrorPoint; }
  virtual TVector3 GetDims() { return fSizeOfFlatMirror; }
  virtual TVector3 GetNormal() { return fNormalOfFlatMirror; }
  void SetPoint(TVector3 point) { fMiddleFlatMirrorPoint = point; }
  void SetDimensions(TVector3 dims) { fSizeOfFlatMirror = dims; }
  void SetNormal(TVector3 normal) { fNormalOfFlatMirror = normal; }

 protected:
  TVector3 fMiddleFlatMirrorPoint;
  TVector3 fSizeOfFlatMirror;
  TVector3 fNormalOfFlatMirror;

  ClassDef(PndRichMirrorSegment, 1)
};

#endif // PNDRICHMIRRORSEGMENT_H
