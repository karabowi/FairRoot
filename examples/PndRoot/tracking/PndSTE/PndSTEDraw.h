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

/** PndSTEDraw
 *@author Jenny Regina <jenny.regina@physics.uu.se>
 *@created 21.06.2021
 *@version 1.0
 **
 ** PANDA class for drawing the reconstructed tracks during the iterations together with the included hits
 **
 ** Task Level RECO
 **/

#ifndef PndSTEDraw_H_
#define PndSTEDraw_H_

#include "PndRiemannTrack.h"

class TClonesArray;

class TEllipse;
class TCanvas;
class TH1F;

class PndSTEDraw {
 public:
  /** Default Constructor **/
  PndSTEDraw(){};

  /** Default Destructor **/
  virtual ~PndSTEDraw(){

  };

  /** @brief Function to draw tracks together with its hits
   *  @param inputRiemannTrack Input track
   */
  void DrawTrack(PndRiemannTrack inputRiemannTrack);

 private:
  /** @brief Tools for drawing tracks and hits during the fitting **/
  TEllipse *fEllipse;
  TEllipse *fHitEllipse;
  TCanvas *fCanvas;

  ClassDef(PndSTEDraw, 1);
};

#endif /* PndSTEDraw_H_ */
