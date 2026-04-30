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

#include "PndSTEDraw.h"

#include "FairLogger.h"

#include "TCanvas.h"
#include "TEllipse.h"
#include "TSystem.h"
#include "TLine.h"

using std::cout;
using std::endl;

ClassImp(PndSTEDraw);

void PndSTEDraw::DrawTrack(PndRiemannTrack track)
{

  LOG(debug) << "------ PndSTEDraw::DrawTrack() ------" << endl;

  fCanvas = new TCanvas();
  fCanvas->Range(-50, -50, 50, 50);
  fCanvas->SetCanvasSize(1400, 1400);
  fCanvas->SetWindowSize(1450, 1450);

  fEllipse = new TEllipse(track.orig()[0], track.orig()[1], track.r(), track.r());

  fEllipse->SetX1(track.orig()[0]);
  fEllipse->SetY1(track.orig()[1]);
  fEllipse->SetR1(track.r());
  fEllipse->SetR2(track.r());
  fEllipse->SetFillStyle(4000);
  fEllipse->SetLineWidth(2);
  fEllipse->SetLineColor(kBlue);

  fEllipse->Draw();

  for (int i = 0; i < (int)track.getNumHits(); ++i) {

    PndRiemannHit *hit = track.getHit(i);

    fHitEllipse = new TEllipse(hit->x()[0], hit->x()[1], 0.2, 0.2);
    fHitEllipse->SetFillStyle(4000);
    fHitEllipse->SetLineWidth(2);
    if (sqrt((hit->x()[0]) * (hit->x()[0]) + (hit->x()[1]) * (hit->x()[1])) < 14) {
      fHitEllipse->SetLineColor(kMagenta);
    } else {
      fHitEllipse->SetLineColor(kBlue);
    }
    fHitEllipse->Draw();
  }

  // The lines below is for drawing the outermost layer of the MVD
  // TEllipse *mvd = new TEllipse(0, 0, 13, 13);
  // mvd->SetFillStyle(4000);
  // mvd->SetLineColor(kCyan);
  // mvd->Draw();
  // fCanvas->cd();
  // fCanvas->Modified();

  fCanvas->Update();

  fCanvas->Print();

  gSystem->Sleep(1000);
}