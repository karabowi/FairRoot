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

/**
 * @class PndGFRecoFit
 * @brief Class to perform a kalman fit via genfit2 within PandaRoot
 *
 * @date 20.04.2022
 * @author Tobias Stockmanns
 */

#pragma once

#include "PndTrack.h"
#include "PndGFRecoHitFactoryCreator.h"

#include "AbsKalmanFitter.h"
#include "MeasurementFactory.h"

#include "eventDisplay/include/EventDisplay.h"

#include "FairGeanePro.h"

class PndGFRecoFit {
 public:
  PndGFRecoFit();
  virtual ~PndGFRecoFit();

  /**
   * @brief Initialization of the fitter
   * @detailed Has to be called in the Init() call of a task to initialize the TClonesArrays
   */
  void Init();

  /**
   * @brief Method to perform the fit of a track
   * @param prefit Track to be fitted including prefit values
   * @param pdgcode Particle hypothesis used for the tracking
   * @return fitted track as PndTrack
   */
  PndTrack *Fit(PndTrack *prefit, int pdgcode);

  /**
   * @brief Set different genfit2 fitters
   * @detailed If no external fitter is set a default fitter is generated
   * @param fitter Fitter to be used
   */
  void SetFitter(genfit::AbsKalmanFitter *fitter) { fFitter = fitter; }

  /**
   * @brief Toggle between initial propagation to primary vertex or to a given length
   * @detailed The start values for the kalman fit are given at the first point of the track
   * This can cause a problem for the propagation. Therefore the start parameters are propagated
   * to a point before the start of the track. This switch selects if the propagation is done
   * to the primary vertex or just by a given distance (SetPropagateDistance() ).
   */
  void SetPropagateToIP(bool val = true) { fPropagateToIP = val; }

  /**
   * @brief Sets the distance the initial parameters are propagated backward along the track
   */
  void SetPropagateDistance(double val) { fPropagateDistance = val; }

  void UseEventDisplay(){ fGFdisplay = genfit::EventDisplay::getInstance(); }

 protected:
  void SetDefaultFitter();

 private:
  genfit::MeasurementFactory<genfit::AbsMeasurement> *fHitFactory = nullptr;
  genfit::AbsKalmanFitter *fFitter = nullptr;
  bool fPropagateToIP = false;
  float fPropagateDistance = 2.0;
  //	bool fPerpPlane = false;
  FairGeanePro *fPro0 = nullptr; // todo: replace Geane propagator with genfit propagator
  genfit::EventDisplay* fGFdisplay = nullptr;

  int fVerbose = 0;
};
