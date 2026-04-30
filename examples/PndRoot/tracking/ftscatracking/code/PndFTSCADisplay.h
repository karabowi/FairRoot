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

//-*- Mode: C++ -*-
// @(#) $Id: PndFTSCADisplay.h,v 1.8 2016/12/16 19:39:31 mpugach Exp $
//  *************************************************************************
//  This file is property of and copyright by the ALICE HLT Project         *
//  ALICE Experiment at CERN, All rights reserved.                          *
//  See cxx source for full Copyright notice                                *
//                                                                          *
//  PndFTSCADisplay class is a debug utility.                            *
//  It is not used in the normal data processing.                           *
//                                                                          *
//***************************************************************************

#ifndef PNDFTSCADISPLAY_H
#define PNDFTSCADISPLAY_H

#include "PndFTSCAParameters.h"
#include "PndFTSPerformanceBase.h"
#include "PndFTSArray.h"
#include "FTSCANPletsV.h"

class PndFTSCAGBTracker;
class PndFTSCATracker;
class PndFTSCATrack;
class PndFTSCATrackParam;
class PndFTSCAParam;
class PndFTSCAPerformance;
// class PndFTSCAMCTrack;
// class PndFTSCALocalMCPoint;
class TCanvas;
class TPad;
class FTSCAHit;
class FTSCAHitV;
class FTSCAHitsV;
class FTSCATracks;

#include "TArc.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TArrow.h"
#include "TBox.h"
#include "TCrown.h"
#include "TMarker.h"
#include "TLatex.h"

#include <vector>
using std::vector;

/**
 * @class PndFTSCADisplay
 */
class PndFTSCADisplay {

 public:
  class PndFTSCADisplayTmpHit;

  static PndFTSCADisplay &Instance();

  PndFTSCADisplay();

  virtual ~PndFTSCADisplay();

  void Init();
  void Update();
  void ClearView();
  void Ask();
  void SetTPC(const PndFTSCAParam &tpcParam);
  void SetGB(const PndFTSCAGBTracker *GBTracker);

  const PndFTSCAGBTracker *GetGB() { return fGB; };
  int GetColor(int i) const;
  int GetColorZ(double z) const;
  int GetColorY(double y) const;
  int GetColorK(double k) const;
  int GetTrackMC(const PndFTSCADisplayTmpHit *vHits, int NHits);

  void DrawTPC();
  void DrawSlice(PndFTSCATracker *slice, bool DrawRows = 0, bool DrawGrid = 1);

  void DrawArc(float x, float y, float r, int Start = 1, Size_t width = 1);
  void DrawPoint(float x, float y, float z, int Start = 1, Size_t width = 1);

  void DrawGBPoint(float x, float y, float z, int Start = 1, Size_t width = 1);
  void DrawGBPoint(float x, float y, float z, float angle = 0, int Start = 1, Size_t width = 1);
  void DrawSliceOutTrackParam(int itr, int color, Size_t width);
  void DrawSliceOutTrack1(int itr, int color, Size_t width);
  void DrawHelix(float p0, float c, float z, float zStart, float z0, float xc, float yc, float r, float b, int color, Size_t width);
  void DrawParticleGlobal(float *param, float q, float tStart, float tEnd, float b, int color = kOrange, Size_t width = 1);
  void DrawParticleGlobal(float *param, float q, float n[4], float b, int color = kOrange, Size_t width = 1);
  void DrawGBLine(float x, float y, float z, float x2, float y2, float z2, int Start = 1, Size_t width = 1, int projection = -1);

  bool DrawTrack(PndFTSCATrackParam t, double Alpha, const PndFTSCADisplayTmpHit *vHits, int NHits, int color = -1, Size_t width = -1, bool pPoint = 0);

  void DrawGBTrack(int itr, int color = -1, int width = -1);
  void DrawRecoTrack(int itr, int color = -1, int width = -1);
  void DrawMCTrack(int itr, int color = -1, int width = -1);

  void HitToGlobal(const FTSCAHit &hit, float &x, float &y, float &z);
  void HitToGlobal(const FTSCAHitV &hit, int iV, float &x, float &y, float &z);

  void DrawGBHits(const FTSCAHitsV &all);

  void DrawGBHits(const FTSCAHits &all);

  void DrawGBPoints();
  void DrawPVHisto(const vector<float> &pvHist, const PndFTSCAParam &param);
  void DrawGBNPlets(const FTSCANPletsV &all);

  void DrawGBNPlets(const FTSCAElementsOnStation<FTSCANPletV> &s);

  void DrawGBTracks(const FTSCATracks &all);

#if !defined(PANDA_FTS)
  void DrawGBTrackFast(const PndFTSCAGBTracker &tracker, int itr, int color = -1);
#endif
  bool DrawTracklet(PndFTSCATrackParam &track, const int *hitstore, int color = -1, int width = -1, bool pPoint = 0);

  //     void DrawGBLinks( const PndFTSCAGBTracker &tracker, int color = -1, Size_t width = -1 );

  void DrawGBHit(const PndFTSCAGBTracker &tracker, int iHit, int color = -1, Size_t width = -1);
  void DrawGBHits(const PndFTSCAGBTracker &tracker, int color = -1, Size_t width = -1, int hitsType = -1);

#if !defined(PANDA_FTS)
  void DrawTrackParam(PndFTSCATrackParam t, int color = 1);
#endif

  void SaveCanvasToFile(TString fileName);

  TPad *CanvasYX() { return fYX; }
  TPad *CanvasZX() { return fZX; }

 protected:
  TCanvas *fCanvas;                  // the canvas
  TPad *fYX,                         // in case of PANDA_FTS fYX states for XY plane
    *fZX, *fZR;                      // two views
  bool fAsk;                         // flag to ask for the pressing key
  const PndFTSCAGBTracker *fGB;      // the Aliglobal tracker
  PndFTSCAPerformance *fPerf;        // Performance class (mc labels etc)
  double fZMin, fZMax, fYMin, fYMax; // view parameters
#ifdef PANDA_FTS
  double fXMin, fXMax;
#else
  double fRInnerMin, fRInnerMax, fROuterMin, fROuterMax, fTPCZMin, fTPCZMax; // view parameters
#endif

  TArc fArc;        // parameters of drawing objects are copied from this members
  TLine fLine;      //!
  TPolyLine fPLine; //!
  TMarker fMarker;  //!
  TBox fBox;        //!
  TCrown fCrown;    //!
  TLatex fLatex;    //!
  TArrow fArrow;

  bool fDrawOnlyRef; // draw only clusters from ref. mc tracks

 private:
  PndFTSCADisplay(const PndFTSCADisplay &);
  PndFTSCADisplay &operator=(const PndFTSCADisplay &);
};

#endif
