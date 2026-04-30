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

// $Id: PndFTSCAGBTracker.cxx,v 1.16 2016/12/22 11:45:25 mpugach Exp $
// **************************************************************************
// This file is property of and copyright by the FIAS PANDA group           *
// PANDA Experiment at FIAS-GSI, All rights reserved.                       *
//                                                                          *
// Primary Authors: Ivan Kisel <I.Kisel@compeng.uni-frankfurt.de>           *
//                  Igor Kulakov <I.Kulakov@gsi.de>                         *
//                  Mykhailo Pugach <M.Pugach@gsi.de>                       *
//                  Maksym Zyzak <M.Zyzak@gsi.de>                                       *
//                  for The FIAS PANDA group .                              *
//                                                                          *
// Permission to use, copy, modify and distribute this software and its     *
// documentation strictly for non-commercial purposes is hereby granted     *
// without fee, provided that the above copyright notice appears in all     *
// copies and that both the copyright notice and this permission notice     *
// appear in the supporting documentation. The authors make no claims       *
// about the suitability of this software for any purpose. It is            *
// provided "as is" without express or implied warranty.                    *
//                                                                          *
//***************************************************************************

#ifdef DRAW
#include "PndFTSCADisplay.h"
#define MAIN_DRAW
#define DRAW_FIT
#define USE_CA_FIT // 25.01.17
// #define DRAW_FIT_LOCAL
#define DRAW_CA
#endif // DRAW

#ifdef STAR_HFT
#define START_FROM_TARGET // how to fit
#elif defined(PANDA_STT) || defined(PANDA_FTS)
#define START_FROM_TARGET
#define USE_CA_FIT // 14.02.17
#else
#endif

#include "PndFTSCAGBTracker.h"
#include "PndFTSCAGBHit.h"
#include "PndFTSCAGBTrack.h"
// #include "PndFTSCATrackParam.h"
#include "PndFTSArray.h"
#include "PndFTSCAMath.h"
#if !defined(PANDA_FTS)
#include "PndFTSCATrackLinearisationVector.h"
#endif
#include "PndFTSCAPerformance.h"
#include "TStopwatch.h"
#include "L1Timer.h"

#include "FTSCATarget.h"
#include "FTSCAHits.h"
#include "FTSCAHitsV.h"
#include "FTSCATracks.h"

#include "Math/SMatrix.h"
#include "TMatrixD.h"
#include "TVectorD.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

// TODO DELL ME!!!
#include "PndFTSCAPerformance.h"
#include "PndFTSCAMCPoint.h"
#include "PndFTSPerformanceBase.h"

bool SINGLE_THREADED = false;

PndFTSCAGBTracker::PndFTSCAGBTracker()
  : fHits(0), fNHits(0), fTrackHits(nullptr), fTracks(nullptr), fNTracks(0), fTime(0), fStatNEvents(0), fSliceTrackerTime(0), fSliceTrackerCpuTime(0), fGTi(),
    fTi(fNFindIterations), fStatGTi(), fStatTi(fNFindIterations)
{
  //*  constructor

  for (int i = 0; i < 20; i++)
    fStatTime[i] = 0;

  fGTi.Add("init  ");
  fGTi.Add("iters ");
  fGTi.Add("tracker");
  fGTi.Add("fitter ");

  fTi.SetNIter(fNFindIterations); // for iterations
  fTi.Add("init  ");

  fTi.Add("1plet ");
  fTi.Add("2plet ");
  fTi.Add("3plet ");
  fTi.Add("4plet ");
  fTi.Add("5plet ");
  fTi.Add("6plet ");
  fTi.Add("convrt");

  fTi.Add("plets ");

  fTi.Add("nghbrs");
  fTi.Add("tracks");
  fTi.Add("merger");
  fTi.Add("finish");

  fStatGTi = fGTi;
  fStatTi = fTi;
}

void PndFTSCAGBTracker::Init()
{
  fNHits = 0;
  fTrackHits = nullptr;
  fTracks = nullptr;
  fNTracks = 0;
  fTime = 0.;
  fStatNEvents = 0;
  fSliceTrackerTime = 0.;
  fSliceTrackerCpuTime = 0.;
  for (int i = 0; i < 20; ++i) {
    fStatTime[i] = 0.;
  }
}

PndFTSCAGBTracker::~PndFTSCAGBTracker()
{
  StartEvent();
}

void PndFTSCAGBTracker::StartEvent()
{
  //* clean up track and hit arrays
  if (fTrackHits)
    delete[] fTrackHits;
  fTrackHits = nullptr;
  if (fTracks)
    delete[] fTracks;
  fTracks = nullptr;
  fNHits = 0;
  fNTracks = 0;
}

void PndFTSCAGBTracker::SetNHits(int nHits)
{
  //* set the number of hits
  fHits.Resize(nHits);
  fNHits = nHits;
}

// void PndFTSCAGBTracker::IdealTrackFinder()
//{
////* Creation of ideal tracks based on hits, which correspond to the MC-points.

////PndFTSCAPerformance* perf = &PndFTSCAPerformance::Instance();

// if (fHits.Size() > 0)
// sort( &(fHits[0]), &(fHits[fHits.Size()-1]), PndFTSCAGBHit::Compare );

// const int NMCTracks = perf->GetMCTracks()->Size();
// vector<vector<int> > hits(NMCTracks+1);

// for(int iH=0; iH<fHits.Size(); iH++)
//{
// int id = fHits[iH].ID();
// int trackId = perf->HitLabel(id).fLab[0];
// for( int k = 1; k < 3 && trackId < 0; k++ )
// trackId = perf->HitLabel(id).fLab[k];
// if ( trackId < 0 ) continue;
// hits[trackId].push_back(iH);
//}

// int nTracks = NMCTracks;

// if(fTracks) delete [] fTracks;
// fTracks = new PndFTSCAGBTrack[nTracks];
// fNTracks = nTracks;

// if(fTrackHits) delete [] fTrackHits;
// fTrackHits = new int[fHits.Size()];

// int curHit = 0;
// int curTr = 0;

// for(int iT=0; iT<NMCTracks; iT++)
//{
// if ( hits[iT].size() < PndFTSCAParameters::MinimumHitsForRecoTrack ) continue;

// int nFirstMC = (*perf->GetMCTracks())[iT].FirstMCPointID();
////21.03 int nMCPoints = (*perf->GetMCTracks())[iT].NMCPoints();
// fTracks[curTr].SetFirstHitRef( curHit );
////begin:mod
////21.03 PndFTSCAMCTrack curMcTrack = perf->MCTrack(curTr);
////21.03 int idmcpoint = curMcTrack.FirstMCPointID();
// PndFTSCALocalMCPoint *points = &((*perf->GetMCPoints()).Data()[nFirstMC]);
// PndFTSCATrackParam mcTrackParam;
//// USING AS INITIAL APPROXIMATION MC-INFO
// mcTrackParam.SetX(points[0].X());
// mcTrackParam.SetY(points[0].Y());
// mcTrackParam.SetTX(points[0].Px()/points[0].Pz());
// mcTrackParam.SetTY(points[0].Py()/points[0].Pz());
// mcTrackParam.SetQP(points[0].QP()); //(1/abs(curMcTrack.P()));
// mcTrackParam.SetZ(points[0].Z());
// fTracks[curTr].SetInnerParam(mcTrackParam);
////fTracks[curTr].SetOuterParam(mcTrackParam);
////end:mod
////fTracks[curTr].SetTrackHitIdsArraySize(hits[iT].size());
// int curStation = -1;
// int nHits = 0;
// for(unsigned int iH=0; iH<hits[iT].size(); iH++)
//{
// int iStation = fHits[ hits[iT][iH] ].IRow();
// if(iStation <= curStation) continue;

// fTrackHits[curHit] = hits[iT][iH];
////fTracks[curTr].SetHitID(iH,hits[iT][iH]);
// curHit++;
// nHits++;
// curStation = iStation;
//}
// fTracks[curTr].SetNHits( nHits );
// curTr++;
//}
// fNTracks = curTr;
//}

void PndFTSCAGBTracker::FitTracks()
{
  //* This function calls either the function FitTrack or FitTrackCA (ifndef USE_CA_FIT) for
  //* fitting tracks and displays information on the screen.
  // std::cout << "fNTracks  " << fNTracks << std::endl;
  cout << "Fitting tracks \n";
  for (int iTr = 0; iTr < fNTracks; iTr += uint_v::Size) {
    int nTracksVector = uint_v::Size;

    if (iTr + uint_v::Size >= fNTracks)
      nTracksVector = fNTracks - iTr;
    //   for(int iTr=0; iTr<fNTracks; iTr+=1)
    //   {
    //     int nTracksVector = 1;

    uint_v::Memory nTrackHits;
    uint_v::Memory memFirstHits;
    nTrackHits = uint_v(Vc::Zero);
    memFirstHits = uint_v(Vc::Zero);

    PndFTSCATrackParam startPoint[uint_v::Size];
    PndFTSCATrackParam endPoint[uint_v::Size];

    for (int iv = 0; iv < nTracksVector; iv++) {
      nTrackHits[iv] = fTracks[iTr + iv].NHits();
      // if we want to consider 6plets starting from the first hit as tracks
      // if (nTrackHits[iv]>6)
      // nTrackHits[iv]=6;
      memFirstHits[iv] = fTracks[iTr + iv].FirstHitRef();
      startPoint[iv] = fTracks[iTr + iv].InnerParam();
      endPoint[iv] = startPoint[iv];
    }

#ifdef DRAW_FIT
    PndFTSCAPerformance *perf = &PndFTSCAPerformance::Instance();

    PndFTSCADisplay::Instance().DrawTPC();
    uint_v nHitsDraw(nTrackHits);

    for (unsigned int ihit = 0; ihit < nHitsDraw.max(); ihit++) {
      for (unsigned int iV = 0; iV < nTracksVector; iV++) {
        if (ihit > nTrackHits[iV] - 1)
          continue;
        const unsigned int jhit = ihit;
        PndFTSCAGBHit &h = fHits[fTrackHits[memFirstHits[iV] + jhit]];
#ifdef DRAW_FIT_LOCAL
        {
          float xLoc, yLoc, zLoc;
          PndFTSCAParameters::GlobalToCALocal(h.X(), h.Y(), h.Z(), h.Angle(), xLoc, yLoc, zLoc);
          PndFTSCADisplay::Instance().DrawGBPoint(xLoc, yLoc, zLoc, kGreen + 1);
        }

        const int iMCP = perf->GetMCPoint(h);
        if (iMCP < 0)
          continue;
        const PndFTSCALocalMCPoint &mcPoint = (*perf->GetMCPoints())[iMCP];
        {
          float xLoc, yLoc, zLoc;
          PndFTSCAParameters::GlobalToCALocal(mcPoint.X(), mcPoint.Y(), mcPoint.Z(), h.Angle(), xLoc, yLoc, zLoc);
          PndFTSCADisplay::Instance().DrawGBPoint(xLoc, yLoc, zLoc, kRed, (Size_t)1);
        }
#else
        PndFTSCADisplay::Instance().DrawGBPoint(h.X(), h.Y(), h.Z(), h.Angle(), kGreen + 1);
#endif
      }
    }

    uint_m mcmask(true);

    foreach_bit(unsigned int iV, mcmask)
    {
      if (mcmask[iV] == 0)
        continue;
      PndFTSCAGBHit &hh = fHits[fTrackHits[memFirstHits[iV]]];

      const PndFTSPerformanceBase::PndFTSCAHitLabel &l = perf->HitLabel(hh.ID());
      const int MCIndex = l.fLab[0];
      if (MCIndex > -1) {
        PndFTSCAMCTrack &mc = (*perf->GetMCTracks())[MCIndex];
        for (int iP = 0; iP < mc.NMCPoints(); iP++) {
          PndFTSCALocalMCPoint mcPoint = (*perf->GetMCPoints())[mc.FirstMCPointID() + iP];
#ifdef DRAW_FIT_LOCAL
          // float xLoc, yLoc, zLoc;
          // PndFTSCAParameters::GlobalToCALocal( mcPoint.X(), mcPoint.Y(), mcPoint.Z(), mcPoint.Angle(), xLoc, yLoc, zLoc );
          // PndFTSCADisplay::Instance().DrawGBPoint( xLoc, yLoc, zLoc, kRed, (Size_t)1);
#else
          double mcX0 = mcPoint.X();
          double mcY0 = mcPoint.Y();
          double mcZ = mcPoint.Z();
          PndFTSCADisplay::Instance().DrawGBPoint((float)mcX0, (float)mcY0, (float)mcZ, kOrange + 1, (Size_t)1);
#endif
        }
      }
    }
    PndFTSCADisplay::Instance().Ask();
#endif

    PndFTSCATrackParamVector vStartPoint;
    PndFTSCATrackParamVector vEndPoint;
    vStartPoint.ConvertTrackParamToVector(startPoint, nTracksVector);
    vEndPoint.ConvertTrackParamToVector(endPoint, nTracksVector);

    float_m active0 = static_cast<float_m>(uint_v(Vc::IndexesFromZero) < nTracksVector);

    uint_v firstHits(memFirstHits);

    float_m fitted = float_m(true);
    fitted &= static_cast<float_m>(static_cast<uint_v>(nTrackHits) > 0);
#ifdef USE_CA_FIT
#ifdef DRAW_FIT1
    // PndFTSCADisplay::Instance().ClearView();
    // PndFTSCADisplay::Instance().SetTPCView();
    // PndFTSCADisplay::Instance().DrawTPC();

    PndFTSCADisplay::Instance().DrawGBPoint(0, 0, 0, kMagenta, 0.5); // target

    PndFTSCADisplay::Instance().Ask();
#endif
    for (unsigned int i = 0; i < 1; i++) {
      vEndPoint = vStartPoint;
      vEndPoint.SetCov(0, 1.f);
      vEndPoint.SetCov(1, 0.f);
      vEndPoint.SetCov(2, 10.f);
      vEndPoint.SetCov(3, 0.f);
      vEndPoint.SetCov(4, 0.f);
      vEndPoint.SetCov(5, 1.f);
      vEndPoint.SetCov(6, 0.f);
      vEndPoint.SetCov(7, 0.f);
      vEndPoint.SetCov(8, 0.f);
      vEndPoint.SetCov(9, 1.f);
      vEndPoint.SetCov(10, 0.f);
      vEndPoint.SetCov(11, 0.f);
      vEndPoint.SetCov(12, 0.f);
      vEndPoint.SetCov(13, 0.f);
      vEndPoint.SetCov(14, 10.f);
      vEndPoint.SetChi2(0.f);
      vEndPoint.SetNDF(-5);

      vEndPoint.SetDirection(true);

      bool init = false;
      if (i == 0)
        init = true;

      fitted &= FitTrackCA(vEndPoint, firstHits, nTrackHits, nTracksVector, active0, 0, init);
      vStartPoint = vEndPoint;
      /*cout<<"End of fit in >> direction\n";
      cout<<vEndPoint<<endl;
      PndFTSCADisplay::Instance().Ask();*/

      vStartPoint.SetCov(0, 1.0f);
      vStartPoint.SetCov(1, 0.f);
      vStartPoint.SetCov(2, 10.0f);
      vStartPoint.SetCov(3, 0.f);
      vStartPoint.SetCov(4, 0.f);
      vStartPoint.SetCov(5, 1.f);
      vStartPoint.SetCov(6, 0.f);
      vStartPoint.SetCov(7, 0.f);
      vStartPoint.SetCov(8, 0.f);
      vStartPoint.SetCov(9, 1.f);
      vStartPoint.SetCov(10, 0.f);
      vStartPoint.SetCov(11, 0.f);
      vStartPoint.SetCov(12, 0.f);
      vStartPoint.SetCov(13, 0.f);
      vStartPoint.SetCov(14, 10.f);
      vStartPoint.SetChi2(0.f);
      vStartPoint.SetNDF(-5);
      vStartPoint.SetDirection(false);

      fitted &= FitTrackCA(vStartPoint, firstHits, nTrackHits, nTracksVector, active0, 1);

      /*cout<<"End of fit in << direction\n";
      cout<<vStartPoint<<endl;
      PndFTSCADisplay::Instance().Ask();*/
    }
#else // USE_CA_FIT
    {
      InitialTrackApproximation(vStartPoint, firstHits, nTrackHits, nTracksVector, active0);
      for (int iTimes = 0; iTimes < 1; iTimes++) {
        vEndPoint = vStartPoint;
        // refit in the forward direction: going from the first hit to the last, mask "fitted" marks with 0 tracks, which are not fitted correctly
        fitted &= FitTrack(vEndPoint, firstHits, nTrackHits, nTracksVector, active0, 0);
#ifdef DRAW_FIT1
        // PndFTSCADisplay::Instance().ClearView();
        // PndFTSCADisplay::Instance().SetTPCView();
        // PndFTSCADisplay::Instance().DrawTPC();

        for (int ihit = 0; ihit < nHitsDraw.max(); ihit++) {
          for (int iV = 0; iV < nTracksVector; iV++) {
            if (ihit > nTrackHits[iV] - 1)
              continue;
            const int jhit = ihit;
            PndFTSCAGBHit &h = fHits[fTrackHits[memFirstHits[iV] + jhit]];
#ifdef DRAW_FIT_LOCAL
            float xLoc, yLoc, zLoc;
            PndFTSCAParameters::GlobalToCALocal(h.X(), h.Y(), h.Z(), h.Angle(), xLoc, yLoc, zLoc);
            PndFTSCADisplay::Instance().DrawGBPoint(xLoc, yLoc, zLoc, iV);
#else
            PndFTSCADisplay::Instance().DrawGBPoint(h.X(), h.Y(), h.Z(), h.Angle(), iV); // TODO kMagenta??
#endif
          }
        }
        // 20.01.17 PndFTSCADisplay::Instance().Ask();
#endif
        vStartPoint = vEndPoint;
        // refit in the backward direction: going from the last hit to the first
        fitted &= FitTrack(vStartPoint, firstHits, nTrackHits, nTracksVector, active0, 1);
#ifdef DRAW_FIT1
        // PndFTSCADisplay::Instance().ClearView();
        // PndFTSCADisplay::Instance().SetTPCView();
        // PndFTSCADisplay::Instance().DrawTPC();

        for (int ihit = 0; ihit < nHitsDraw.max(); ihit++) {
          for (int iV = 0; iV < nTracksVector; iV++) {
            if (ihit > nTrackHits[iV] - 1)
              continue;
            const int jhit = ihit;
            PndFTSCAGBHit &h = fHits[fTrackHits[memFirstHits[iV] + jhit]];
#ifdef DRAW_FIT_LOCAL
            float xLoc, yLoc, zLoc;
            PndFTSCAParameters::GlobalToCALocal(h.X(), h.Y(), h.Z(), h.Angle(), xLoc, yLoc, zLoc);
            PndFTSCADisplay::Instance().DrawGBPoint(xLoc, yLoc, zLoc, iV);
#else
            PndFTSCADisplay::Instance().DrawGBPoint(h.X(), h.Y(), h.Z(), h.Angle(), iV); // TODO kMagenta??
#endif
          }
        }
        // 20.01.17 PndFTSCADisplay::Instance().Ask();
#endif
      }
    }
#endif // USE_CA_FIT
    for (int iV = 0; iV < nTracksVector; iV++) {
      startPoint[iV] = PndFTSCATrackParam(vStartPoint, iV);
      endPoint[iV] = PndFTSCATrackParam(vEndPoint, iV);
      if (!fitted[iV]) {
        startPoint[iV].SetAsInvalid();
        endPoint[iV].SetAsInvalid();
      }
    }
    // cout<<"begin fitted "<<(static_cast<uint_v>(nTrackHits) > 0)<<endl;
    // cout<<"end fitted "<<fitted<<endl;
    for (int iV = 0; iV < nTracksVector; iV++) {
      PndFTSCAGBTrack &trackGB = fTracks[iTr + iV];
      trackGB.SetInnerParam(startPoint[iV]);
      trackGB.SetOuterParam(endPoint[iV]);
      trackGB.SetDeDx(0);
    }
  }
}

// used to check fit in CA
float_m PndFTSCAGBTracker::FitTrackCA(PndFTSCATrackParamVector &t, uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0, bool dir, bool init)
{
  UNUSED_PARAM2(nTracksV, dir); // TODO clean me
  float_m active = active0;

  uint_v NTHits(NTrackHits);
  active &= NTHits >= 3;

  if (active.isEmpty())
    return active;
  //   cout<<"active1 "<<active<<endl;
  NTHits.setZero(static_cast<uint_m>(!active));
  /* //nplets-case
  bool SkipMe;
  for (unsigned int  xxx=0; xxx<42; xxx++){
   SkipMe = false;
   cout<<"############# \n";
   cout<<"#6-plet start from "<<xxx+1<<" hit \n";
   //nplets-case*/
//   active = active0;//dummy-var
#if 1                                         // check tracks
  const unsigned int NHitsMax = NTHits.max(); // for tracks; 6 for sixtiplets
  unsigned int FirstHit = 0;                  // xxx //was 4 check part of the track begining from x-th hit
  // const bool AddLastHit = true;
#else // check tracklets
  //  if (NTHits.max() < 7) return static_cast<float_m>(false);
  const unsigned int NHitsMax = 6;  // check x-lets fit
  const unsigned int FirstHit = 15; // check part of the track begining from x-th hit
  const bool AddLastHit = true;
#endif

  // active &= NTHits >= NHitsMax+FirstHit;
  if (active.isEmpty())
    return active;
  //   cout<<"active2 "<<active<<endl;
  vector<FTSCAHitV> hits(NHitsMax);
  // cout<<"track-begin\n";
  for (unsigned int ihit = FirstHit; (ihit - FirstHit) < NHitsMax; ihit++) {
    const uint_m valid = ihit < NTHits;
    uint_v::Memory id;

    const PndFTSCAGBHit **hs = new const PndFTSCAGBHit *[float_v::Size];
    foreach_bit(unsigned int iV, valid)
    {
      if (dir)
        id[iV] = firstHits[iV] + (NTHits[iV] - static_cast<unsigned int>(1)) - ihit;
      else
        id[iV] = firstHits[iV] + ihit;
      hs[iV] = &(fHits[fTrackHits[id[iV]]]);
    }
    hits[ihit - FirstHit] = FTSCAHitV(hs, uint_v(id), static_cast<float_m>(valid));
    // cout<<"hits[ihit-FirstHit].X0() "<<hits[ihit-FirstHit].X0()<<"hits[ihit-FirstHit].X1() "<<hits[ihit-FirstHit].X1()<<endl;
  }
  // cout<<"track-end\n";
  const FTSCAHitV &hit0 = hits[0];

  // Init magnetic field
  const CAFieldValue &b2 = GetParameters().GetFieldValue(hit0.IStations(), hit0.X1(), hit0.X2(), active);
  const float_v &z2 = GetParameters().GetX0(hit0.IStations(), active);
  t.SetField(0, b2, z2);
  t.SetField(1, b2, z2);

#if (defined(USE_MC_PV) && defined(STAR_HFT))
  const double *pv = PndFTSCAPerformance::Instance().PV(); // dbg
  float xT = pv[0], yT = pv[1], zT = pv[2];
#else
  float xT = 0, yT = 0, zT = 0;
#endif

  fMaxInvMom = 20.f; // 0.1 GeV tracks
#if defined(PANDA_STT)
  FTSCATarget target(xT, yT, zT, 10, 10, fMaxInvMom / 3.f, GetParameters(), 0);
#elif defined(PANDA_FTS)
  FTSCATarget target(xT, yT, zT, 10, 10, fMaxInvMom / 3.f, GetParameters(), 0);
#else
  FTSCATarget target(xT, yT, zT, 0.1, 10, fMaxInvMom / 3.f, GetParameters(), 1);
#endif

#ifdef START_FROM_TARGET // target + hit
  // fMaxInvMom = 1.f; // 0.1 GeV tracks
  // FTSCATarget target( xT, yT, zT, 0.03, 0.03, fMaxInvMom/3.f, GetParameters(), 2 ); // target at 0 with 3 cm error // 0.1 GeV tracks
  //  FTSCATarget target = fTarget;
  // target.SetErrQMom(20/3.f);

  if (init) {
    // no need if mc-init
    t.InitByTarget(target);
    t.SetAngle(hit0.Angle());
    t.InitDirection(hit0.X0(), hit0.X1(), hit0.X2());

    t.SetX(hit0.X1());
    t.SetY(hit0.X2()); // we don't have Y at this point (strip crossing required)
    t.SetZ(hit0.X0());

    //     t.Cov(0) = hit0.Err2X1();
    //     t.Cov(1) = hit0.ErrX12();
    //     t.Cov(2) = hit0.Err2X2();
  }
  float_v qp0 = t.QP(); // good if mc-init
  PndFTSCATrackParamVector buffer;
  for (unsigned int ihit = 0; ihit < NHitsMax; ihit++) {
#elif 0 /* // start from 2 hits initialization                                                                          \
 #if (defined(USE_MC_PV) && defined(STAR_HFT))                                                                          \
   const double *pv = PndFTSCAPerformance::Instance().PV(); // dbg                                                      \
   float xT = pv[0], yT = pv[1], zT = pv[2];                                                                            \
 #else                                                                                                                  \
   float xT = 0, yT = 0, zT = 0;                                                                                        \
 #endif                                                                                                                 \
                                                                                                                      \ \
   const FTSCAHitV& hit1 = hits[1];                                                                                     \
   fMaxInvMom = 1.f; // 0.05 GeV tracks                                                                                 \
   FTSCATarget target( xT, yT, zT, 10e10, 10e10, fMaxInvMom/3.f, GetParameters(), 0 ); // target at 0 with 3 cm error   \
                                                                                                                      \ \
   t.InitByTarget( target );                                                                                            \
   t.SetAngle( hit0.Angle() );                                                                                          \
                                                                                                                      \ \
   float_v xg0,yg0,zg0,xg1,yg1,zg1;                                                                                     \
   PndFTSCAParameters::CALocalToGlobal( hit0.X0(), hit0.X1(), hit0.X2(), hit0.Angle(), xg0, yg0, zg0 );                 \
   PndFTSCAParameters::CALocalToGlobal( hit1.X0(), hit1.X1(), hit1.X2(), hit1.Angle(), xg1, yg1, zg1 );                 \
   float_v dx = xg1-xg0;                                                                                                \
   float_v dy = yg1-yg0;                                                                                                \
   float_v dz = zg1-zg0;                                                                                                \
   const float_v cA = CAMath::Cos( hit0.Angle() );                                                                      \
   const float_v sA = CAMath::Sin( hit0.Angle() );                                                                      \
                                                                                                                      \ \
   t.InitDirection( dx*cA + dy*sA, -dx*sA + dy*cA, dz );                                                                \
   t.SetErr2QPt( fMaxInvMom*fMaxInvMom/9.f );                                                                           \
   t.SetErr2Y( hit0.Err2X1() );                                                                                         \
   t.SetErr2Z( hit0.Err2X2() );                                                                                         \
 */
#else   // start from hit + target

  t.InitByHit(hit0, GetParameters(), fMaxInvMom / 3.f);
  t.InitDirection(hit0.X0(), hit0.X1(), hit0.X2());
  t.AddTarget(target, active);

#endif // START_FROM_TARGET

    active &= float_m(uint_v(ihit) < NTHits);

    const FTSCAHitV &hit = hits[ihit];
    // qp0 = t.QP();
    float_m bufactive(false);
    float_m flashback(false);
    int_v ista(Vc::Zero);
    ista(hit.IsValid()) = hit.IStations();
    // cout<<"ista "<<ista<<endl;
    // if (1)
    if (((ista[0] == 8 || ista[0] == 16 || ista[0] == 24 || ista[0] == 32 || ista[0] == 40) && !dir) ||
        ((ista[0] == 7 || ista[0] == 15 || ista[0] == 23 || ista[0] == 31 || ista[0] == 39) && dir)) {
      flashback = (ista == 8 || ista == 16 || ista == 24 || ista == 32 || ista == 40 || ista == 39 || ista == 31 || ista == 23 || ista == 15 || ista == 7);
      //      flashback = (ista==ista);
      buffer.SetTrackParam(t, flashback);
      bufactive = active;
    }

    if (!(ihit == 0)) {
      if (!init)
        active &= t.Transport(hit, GetParameters(), qp0, active);
      else
        active &= t.Transport(hit, GetParameters(), active);
    }

    // cout<<"hit.IStations() "<<hit.IStations()<<endl;
    // if (1)
    if (((ista[0] == 8 || ista[0] == 16 || ista[0] == 24 || ista[0] == 32 || ista[0] == 40) && !dir) ||
        ((ista[0] == 7 || ista[0] == 15 || ista[0] == 23 || ista[0] == 31 || ista[0] == 39) && dir)) {
      flashback = (abs(t.Err2X()) > 20.f || isnan(t.Err2X())); // here to add IsNAN-condition for Err2X
      // cout<<"flashback "<<flashback<<endl;
      t.SetTrackParam(buffer, flashback);
      t.SetQP(t.QP() / 10.f, flashback);
      active = bufactive;
      t.Transport(hit, GetParameters(), qp0, flashback);
      // t.TransportByLine(hit, GetParameters(), flashback);
#ifdef DRAW_FIT
      {
        float_v xGl, yGl, zGl;
        PndFTSCAParameters::CALocalToGlobal(t.X(), t.Y(), t.Z(), t.Angle(), xGl, yGl, zGl);
        foreach_bit(unsigned int iV, flashback)
        // int iV = 0;
        {
          if (!dir)
            PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kPink, 2.5);
          else
            PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kPink + 2, 2.5);
        }
        // PndFTSCADisplay::Instance().Ask();
      }
#endif
    }
    // cout<<"transport\n"<<t<<endl;
    // t.PrintCovMat();
#ifdef DRAW_FIT
    {
      float_v xGl, yGl, zGl;
      PndFTSCAParameters::CALocalToGlobal(t.X(), t.Y(), t.Z(), t.Angle(), xGl, yGl, zGl);
      foreach_bit(unsigned int iV, active)
      // int iV = 0;
      {

#ifdef DRAW_FIT_LOCAL
        PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kBlue + 2, 0.75);
        cout << t.X()[iV] << " " << t.Y()[iV] << " " << t.Z()[iV] << endl;
#else
        if (!dir)
          PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kGray, 1.25);
        else
          PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kGray + 2, 1.25);
#endif
      }
      // PndFTSCADisplay::Instance().Ask();
    }
#endif

    active &= t.Filter(hit, GetParameters(), active);
    // cout<<"filter\n"<<t<<endl;
    // t.PrintCovMat();
    // for tracks
    /*cout<<t<<endl;
    cout<<"Chi2 "<<t.Chi2()[0]<<" NDF "<<t.NDF()[0]<<" Chi2/NDF "<<double(t.Chi2()[0]/double(t.NDF()[0]))<<endl;
    cout<<"hit ErrX "<<hit.Err2X1()[0]<<" ErrY "<<hit.Err2X2()[0]<<endl;*/
    // for tracks

    /*if (t.NDF()[0]<2 && !SkipMe)
    { //this block defines that we check segments which have less than 6 hits (NDF==1)
      //cout<<"hit.Z() "<<hit.X0()[0]<<" Chi2 "<<t.Chi2()[0]<<" NDF "<<t.NDF()[0]<<" Chi2/NDF "<<double(t.Chi2()[0]/double(t.NDF()[0]))<<" Prob "<<TMath::Prob( t.Chi2()[0],
t.NDF()[0] )<< " x "<<t.X()[0]<<" y "<<t.Y()[0]<<" tx "<<t.Tx()[0]<<" ty "<<t.Ty()[0]<<" qp "<<t.QP()[0]<<endl;
      //cout<<"param ErrX "<<t.Cov(0)[0]<<" ErrY "<<t.Cov(2)[0]<<" ErrTy "<<t.Cov(9)[0]<<endl;
      cout<<t<<endl;
      cout<<"Chi2 "<<t.Chi2()[0]<<" NDF "<<t.NDF()[0]<<" Chi2/NDF "<<double(t.Chi2()[0]/double(t.NDF()[0]))<<endl;
      cout<<"hit ErrX "<<hit.Err2X1()[0]<<" ErrY "<<hit.Err2X2()[0]<<endl;
      if (t.NDF()[0]==1)
      {
// 	  cout<<"@@@backwards@@@ \n";
// 	  for ( unsigned int iihit = NHitsMax-2; iihit > 0; iihit-- ) {
// 	    active &= float_m(uint_v(iihit)>=0);
// 	    const FTSCAHitV& hit1 = hits[iihit];
// 	    active &= t.Transport( hit1, GetParameters(), qp0, active ) ;
// 	    active &= t.Filter( hit1, GetParameters(), active );
// 	    cout<<t<<endl;
// 	    cout<<"Chi2 "<<t.Chi2()[0]<<" NDF "<<t.NDF()[0]<<" Chi2/NDF "<<double(t.Chi2()[0]/double(t.NDF()[0]))<<endl;
// 	    cout<<"hit ErrX "<<hit1.Err2X1()[0]<<" ErrY "<<hit1.Err2X2()[0]<<endl;
// 	  }
// 	  cout<<"@@@fwd@@@ \n";
// 	  for ( unsigned int iihit = 1; iihit <NHitsMax; iihit++ ) {
// 	    active &= float_m(uint_v(iihit)<NTHits);
// 	    const FTSCAHitV& hit1 = hits[iihit];
// 	    active &= t.Transport( hit1, GetParameters(), qp0, active ) ;
// 	    active &= t.Filter( hit1, GetParameters(), active );
// 	    cout<<t<<endl;
// 	    cout<<"Chi2 "<<t.Chi2()[0]<<" NDF "<<t.NDF()[0]<<" Chi2/NDF "<<double(t.Chi2()[0]/double(t.NDF()[0]))<<endl;
// 	    cout<<"hit ErrX "<<hit1.Err2X1()[0]<<" ErrY "<<hit1.Err2X2()[0]<<endl;
// 	  }
  SkipMe=true; break;}
    }*/

#ifdef DRAW_FIT
    {
      float_v xGl, yGl, zGl;
      //       PndFTSCAParameters::CALocalToGlobal(t.X(), t.Y(), t.Z(), t.Angle(), xGl, yGl, zGl);
      // foreach_bit( unsigned int iV, active & hit.IsValid() )
      foreach_bit(unsigned int iV, active)
      // int iV=0;
      {
        float gx, gy, gz;
        hit.GetGlobalCoor(iV, gx, gy, gz);
#ifdef DRAW_FIT_LOCAL
        PndFTSCADisplay::Instance().DrawGBPoint(hit.X0()[iV], hit.X1()[iV], hit.X2()[iV], kGreen + 2, (Size_t)1);
        PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kBlue, 0.75);
        cout << 0.5 * atan(2 * hit.ErrX12() / (hit.Err2X2() - hit.Err2X1()))[iV] << endl;
        cout << hit.X0()[iV] << " " << hit.X1()[iV] << " " << hit.X2()[iV] << " " << t.X()[iV] << " " << t.Y()[iV] << " " << t.Z()[iV] << endl;
#else
        PndFTSCADisplay::Instance().DrawGBPoint(gx, gy, gz, kRed, .75);
        // cout<<"Adding the measurement \n";
        //         cout << hit.X0()[iV] << " " << hit.X1()[iV] << " " << hit.X2()[iV] << " " << t.X()[iV] << " " << t.Y()[iV] << " " << t.Z()[iV] << endl;
        //         std::cout << "tx " << t.Tx()[iV] << " ty " << t.Ty()[iV] << " qp " << t.QP()[iV] << std::endl;
        if (!dir)
          PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kBlue + 2, 1.);
        else
          PndFTSCADisplay::Instance().DrawGBPoint(t.X()[iV], t.Y()[iV], t.Z()[iV], kBlue - 2, 1.);
#endif
        // cout << xGl[iV] << " " << yGl[iV] << " " << t.Z()[iV] << endl;
      }
      // comment out if u want to see step-by-step visualisation for track fitting
      // PndFTSCADisplay::Instance().Ask();
    }

#endif
  }

#ifdef DRAW_FIT
  PndFTSCADisplay::Instance().Ask();
#endif
  // fit bckwd
  /*cout<<"FIT BCKWD \n";
  for ( unsigned int ihit = NHitsMax-2; ihit >= 0; ihit-- )
  {
    const FTSCAHitV& hit = hits[ihit];
    active &= float_m(uint_v(ihit)<NTHits);
    active &= t.Transport( hit, GetParameters(),qp0, active );
    active &= t.Filter( hit, GetParameters(), active );
  }
  //t.InitCovMatrix(target.Err2QMom());
  cout<<"FIT FWD \n";
  //fit fwd
  for ( unsigned int ihit = 1; ihit < NHitsMax; ihit++ )
  {
    const FTSCAHitV& hit = hits[ihit];
    active &= float_m(uint_v(ihit)<NTHits);
    active &= t.Transport( hit, GetParameters(),qp0, active );
    active &= t.Filter( hit, GetParameters(), active );
  }*/
#if !defined(PANDA_FTS)
  t.SetQPt(float_v(1.e-8f), CAMath::Abs(t.QPt()) < 1.e-8f);
#endif
  // nplets-case cin.get();
  // nplets-case }

  float_m ok = active0;

  // if track has infinite partameters or covariances, or negative diagonal elements of Cov. matrix, mark it as fitted uncorrectly
  for (unsigned char i = 0; i < 15; i++)
    ok &= CAMath::Finite(t.Cov(i));
  for (unsigned char i = 0; i < 5; i++)
    ok &= CAMath::Finite(t.Par(i));

  ok &= (t.Cov(0) > Vc::Zero) && (t.Cov(2) > Vc::Zero) && (t.Cov(5) > Vc::Zero) && (t.Cov(9) > Vc::Zero) && (t.Cov(14) > Vc::Zero);
  // t.SetNDF(1);//HACK
  return ok;
}

void PndFTSCAGBTracker::InitialTrackApproximation(PndFTSCATrackParamVector & /*track*/, //[R.K. 9/2018] unused
                                                  uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0)
{
  //*Initial approximation for the track reconstruction by the least-square method.

  uint_v nHits(NTrackHits);
  nHits.setZero(static_cast<uint_m>(!active0));
  // float_m good = nHits > 2;

  unsigned int nHitsMax = nHits.max();

  float_v X(Vc::Zero), Y(Vc::Zero), R(Vc::Zero);
  float_v lx(Vc::Zero), ly(Vc::Zero), lx2(Vc::Zero), ly2(Vc::Zero), lr2(Vc::Zero);
  float_v x(Vc::Zero), y(Vc::Zero), x2(Vc::Zero), y2(Vc::Zero), xy(Vc::Zero), r2(Vc::Zero), xr2(Vc::Zero), yr2(Vc::Zero), r4(Vc::Zero);

  float_v::Memory xH0, yH0, zH0, angle0;
  for (int iV = 0; iV < nTracksV; iV++) {
    if (!(active0[iV]))
      continue;
    PndFTSCAGBHit &h = fHits[fTrackHits[firstHits[iV]]];

    xH0[iV] = h.X();
    yH0[iV] = h.Y();
    zH0[iV] = h.Z();
    angle0[iV] = h.Angle();
  }
  float_v xTmp(xH0);
  float_v yTmp(yH0);
  float_v zTmp(yH0);
  const float_v angleV0(angle0);
  float_v xV0, yV0, zV0(zH0);
  PndFTSCAParameters::GlobalToCALocal(xTmp, yTmp, zV0, angleV0, xV0, yV0, zV0);

  vector<float_v> xV(nHitsMax), yV(nHitsMax), zV(nHitsMax);

  for (unsigned int ihit = 0; ihit < nHitsMax; ihit++) {

    float_m active = static_cast<float_m>(ihit < nHits);
    if (active.isEmpty())
      continue;
    float_v::Memory xH, yH, zH;

    for (int iV = 0; iV < nTracksV; iV++) {
      if (!(active[iV]))
        continue;
      if (ihit > NTrackHits[iV] - 1)
        continue;
      PndFTSCAGBHit &h = fHits[fTrackHits[firstHits[iV] + ihit]];

      xH[iV] = h.X();
      yH[iV] = h.Y();
      zH[iV] = h.Z();
    }

    xTmp.load(xH);
    yTmp.load(yH);
    zTmp.load(zH);

    PndFTSCAParameters::GlobalToCALocal(xTmp, yTmp, zTmp, angleV0, xV[ihit], yV[ihit], zV[ihit]);
  }

#ifdef PANDA_FTS

  //   float_v A0, A1=0.f, A2=0.f, A3=0.f, A4=0.f, A5=0.f, a0, a1=0.f, a2=0.f,
  //   b0, b1=0.f, b2=0.f;
  //   float_v z0, x, y, z, S, w, wz, wS;
  //
  //   int i=NHits-1;
  //   z0 = zV[i];
  //   w = 1.f;
  //   A0 = w;
  //   a0 = w*xV[i];
  //   b0 = w*yV[i];

  //   for( int ihit = 0; ihit < nHitsMax; ihit++)
  //   {
  //     float_m active = static_cast<float_m>( ihit < nHits );
  //     x = xV[i];
  //     y = yV[i];
  //     w = 1.f;
  //     z = zV[i] - z0;
  //     S = Sy[i];
  //     wz = w*z;
  //     wS = w*S;
  //     A0(active) +=w;
  //     A1(active) +=wz;  A2(active) +=wz*z;
  //     A3(active) +=wS;  A4(active) +=wS*z; A5(active) +=wS*S;
  //     a0(active) +=w*x; a1(active) +=wz*x; a2(active) +=wS*x;
  //     b0(active) +=w*y; b1(active) +=wz*y; b2(active) +=wS*y;
  //   }
  //
  //

  //
  //   float_v ANew[6] = {A0, A1, A2, A3, A4, A5};
  //   InvertCholetsky3(ANew);
  //
  //   float_v L, L1;
  //   t.SetX( ANew[0]*a0 + ANew[1]*a1 + ANew[3]*a2 );
  //   t.SetTx( ANew[1]*a0 + ANew[2]*a1 + ANew[4]*a2 );
  //   float_v txtx1 = 1.+t.tx*t.tx;
  //   L    = (ANew[3]*a0 + ANew[4]*a1 + ANew[5]*a2) /(txtx1);
  //   if(fabs(A3[0]) < 1.e-6)
  //   {
  //         ANew[0] = A0;
  //         ANew[1] = A1;
  //         ANew[2] = A2;
  //         InvertCholetsky2(ANew);
  //         t.x = ANew[0]*a0 + ANew[1]*a1;
  //         t.tx =ANew[1]*a0 + ANew[2]*a1;
  //   }
  //
  //   L1 = L*t.tx;
  //   A1 = A1 + A3*L1;
  //   A2 = A2 + ( A4 + A4 + A5*L1 )*L1;
  //   b1+= b2 * L1;
  //   ANew[0] = A0;
  //   ANew[1] = A1;
  //   ANew[2] = A2;
  //   InvertCholetsky2(ANew);
  //
  //   track.SetY(  ANew[0]*b0 + ANew[1]*b1 );
  //   track.SetTy ( ANew[1]*b0 + ANew[2]*b1 );
  //
  //   float_v zeroS = !float_v(fabs(A5) < float_v(1.e-8f));
  //   track.q = -L*c_light_i*rsqrt(txtx1 + t.ty*t.ty);
  //   track.qp = track.qp & zeroS;
  //   track.z = z0;

#else
  for (int ihit = 0; ihit < nHitsMax; ihit++) {
    float_m active = static_cast<float_m>(ihit < nHits);
    if (active.isEmpty())
      continue;

    lx = xV[ihit] - xV0;
    ly = yV[ihit] - yV0;
    lx2 = lx * lx;
    ly2 = ly * ly;
    lr2 = lx2 + ly2;

    x(active) += lx;
    y(active) += ly;
    x2(active) += lx2;
    y2(active) += ly2;
    xy(active) += lx * ly;
    r2(active) += lr2;
    xr2(active) += lx * lr2;
    yr2(active) += ly * lr2;
    r4(active) += lr2 * lr2;
  }

  x /= static_cast<float_v>(nHits);
  y /= static_cast<float_v>(nHits);
  xy /= static_cast<float_v>(nHits);
  x2 /= static_cast<float_v>(nHits);
  y2 /= static_cast<float_v>(nHits);
  xr2 /= static_cast<float_v>(nHits);
  yr2 /= static_cast<float_v>(nHits);
  r2 /= static_cast<float_v>(nHits);
  r4 /= static_cast<float_v>(nHits);

  const float_v Cxx = x2 - x * x;
  const float_v Cxy = xy - x * y;
  const float_v Cyy = y2 - y * y;
  const float_v Cxr2 = xr2 - x * r2;
  const float_v Cyr2 = yr2 - y * r2;
  const float_v Cr2r2 = r4 - r2 * r2;

  const float_v Q1 = Cr2r2 * Cxy - Cxr2 * Cyr2;
  const float_v Q2 = Cr2r2 * (Cxx - Cyy) - Cxr2 * Cxr2 + Cyr2 * Cyr2;
  const float_v Phi = 0.5f * CAMath::ATan2(2.f * Q1, Q2);
  const float_v SinPhi = CAMath::Sin(Phi);
  const float_v CosPhi = CAMath::Cos(Phi);
  const float_v Kappa = (SinPhi * Cxr2 - CosPhi * Cyr2) / Cr2r2;

  const float_v Delta = SinPhi * x - CosPhi * y - Kappa * r2;

  const float_v Rho = 2.f * Kappa / CAMath::Sqrt(float_v(Vc::One) - 4.f * Delta * Kappa);
  const float_v D = 2.f * Delta / (float_v(Vc::One) + CAMath::Sqrt(float_v(Vc::One) - 4.f * Delta * Kappa));

  R = float_v(Vc::One) / Rho;
  X = (D + R) * SinPhi;
  Y = -(D + R) * CosPhi;

  float_v kappa(Vc::Zero);
  kappa(good) = Rho;

  const float_v sinPhi0 = -X * kappa;
  const float_v cosPhi0 = Y * kappa;

  float_v dS(Vc::Zero), dS2(Vc::Zero), Z(Vc::Zero), dSZ(Vc::Zero);
  for (int ihit = 0; ihit < nHitsMax; ihit++) {

    float_m active = static_cast<float_m>(ihit < nHits);
    if (active.isEmpty())
      continue;

    lx(active) = xV[ihit] - xV0;
    ly(active) = yV[ihit] - yV0;

    const float_v ex = cosPhi0;
    const float_v ey = sinPhi0;
    const float_v dx = lx;

    float_v ey1 = kappa * dx + ey;

    // check for intersection with X=x

    float_v ex1 = 1.f - ey1 * ey1;
    ex1(ex1 < float_v(Vc::Zero)) = float_v(Vc::Zero);
    ex1 = CAMath::Sqrt(ex1);
    ex1(ex < Vc::Zero) = -ex1;

    // 27.01 const float_v dx2 = dx * dx;
    const float_v ss = ey + ey1;
    const float_v cc = ex + ex1;

    float_v cci = 1.f / cc;
    cci(CAMath::Abs(cc) < 1.e-8f) = 1.e8f;
    float_v exi = 1.f / ex;
    exi(CAMath::Abs(ex) < 1.e-8f) = 1.e8f;
    float_v ex1i = 1.f / ex1;
    ex1i(CAMath::Abs(ex1) < 1.e-8f) = 1.e8f;
    const float_v tg = ss * cci; // tan((phi1+phi)/2)

    // 27.01 const float_v dy = dx * tg;
    float_v dl = dx * CAMath::Sqrt(1.f + tg * tg);

    dl(cc < Vc::Zero) = -dl;
    const float_v dSin = CAMath::Max(float_v(-1.f), CAMath::Min(float_v(Vc::One), dl * kappa * 0.5f));
    const float_v ds = (CAMath::Abs(kappa) > 1.0e-4f) ? (2.f * CAMath::ASin(dSin) / kappa) : dl;

    dS(active) += ds;
    dS2(active) += ds * ds;
    Z(active) += zV[ihit];
    dSZ(active) += ds * zV[ihit];
  }

  float_v det = Vc::One / (static_cast<float_v>(nHits) * dS2 - dS * dS);
  float_v Z0(Vc::Zero), X0(Vc::Zero), Y0(Vc::Zero), dzds(Vc::Zero);
  Z0(good) = det * (dS2 * Z - dS * dSZ);
  dzds(good) = det * (static_cast<float_v>(nHits) * dSZ - Z * dS);

  float_v signCosPhi0(Vc::One);
  signCosPhi0(cosPhi0 < Vc::Zero) = -Vc::One;

#ifdef DRAW_FIT1
  // PndFTSCADisplay::Instance().ClearView();
  // PndFTSCADisplay::Instance().SetTPCView();
  // PndFTSCADisplay::Instance().DrawTPC();
#ifdef DRAW_FIT_LOCAL
  assert(0); // TODO
#endif
  for (unsigned int ihit = 0; ihit < nHitsMax; ihit++) {
    for (int iV = 0; iV < nTracksV; iV++) {
      float_m active = static_cast<float_m>(ihit < nHits);
      if (!(active[iV]))
        continue;
      if (ihit > NTrackHits[iV] - 1)
        continue;
      PndFTSCAGBHit &h = fHits[fTrackHits[firstHits[iV] + ihit]];
      PndFTSCADisplay::Instance().DrawGBPoint(h.X(), h.Y(), h.Z(), h.Angle(), iV);
    }
  }

  double xCL = (X + xV0)[0];
  double yCL = (Y + yV0)[0];
  double zCL = 0, xC = 0, yC = 0, zC = 0;
  PndFTSCAParameters::CALocalToGlobal(xCL, yCL, zCL, double(angleV0[0]), xC, yC, zC);
  double rad = fabs(R[0]);

  PndFTSCADisplay::Instance().DrawArc(xC, yC, rad, -1, 1);
  PndFTSCADisplay::Instance().DrawGBPoint(xC, yC, 0.f, 0.f, -1);
  PndFTSCADisplay::Instance().Ask();
#endif

  track.SetAngle(angleV0);
  track.SetSinPhi(-sinPhi0); // -1 factor is a correction for direction (should be from inner to outer, but previous formula calculate for fromOuterToInner)
  track.SetSignCosPhi(-signCosPhi0);
  track.SetX(xV0);
  track.SetY(yV0);
  track.SetZ(Z0);
  track.SetDzDs(-dzds);
  track.SetQPt(-kappa / GetParameters().cBz());
#endif
}

float_m PndFTSCAGBTracker::FitTrack(PndFTSCATrackParamVector &t, uint_v &firstHits, uint_v::Memory &NTrackHits, int &nTracksV, float_m active0, bool dir)
{
  //* Fitting of the track by using Kalman Filter,
  //* taking into account changes of his parameters due crossing through the detector material.

#ifdef PANDA_FTS
  UNUSED_PARAM6(t, firstHits, NTrackHits, nTracksV, active0, dir);
  return float_m(true);
#else
  PndFTSCATrackParamVector::PndFTSCATrackFitParam fitPar;

  PndFTSCATrackLinearisationVector l(t);

  bool first = 1;

  t.CalculateFitParameters(fitPar);

  uint_v nHits(NTrackHits);
  nHits.setZero(static_cast<uint_m>(!active0));

  int nHitsMax = nHits.max();

  for (unsigned int ihit = 0; ihit < nHitsMax; ihit++) {

    float_m active = active0 && static_cast<float_m>(ihit < nHits);
    if (active.isEmpty())
      continue;
    float_v::Memory xH, yH, zH, hitAlpha;
    float_v::Memory memXOverX0, memXTimesRho;

    float_v::Memory err2X1h, err2x2h, errX12h;
    int_v::Memory mHitNDF;
#ifdef DRIFT_TUBES
    float_v::Memory rh, isLeftH;
#endif

    for (int iV = 0; iV < nTracksV; iV++) {
      if (!(active[iV]))
        continue;
      if (ihit > NTrackHits[iV] - 1)
        continue;
      const unsigned int jhit = dir ? (NTrackHits[iV] - 1 - ihit) : ihit;
      PndFTSCAGBHit &h = fHits[fTrackHits[firstHits[iV] + jhit]];

      hitAlpha[iV] = h.Angle();

      xH[iV] = h.X();
      yH[iV] = h.Y();
      zH[iV] = h.Z();

      err2X1h[iV] = h.Err2X1();
#ifdef PANDA_FTS // TODO why "-" ?
      errX12h[iV] = -h.ErrX12();
#else
      errX12h[iV] = h.ErrX12();
#endif
      err2x2h[iV] = h.Err2X2();
#ifdef DRIFT_TUBES
      rh[iV] = h.R();
      isLeftH[iV] = h.IsLeft() ? 1.f : 0.f;
#endif
      mHitNDF[iV] = GetParameters().Station(h.IRow()).NDF;
      memXOverX0[iV] = GetParameters().GetXOverX0(h.IRow());
      memXTimesRho[iV] = GetParameters().GetXTimesRho(h.IRow());
    }

    const float_v err2X1(err2X1h), err2x2(err2x2h), errX12(errX12h);
#ifdef DRIFT_TUBES
    const float_v isLeftF(isLeftH), r(rh);
    const float_m isLeft = isLeftF > float_v(Vc::Zero);
#endif
    const int_v hitNDF(mHitNDF);

    float_v xV(xH);
    float_v yV(yH);
    float_v zV(zH);
    const float_v hitAlphaV(hitAlpha);
    const float_v xOverX0(memXOverX0);
    const float_v xTimesRho(memXTimesRho);

#ifdef DRAW_FIT
#ifdef DRAW_FIT_LOCAL
    assert(0); // TODO
#endif
    for (int iV = 0; iV < nTracksV; iV++) {
      if (!active[iV])
        continue;
      PndFTSCADisplay::Instance().DrawGBPoint((float)xV[iV], (float)yV[iV], (float)zV[iV], (int)4, (Size_t)1);
    }
#endif

    const float_m &rotated = t.Rotate(-t.Angle() + hitAlphaV, l, .999f, active);

    active &= rotated;
    if (active.isEmpty())
      continue;

    const float_v x0 = xV, y0 = yV;
    PndFTSCAParameters::GlobalToCALocal(x0, y0, zV, hitAlphaV, xV, yV, zV);

    t.SetAngle(hitAlphaV, active);
    const float_m &transported = t.TransportToX0WithMaterial(xV, l, fitPar, xOverX0, xTimesRho, GetParameters().cBz(), 0.999f, active);

#ifdef DRAW_FIT
#ifdef DRAW_FIT_LOCAL
    assert(0); // TODO
#endif
    float_v xL = t.X(), yL = t.Y(), zL = t.Z(), xGl, yGl, zGl;
    PndFTSCAParameters::CALocalToGlobal(xL, yL, zL, t.Angle(), xGl, yGl, zGl);

    for (int iV = 0; iV < nTracksV; iV++) {
      if (!active[iV])
        continue;
      PndFTSCADisplay::Instance().DrawGBPoint(xGl[iV], yGl[iV], zGl[iV], 2, 1.);
    }
    PndFTSCADisplay::Instance().Ask();
#endif

    active &= transported;
    if (active.isEmpty())
      continue;
    if (first) {
      t.SetCov(0, 10.f, active);
      t.SetCov(1, 0.f, active);
      t.SetCov(2, 10.f, active);
      t.SetCov(3, 0.f, active);
      t.SetCov(4, 0.f, active);
      t.SetCov(5, 1.f, active);
      t.SetCov(6, 0.f, active);
      t.SetCov(7, 0.f, active);
      t.SetCov(8, 0.f, active);
      t.SetCov(9, 10.f, active);
      t.SetCov(10, 0.f, active);
      t.SetCov(11, 0.f, active);
      t.SetCov(12, 0.f, active);
      t.SetCov(13, 0.f, active);
      t.SetCov(14, 10.f, active);
      t.SetChi2(0.f, active);
      t.SetNDF(int_v(-5), static_cast<int_m>(active));
      t.CalculateFitParameters(fitPar);
      t.SetAngle(float_v(hitAlpha));
      first = 0;
    }

    float_v x1 = yV;
#ifdef DRIFT_TUBES
    float_v sinPhi = t.SinPhi();
    float_v xCorr = r - r / (sqrt(1 - sinPhi * sinPhi));
    x1(isLeft) += xCorr;
    x1(!isLeft) -= xCorr;
#endif
    const float_m &filtered = t.FilterWithMaterial(x1, zV, err2X1, errX12, err2x2, 0.999f, active, hitNDF);

    active &= filtered;
    if (active.isEmpty())
      continue;

#ifdef DRAW_FIT
#ifdef DRAW_FIT_LOCAL
    assert(0); // TODO
#endif
    xL = t.X();
    yL = t.Y();
    zL = t.Z();
    PndFTSCAParameters::CALocalToGlobal(xL, yL, zL, t.Angle(), xGl, yGl, zGl);

    for (int iV = 0; iV < nTracksV; iV++) {
      if (!active[iV])
        continue;
      PndFTSCADisplay::Instance().DrawGBPoint(xGl[iV], yGl[iV], zGl[iV], 3, 1.);
    }
    PndFTSCADisplay::Instance().Ask();
#endif
  }
  t.SetQPt(float_v(1.e-8f), CAMath::Abs(t.QPt()) < 1.e-8f);

  float_m ok = active0;

  // if track has infinite partameters or covariances, or negative diagonal elements of Cov. matrix, mark it as fitted uncorrectly
  for (unsigned char i = 0; i < 15; i++)
    ok &= CAMath::Finite(t.Cov(i));
  for (unsigned char i = 0; i < 5; i++)
    ok &= CAMath::Finite(t.Par(i));
  ///  ok = ok && ( t.GetX() > 50 ); //this check is wrong! X could be < 50, when a sector is rotated!
  ok &= (t.Cov(0) > Vc::Zero) && (t.Cov(2) > Vc::Zero) && (t.Cov(5) > Vc::Zero) && (t.Cov(9) > Vc::Zero) && (t.Cov(14) > Vc::Zero);
  ok &= CAMath::Abs(t.SinPhi()) < .999f;

  t.SetSignCosPhi(1.f, ok && static_cast<float_m>(l.CosPhi() >= Vc::Zero));
  t.SetSignCosPhi(-1.f, ok && static_cast<float_m>(l.CosPhi() < Vc::Zero));

  return ok;
#endif
}

void PndFTSCAGBTracker::FindTracks()
{
  //* main tracking routine
  fStatNEvents++;

#ifdef MAIN_DRAW
  PndFTSCAPerformance::Instance().SetTracker(this);
  PndFTSCADisplay::Instance().Init();
  PndFTSCADisplay::Instance().SetGB(this);
  PndFTSCADisplay::Instance().SetTPC(GetParameters());
#endif // MAIN_DRAW

#ifdef MAIN_DRAW

#if defined(PANDA_STT) || defined(PANDA_FTS)
  PndFTSCADisplay::Instance().SetTPC(GetParameters());
  PndFTSCADisplay::Instance().DrawTPC();
  // PndFTSCADisplay::Instance().DrawGBPoints();
  PndFTSCADisplay::Instance().DrawGBHits(*this, kGreen + 2, 0.1, 1);
  PndFTSCADisplay::Instance().SaveCanvasToFile("Hits.pdf");
  PndFTSCADisplay::Instance().Ask();
#endif

// //   PndFTSCADisplay::Instance().DrawGBHits( *this, kRed, 0.2, 2 );
// //   PndFTSCADisplay::Instance().SaveCanvasToFile( "Hits_b.pdf");
// //   PndFTSCADisplay::Instance().Ask();
#endif // MAIN_DRAW

  // cout << " NHits = " << fNHits << endl;

  /// \brief The necessary data is transfered to the track-finder
  /// The necessary data is transfered to the track-finder
  /// Data is structured and saved by track-finders for each sector.
  /// To speed up the process  in each row 2D-grid with the bin size
  /// inversely proportional to the number of hits in the row is introduced.
  /// Hits are sorted by grid bins and for each grid bin 1st  hit is found and saved.
  /// Such data structure allows to quickly   find closest hits to the point with given
  /// coordinates, which is required while neighbours hits are searched and additional
  /// hits are attached to segments.

  //#ifdef USE_IDEAL_TF
  // IdealTrackFinder();
  //#else // USE_IDEAL_TF
  CATrackFinder();
  FitTracks();
#ifndef USE_CA_FIT // fitted in CATrackFinder
  FitTracks();
#endif

  //#endif // USE_IDEAL_TF

#ifdef DRAW_CA
  PndFTSCADisplay &disp = PndFTSCADisplay::Instance();
  for (int i = 0; i < fNTracks; i++) {
    switch (i) {
    case 0: disp.DrawRecoTrack(i, kRed, 2); break;
    case 1: disp.DrawRecoTrack(i, kBlue, 2); break;
    case 2: disp.DrawRecoTrack(i, kOrange, 2); break;
    case 3: disp.DrawRecoTrack(i, kPink, 2); break;
    case 4: disp.DrawRecoTrack(i, kBlack, 2); break;
    default: disp.DrawRecoTrack(i, kGreen, 2); break;
    }
    // disp.Ask();
  }
  disp.Ask();

#endif
}

void PndFTSCAGBTracker::SetHits(std::vector<PndFTSCAGBHit> &hits)
{
  const int NHits2 = hits.size();

  SetNHits(NHits2);
  fFStrips.Resize(NHits2); // create a virtual strip for each hit (currently strips position is not used for FTS, so don't have to fill them)
  fBStrips.Resize(NHits2); // create a virtual strip for each hit

  fHits.Resize(NHits2);
  for (int iH = 0; iH < NHits2; iH++) {
    fHits[iH] = hits[iH];
    fHits[iH].SetFStripP(&fFStrips[iH]);
    fHits[iH].SetBStripP(&fBStrips[iH]);
  }
}

void PndFTSCAGBTracker::ReadSettings(std::istream &in)
{
  //* Read settings from the file
  in >> fParameters;
}

bool PndFTSCAGBTracker::ReadSettingsFromFile(string prefix)
{
  ifstream ifile((prefix).data());
  if (!ifile.is_open())
    return 0;
  ReadSettings(ifile);
  return 1;
}

const int StartStationShift = 1;

// BEGIN@@@@!!!!STT===>>>FTS COMBINATORIAL PART!!!!@@@@@
// ATTENTION LAST VERSION OF STT_CA_TRACK_FINDER FROM PANDAROOT_TRUNK
/*
 */
void PndFTSCAGBTracker::CATrackFinder()
{

#ifdef DRAW_CA
  PndFTSCADisplay::Instance().SetTPC(GetParameters());
  // PndFTSCADisplay::Instance().SetTPCView();
  PndFTSCADisplay::Instance().ClearView();
#endif
  // prepare memory for tracks
  if (fTracks)
    delete[] fTracks;
  if (fTrackHits)
    delete[] fTrackHits;
  fTracks = new PndFTSCAGBTrack[5000];                           // TODO
  fTrackHits = new int[3000 * PndFTSCAParameters::MaxNStations]; // TODO
  fNTracks = 0;

  // std::sort( fHits.Data(), fHits.Data() + fNHits, PndFTSCAGBHit::Compare ); to make convertion faster
  FTSCAHits hits(NStations(), fHits.Size() / NStations()); // suppose approximately equal nHits per station

  // convert hits
  for (int iH = 0; iH < fNHits; ++iH) {
    hits.Add(FTSCAHit(fHits[iH], iH));
  }

  hits.Sort();

  // estimate PV
  float xT = 0, yT = 0, zT = 0;

  /// Find Tracks
  // set parameters; TODO depend on iteration
  //  const float kCorr = 4.;//1;//.2; // correction on pulls width
  const float kCorr = 2.;
  const float kCorr2 = kCorr * kCorr;
  fPick_m = 3. * kCorr;          // 3.*kCorr;
  fPick_r = 5. * kCorr;          // 5.*kCorr;
  fPickNeighbour = 7. * kCorr;   // 3.*kCorr; 7*kCorr
  TRACK_PROB_CUT = 0.01;         // 0.01;
  TRACK_CHI2_CUT = 10. * kCorr2; // 10.*kCorr2;
  //  TRIPLET_CHI2_CUT = 15.*kCorr2; // TMath::Prob(20,-3+1+6) = TMath::Prob(15,2) = 5e-04
  TRIPLET_CHI2_CUT = 20. * kCorr2; // TMath::Prob(20,-3+1+6) = TMath::Prob(15,2) = 5e-04

  // Set correction in order to take into account overlaping
  // The reason is that low momentum tracks are too curved and goes not from target direction. That's why hit sort is not work idealy
  fMaxDX0 = 0;

  //  fMaxInvMom = 2;
  fMaxInvMom = 5;

  //  fTarget = FTSCATarget( xT, yT, zT, 10, 10, fMaxInvMom/3.f, GetParameters(), 0 );
  fTarget = FTSCATarget(xT, yT, zT, 10, 10, fMaxInvMom, GetParameters(), 0);

  // cout<<"MaxCellLength = "<<PndCAParameters::MaxCellLength<<endl;

  // reconstruct
  int maxCellLength = PndFTSCAParameters::MaxCellLength;
  // maxCellLength = 6;

#ifdef DRAW_CA
  PndFTSCADisplay::Instance().ClearView();
  PndFTSCADisplay::Instance().DrawGBHits(hits);
  PndFTSCADisplay::Instance().SaveCanvasToFile("DrawHits.pdf");
  PndFTSCADisplay::Instance().Ask();
#endif

  FTSCANPletsV pletsV(NStations(), &hits);

  int iMin = (maxCellLength < PndFTSCAParameters::LastCellLength) ? maxCellLength : PndFTSCAParameters::LastCellLength;

  fPick = fPick_r;
  FTSCATracks tracks(&hits);

  for (char iS = 0; iS <= NStations() - iMin; iS += StartStationShift)

  {
    CreateNPlets(fTarget, hits, pletsV.OnStation(iS), iS, maxCellLength - 1);
  }

#ifdef DRAW_CA1
  PndFTSCADisplay::Instance().DrawGBNPlets(pletsV);
  PndFTSCADisplay::Instance().SaveCanvasToFile("DrawCells.pdf");
  PndFTSCADisplay::Instance().Ask();
#endif

  FTSCANPlets triplets(pletsV);

  fPick = fPickNeighbour;
  FindNeighbours(triplets);

  CreateTracks(triplets, tracks);

  Merge(tracks);

  // save tracks in compact format
  int curHit = 0; // counters, used to save tracks in output format
  int curTr = 0;

  const int NRTracks = tracks.size();

  for (int iT = 0; iT < NRTracks; iT++) {
    const int NTHits = tracks[iT].NHits();
    PndFTSCAGBTrack &oT = fTracks[curTr];
    oT.SetNHits(NTHits);
    // cout<<"NTHits "<<NTHits<<endl;
    oT.SetFirstHitRef(curHit);
#ifdef USE_CA_FIT
    // oT.SetOuterParam( tracks[iT].Fit( hits, fTarget, GetParameters(), false ) );
    // oT.SetInnerParam( tracks[iT].Fit( hits, fTarget, GetParameters(), true ) );
    // InnerParam turns out to be full of nan's for long tracks
    // oT.SetInnerParam( tracks[iT].Fit2Times( hits, fTarget, GetParameters(), false ) );
#endif
    for (int iH = 0; iH < NTHits; iH++) {
      fTrackHits[curHit] = tracks.Hit(iH, iT).Id();
      curHit++;
    }
    curTr++;
  }
  fNTracks += NRTracks;

  hits.Clean(); // remove used hits
}

struct TrackHitRecord {
  int fPrevHit; // index of previous TrackHitRecord of same track in an array of records
  unsigned short fIHit;
  char fStation;
};

vector<TrackHitRecord> gTrackHitRecords;

void PndFTSCAGBTracker::Create1Plets(const FTSCATarget &target, const FTSCAHits &hits, FTSCAElementsOnStation<FTSCANPletV> &r, int iS)
{
  const FTSCAElementsOnStation<FTSCAHit> &hs = hits.OnStation(iS);

  r.clear();
  r.reserve(hs.size() / float_v::Size + 1);

  TrackHitRecord hitRecord;
  hitRecord.fPrevHit = -1;
  hitRecord.fStation = iS;

  // no vectorization for hits...
  for (unsigned int iH = 0; iH < hs.size(); iH += 1) // check with 2508
  {
    const FTSCAHit &hit_t = hs[iH];
    int ISta = (int)hit_t.IStation();

    // const PndFTSCAGBHit &hit_1 = fHits[hit_t.Id()];
    // float_v Tx_1 = float_v(hit_1.point_Px/hit_1.point_Pz);

    float_m valid = static_cast<float_m>(uint_v::IndexesFromZero() < uint_v(hs.size() - iH));

    FTSCAHitV hit(&(hs[iH]), valid);
    PndFTSCATrackParamVector param;
    float_m active = hit.IsValid();

    if (ISta < 32) {
      // start from target
      param.InitByTarget(target);
      float_v r0(10.f);
      float_v r1(10.f);
      float_v r2(10.f);
      r0(active) = hit.X0() - target.X0();
      r1(active) = hit.X1() - target.X1();
      r2(active) = hit.X2() - target.X2();
      param.InitDirection(r0, r1, r2);
      // param.InitDirection( hit.X0() - target.X0(), hit.X1() - target.X1(), hit.X2() - target.X2() );

      param.SetAngle(hit.Angle());
      active &= param.Transport(hit, GetParameters(), active /*float_m(true)*/);
      //	active &= param.Filter( hit, GetParameters(), active );
    } else {
      param.SetAngle(hit.Angle());
      param.InitByHit(hit, GetParameters(), target.DQMom());
      param.SetTx(0.f);
    }

    if (active.isEmpty())
      continue;
    uint_v iHit = uint_v::IndexesFromZero() + iH;
    r.resize(r.size() + 1);
    FTSCANPletV &nPlet = r.back();
    nPlet = FTSCANPletV(iHit, int_v(hit.IStation()), param, active);

    nPlet.fNHits = 1;
    // nPlet.fParam=param;
    // nPlet.fIsValid = active;
    nPlet.fLastHit = -1;
    foreach_bit(unsigned int iBit, active)
    {
      hitRecord.fIHit = iHit[iBit];
      nPlet.fLastHit[iBit] = gTrackHitRecords.size();
      gTrackHitRecords.push_back(hitRecord);
    }
  }
  // cout<<"singlets on stat "<<iS<<" size "<<r.size()<<endl;
}

void PndFTSCAGBTracker::CreateNPlets(const FTSCATarget &target, const FTSCAHits &hits, FTSCAElementsOnStation<FTSCANPletV> &nPlets, int iS, int cellLength)
{
  FTSCAElementsOnStation<FTSCANPletV> tmp0;
  FTSCAElementsOnStation<FTSCANPletV> tmp1;
  tmp0.fHitsRef = &hits;
  tmp0.fISta = iS;
  tmp1.fHitsRef = &hits;
  tmp1.fISta = iS;

  FTSCAElementsOnStation<FTSCANPletV> *rCurr = &tmp0;
  FTSCAElementsOnStation<FTSCANPletV> *rNext = &tmp1;

  Create1Plets(target, hits, *rCurr, iS);

// draw singlets
#ifdef MAIN_DRAW1
  PndFTSCADisplay::Instance().DrawGBNPlets(*rCurr);
  PndFTSCADisplay::Instance().Ask();
#endif

  for (int iLen = 1; iLen <= cellLength; iLen++) {
    if (rCurr->size() <= 0)
      break;
    if ((*rCurr)[0].N() >= GetParameters().Station(iS).CellLength)
      break; // track-segment size check
    if ((rCurr->HitsRef())->OnStationConst(iS).size() == 0)
      continue; // to take into account gapped tracks
    rNext->clear();
    // cout << " iS+iLen  " << (int) (iS+iLen) << endl;
    PickUpHits(*rCurr, *rNext, iS + iLen);
    FTSCAElementsOnStation<FTSCANPletV> *rr = rCurr;
    rCurr = rNext;
// draw cells
#ifdef MAIN_DRAW1
    PndFTSCADisplay::Instance().DrawGBNPlets(*rr);
    PndFTSCADisplay::Instance().Ask();
#endif
    rNext = rr;
  }
  nPlets = *rCurr;
}

void PndFTSCAGBTracker::PickUpHits(FTSCAElementsOnStation<FTSCANPletV> &curr, FTSCAElementsOnStation<FTSCANPletV> &next, int iS)
{
  next.SetStation(curr.IStation());
  next.clear();

  if (curr.size() <= 0)
    return;

  next.reserve(5 * curr.size());

  const float_v Pick2 = fPick * fPick;
  const FTSCAHits *allHits = curr.HitsRef();
  const FTSCAElementsOnStation<FTSCAHit> &hits = allHits->OnStationConst(iS);
  // ATTENTION the hit-size is doubled because of lr-division
  // therefore twice more than necessary track-segments are created

  for (unsigned int iD1 = 0; iD1 < curr.size(); ++iD1) {
    FTSCANPletV &D1 = curr[iD1];
    float_m valid1G = D1.IsValid();
    if (valid1G.isEmpty())
      continue;
    // mod:begin
    /*PndFTSCATrackParamVector paramTransp;
    paramTransp = D1.ParamRef();
    float_m activeTransp = valid1G;
    activeTransp &= paramTransp.Transport( hits[0], GetParameters(), valid1G );*/
    // mod:end
    for (unsigned int ih = 0; ih < hits.size(); ih++) {
      const FTSCAHit &hit = hits[ih];
      float_m active = valid1G;
      // 27.09 float_m active = activeTransp;
      PndFTSCATrackParamVector param;
      param = D1.ParamRef();
      // 27.09 param = paramTransp;
      // init by hit
      if (param.Tx()[0] == 0.f) {
        param.Tx() = (hit.X1() - param.X()) / (hit.X0() - param.Z());
      }
      // cout<<"D1.param \n"<<param<<"\n ";
      // cout<<"adding hit X0 "<<hit.X0()<<" X1 "<<hit.X1()<<endl;

      // TODO get this procedure out of the cycle
      active &= param.Transport(hit, GetParameters(), valid1G);

      if (param.Cov(0)[0] < 0.f)
        param.Cov(0)[0] = 0.2;

      if (active.isEmpty())
        continue;

      const float_v dx1 = hit.X1() - param.X1();

      const float_v Pick_temp = float_v(10.0 * 10.0);
      int coeff = 1;

      float_v OOnne = float_v(1.0 * 1.0);
      float_v Correction2 = param.Tx() * param.Tx() + OOnne;

      if (param.Err2X1() > hit.Err2X1() * hit.Err2X1()) {
        active &= dx1 * dx1 < Pick_temp * ((hit.R() * hit.R() + hit.Err2R()) * Correction2 + hit.Err2X1());
      } else {
        active &= dx1 * dx1 < coeff * Pick2 * (param.Err2X1() + hit.Err2X1() + hit.Err2R() * Correction2);
      }

      if (active.isEmpty())
        continue;

      active &= param.Filter(hit, GetParameters(), active, TRIPLET_CHI2_CUT);

      if (active.isEmpty())
        continue;

      TrackHitRecord hitRecord;
      hitRecord.fStation = iS;
      hitRecord.fIHit = ih;
      FTSCANPletV nPlet(D1, iS, ih, param, active);
      // compare these two constructors and if necessary update the new one with features from the old
      // FTSCANPletV( D1, D2, iV, param, active )
      nPlet.fLastHit = -1;
      foreach_bit(unsigned int iBit, active)
      {
        nPlet.fLastHit[iBit] = gTrackHitRecords.size();
        hitRecord.fPrevHit = D1.fLastHit[iBit];
        gTrackHitRecords.push_back(hitRecord);
      }

      // if (nPlet.N()>1)
      //{ //amount of hits on the segment is manual
      // cout<<"beforeRefit N "<<nPlet.N()<<" QP "<<nPlet.Param().QP()<<" tx "<<nPlet.Param().Tx()<<" ty "<<nPlet.Param().Ty()<<" Chi2 "<< nPlet.Param().Chi2()<<" NDF "<<
      // nPlet.Param().NDF()<<endl; 05.10 if ( (Refit( nPlet, *allHits)).isEmpty() ) continue; cout<<"afterRefit N "<<nPlet.N()<<" QP "<<nPlet.Param().QP()<<" tx
      // "<<nPlet.Param().Tx()<<" ty "<<nPlet.Param().Ty()<<" Chi2 "<< nPlet.Param().Chi2()<<" NDF "<< nPlet.Param().NDF()<<endl; 08.09 if ( (nPlet.Param().Chi2() <
      // TRIPLET_CHI2_CUT).isEmpty() ) continue; 25.08 if ( nPlet.Param().Chi2()[0] > TRIPLET_CHI2_CUT ) continue;
      next.push_back(nPlet);
      //}
      // else {next.push_back( nPlet );}
      // cout<<"nPlet.N() "<<nPlet.N()<<" nPlet.Param().QP() "<<nPlet.Param().QP()<<endl;
    }
  }
}

//#include "SQM.h"

float_m PndFTSCAGBTracker::Refit(FTSCANPletV &triplet, const FTSCAHits &hits)
{
  //* Fitting of the triplet by using Kalman Filter.

  const int N = triplet.N();

  PndFTSCATrackParamVector &param = triplet.Param();

  vector<FTSCAHitV> thits(N);

  float_m active = triplet.IsValid();
  for (int ihit = 0; ihit < N; ihit++) {
    const TESV &index = triplet.IHit(ihit);

    FTSCAHit hs[float_v::Size];
    foreach_bit(unsigned int iV, active) { hs[iV] = hits[index.s[iV]][index.e[iV]]; }
    thits[ihit] = FTSCAHitV(hs, active);
  }

  const FTSCAHitV &hit0 = thits[0];

  FTSCATarget target = fTarget;

  // const float_v  qp = param.QP();
  // param.SetQP(qp);

  /*param.InitByTarget(target);
  param.InitDirection( hit0.X0(), hit0.X1(), hit0.X2() );
  param.SetAngle( hit0.Angle() );*/

  // param.Transport( hit0, GetParameters(), active );
  param.InitByTarget(target);

  // direction initialization (tx,ty) by hit-neighbour
  //   if (hit0.IsValid() != thits[1].IsValid())
  //   {
  //       //FTSCAHitV hit1(); //create from thits[1] such a hit-set that it would work out with all hits from thits[0]
  //       cout<<"WE'VE GOT A PROBLEM \n";
  //   }
  // param.InitDirection(thits[1].X0() - hit0.X0(), thits[1].X1() - hit0.X1(), thits[1].X2() - hit0.X2());
  param.InitDirection(hit0.X0(), hit0.X1(), hit0.X2());
  param.SetAngle(hit0.Angle());

  // param.InitCovMatrix(target.Err2QMom());
  // cout<<"1. fit fwd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  // fit fwd
  for (int ihit = 0; ihit < N; ihit++) {
    const FTSCAHitV &hit = thits[ihit];
    active &= param.Transport(hit, GetParameters(), active);
    active &= param.Filter(hit, GetParameters(), active);
    // cout<<"1.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }
  // param.PrintCovMat();
  // param.InitCovMatrix(target.Err2QMom());
  int_v ndf;
  ndf(static_cast<int_m>(active)) = -4;
  param.SetNDF(ndf);
  param.SetChi2(0.f);
  // cout<<"2. fit bckwrd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  // fit bckwd
  for (int ihit = N - 2; ihit >= 0; ihit--) {
    const FTSCAHitV &hit = thits[ihit];
    active &= param.Transport(hit, GetParameters(), active);
    active &= param.Filter(hit, GetParameters(), active);
    // cout<<"2.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }
  // param.PrintCovMat();
  // param.InitCovMatrix(target.Err2QMom());

  param.SetNDF(ndf);
  param.SetChi2(0.f);
  // cout<<"3. fit fwd NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  // fit fwd
  for (int ihit = 1; ihit < N; ihit++) {
    const FTSCAHitV &hit = thits[ihit];
    active &= param.Transport(hit, GetParameters(), active);
    active &= param.Filter(hit, GetParameters(), active);
    // cout<<"3.after filter NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  }
  // param.PrintCovMat();
  // cout<<"NDF "<<param.NDF()<<" Chi2 "<<param.Chi2()<<endl;
  // cin.get();
  return active;
}

inline bool IsRightNeighbour(const FTSCANPlet &a, const FTSCANPlet &b, float pick, float &chi2)
{
  int start = (a.N() < b.N()) ? 0 : a.N() - b.N();
  for (int i = start; i < a.N() - StartStationShift; i++) {
    if (a.IHit(i + StartStationShift) != b.IHit(i)) {
      return false;
    }
  }

  // chi2 = fabs(a.QMomentum() - b.QMomentum())/sqrt(a.QMomentumErr2() + b.QMomentumErr2());

  chi2 = fabs(a.Param().Tx() - b.Param().Tx()) / sqrt(a.Param().Err2Tx() + b.Param().Err2Tx());
  // cout<<"b.ISta(b.N()-1) "<<b.ISta(b.N()-1)<<endl;
  if (b.ISta(b.N() - 1) == 26) {
    chi2 = fabs(a.QMomentum() - b.QMomentum()) / sqrt(a.QMomentumErr2() + b.QMomentumErr2());
  }
  /*
  float chi2_qp = fabs(a.QMomentum() - b.QMomentum())/sqrt(a.Param().Err2QMomentum() + b.Param().Err2QMomentum());
  float chi2_x = fabs(a.Param().X() - b.Param().X())/sqrt(a.Param().Err2X() + b.Param().Err2X());

  cout<<"a.QP "<<a.QMomentum()<<" b.QP "<<b.QMomentum()<<endl;
  cout<<"a.C44 "<<a.Param().Err2QMomentum()<<" b.C44 "<<b.Param().Err2QMomentum()<<endl;
  cout<<"a.Tx() "<<a.Param().Tx()<<" b.Tx() "<<b.Param().Tx()<<endl;
  cout<<"a.C22 "<<a.Param().Err2Tx()<<" b.C22 "<<b.Param().Err2Tx()<<endl;
  cout<<"a.X() "<<a.Param().X()<<" b.X() "<<b.Param().X()<<endl;
  cout<<"a.C00 "<<a.Param().Err2X()<<" b.C00 "<<b.Param().Err2X()<<endl;
  cout<<"dif qp "<<a.QMomentum()-b.QMomentum()<<" tx "<<a.Param().Tx()-b.Param().Tx()<<" x "<<a.Param().X()-b.Param().X()<<endl;
  cout<<"chi2_tx "<<chi2<<" chi2_qp "<<chi2_qp<<" chi2_x "<<chi2_x<<endl;
  */
  if (chi2 > pick)
    return false; // neighbours must have same qp (or some other criteria)

  chi2 *= chi2;
  // cout<<"picked neigh \n";
  // cout<<endl;
  return true;
}

// find and store neighbours triplets
// to make recursive search faster saves only neighbours with level = level - 1
void PndFTSCAGBTracker::FindNeighbours(FTSCANPlets &triplets)
{
  for (int iS = triplets.NStations() - 1; iS >= 0; --iS) { // CHECKME
    FTSCAElementsOnStation<FTSCANPlet> &ts1 = triplets.OnStation(iS);
    for (unsigned int iT1 = 0; iT1 < ts1.size(); ++iT1) {
      FTSCANPlet &t1 = ts1[iT1];
      int neighIStation = t1.ISta(0) + StartStationShift;
      // cout<<"neighIStation "<<neighIStation<<endl;
      if (neighIStation >= triplets.NStations())
        continue; // triplets can't start from this (non-existent) station

      FTSCAElementsOnStation<FTSCANPlet> &ts2 = triplets.OnStation(neighIStation);
      /*if (ts2.size()==0)
      {
            int ncalls = 0;
            while (triplets.OnStation( neighIStation ).size()==0 && (neighIStation >=0 && neighIStation<47))
            {
                if (ncalls>5) break;
                if (neighIStation==iS) {neighIStation-=1; continue;}
                neighIStation-=1;
                ncalls++;
            }
      }
      ts2 = triplets.OnStation( neighIStation );*/
      char maxLevel = -1;                           // maxLevel of neighbour triplets
      vector<pair<float, unsigned int>> neighCands; // save neighbour candidates
      for (unsigned int iT2 = 0; iT2 < ts2.size(); ++iT2) {
        const FTSCANPlet &t2 = ts2[iT2];
        float chi2;
        if (!IsRightNeighbour(t1, t2, fPick, chi2))
          continue;
        if (maxLevel < t2.Level())
          maxLevel = t2.Level();
        if (maxLevel == t2.Level())
          neighCands.push_back(pair<float, unsigned int>(chi2 + t2.Chi2Level(), iT2));
      }
      t1.Level() = maxLevel + 1;
      // cout<<"neighCands.size() "<<neighCands.size()<<endl;
      // save
      for (unsigned int iN = 0; iN < neighCands.size(); ++iN) {
        const FTSCANPlet &t2 = ts2[neighCands[iN].second];
        // cout<<" t2.Level() "<<int(t2.Level())<<" maxLevel "<<int(maxLevel)<<endl;
        if (maxLevel == t2.Level()) {
          // cout<<"Adding this one to Neighbours-list \n";
          // cout<<"maxLevel "<<int(maxLevel)<<endl;
          t1.Neighbours().push_back(neighCands[iN]);
        }
      }
      sort(t1.Neighbours().begin(), t1.Neighbours().end());
      // cout<<"t1.NNeighbours() "<<t1.NNeighbours()<<endl;
      if (t1.NNeighbours() > 0) {
        t1.Chi2Level() = t1.Chi2Neighbours(0);
        const pair<float, unsigned int> tmp = t1.Neighbours()[0]; // leave only one. CHECKME for 1000tracks events
        t1.Neighbours().clear();
        t1.Neighbours().push_back(tmp);
        // cout<<"Got a neighbour! \n";
      }
    } // iTrip1

    // sort( ts1.begin(), ts1.end(), PndCANPlet::compare );
  } // iStation
}

void PndFTSCAGBTracker::CreateTracks(const FTSCANPlets &triplets, FTSCATracks &tracks)
{
  const int Nlast = PndFTSCAParameters::LastCellLength; // N hits in the rightmost triplet

  int min_level = 1; // min level to start triplet. So min track length = min_level+3.

  // collect consequtive: the longest tracks, shorter, more shorter and so on
  for (int ilev = NStations() - Nlast; ilev >= min_level; ilev--) { // choose length
    FTSCATracks vtrackcandidate(tracks.HitsRef());
    // how many levels to check

    // lose maximum one hit and find all hits for min_level
    // const unsigned char min_best_l = (ilev > min_level) ? ilev-1 : min_level;
    // find all hits (slower)
    const unsigned char min_best_l = ilev + 3;
    // find candidates
    for (int istaF = 0; istaF <= NStations() - Nlast - ilev; istaF++) {
      const FTSCAElementsOnStation<FTSCANPlet> &tsF = triplets.OnStation(istaF);
      for (unsigned int iTrip = 0; iTrip < tsF.size(); iTrip++) {
        const FTSCANPlet *tripF = &(tsF[iTrip]);
        if (0) { // ghost supression !!!
          if (tripF->Level() == 0)
            continue; // ghost suppression // find track with 3 hits only if it was created from a chain of triplets, but not from only one triplet
          if (tripF->Level() < ilev)
            continue; // try only triplets, which can start track with ilev+3 length. w\o it have more ghosts, but efficiency either
          if ((ilev == 0) && (tripF->ISta(0) != 0))
            continue; // ghost supression // collect only MAPS tracks-triplets
        }
        if (tripF->Level() < min_best_l)
          continue;

        FTSCATrack curr_tr;

        bool isUsed = false;
        for (int i = 0; i < tripF->N(); i++) {
          if (triplets.OnStation(tripF->ISta(0)).GetHit(i, iTrip).IsUsed()) {
            isUsed = true;
            break;
          }
          curr_tr.AddHit(tripF->IHit(i));
        }
        if (isUsed)
          continue;
        curr_tr.Level()++;
        curr_tr.Chi2() = tripF->Param().Chi2();

        FTSCATrack best_tr = curr_tr;
        unsigned int nCalls = 0;
        FindBestCandidate(istaF, best_tr, iTrip, curr_tr, min_best_l, triplets, nCalls);

        if (best_tr.Level() < min_best_l)
          continue;
        if (best_tr.NHits() < PndFTSCAParameters::MinimumHitsForRecoTrack)
          continue;

        //      if ( best_tr.Fit( *tracks.HitsRef(), fTarget, GetParameters(), tripF->QMomentum() ).QPt() > 0.5*fMaxInvMom ) continue; // fit to determine Chi2 and NDF

        vtrackcandidate.push_back(best_tr);
        // best_tr.SetHitsAsUsed(*tracks.HitsRef()); TODO
        // tracks.push_back(best_tr);
      }
    } // istaF

    // select and save best candidates
    vtrackcandidate.SelectAndSaveTracks(tracks);
  } // ilev
}

void PndFTSCAGBTracker::FindBestCandidate(int ista,
                                          FTSCATrack &bT, // best track
                                          int currITrip,  // index of current triplet
                                          FTSCATrack &cT, // current track
                                          unsigned char min_best_l, const FTSCANPlets &triplets, unsigned int &nCalls)
{
  // if (nCalls > 100) return; // avoid long processing in confusing cases
  nCalls++;

  const FTSCAElementsOnStation<FTSCANPlet> &trs = triplets.OnStation(ista);
  const FTSCANPlet *curr_trip = &(trs[currITrip]);

  if (curr_trip->Level() == 0) // && nCalls>7)
  {                            // the end of the track -> check and store

    // -- finish with current track

    //    if( cT.Level() < min_best_l - 1 ) return; // suppose that only one hit can be added by extender
    //    cT.Fit( *trs.HitsRef(), fTarget, GetParameters() ); // fit to determine Chi2 and NDF // takes 20 times more time then the rest

    // -- select the best
    if ((cT.NDF() > bT.NDF()) || ((cT.NDF() == bT.NDF()) && (cT.Chi2() < bT.Chi2())))
      bT = cT;

  } else { // level != 0

    // try to extend. try all possible triplets
    int NNeighs = curr_trip->NNeighbours();
    for (int in = 0; in < NNeighs; in++) {
      int newITrip = curr_trip->INeighbours(in);
      const FTSCANPlet *new_trip = &(triplets.OnStation(curr_trip->ISta(0) + StartStationShift)[newITrip]);

      // check new triplet
      bool isUsed = false;
      const int newTripLength = new_trip->N();
      ASSERT(newTripLength - curr_trip->N() >= -1, newTripLength << " - " << curr_trip->N());
      for (int i = curr_trip->N() - 1; i < newTripLength; i++) {
        if (triplets.OnStation(new_trip->ISta(0)).GetHit(i, newITrip).IsUsed()) {
          isUsed = true;
          break;
        }
      }

      if (isUsed) {
        // cT.Fit( *trs.HitsRef(), fTarget, GetParameters() ); // fit to determine Chi2 and NDF

        //  no used hits allowed -> compare and store track
        if ((cT.NDF() > bT.NDF()) || ((cT.NDF() == bT.NDF()) && (cT.Chi2() < bT.Chi2())))
          bT = cT;
      } else { //  add new triplet to the current track

        // restore current track
        // save current tracklet
        FTSCATrack nT = cT; // new track

        // add new triplet
        nT.Level()++;
        int start = curr_trip->N() - StartStationShift;
        if (start < 0)
          start = 0;
        for (int i = start; i < newTripLength; i++) {
          nT.AddHit(new_trip->IHit(i));
        }

        const float qp1 = curr_trip->QMomentum();
        const float qp2 = new_trip->QMomentum();
        float dqp = fabs(qp1 - qp2);
        float Cqp = curr_trip->QMomentumErr();
        Cqp += new_trip->QMomentumErr();
        dqp = dqp / Cqp;
        nT.Chi2() += dqp * dqp;
        FindBestCandidate(new_trip->ISta(0), bT, newITrip, nT, min_best_l, triplets, nCalls);
      } // add triplet to track
    }   // for neighbours
  }     // level != 0
}

void PndFTSCAGBTracker::Merge(FTSCATracks &tracks)
{
  const int NTracksS = tracks.size();
  vector<pair<PndFTSCATrackParam, PndFTSCATrackParam>> fittedTracks;
  fittedTracks.reserve(NTracksS);
  // cout<<"NTracksS "<<NTracksS<<endl;
  tracks.SortTracksByZ();
  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    FTSCATrack &t1 = tracks[iT1];
    PndFTSCATrackParam outerParam = t1.Fit(*tracks.HitsRef(), fTarget, GetParameters(), false);
    // PndFTSCATrackParam outerBufParam = outerParam;
    PndFTSCATrackParam innerParam =
      t1.Fit(*tracks.HitsRef(), fTarget, GetParameters(), true); //, true, outerParam); //we use the obtained outerParam as seed to avoid numerical divergencies
    if (abs(innerParam.Err2X()) > 20. || isnan(innerParam.Err2X())) {
      // innerParam = t1.Fit( *tracks.HitsRef(), fTarget, GetParameters(), true, true, outerParam );
      innerParam = outerParam;
      innerParam.SetX((*tracks.HitsRef())[t1.IHits()[0]].X1());
      innerParam.SetY((*tracks.HitsRef())[t1.IHits()[0]].X2());
      innerParam.SetZ((*tracks.HitsRef())[t1.IHits()[0]].X0());
      innerParam.SetTX(innerParam.X() / innerParam.Z());
      innerParam.SetTY(innerParam.Y() / innerParam.Z());
    }
    if (abs(outerParam.Err2X()) > 20. || isnan(outerParam.Err2X())) {
      outerParam = innerParam;
      outerParam.SetX((*tracks.HitsRef())[t1.IHits()[t1.NHits() - 1]].X1());
      outerParam.SetY((*tracks.HitsRef())[t1.IHits()[t1.NHits() - 1]].X2());
      outerParam.SetZ((*tracks.HitsRef())[t1.IHits()[t1.NHits() - 1]].X0());
      outerParam.SetTX(outerParam.X() / outerParam.Z());
      outerParam.SetTY(outerParam.Y() / outerParam.Z());
    }
    fittedTracks.push_back(pair<PndFTSCATrackParam, PndFTSCATrackParam>(innerParam, outerParam));
    // float X0_back = (*tracks.HitsRef())[t1.IHits()[0]].X0();
    // float X0_front = (*tracks.HitsRef())[t1.IHits()[t1.NHits()-1]].X0();
    // cout<<"Z_first "<<X0_back<<" Z_last "<<X0_front<<endl;
    // cout<<"InnerParam.X() "<<innerParam.X()<<" OuterParam.X() "<<outerParam.X()<<endl;
  }
  // PndFTSCATrackParam recursiveParam;

  for (int iT_middle = 0; iT_middle < NTracksS; iT_middle++) {
    FTSCATrack &t_middle = tracks[iT_middle];
    // cout<<"t_middle.NHits() "<<t_middle.NHits()<<endl;
    if (t_middle.NHits() <= 0)
      continue;
    PndFTSCATrackParam t_middleOuter;
    t_middleOuter = fittedTracks[iT_middle].second;
    // try to extend forward (from the last hit in middle-track to the first hit in end-track)
    for (int iT_end = 0; iT_end < NTracksS; iT_end++) {
      FTSCATrack &t_end = tracks[iT_end];
      // cout<<"t_end.NHits() "<<t_end.NHits()<<endl;
      if ((t_end.NHits() <= 0) || (iT_middle == iT_end))
        continue;
      const float xDiff1 = (*tracks.HitsRef())[t_end.IHits().front()].X0() - (*tracks.HitsRef())[t_middle.IHits().back()].X0();
      if (xDiff1 <= 0)
        continue;

      PndFTSCATrackParam t_endInner = fittedTracks[iT_end].first;
      // cout<<"t_endInner.Z() "<<t_endInner.Z()<<" t_middleOuter.Z() "<<t_middleOuter.Z()<<endl;
      // cout<<"before transp t2Outer.X() "<<(fittedTracks[iT2].second).X()<<" t1Inner.X() "<<(fittedTracks[iT1].first).X()<<endl;
      float Xt_endInnerBuf = t_endInner.X();
      float Xt_middleOuterBuf = t_middleOuter.X();

      // PndFTSCATrackParam buf_t_end = t_endInner;
      PndFTSCATrackParam buf_t_middle = t_middleOuter;

      t_endInner.Transport((*tracks.HitsRef())[t_middle.IHits().back()], GetParameters());
      t_middleOuter.Transport((*tracks.HitsRef())[t_end.IHits().front()], GetParameters()); // HINT because sometimes backward fit fails

      if (abs(t_middleOuter.Err2X()) > 20. || isnan(t_middleOuter.Err2X())) {
        float coefQP = 10.;
        int counterX = 0;
        while ((abs(t_middleOuter.Err2X()) > 20. || isnan(t_middleOuter.Err2X())) && (counterX < 5)) {
          t_middleOuter = buf_t_middle;
          t_middleOuter.SetQP(buf_t_middle.QP() / coefQP);
          t_middleOuter.Transport((*tracks.HitsRef())[t_end.IHits().front()], GetParameters());
          counterX += 1;
          coefQP /= 2.;
        }
      }
      // cout<<"1. after transp t_endInner.X() "<<t_endInner.X()<<" t_middleOuterBufX "<<Xt_middleOuterBuf<<endl;
      // cout<<"2. after transp t_middleOuter.X() "<<t_middleOuter.X()<<" t_endInnerBufX "<<Xt_endInnerBuf<<endl;
      float coef1 = 1.;
      float deltaX1 = 0;
      if (xDiff1 < 1)
        deltaX1 = 2.5;
      if (xDiff1 >= 1 && xDiff1 < 6)
        deltaX1 = 5.0;
      if (xDiff1 >= 6 && xDiff1 < 10)
        deltaX1 = 7.5;
      if (xDiff1 >= 10 && xDiff1 < 20)
        deltaX1 = 10.0;
      if (xDiff1 >= 20 && xDiff1 < 30)
        deltaX1 = 11.0;
      if (xDiff1 >= 30)
        deltaX1 = 12.0;
      if (xDiff1 >= 120)
        deltaX1 = 25.0;

      if ((fabs(t_endInner.X() - Xt_middleOuterBuf) < deltaX1 * coef1) || (fabs(t_middleOuter.X() - Xt_endInnerBuf) < deltaX1 * coef1)) {
        // merge t_middle & t_end
        // cout<<"MERGED t_middle & t_end \n";
        for (int ih = 0; ih < t_end.NHits(); ih++) {
          t_middle.AddHit(t_end.IHits()[ih]);
        }
        t_end.IHits().clear(); // marking this track as used
      }
      // recursiveParam = fittedTracks[iT_middle].first;
      // try to extend backward (from the first hit in middle-track to the last hit in start-track)

      /*
            for ( int iT_start = 0; iT_start < NTracksS; iT_start++ )
            {
              FTSCATrack& t_start = tracks[iT_start];
              //cout<<"t_start.NHits() "<<t_start.NHits()<<endl;
              if (( t_start.NHits() <= 0 ) || (iT_middle==iT_start) ) continue;
              const float xDiff2 = (*tracks.HitsRef())[t_start.IHits().front()].X0() - (*tracks.HitsRef())[t_middle.IHits().back()].X0();
              if (xDiff2>=0) continue;

              PndFTSCATrackParam t_startOuter = fittedTracks[iT_start].second;
              //recursiveParam=t_middleInner
              cout<<"t_startOuter.Z() "<<t_startOuter.Z()<<" t_middleInner.Z() "<<recursiveParam.Z()<<endl;
              //cout<<"before transp t2Outer.X() "<<(fittedTracks[iT2].second).X()<<" t1Inner.X() "<<(fittedTracks[iT1].first).X()<<endl;
              float Xt_startOuterBuf = t_startOuter.X();
              float Xt_middleInnerBuf = recursiveParam.X();

              t_startOuter.Transport( (*tracks.HitsRef())[t_middle.IHits().front()], GetParameters() );
              recursiveParam.Transport( (*tracks.HitsRef())[t_start.IHits().back()], GetParameters() );//HINT because sometimes backward fit fails

              cout<<"1. after transp t_startOuter.X() "<<t_startOuter.X()<<" Xt_middleInnerBuf "<<Xt_middleInnerBuf<<endl;
              cout<<"2. after transp t_middleInner.X() "<<recursiveParam.X()<<" Xt_startOuterBuf "<<Xt_startOuterBuf<<endl;

              float coef2=1.;
              float deltaX2=0;
              if (xDiff2>-1)
                deltaX2=2.5;
              if (xDiff2<=-1 && xDiff2>-6)
                deltaX2=5.0;
              if (xDiff2<=-6 && xDiff2>-10)
                deltaX2=7.5;
              if (xDiff2<=-10 && xDiff2>-20)
                deltaX2=10.0;
              if (xDiff2<=-20 && xDiff2>-30)
                deltaX2=12.5;
              if (xDiff2<=-30)
                deltaX2=15.0;
              if ((fabs(t_endInner.X()-Xt_middleOuterBuf)<deltaX2*coef2) || (fabs(t_middleOuter.X()-Xt_endInnerBuf)<deltaX2*coef2))
              {
                //merge t_middle & t_start
                cout<<"MERGED t_middle & t_start \n";
                t_middle.AddHitsToTheBeginning(t_start.IHits());
                t_start.IHits().clear(); //marking this track as used
              }
            }//iT_start
      */
    } // iT_end
  }   // iT_middle

  // remove tracks, which were atached to other tracks
  // remove clones if any
  FTSCATracks tracks_saved = tracks;
  tracks.clear();
  // bool addTrack = true;
  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    FTSCATrack &t1 = tracks_saved[iT1];
    if (t1.NHits() != 0)
      tracks.push_back(t1);
  }
  /*tracks_saved = tracks;
  tracks.clear();
  tracks_saved.SelectAndSaveTracks(tracks);*/
}

// END@@@@!!!!STT===>>>FTS COMBINATORIAL PART!!!!@@@@@

void PndFTSCAGBTracker::EstimatePV(const FTSCAHitsV &all, float &zPV)
{
  //* Estimate coordinates of the primary vertex.

  int iS = 0;
  const float maxZ = GetParameters().MaxZ();
  const float maxPVR = 0.1;               // max distance between PV and beam line
  const unsigned int N = 2 * maxZ / 0.01; // n bins in histo

  vector<float> pvHist(N, 0);
  const FTSCAElementsOnStation<FTSCAHitV> &s = all.OnStation(iS);
  const FTSCAElementsOnStation<FTSCAHitV> &s2 = all.OnStation(iS + 1);
  for (unsigned int i = 0; i < s.size(); ++i) {
    const FTSCAHitV &h = s[i];
    foreach_bit(int iV, h.IsValid())
    {
      float gx, gy, gz;
      h.GetGlobalCoor(iV, gx, gy, gz);
      PndFTSCAParameters::CALocalToGlobal(float(h.X0()[iV]), float(h.X1()[iV]), float(h.X2()[iV]), float(h.Angle()[iV]), gx, gy, gz);
      float r = sqrt(gx * gx + gy * gy);

      for (unsigned int i2 = 0; i2 < s2.size(); ++i2) {
        const FTSCAHitV &h2 = s2[i2];
        foreach_bit(int iV2, h2.IsValid())
        {
          float gx2, gy2, gz2;
          h2.GetGlobalCoor(iV2, gx2, gy2, gz2);
          float r2 = sqrt(gx2 * gx2 + gy2 * gy2);

          float gz0 = -(gz2 - gz) / (r2 - r) * r + gz;
          float gx0 = -(gx2 - gx) / (r2 - r) * r + gx;
          float gy0 = -(gy2 - gy) / (r2 - r) * r + gy;
          if (abs(gz0) < maxZ && abs(gx0) < maxPVR && abs(gy0) < maxPVR)
            pvHist[(gz0 / maxZ + 1) * N / 2]++;
        }
      }
    }
  }

#ifdef DRAW_CA
  const double *pv = PndFTSCAPerformance::Instance().PV(); // dbg
  PndFTSCADisplay::Instance().DrawGBHits(all);
  PndFTSCADisplay::Instance().DrawPVHisto(pvHist, GetParameters());
  PndFTSCADisplay::Instance().DrawGBPoint(pv[0], pv[1], pv[2], 2, 0.5);
  PndFTSCADisplay::Instance().SaveCanvasToFile("DrawPVHisto.pdf");
  PndFTSCADisplay::Instance().Ask();
#endif

  float max = -1;
  int maxI = -1;
  for (unsigned int i = 0; i < pvHist.size(); ++i) {
    if (max < pvHist[i]) {
      max = pvHist[i];
      maxI = i;
    }
  }

  zPV = (2.f * maxI / N - 1) * maxZ;
}

void PndFTSCAGBTracker::InvertCholetsky(float a[15]) const
{
  float d[5], uud, u[5][5];
  for (int i = 0; i < 5; i++) {
    d[i] = 0.f;
    for (int j = 0; j < 5; j++)
      u[i][j] = 0.;
  }

  for (int i = 0; i < 5; i++) {
    uud = 0.;
    for (int j = 0; j < i; j++)
      uud += u[j][i] * u[j][i] * d[j];
    uud = a[i * (i + 3) / 2] - uud;

    if (fabs(uud) < 1.e-12)
      uud = 1.e-12;
    d[i] = uud / fabs(uud);
    u[i][i] = sqrt(fabs(uud));

    for (int j = i + 1; j < 5; j++) {
      uud = 0.;
      for (int k = 0; k < i; k++)
        uud += u[k][i] * u[k][j] * d[k];
      uud = a[j * (j + 1) / 2 + i] - uud;
      u[i][j] = d[i] / u[i][i] * uud;
    }
  }

  float u1[5];

  for (int i = 0; i < 5; i++) {
    u1[i] = u[i][i];
    u[i][i] = 1.f / u[i][i];
  }
  for (int i = 0; i < 4; i++) {
    u[i][i + 1] = -u[i][i + 1] * u[i][i] * u[i + 1][i + 1];
  }
  for (int i = 0; i < 3; i++) {
    u[i][i + 2] = u[i][i + 1] * u1[i + 1] * u[i + 1][i + 2] - u[i][i + 2] * u[i][i] * u[i + 2][i + 2];
  }
  for (int i = 0; i < 2; i++) {
    u[i][i + 3] = u[i][i + 2] * u1[i + 2] * u[i + 2][i + 3] - u[i][i + 3] * u[i][i] * u[i + 3][i + 3];
    u[i][i + 3] -= u[i][i + 1] * u1[i + 1] * (u[i + 1][i + 2] * u1[i + 2] * u[i + 2][i + 3] - u[i + 1][i + 3]);
  }
  u[0][4] = u[0][2] * u1[2] * u[2][4] - u[0][4] * u[0][0] * u[4][4];
  u[0][4] += u[0][1] * u1[1] * (u[1][4] - u[1][3] * u1[3] * u[3][4] - u[1][2] * u1[2] * u[2][4]);
  u[0][4] += u[3][4] * u1[3] * (u[0][3] - u1[2] * u[2][3] * (u[0][2] - u[0][1] * u1[1] * u[1][2]));

  for (int i = 0; i < 5; i++)
    a[i + 10] = u[i][4] * d[4] * u[4][4];
  for (int i = 0; i < 4; i++)
    a[i + 6] = u[i][3] * u[3][3] * d[3] + u[i][4] * u[3][4] * d[4];
  for (int i = 0; i < 3; i++)
    a[i + 3] = u[i][2] * u[2][2] * d[2] + u[i][3] * u[2][3] * d[3] + u[i][4] * u[2][4] * d[4];
  for (int i = 0; i < 2; i++)
    a[i + 1] = u[i][1] * u[1][1] * d[1] + u[i][2] * u[1][2] * d[2] + u[i][3] * u[1][3] * d[3] + u[i][4] * u[1][4] * d[4];
  a[0] = u[0][0] * u[0][0] * d[0] + u[0][1] * u[0][1] * d[1] + u[0][2] * u[0][2] * d[2] + u[0][3] * u[0][3] * d[3] + u[0][4] * u[0][4] * d[4];
}

void PndFTSCAGBTracker::MultiplySS(float const C[15], float const V[15], float K[5][5]) const
{
  //*multiply 2 symmetric matricies
  K[0][0] = C[0] * V[0] + C[1] * V[1] + C[3] * V[3] + C[6] * V[6] + C[10] * V[10];
  K[0][1] = C[0] * V[1] + C[1] * V[2] + C[3] * V[4] + C[6] * V[7] + C[10] * V[11];
  K[0][2] = C[0] * V[3] + C[1] * V[4] + C[3] * V[5] + C[6] * V[8] + C[10] * V[12];
  K[0][3] = C[0] * V[6] + C[1] * V[7] + C[3] * V[8] + C[6] * V[9] + C[10] * V[13];
  K[0][4] = C[0] * V[10] + C[1] * V[11] + C[3] * V[12] + C[6] * V[13] + C[10] * V[14];

  K[1][0] = C[1] * V[0] + C[2] * V[1] + C[4] * V[3] + C[7] * V[6] + C[11] * V[10];
  K[1][1] = C[1] * V[1] + C[2] * V[2] + C[4] * V[4] + C[7] * V[7] + C[11] * V[11];
  K[1][2] = C[1] * V[3] + C[2] * V[4] + C[4] * V[5] + C[7] * V[8] + C[11] * V[12];
  K[1][3] = C[1] * V[6] + C[2] * V[7] + C[4] * V[8] + C[7] * V[9] + C[11] * V[13];
  K[1][4] = C[1] * V[10] + C[2] * V[11] + C[4] * V[12] + C[7] * V[13] + C[11] * V[14];

  K[2][0] = C[3] * V[0] + C[4] * V[1] + C[5] * V[3] + C[8] * V[6] + C[12] * V[10];
  K[2][1] = C[3] * V[1] + C[4] * V[2] + C[5] * V[4] + C[8] * V[7] + C[12] * V[11];
  K[2][2] = C[3] * V[3] + C[4] * V[4] + C[5] * V[5] + C[8] * V[8] + C[12] * V[12];
  K[2][3] = C[3] * V[6] + C[4] * V[7] + C[5] * V[8] + C[8] * V[9] + C[12] * V[13];
  K[2][4] = C[3] * V[10] + C[4] * V[11] + C[5] * V[12] + C[8] * V[13] + C[12] * V[14];

  K[3][0] = C[6] * V[0] + C[7] * V[1] + C[8] * V[3] + C[9] * V[6] + C[13] * V[10];
  K[3][1] = C[6] * V[1] + C[7] * V[2] + C[8] * V[4] + C[9] * V[7] + C[13] * V[11];
  K[3][2] = C[6] * V[3] + C[7] * V[4] + C[8] * V[5] + C[9] * V[8] + C[13] * V[12];
  K[3][3] = C[6] * V[6] + C[7] * V[7] + C[8] * V[8] + C[9] * V[9] + C[13] * V[13];
  K[3][4] = C[6] * V[10] + C[7] * V[11] + C[8] * V[12] + C[9] * V[13] + C[13] * V[14];

  K[4][0] = C[10] * V[0] + C[11] * V[1] + C[12] * V[3] + C[13] * V[6] + C[14] * V[10];
  K[4][1] = C[10] * V[1] + C[11] * V[2] + C[12] * V[4] + C[13] * V[7] + C[14] * V[11];
  K[4][2] = C[10] * V[3] + C[11] * V[4] + C[12] * V[5] + C[13] * V[8] + C[14] * V[12];
  K[4][3] = C[10] * V[6] + C[11] * V[7] + C[12] * V[8] + C[13] * V[9] + C[14] * V[13];
  K[4][4] = C[10] * V[10] + C[11] * V[11] + C[12] * V[12] + C[13] * V[13] + C[14] * V[14];
}

void PndFTSCAGBTracker::MultiplyMS(float const C[5][5], float const V[15], float K[15]) const
{
  //*multiply symmetric and nonsymmetric matricies
  K[0] = C[0][0] * V[0] + C[0][1] * V[1] + C[0][2] * V[3] + C[0][3] * V[6] + C[0][4] * V[10];

  K[1] = C[1][0] * V[0] + C[1][1] * V[1] + C[1][2] * V[3] + C[1][3] * V[6] + C[1][4] * V[10];
  K[2] = C[1][0] * V[1] + C[1][1] * V[2] + C[1][2] * V[4] + C[1][3] * V[7] + C[1][4] * V[11];

  K[3] = C[2][0] * V[0] + C[2][1] * V[1] + C[2][2] * V[3] + C[2][3] * V[6] + C[2][4] * V[10];
  K[4] = C[2][0] * V[1] + C[2][1] * V[2] + C[2][2] * V[4] + C[2][3] * V[7] + C[2][4] * V[11];
  K[5] = C[2][0] * V[3] + C[2][1] * V[4] + C[2][2] * V[5] + C[2][3] * V[8] + C[2][4] * V[12];

  K[6] = C[3][0] * V[0] + C[3][1] * V[1] + C[3][2] * V[3] + C[3][3] * V[6] + C[3][4] * V[10];
  K[7] = C[3][0] * V[1] + C[3][1] * V[2] + C[3][2] * V[4] + C[3][3] * V[7] + C[3][4] * V[11];
  K[8] = C[3][0] * V[3] + C[3][1] * V[4] + C[3][2] * V[5] + C[3][3] * V[8] + C[3][4] * V[12];
  K[9] = C[3][0] * V[6] + C[3][1] * V[7] + C[3][2] * V[8] + C[3][3] * V[9] + C[3][4] * V[13];

  K[10] = C[4][0] * V[0] + C[4][1] * V[1] + C[4][2] * V[3] + C[4][3] * V[6] + C[4][4] * V[10];
  K[11] = C[4][0] * V[1] + C[4][1] * V[2] + C[4][2] * V[4] + C[4][3] * V[7] + C[4][4] * V[11];
  K[12] = C[4][0] * V[3] + C[4][1] * V[4] + C[4][2] * V[5] + C[4][3] * V[8] + C[4][4] * V[12];
  K[13] = C[4][0] * V[6] + C[4][1] * V[7] + C[4][2] * V[8] + C[4][3] * V[9] + C[4][4] * V[13];
  K[14] = C[4][0] * V[10] + C[4][1] * V[11] + C[4][2] * V[12] + C[4][3] * V[13] + C[4][4] * V[14];
}

void PndFTSCAGBTracker::MultiplySR(float const C[15], float const r_in[5], float r_out[5]) const
{
  //*multiply vector and symmetric matrix
  r_out[0] = r_in[0] * C[0] + r_in[1] * C[1] + r_in[2] * C[3] + r_in[3] * C[6] + r_in[4] * C[10];
  r_out[1] = r_in[0] * C[1] + r_in[1] * C[2] + r_in[2] * C[4] + r_in[3] * C[7] + r_in[4] * C[11];
  r_out[2] = r_in[0] * C[3] + r_in[1] * C[4] + r_in[2] * C[5] + r_in[3] * C[8] + r_in[4] * C[12];
  r_out[3] = r_in[0] * C[6] + r_in[1] * C[7] + r_in[2] * C[8] + r_in[3] * C[9] + r_in[4] * C[13];
  r_out[4] = r_in[0] * C[10] + r_in[1] * C[11] + r_in[2] * C[12] + r_in[3] * C[13] + r_in[4] * C[14];
}

void PndFTSCAGBTracker::FilterTracks(float const r[5], float const C[15], float const m[5], float const V[15], float R[5], float W[15], float &chi2) const
{
  //* Track filtering is realized with the Kalman Filter,
  //* which allows one to obtain optimal estimate of track's parameters.

  float S[15];
  for (int i = 0; i < 15; i++) {
    W[i] = C[i];
    S[i] = C[i] + V[i];
  }
  for (int i = 0; i < 5; i++)
    R[i] = r[i];

  InvertCholetsky(S);

  float K[5][5];
  MultiplySS(C, S, K);
  float dzeta[5];
  for (int i = 0; i < 5; i++)
    dzeta[i] = m[i] - r[i];
  float KC[15];
  MultiplyMS(K, C, KC);
  for (int i = 0; i < 15; i++)
    W[i] -= KC[i];

  float kd;
  for (int i = 0; i < 5; i++) {
    kd = 0.f;
    for (int j = 0; j < 5; j++)
      kd += K[i][j] * dzeta[j];
    R[i] += kd;
  }
  float S_dzeta[5];
  MultiplySR(S, dzeta, S_dzeta);
  chi2 = dzeta[0] * S_dzeta[0] + dzeta[1] * S_dzeta[1] + dzeta[2] * S_dzeta[2] + dzeta[3] * S_dzeta[3] + dzeta[4] * S_dzeta[4];
}
