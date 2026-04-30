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

#include "PndPropagator.h"
#include <TGenericClassInfo.h>  // for TGenericClassInfo


PndPropagator::PndPropagator()
    : TNamed("PndPropagator", "Base class for propagators"), fPcaMode(0), fPoint(TVector3(0., 0., 0.)),
      fWire1(TVector3(0., 0., 0.)), fWire2(TVector3(0., 0., 0.)), fDestinationLength(0),
      fVolumeName(""), fVolumeCopyNo(0), fVolumeEnter(kTRUE), fInitialPosition(TVector3(0,0,0)),
      fInitialMomentum(TVector3(0,0,0)), fFinalPosition(TVector3(0,0,0)), fFinalMomentum(TVector3(0,0,0)),
      fPcaOutput(PndProp::PCAOutputStruct())
{}

PndPropagator::PndPropagator(const TString& name, const TString& title)
    : TNamed(name, title), fPcaMode(0), fPoint(TVector3(0., 0., 0.)), fWire1(TVector3(0., 0., 0.)),
      fWire2(TVector3(0., 0., 0.)), fDestinationLength(0), fVolumeName(""), fVolumeCopyNo(0),
      fVolumeEnter(kTRUE), fInitialPosition(TVector3(0,0,0)), fInitialMomentum(TVector3(0,0,0)),
      fFinalPosition(TVector3(0,0,0)), fFinalMomentum(TVector3(0,0,0)), fPcaOutput(PndProp::PCAOutputStruct())
{}

ClassImp(PndPropagator);