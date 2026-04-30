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

// $Id: PndCAGBTracker.cxx,v 1.12 2010/09/01 10:38:27 ikulakov Exp $
// **************************************************************************
// This file is property of and copyright by the ALICE HLT Project          *
// ALICE Experiment at CERN, All rights reserved.                           *
//                                                                          *
// Primary Authors: Sergey Gorbunov <sergey.gorbunov@kip.uni-heidelberg.de> *
//                  Ivan Kisel <kisel@kip.uni-heidelberg.de>                *
//                  for The ALICE HLT Project.                              *
//                                                                          *
// Developed by:   Igor Kulakov <I.Kulakov@gsi.de>                          *
//                 Maksym Zyzak <M.Zyzak@gsi.de>                            *
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
// List of most recent changes:                                             *
// 29-05-17 Adjustment of initialization for PANDA data (Irina Rostovtseva) *
//***************************************************************************
#include "PndCAStationSTT.h"
#include "PndCAGBTracker.h"
#include "PndCAGBHit.h"
#include "PndCAGBTrack.h"
#include "PndCATrackParam.h"

#include "PndCAMath.h"
#include "PndCATrackLinearisationVector.h"
#include "PndCAPerformance.h"
#include "TStopwatch.h"

#include "PndCATarget.h"
#include "PndCAHits.h"
#include "PndCAHitsV.h"
#include "PndCATracks.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

// TODO DELL ME!!!
#include "PndCAPerformance.h"
#include "PndCAMCPoint.h"
#include "PndCAPerformanceBase.h"

#ifdef CATRACKER_DISPLAY
#include "PndCADisplay.h"
#endif

struct TrackHitRecord {
  int fPrevHit; // index of previous TrackHitRecord of same track in an array of records
  unsigned short fIHit;
  char fStation;
};

vector<TrackHitRecord> gTrackHitRecords;

const int StartStationShift = 1; // 3;

PndCANPlets::PndCANPlets(const PndCANPletsV &p) : PndCAStationArray<PndCANPlet>(p.NStations(), p.OnStation(0).HitsRef())
{
  for (int iS = 0; iS < NStations(); ++iS) {
    PndCAElementsOnStation<PndCANPlet> &tOnSta = OnStation(iS);

    const PndCAElementsOnStation<PndCANPletV> &ts = p.OnStation(iS);

    tOnSta.clear();
    tOnSta.reserve(ts.size() * float_v::Size);
    for (unsigned int iT = 0; iT < ts.size(); iT++) {
      const PndCANPletV &t = ts[iT];
      foreach_bit(unsigned int iV, t.IsValid())
      {
        tOnSta.push_back(PndCANPlet(PndCATrackParam(t.Param(), iV)));
        PndCANPlet &nPlet = tOnSta.back();
        int irec = t.fLastHit[iV];
        if (irec < 0)
          continue;
        int nHits = t.N();
        nPlet.fIHit.clear();
        nPlet.fIHit.resize(nHits);
        for (int i = nHits - 1; i >= 0; i--) {
          if (irec < 0) {
            cout << "CA tracker: something wrong with hit links!!!" << endl;
            exit(0);
            break;
          }
          TrackHitRecord &rec = gTrackHitRecords[irec];
          nPlet.fIHit[i] = (PndCATES(rec.fStation, rec.fIHit));
          irec = rec.fPrevHit;
        }
      }
    }
  }
}

bool SINGLE_THREADED = false;

PndCAGBTracker::PndCAGBTracker()
  : fHits(), fNHits(0), fTrackHits(nullptr), fTracks(nullptr), fNTracks(0), fTime(0), fStatNEvents(0), fSliceTrackerTime(0), fSliceTrackerCpuTime(0), fGTi(), fTi(1), fStatGTi(),
    fStatTi(1)
{
  //* constructor
  for (int i = 0; i < 20; i++)
    fStatTime[i] = 0;

  fGTi.Add("init  ");
  fGTi.Add("iters ");
  fGTi.Add("tracker");
  fGTi.Add("fitter ");

  fTi.SetNIter(1); // for iterations
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

void PndCAGBTracker::Init()
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
  gTrackHitRecords.reserve(10000);
}

PndCAGBTracker::~PndCAGBTracker()
{
  //* destructor
  StartEvent();
}

void PndCAGBTracker::StartEvent()
{
  //* clean up track and hit arrays

  delete[] fTrackHits;
  fTrackHits = nullptr;
  delete[] fTracks;
  fTracks = nullptr;
  fNHits = 0;
  fNTracks = 0;
  gTrackHitRecords.resize(0);
}

inline void ConvertTrackParamToVector(PndCATrackParam t0[uint_v::Size], PndCATrackParamVector &t, int &nTracksV)
{
  float_v tmpVec;
  int_v tmpVecShort;
  float_v::Memory tmpFloat;
  int_v::Memory tmpShort;

  for (int iV = 0; iV < nTracksV; iV++)
    tmpFloat[iV] = t0[iV].X();
  tmpVec.load(tmpFloat);
  t.SetX(tmpVec);
  for (int iV = 0; iV < nTracksV; iV++)
    tmpFloat[iV] = t0[iV].SignCosPhi();
  tmpVec.load(tmpFloat);
  t.SetSignCosPhi(tmpVec);

  for (int iP = 0; iP < 5; iP++) {
    for (int iV = 0; iV < nTracksV; iV++)
      tmpFloat[iV] = t0[iV].Par()[iP];
    tmpVec.load(tmpFloat);
    t.SetPar(iP, tmpVec);
  }
  for (int iC = 0; iC < 15; iC++) {
    for (int iV = 0; iV < nTracksV; iV++)
      tmpFloat[iV] = t0[iV].Cov()[iC];
    tmpVec.load(tmpFloat);
    t.SetCov(iC, tmpVec);
  }
  for (int iV = 0; iV < nTracksV; iV++)
    tmpFloat[iV] = t0[iV].Chi2();
  tmpVec.load(tmpFloat);
  t.SetChi2(tmpVec);
  for (int iV = 0; iV < nTracksV; iV++)
    tmpShort[iV] = t0[iV].NDF();
  tmpVecShort.load(tmpShort);
  t.SetNDF(tmpVecShort);
}

void PndCAGBTracker::FindTracks()
{
  //* main tracking routine
  fTime = 0;
  fStatNEvents++;

  // cout << " NHits = " << fNHits << endl;
#ifdef CATRACKER_DISPLAY
  PndCADisplay &disp = PndCADisplay::Instance();
  disp.Init();
  disp.SetTPC(fParameters);
  disp.SetGB(this);
  disp.DrawTPC();
  disp.DrawGBPoints();
  disp.Ask();
  disp.DrawGBHits(*this);
  // disp.Update();
  disp.Ask();
#endif

  TStopwatch timer1;
  TStopwatch timer2;

  fSliceTrackerTime = 0;
  fSliceTrackerCpuTime = 0;
  fTime = 0;
  for (int i = 0; i < 20; ++i) {
    fStatTime[i] = 0.;
  }

  //#ifdef USE_TIMERS
  timer1.Start();
  //#endif /// USE_TIMERS

#ifdef USE_DBG_TIMERS
  fGTi.Clear();
  fTi.Clear();
#endif

#ifdef USE_DBG_TIMERS
  TStopwatch timer;
  timer.Start(1);
#endif

  CATrackFinder();

#ifdef USE_DBG_TIMERS
  timer.Stop();
  fGTi["tracker"] = timer;
  timer.Start(1);
#endif

#ifdef USE_DBG_TIMERS
  timer.Stop();
  fGTi["fitter "] = timer;
#endif

  //#ifdef USE_TIMERS
  timer1.Stop();
  fStatTime[12] = timer1.RealTime();
  //#endif /// USE_TIMERS
  /// Read hits, row by row

  fSliceTrackerTime += timer1.RealTime();
  fSliceTrackerCpuTime += timer1.CpuTime();
  // fTime+=timerMerge.RealTime();
  // std::cout<<"Merge time = "<<timerMerge.RealTime()*1.e3<<"ms"<<std::endl;
  // std::cout<<"End CA merging"<<std::endl;
  fTime += timer1.RealTime();

#ifdef USE_DBG_TIMERS
  static int stat_N = 0;
  stat_N++;

  cout << endl << " --- Timers, ms --- " << endl;
  fTi.Calc();
  fStatTi += fTi;
  L1CATFTimerInfo tmp_ti = fStatTi / 0.001 / stat_N; // ms

  tmp_ti.PrintReal();
  fStatGTi += fGTi;
  L1CATFIterTimerInfo tmp_gti = fStatGTi / 0.001 / stat_N; // ms
  tmp_gti.PrintReal(1);
#endif

#ifdef CATRACKER_DISPLAY
  for (int i = 0; i < fNTracks; i++) {
    disp.DrawRecoTrack(i);
  }
  disp.Ask();
#endif
}

void PndCAGBTracker::WriteSettings(std::ostream &out) const
{
  //* write settings to the file
  UNUSED_PARAM1(out); // TODO
  //    out << fSlices[iSlice].Param();
}

void PndCAGBTracker::ReadSettings(std::istringstream &in)
{
  //* Read settings from the file
  //  PndCAParam param;
  in >> fParameters;

  //  fSlices[iSlice].Initialize( param );
}

void PndCAGBTracker::WriteEvent(FILE *file) const
{
  // write event to the file

  const int nHits = NHits();
  int written = std::fwrite(&nHits, sizeof(int), 1, file);
  assert(written == 1);
  written = std::fwrite(&fHits[0], sizeof(PndCAGBHit), nHits, file);
  assert(written == nHits);
  std::fflush(file);
  UNUSED_PARAM1(written);
}

bool PndCAGBTracker::SaveTracksInFile(string prefix) const
{
  ofstream out((prefix + "tracks.data").data());
  if (!out.is_open())
    return 0;

  // ostream& out = cout;

  out << fNTracks << std::endl;
  for (int itr = 0; itr < fNTracks; itr++) {
    PndCAGBTrack &t = fTracks[itr];

    for (int ih = t.FirstHitRef(), i = 0; i < t.NHits(); ih++, i++) {
      out << fTrackHits[ih] << " ";
    }
    out << endl;
  }
  return 1;
}

// void PndCAGBTracker::ReadTracks( std::istream &in )
// {
//   //* Read tracks  from file
//
//   in >> fTime;
//   fSliceTrackerTime = fTime;
//   fStatTime[0] += fTime;
//   fStatNEvents++;
//   delete[] fTrackHits;
//   fTrackHits = 0;
//   int nTrackHits = 0;
//   in >> nTrackHits;
//   fTrackHits = new int [nTrackHits];
//   for ( int ih = 0; ih < nTrackHits; ih++ ) {
//     in >> TrackHits()[ih];
//   }
//   delete[] fTracks;
//   fTracks = 0;
//   in >> fNTracks;
//   fTracks = new PndCAGBTrack[fNTracks];
//   for ( int itr = 0; itr < fNTracks; itr++ ) {
//     PndCAGBTrack &t = Tracks()[itr];
//     in >> t;
//   }
// }

void PndCAGBTracker::SetHits(std::vector<PndCAGBHit> &hits)
{
  const int NHits2 = hits.size();

  fNHits = 0;
  fHits.resize(0);
  fHits.reserve(NHits2);
  // cout<<"Z min, max: "<<fParameters.MinZ()<<" "<<fParameters.MaxZ()<<endl;
  for (int iH = 0; iH < NHits2; iH++) {
    PndCAGBHit l = hits[iH];
    // cout<<"hit z: "<<l.Z()<<" r: "<<l.R()<<endl;
    if (fabs(l.Angle()) > 10) {
      // cout<<"read angle "<<l.Angle()<<" station "<<(int) l.IRow()<<endl;
      continue; // skip forward detectors
    }
    if (l.IRow() >= PndCAParameters::MaxNStations) {
      cout << "CA tracker: wrong hit station number: " << (int)l.IRow() << " out of " << PndCAParameters::MaxNStations << endl;
      l.SetIRow(PndCAParameters::MaxNStations - 1);
      continue;
    }
    fHits.push_back(l);
  }
  fNHits = fHits.size();
} // need for StRoot

void PndCAGBTracker::SaveHitsInFile(string prefix) const
{
  ofstream ofile((prefix + "hits.data").data(), std::ios::out | std::ios::app);
  const int Size = fHits.size();
  ofile << Size << std::endl;
  for (unsigned int i = 0; i < fHits.size(); i++) {
    const PndCAGBHit &l = fHits[i];
    ofile << l;
  }
  ofile.close();
}

// void PndCAGBTracker::SaveSettingsInFile(string prefix) const
// {
//   ofstream ofile((prefix+"settings.data").data(),std::ios::out|std::ios::app);
//   WriteSettings(ofile);
// }

bool PndCAGBTracker::ReadHitsFromFile(string prefix)
{
  ifstream ifile((prefix + "hits.data").data());
  if (!ifile.is_open())
    return 0;
  int Size;
  ifile >> Size;

  fHits.clear();
  fHits.reserve(Size);
  fNHits = 0;
  for (int i = 0; i < Size; i++) {
    PndCAGBHit l;
    ifile >> l;
    if (fabs(l.Angle()) > 10) {
      // cout<<"read angle "<<l.Angle()<<" station "<<(int) l.IRow()<<endl;
      continue; // skip forward detectors
    }
    if (l.IRow() >= PndCAParameters::MaxNStations) {
      cout << "CA tracker: wrong hit station number: " << (int)l.IRow() << " out of " << PndCAParameters::MaxNStations << endl;
      l.SetIRow(PndCAParameters::MaxNStations - 1);
      continue;
    }
    fHits.push_back(l);
  }
  fNHits = fHits.size();
  ifile.close();
  return 1;
}

// bool PndCAGBTracker::ReadSettingsFromFile(string prefix)
// {
//   ifstream ifile((prefix+"settings.data").data());
//   if ( !ifile.is_open() ) return 0;
//   ReadSettings(ifile);
//   return 1;
// }

void PndCAGBTracker::CATrackFinder()
{

#ifdef USE_DBG_TIMERS
  TStopwatch timer;
  timer.Start(1);
#endif

  // prepare memory for tracks
  if (fTracks)
    delete[] fTracks;
  if (fTrackHits)
    delete[] fTrackHits;
  fTracks = new PndCAGBTrack[5000];                           // TODO
  fTrackHits = new int[3000 * PndCAParameters::MaxNStations]; // TODO
  fNTracks = 0;

  // std::sort( fHits.Data(), fHits.Data() + fNHits, PndCAGBHit::Compare ); to make convertion faster
  PndCAHits hits(NStations(), fHits.size() * 3 / NStations()); // suppose approximately eaqul nHits per station

  //
  {
    const int nHits = fHits.size();
    for (int i = 0; i < nHits; i++) {
      PndCAGBHit &h = fHits[i];
      h.SetIsLeft(false);
      if (h.IRow() < PndCAParameters::NMVDStations)
        continue;
      const float k = 1.5;                // diff between real size and sigma
      const float errT = 0.02 / k;        // tangential error 2 mm
      const float errN = sqrt(h.Err2R()); // normal error
      {
        const double beta = 0.5 * atan(2 * h.ErrX12() / (h.Err2X2() - h.Err2X1())); // strip angle
        const double C11 = errN * errN;
        const double C22 = h.Err2X2();
        const double s = sin(beta);
        const double c = cos(beta);
        h.SetErr2X0(errT * errT);
        h.SetErrX12(-s * c * (C11 - C22));
        h.SetErr2X1(c * c * C11 + s * s * C22);
        h.SetErr2X2(s * s * C11 + c * c * C22);
      }
    }
  }

  // convert hits & create left-right hits
  // cout<<"Original hits (no l/r) total: "<<fNHits<<endl;
  for (int iH = 0; iH < fNHits; ++iH) {
    PndCAHit h(fHits[iH], iH);
    if (h.IStation() < PndCAParameters::NMVDStations) {
      hits.Add(h);
    } else {
      float u = h.fX1;
      h.fIsLeft = false;
      h.fX1 = u + h.fR;
      hits.Add(h);
      h.fIsLeft = true;
      h.fX1 = u - h.fR;
      hits.Add(h);
    }
  }

  double p6 = TMath::TwoPi() / 6.;
  double p12 = p6 / 2.;

  for (int ist = 0; ist < NStations(); ist++) {
    // cout<<"station "<<ist<<endl;
    const PndCAStation &station = GetParameters().Station(ist);
    const float sb = station.f.sin;
    const float cb = station.f.cos;
    PndCAElementsOnStation<PndCAHit> &stHits = hits.OnStation(ist);
    // cout<<"station "<<ist<<": nhits "<<stHits.size()<<endl;
    for (unsigned int ih = 0; ih < stHits.size(); ih++) {
      PndCAHit &hit = stHits[ih];
      // cout<<(int)ist<<" "<<station.r<<" "<<hit.X0()<<" "<<hit.R()<<endl;
      hit.fU = cb * hit.X1() + sb * hit.X2();
      hit.fDR = (hit.IsLeft() ? hit.R() : -hit.R());
      hit.fU += hit.fDR;
      double ang = hit.Angle();
      // cout<<"angle "<<ang<<endl;
      if (ang < 0)
        ang += TMath::TwoPi();
      hit.fAngle = ang;
      hit.fISec = TMath::Nint((ang - p12) / p6);
      if (ist >= PndCAParameters::NMVDStations) {
        // cout<< hit.fAngle - (p12+p6*hit.fISec)<<endl;
        // if( fabs(hit.fAngle - (p12+p6*hit.fISec) )>.001 ) exit(0);
      }
      if (hit.fISec < 0 || hit.fISec >= 6) {
        cout << "CA tracker: Wrong hit sector " << hit.fISec << endl;
        exit(0);
      }
    }
  }

  // hits.Sort();

  for (int ista = 0; ista < NStations(); ista++) {

    PndCAStationSTT &sta = fStations[ista];
    sta.Init();
    const PndCAStation &station = GetParameters().Station(ista);

    sta.fSin = float_v(station.f.sin);
    sta.fCos = float_v(station.f.cos);

    const PndCAElementsOnStation<PndCAHit> &stHits = hits.OnStationConst(ista);
    for (unsigned int ih = 0; ih < stHits.size(); ih++) {
      const PndCAHit &hit = stHits[ih];
      PndCAHitSTT h1d;
      h1d.fISta = ista;
      h1d.fISec = hit.fISec;
      h1d.fOrigID = ih;
      h1d.fU = hit.U();
      h1d.fDR = hit.DR();
      sta.fHits1D.push_back(h1d);
    }
    std::sort(sta.fHits1D.begin(), sta.fHits1D.end());
    int lastSec = -1;
    for (unsigned int ih = 0; ih < sta.fHits1D.size(); ih++) {
      const PndCAHitSTT &hit = sta.fHits1D[ih];
      int iSec = hit.fISec;
      if (iSec > lastSec) {
        sta.fSectors[iSec].fFirstHit = ih;
        sta.fSectors[iSec].fNHits = 0;
        lastSec = iSec;
      }
      sta.fSectors[iSec].fNHits++;
    }
  }

  // estimate PV
  float xT = 0, yT = 0, zT = 0;

#ifdef USE_DBG_TIMERS
  timer.Stop();
  fGTi["init  "] = timer;
  timer.Start(1);
  TStopwatch itimer;
#endif

  /// Find Tracks
  // set parameters; TODO depend on iteration
  const float kCorr = 1; //.2; // correction on pulls width
  const float kCorr2 = kCorr * kCorr;
  fPick_m = 3. * kCorr;
  fPick_r = 5. * kCorr;
  fPickNeighbour = 3. * kCorr;
  TRACK_PROB_CUT = 0.01;
  TRACK_CHI2_CUT = 10. * kCorr2;
  TRIPLET_CHI2_CUT = 15. * kCorr2; // TMath::Prob(20,-3+1+6) = TMath::Prob(15,2) = 5e-04

  // Set correction in order to take into account overlaping
  // The reason is that low momentum tracks are too curved and goes not from target direction. That's why hit sort is not work idealy
  fMaxDX0 = 0;

  // fMaxInvMom = 2;
  fMaxInvMom = 5;
  // fTarget = PndCATarget( xT, yT, zT, 1, 1, fMaxInvMom/3.f, GetParameters().VtxFieldValue(), 3 ); // 3 so triplets can have NDF=1
  fTarget = PndCATarget(xT, yT, zT, 1, 1, fMaxInvMom, GetParameters().VtxFieldValue(), 3); // 3 so triplets can have NDF=1
  fMaxDX0 = 0;

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["init  "] = itimer;
  itimer.Start(1);
  TStopwatch ptimer;
  ptimer.Start(1);
#endif

  // cout<<"MaxCellLength = "<<PndCAParameters::MaxCellLength<<endl;

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["0plet "] = itimer;
  itimer.Start(1);
#endif

  // reconstruct
  int maxCellLength = PndCAParameters::MaxCellLength;
  maxCellLength = 6;

  PndCANPletsV pletsV(NStations(), &hits);

  int iMin = (maxCellLength < PndCAParameters::LastCellLength) ? maxCellLength : PndCAParameters::LastCellLength;

  for (char iS = 0; iS < NStations() - iMin; iS += StartStationShift) {
    CreateNPlets(fTarget, hits, pletsV.OnStation(iS), iS, maxCellLength - 1);
  }

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  std::stringstream ss;
  ss << 0 << "plet ";
  fTi[0][ss.str()] = itimer;
  itimer.Start(1);
#endif

  PndCANPlets triplets(pletsV);

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["convrt"] = itimer;
  ptimer.Stop();
  fTi[0]["plets "] = ptimer;

  itimer.Start(1);
#endif

  fPick = fPickNeighbour;
  FindNeighbours(triplets);

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["nghbrs"] = itimer;
  itimer.Start(1);
#endif

  PndCATracks tracks(&hits);
  CreateTracks(triplets, tracks);
  // cout<<"n tracks after CreateTracks: "<<tracks.size()<<endl;

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["tracks"] = itimer;
  itimer.Start(1);
#endif

  Merge(tracks);

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["merger"] = itimer;
  itimer.Start(1);
#endif

  // save tracks in compact format

  int curHit = 0; // counters, used to save tracks in output format
  int curTr = 0;

  const int NRTracks = tracks.size();

  for (int iT = 0; iT < NRTracks; iT++) {
    const int NTHits = tracks[iT].NHits();
    PndCAGBTrack &oT = fTracks[curTr];
    oT.SetNHits(NTHits);
    oT.SetFirstHitRef(curHit);
    //#ifdef USE_CA_FIT
    oT.SetOuterParam(tracks[iT].Fit(hits, fTarget, GetParameters()));
    oT.SetInnerParam(tracks[iT].Fit(hits, fTarget, GetParameters(), false));
    //#endif

    for (int iH = 0; iH < NTHits; iH++) {
      fTrackHits[curHit] = tracks.Hit(iH, iT).Id();
      curHit++;
    }
    curTr++;
  }
  fNTracks += NRTracks;

  hits.Clean(); // remove used hits

#ifdef USE_DBG_TIMERS
  itimer.Stop();
  fTi[0]["finish"] = itimer;
  itimer.Start(1);
#endif

#ifdef USE_DBG_TIMERS
  timer.Stop();
  fGTi["iters "] = timer;
#endif
}

inline bool IsRightNeighbour(const PndCANPlet &a, const PndCANPlet &b, float pick, float &chi2)
{
  int start = (a.N() < b.N()) ? 0 : a.N() - b.N();
  for (int i = start; i < a.N() - StartStationShift; i++) {
    if (a.IHit(i + StartStationShift) != b.IHit(i)) {
      return false;
    }
  }
  chi2 = fabs(a.QMomentum() - b.QMomentum()) / sqrt(a.QMomentumErr2() + b.QMomentumErr2());
  if (chi2 > pick)
    return false; // neighbours must have same qp
  chi2 *= chi2;
  return true;
}

// find and store neighbours triplets
// to make recursive search faster saves only neighbours with level = level - 1
void PndCAGBTracker::FindNeighbours(PndCANPlets &triplets)
{
  for (int iS = triplets.NStations() - 1; iS >= 0; --iS) { // CHECKME
    PndCAElementsOnStation<PndCANPlet> &ts1 = triplets.OnStation(iS);

    for (unsigned int iT1 = 0; iT1 < ts1.size(); ++iT1) {
      PndCANPlet &t1 = ts1[iT1];

      int neighIStation = t1.ISta(0) + StartStationShift;

      if (neighIStation >= triplets.NStations())
        continue; // triplets can't start from this station
      PndCAElementsOnStation<PndCANPlet> &ts2 = triplets.OnStation(neighIStation);

      char maxLevel = -1;                           // maxLevel of neighbour triplets
      vector<pair<float, unsigned int>> neighCands; // save neighbour candidates
      for (unsigned int iT2 = 0; iT2 < ts2.size(); ++iT2) {
        const PndCANPlet &t2 = ts2[iT2];
        float chi2;
        if (!IsRightNeighbour(t1, t2, fPick, chi2))
          continue;

        if (maxLevel < t2.Level())
          maxLevel = t2.Level();
        if (maxLevel == t2.Level())
          neighCands.push_back(pair<float, unsigned int>(chi2 + t2.Chi2Level(), iT2));
      }
      t1.Level() = maxLevel + 1;

      // save
      for (unsigned int iN = 0; iN < neighCands.size(); ++iN) {
        const PndCANPlet &t2 = ts2[neighCands[iN].second];

        if (maxLevel == t2.Level()) {
          t1.Neighbours().push_back(neighCands[iN]);
        }
      }
      sort(t1.Neighbours().begin(), t1.Neighbours().end());
      if (t1.NNeighbours()) {
        t1.Chi2Level() = t1.Chi2Neighbours(0);
        const pair<float, unsigned int> tmp = t1.Neighbours()[0]; // leave only one. CHECKME for 1000tracks events
        t1.Neighbours().clear();
        t1.Neighbours().push_back(tmp);
      }
    } // iTrip1

    // sort( ts1.begin(), ts1.end(), PndCANPlet::compare );
  } // iStation
}

void PndCAGBTracker::CreateTracks(const PndCANPlets &triplets, PndCATracks &tracks)
{
  const int Nlast = PndCAParameters::LastCellLength; // N hits in the rightmost triplet

  int min_level = 1; // min level to start triplet. So min track length = min_level+3.

  // collect consequtive: the longest tracks, shorter, more shorter and so on
  for (int ilev = NStations() - Nlast; ilev >= min_level; ilev--) { // choose length
    PndCATracks vtrackcandidate(tracks.HitsRef());

    //  how many levels to check
    const unsigned char min_best_l = (ilev > min_level) ? ilev - 1 : min_level; // lose maximum one hit and find all hits for min_level
                                                                                // const unsigned char min_best_l = ilev + 3; // find all hits (slower)

    // find candidates
    for (int istaF = 0; istaF <= NStations() - Nlast - ilev; istaF++) {
      const PndCAElementsOnStation<PndCANPlet> &tsF = triplets.OnStation(istaF);
      for (unsigned int iTrip = 0; iTrip < tsF.size(); iTrip++) {
        const PndCANPlet *tripF = &(tsF[iTrip]);

        if (0) { // ghost supression !!!
          if (tripF->Level() == 0)
            continue; // ghost suppression // find track with 3 hits only if it was created from a chain of triplets, but not from only one triplet
          if (tripF->Level() < ilev)
            continue; // try only triplets, which can start track with ilev+3 length. w\o it have more ghosts, but efficiency either
          if ((ilev == 0) && (tripF->ISta(0) != 0))
            continue; // ghost supression // collect only MAPS tracks-triplets
        } else if (tripF->Level() < min_best_l)
          continue;

        PndCATrack curr_tr;

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

        PndCATrack best_tr = curr_tr;
        unsigned int nCalls = 0;
        FindBestCandidate(istaF, best_tr, iTrip, curr_tr, min_best_l, triplets, nCalls);

        if (best_tr.Level() < min_best_l)
          continue;
        if (best_tr.NHits() < PndCAParameters::MinimumHitsForRecoTrack)
          continue;

        //       if ( best_tr.Fit( *tracks.HitsRef(), fTarget, GetParameters(), tripF->QMomentum() ).QPt() > 0.5*fMaxInvMom ) continue; // fit to determine Chi2 and NDF

        vtrackcandidate.push_back(best_tr);
        // best_tr.SetHitsAsUsed(*tracks.HitsRef()); TODO
        // tracks.push_back(best_tr);
      }
    } // istaF

    // select and save best candidates
    vtrackcandidate.SelectAndSaveTracks(tracks);
  } // ilev
}

// --------- Merger ---------

void PndCAGBTracker::InvertCholetsky(float a[15]) const
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

void PndCAGBTracker::MultiplySS(float const C[15], float const V[15], float K[5][5]) const
{
  // multiply 2 symmetric matricies
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

void PndCAGBTracker::MultiplyMS(float const C[5][5], float const V[15], float K[15]) const
{
  // multiply symmetric and nonsymmetric matricies
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

void PndCAGBTracker::MultiplySR(float const C[15], float const r_in[5], float r_out[5]) const
{
  // multiply vector and symmetric matrix
  r_out[0] = r_in[0] * C[0] + r_in[1] * C[1] + r_in[2] * C[3] + r_in[3] * C[6] + r_in[4] * C[10];
  r_out[1] = r_in[0] * C[1] + r_in[1] * C[2] + r_in[2] * C[4] + r_in[3] * C[7] + r_in[4] * C[11];
  r_out[2] = r_in[0] * C[3] + r_in[1] * C[4] + r_in[2] * C[5] + r_in[3] * C[8] + r_in[4] * C[12];
  r_out[3] = r_in[0] * C[6] + r_in[1] * C[7] + r_in[2] * C[8] + r_in[3] * C[9] + r_in[4] * C[13];
  r_out[4] = r_in[0] * C[10] + r_in[1] * C[11] + r_in[2] * C[12] + r_in[3] * C[13] + r_in[4] * C[14];
}

void PndCAGBTracker::FilterTracks(float const r[5], float const C[15], float const m[5], float const V[15], float R[5], float W[15], float &chi2) const
{
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

void PndCAGBTracker::Merge(PndCATracks &tracks)
{
  const int NTracksS = tracks.size();
  vector<vector<pair<float, int>>> InNeighbour(NTracksS);
  vector<vector<pair<float, int>>> OutNeighbour(NTracksS);

  vector<PndCATrackParam> fittedTracks;
  fittedTracks.reserve(NTracksS);
  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    PndCATrack &t1 = tracks[iT1];
    // CHECKME why fit backward doesn't help. CHECKME why fit without target doesn't work
    PndCATrackParam p1 = t1.Fit(*tracks.HitsRef(), fTarget, GetParameters());
    fittedTracks.push_back(p1);
  }

  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    PndCATrack &t1 = tracks[iT1];
    if (t1.NHits() <= 0)
      continue;

    const PndCATrackParam &p1 = fittedTracks[iT1];

    for (int iT2 = 0; iT2 < NTracksS; iT2++) {
      PndCATrack &t2 = tracks[iT2];
      if (t2.NHits() <= 0)
        continue;

      const int nStaDiff = t2.IHits().front().s - t1.IHits().back().s;
      if (nStaDiff <= 0)
        continue;

      PndCATrackParam p2 = fittedTracks[iT2];
      // if ( t2.NHits() >= t1.NHits() ) // CHECK me: why it is better to always use p2
      p2.Transport((*tracks.HitsRef())[t1.IHits().back()], GetParameters().cBz());
      // else
      //   p1.Transport( (*tracks.HitsRef())[t2.IHits().back()], GetParameters().cBz() );

      float C[15], r[5], chi2(0);
      FilterTracks(p1.Par(), p1.Cov(), p2.Par(), p2.Cov(), r, C, chi2);

      if (chi2 > 20)
        continue;            // TMath::Prob(20,5) = 1.25e-03
      chi2 += 10 * nStaDiff; // nStaDiff is more important than chi2 - this is an empiric choice
      OutNeighbour[iT1].push_back(pair<float, int>(chi2, iT2));
      InNeighbour[iT2].push_back(pair<float, int>(chi2, iT1));
    } // iT2
  }   // iT1

  // sort by chi2
  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    sort(InNeighbour[iT1].begin(), InNeighbour[iT1].end());
    sort(OutNeighbour[iT1].begin(), OutNeighbour[iT1].end());
  }

  // combine best neighbours. TODO: speed up by sorting all pairs together by chi2
  bool allPairsFound = false;
  while (!allPairsFound) {
    allPairsFound = true;

    // { // dbg
    //   cout << " new " << endl;
    //   for ( int iT1 = 0; iT1 < NTracksS; iT1++ ) {
    //     for( unsigned int iN = 0; iN < InNeighbour[iT1].size(); iN++ ) {
    //       cout << iT1 << " " << iN << " I " << InNeighbour[iT1][iN].first << " " << InNeighbour[iT1][iN].second << endl;
    //     }
    //     for( unsigned int iN = 0; iN < OutNeighbour[iT1].size(); iN++ ) {
    //       cout << iT1 << " " <<  iN << " O " <<  OutNeighbour[iT1][iN].first << " " << OutNeighbour[iT1][iN].second << endl;
    //     }
    //   }
    // }

    for (int iT1 = 0; iT1 < NTracksS; iT1++) {
      PndCATrack &t1 = tracks[iT1];
      if (t1.NHits() <= 0)
        continue;

      // find best unused outer track
      int iT2 = -1;
      for (unsigned int iN = 0; iN < OutNeighbour[iT1].size(); iN++) {
        iT2 = OutNeighbour[iT1][iN].second;
        if (iT2 >= 0) {
          break;
        }
      }
      if (iT2 < 0)
        continue;

      // find best unused inner track for outer track
      int iT21 = -1;
      for (unsigned int iN = 0; iN < InNeighbour[iT2].size(); iN++) {
        iT21 = InNeighbour[iT2][iN].second;
        if (iT21 >= 0) {
          break;
        }
      }
      assert(iT21 >= 0); // at least iT1 should be found

      if (iT1 != iT21) {
        allPairsFound = false;
        continue; // find them at the next iteration
      }
      PndCATrack &t2 = tracks[iT2];

      // atach track
      for (int ih = 0; ih < t2.NHits(); ih++) {
        t1.AddHit(t2.IHits()[ih]);
      }
      t2.IHits().clear();

      // clean connections
      for (unsigned int iN = 0; iN < OutNeighbour[iT1].size(); iN++) {
        const int iT22 = OutNeighbour[iT1][iN].second;
        if (iT22 < 0)
          continue;
        for (unsigned int iN2 = 0; iN2 < InNeighbour[iT22].size(); iN2++) {
          if (InNeighbour[iT22][iN2].second == iT1) {
            InNeighbour[iT22][iN2].second = -1;
            break;
          }
        }
      }
      for (unsigned int iN = 0; iN < InNeighbour[iT2].size(); iN++) {
        const int iT22 = InNeighbour[iT2][iN].second;
        if (iT22 < 0)
          continue;
        for (unsigned int iN2 = 0; iN2 < OutNeighbour[iT22].size(); iN2++) {
          if (OutNeighbour[iT22][iN2].second == iT2) {
            OutNeighbour[iT22][iN2].second = -1;
            break;
          }
        }
      }
      for (unsigned int iN = 0; iN < OutNeighbour[iT2].size(); iN++) {
        const int iT22 = OutNeighbour[iT2][iN].second;
        if (iT22 < 0)
          continue;
        for (unsigned int iN2 = 0; iN2 < InNeighbour[iT22].size(); iN2++) {
          if (InNeighbour[iT22][iN2].second == iT2) {
            InNeighbour[iT22][iN2].second = iT1;
            break;
          }
        }
      }
      OutNeighbour[iT1] = OutNeighbour[iT2];
      InNeighbour[iT2].clear();
      OutNeighbour[iT2].clear();
      // check the newly created track
      iT1--;
    }
  } // allPairsFound

  // remove tracks, which were atached to other tracks

  PndCATracks tracks_saved = tracks;
  tracks.clear();
  for (int iT1 = 0; iT1 < NTracksS; iT1++) {
    PndCATrack &t1 = tracks_saved[iT1];
    if (t1.NHits() != 0)
      tracks.push_back(t1);
  }
  // currently is not needed
}

void PndCAGBTracker::FindBestCandidate(int ista,
                                       PndCATrack &bT, // best track
                                       int currITrip,  // index of current triplet
                                       PndCATrack &cT, // current track
                                       unsigned char min_best_l, const PndCANPlets &triplets, unsigned int &nCalls)
{
  //  if (nCalls > 100) return; // avoid long processing in confusing cases
  nCalls++;

  const PndCAElementsOnStation<PndCANPlet> &trs = triplets.OnStation(ista);
  const PndCANPlet *curr_trip = &(trs[currITrip]);

  if (curr_trip->Level() == 0) { // the end of the track -> check and store

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
      const PndCANPlet *new_trip = &(triplets.OnStation(curr_trip->ISta(0) + StartStationShift)[newITrip]);

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
        PndCATrack nT = cT; // new track

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
  }     // level = 0
}

void PndCAGBTracker::Create1Plets(const PndCATarget &target, const PndCAHits &hits, PndCAElementsOnStation<PndCANPletV> &r, int iS)
{

  // const PndCAStation &station = GetParameters().Station( iS );

  const PndCAElementsOnStation<PndCAHit> &hs = hits.OnStation(iS);

  r.clear();
  r.reserve(hs.size() / float_v::Size + 1);

  TrackHitRecord hitRecord;
  hitRecord.fPrevHit = -1;
  hitRecord.fStation = iS;

  for (unsigned int iH = 0; iH < hs.size(); iH += float_v::Size) {
    float_m valid = static_cast<float_m>(uint_v::IndexesFromZero() < uint_v(hs.size() - iH));

    PndCAHitV hit(&(hs[iH]), valid);
    PndCATrackParamVector param;
    float_m active = hit.IsValid();

    if (1) { // start from target

      // float_v d2QMom_Init=float_v(25.0);
      // param.InitCovMatrix(d2QMom_Init);
      param.InitByTarget(target);
      param.InitDirection(hit.X0() - target.X0(), hit.X1() - target.X1(), hit.X2() - target.X2());

      param.SetAngle(hit.Angle());
      param.SetISec(hit.ISec());
      active &= param.Transport0ToX(hit.X0(), GetParameters().cBz(), float_m(true));
      active &= param.Filter(hit, GetParameters(), active);

    } else { // start from hit -> does not work for some reason
      /*
      param.InitByHit( hit, GetParameters(), target.DQMom() );
      param.InitDirection( hit.X0() - target.X0(), hit.X1() - target.X1(), hit.X2() - target.X2() ); // works only of t.X() = Y() = 0
      param.SetAngle( hit.Angle() );
      param.SetISec( hit.ISec() );
      active &= param.AddTarget( target, active );
      */
    }

    if (active.isEmpty())
      continue;
    uint_v iHit = uint_v::IndexesFromZero() + iH;
    r.resize(r.size() + 1);
    PndCANPletV &nPlet = r.back();
    nPlet.fNHits = 1;
    nPlet.fParam = param;
    nPlet.fIsValid = active;
    nPlet.fLastHit = -1;
    foreach_bit(unsigned int iBit, active)
    {
      hitRecord.fIHit = iHit[iBit];
      nPlet.fLastHit[iBit] = gTrackHitRecords.size();
      gTrackHitRecords.push_back(hitRecord);
    }
  }
}

void PndCAGBTracker::CreateNPlets(const PndCATarget &target, const PndCAHits &hits, PndCAElementsOnStation<PndCANPletV> &nPlets, int iS, int cellLength)
{
  // if( iS<PndCAParameters::NMVDStations ) return;//SG!!

  PndCAElementsOnStation<PndCANPletV> tmp0;
  PndCAElementsOnStation<PndCANPletV> tmp1;
  tmp0.fHitsRef = &hits;
  tmp0.fISta = iS;
  tmp1.fHitsRef = &hits;
  tmp1.fISta = iS;

  PndCAElementsOnStation<PndCANPletV> *rCurr = &tmp0;
  PndCAElementsOnStation<PndCANPletV> *rNext = &tmp1;

  Create1Plets(target, hits, *rCurr, iS);

  for (int iLen = 1; iLen <= cellLength; iLen++) {
    if (rCurr->size() <= 0)
      break;
    if ((*rCurr)[0].N() >= GetParameters().Station(iS).CellLength)
      break;
    rNext->clear();
    PickUpHits(*rCurr, *rNext, iS + iLen);
    PndCAElementsOnStation<PndCANPletV> *rr = rCurr;
    rCurr = rNext;
    rNext = rr;
    // cout<<" station "<<iS<<" created "<<rCurr->size()<<" plets of length "<<iLen<<endl;
  }
  nPlets = *rCurr;
}

struct FitStore {
  float_v F0, F1, F2, du, err2U;
  PndCATrackParamVector param;
  int_v fLastHit;
  int fNHits;
  int_v iHit;
};

vector<FitStore> store(3000);

void PndCAGBTracker::PickUpHits(PndCAElementsOnStation<PndCANPletV> &a, PndCAElementsOnStation<PndCANPletV> &r, int iS)
{

  // new(&r) PndCAElementsOnStation<PndCANPletV>( a.HitsRef() );

  r.SetStation(a.IStation());
  r.clear();

  if (a.size() <= 0)
    return;

  r.reserve(5 * a.size());

  // const float_v Pick2 = float_v(3.5*3.5);//fPick*fPick;
  const float_v Pick2 = float_v(10.0 * 10.0); // fPick*fPick;

  // int iS = a.IStation()+ N-1;

  const PndCAHits *allHits = a.HitsRef();
  const PndCAElementsOnStation<PndCAHit> &hits = allHits->OnStationConst(iS);

  const PndCAStation &station = GetParameters().Station(iS);
  PndCAStationSTT &stationMy = fStations[iS];

  const float_v p6 = float_v(TMath::TwoPi() / 6.);
  const float_v p12 = float_v(TMath::TwoPi() / 12.);

  if (station.NDF == 1) { // STT tubes

    const float_v sb = stationMy.fSin;
    const float_v cb = stationMy.fCos;

    store.resize(1);

    unsigned int vN = 0, vM = 0;
    float_v err2R = stationMy.fResolution;
    for (unsigned int iD1 = 0; iD1 < a.size(); ++iD1) {
      PndCANPletV &D1 = a[iD1];
      float_m valid1G = D1.IsValid();
      if (valid1G.isEmpty())
        continue;
      PndCATrackParamVector &param = D1.ParamRef();
      if (iS == PndCAParameters::NMVDStations) {
        float_v secAngle = p12 + p6 * param.ISec();
        valid1G &= param.Rotate(-param.Angle() + secAngle, .999f, valid1G);
      }
      valid1G &= param.Transport0(int_v(iS), GetParameters(), valid1G);
      if (valid1G.isEmpty())
        continue;

      D1.SetValid(valid1G);

      const float_v &c00 = param.fC[0];
      const float_v &c10 = param.fC[1];
      const float_v &c11 = param.fC[2];
      const float_v &c20 = param.fC[3];
      const float_v &c21 = param.fC[4];

      // F = CH'
      const float_v F0 = cb * c00 + sb * c10;
      const float_v F1 = cb * c10 + sb * c11;
      const float_v F2 = cb * c20 + sb * c21;
      const float_v HCH = (F0 * cb + F1 * sb);
      const float_v err2U = (HCH + err2R);
      const float_v pickUp2 = Pick2 * err2U;
      float_v trSinPhiU = cb * param.SinPhi() + sb * param.DzDs();
      float_v trU = cb * param.Y() + sb * param.Z();
      float_v trUCorr = -rsqrt(float_v(1.f) - trSinPhiU * trSinPhiU);

      int secMin = param.fISec.min(valid1G);
      int secMax = param.fISec.max(valid1G);

      for (int iSec = secMin; iSec <= secMax; iSec++) {

        int_m sectorOK = valid1G & (int_v(iSec) == param.ISec());
        if (sectorOK.isEmpty())
          continue;

        PndCAStationSTTSector &sector = stationMy.fSectors[iSec];

        // if( sector.fNHits>30 ) continue; //SG!!!
        for (int jh = 0; jh < sector.fNHits; jh++) {
          PndCAHitSTT &h1d = stationMy.fHits1D[sector.fFirstHit + jh];
          // if( iS==18+4 ) cout<<iSec<<" "<<jh<<" "<<h1d.fU<<" "<<h1d.fDR<<endl;
          float_v hitU = h1d.fU + h1d.fDR * trUCorr;
          const float_v du = trU - hitU;

          float_m active = sectorOK;
          active &= (du * du <= pickUp2);
          if (active.isEmpty())
            continue;

          // active &= abs( param.fP[2]*err2U - F2*du ) < float_v(.999f)*err2U;
          // if( active.isEmpty() ) continue;

          foreach_bit(unsigned int iBit, active)
          {
            if (vM == float_v::Size) {
              vM = 0;
              vN++;
              store.resize(vN + 1);
            }
            FitStore &s = store[vN];
            s.F0[vM] = F0[iBit];
            s.F1[vM] = F1[iBit];
            s.F2[vM] = F2[iBit];

            s.du[vM] = du[iBit];
            s.err2U[vM] = err2U[iBit];
            s.iHit[vM] = h1d.fOrigID;
            s.fLastHit[vM] = D1.fLastHit[iBit];
            s.fNHits = D1.fNHits;
            s.param.SetTrackParam(param, vM, iBit);
            vM++;
          }
        } // ih
      }   // iSec
    }     // iD1

    for (unsigned int i = 0; i <= vN; i++) {
      if (i == vN && vM == 0)
        break;
      FitStore &s = store[i];
      TrackHitRecord hitRecord;
      hitRecord.fStation = iS;
      r.resize(r.size() + 1);
      PndCANPletV &nPlet = r.back();
      nPlet.fLastHit = int_v(-1);
      nPlet.fNHits = s.fNHits + 1;
      nPlet.fParam = s.param;
      unsigned int nBit = (i < vN) ? ((unsigned int)float_v::Size) : vM;

      nPlet.fIsValid = (uint_v::IndexesFromZero() < nBit);

      for (unsigned int iBit = 0; iBit < nBit; iBit++) {
        hitRecord.fIHit = s.iHit[iBit];
        hitRecord.fPrevHit = s.fLastHit[iBit];
        nPlet.fLastHit[iBit] = gTrackHitRecords.size();
        gTrackHitRecords.push_back(hitRecord);
      }

      PndCATrackParamVector &p = nPlet.fParam;
      const float_v &c30 = p.fC[6];
      const float_v &c31 = p.fC[7];
      const float_v &c40 = p.fC[10];
      const float_v &c41 = p.fC[11];

      const float_v zeta = s.du;
      const float_v wi = float_v(1.f) / s.err2U;
      const float_v zetawi = zeta * wi;

      const float_v F0 = s.F0;
      const float_v F1 = s.F1;
      const float_v F2 = s.F2;
      const float_v F3 = cb * c30 + sb * c31;
      const float_v F4 = cb * c40 + sb * c41;

      const float_v K0 = F0 * wi;
      const float_v K1 = F1 * wi;
      const float_v K2 = F2 * wi;
      const float_v K3 = F3 * wi;
      const float_v K4 = F4 * wi;

      p.fNDF += 1;
      p.fChi2 += zeta * zetawi;

      p.fP[0] += -F0 * zetawi;
      p.fP[1] += -F1 * zetawi;
      p.fP[2] += -F2 * zetawi;
      p.fP[3] += -F3 * zetawi;
      p.fP[4] += -F4 * zetawi;

      p.fC[0] -= K0 * F0;

      p.fC[1] -= K1 * F0;
      p.fC[2] -= K1 * F1;

      p.fC[3] -= K2 * F0;
      p.fC[4] -= K2 * F1;
      p.fC[5] -= K2 * F2;

      p.fC[6] -= K3 * F0;
      p.fC[7] -= K3 * F1;
      p.fC[8] -= K3 * F2;
      p.fC[9] -= K3 * F3;

      p.fC[10] -= K4 * F0;
      p.fC[11] -= K4 * F1;
      p.fC[12] -= K4 * F2;
      p.fC[13] -= K4 * F3;
      p.fC[14] -= K4 * F4;
    }

  } else { // NDF = 2
    // return;
    for (unsigned int iD1 = 0; iD1 < a.size(); ++iD1) {
      PndCANPletV &D1 = a[iD1];
      float_m valid1G = D1.IsValid();
      if (valid1G.isEmpty())
        continue;
      // PndCATrackParamVector &param = D1.ParamRef();
      // valid1G &= param.Transport( int_v(iS), GetParameters(), valid1G );
      // D1.SetValid( valid1G );

      for (unsigned int ih = 0; ih < hits.size(); ih++) {
        const PndCAHit &hit = hits[ih];
        float_m active = valid1G; // & ( abs(hit.Angle() - param.Angle())<.1f );
        PndCATrackParamVector param = D1.ParamRef();
        active &= param.Transport0(hit, GetParameters(), valid1G);

        if (active.isEmpty())
          continue;
        const float_v dx1 = hit.X1() - param.X1();
        active &= dx1 * dx1 < Pick2 * (param.Err2X1() + hit.Err2X1());
        const float_v dx2 = hit.X2() - param.X2();
        active &= dx2 * dx2 < Pick2 * (param.Err2X2() + hit.Err2X2());
        if (active.isEmpty())
          continue;
        active &= param.Accept(hit, GetParameters(), active, TRIPLET_CHI2_CUT);
        if (active.isEmpty())
          continue;
        PndCATrackParamVector param1 = param;
        active &= param1.Filter(hit, GetParameters(), active, TRIPLET_CHI2_CUT);
        if (active.isEmpty())
          continue;

        TrackHitRecord hitRecord;
        hitRecord.fStation = iS;
        hitRecord.fIHit = ih;
        param1.fISec = hit.fISec;
        PndCANPletV nPlet(D1, param1, active);
        nPlet.fLastHit = -1;
        foreach_bit(unsigned int iBit, active)
        {
          nPlet.fLastHit[iBit] = gTrackHitRecords.size();
          hitRecord.fPrevHit = D1.fLastHit[iBit];
          gTrackHitRecords.push_back(hitRecord);
        }
        r.push_back(nPlet);
      } //
    }   //
  }
}
