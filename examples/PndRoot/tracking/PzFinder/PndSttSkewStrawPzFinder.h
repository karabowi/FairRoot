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
 * PndSttSkewStrawPzFinder.h
 *
 *  Created on: Mar 17, 2016
 *      Author: walan603
 */

#ifndef PNDSTTSKEWSTRAWPZFINDER_H_
#define PNDSTTSKEWSTRAWPZFINDER_H_

#include <iostream>

// Root includes
#include <TH2D.h>
#include "TClonesArray.h"

// Pandaroot includes
#include <PndSttSkewStrawPzFinderData.h>
//#include "PndSttStrawMap.h"
#include <PndRiemannTrack.h>

class PndSttHit;
class FairHit;
class PndTrack;
class PndTrackCand;

class TH2D;

using namespace std;

class PndSttSkewStrawPzFinder {
 public:
  PndSttSkewStrawPzFinder(TClonesArray *sttTubeArray, TClonesArray *sttHitArray);

  virtual ~PndSttSkewStrawPzFinder() {}

  void WithRiemann(bool set) { fWithRiemann = set; };

  int getMethod() const { return fMethod; }
  void setMethod(int input) { fMethod = input; }

  void setStepTheta(double input) { fStepTheta = input; }
  void setStepR(int input) { fStepR = input; }

  void AddPndRiemannTracks(vector<PndRiemannTrack> AllRiemannTracks);

  void AddPndTracks(vector<PndTrack> AllTracks);

  void AddPndTrackCands(vector<PndTrackCand> AllTrackCands);

  void AddPndSttHits(vector<vector<PndSttHit>> AllSkewedHits);

  void AddMvdPixelHits(vector<vector<FairHit>> AllMvdPixelHits);

  void AddMvdStripHits(vector<vector<FairHit>> AllMvdStripHits);

  void AddGemHits(vector<vector<FairHit>> AllMvdStripHits);

  void ExtractPz();

  PndRiemannTrack GetRiemannTrack(int i) { return fResultPndRiemannTrack[i]; };

  PndTrack GetTrack(int i) { return fResultPndTrack[i]; };

  PndTrackCand GetTrackCand(int i) { return fResultPndTrackCand[i]; };

  PndSttSkewStrawPzFinderData GetPzData(int i) { return fResultPzData[i]; };

  vector<PndSttHit> GetSttHits(int i) { return fResultSkewedSttHits[i]; };

  vector<FairHit> GetMvdPixelHits(int i) { return fResultMvdPixelHits[i]; };

  vector<FairHit> GetMvdStripHits(int i) { return fResultMvdStripHits[i]; };

  vector<FairHit> GetGemHits(int i) { return fResultGemHits[i]; };

  int NumPndRiemannTracks() { return fResultPndRiemannTrack.size(); };

  int NumPndTracks() { return fResultPndTrack.size(); };

  int NumPndTrackCands() { return fResultPndTrackCand.size(); };

  int NumPndPzData() { return fResultPzData.size(); };

  int NumPndSttHits() { return fResultSkewedSttHits.size(); };

  int NumPndMvdPixelHits() { return fResultMvdPixelHits.size(); };

  int NumPndMvdStripHits() { return fResultMvdStripHits.size(); };

  int NumPndGemHits() { return fResultGemHits.size(); };

  void Reset()
  {

    fVectorPndRiemannTrack.clear();
    fVectorPndTrack.clear();
    fVectorPndTrackCand.clear();
    fVectorSkewedSttHits.clear();
    fVectorMvdPixelHits.clear();
    fVectorMvdStripHits.clear();
    fVectorGemHits.clear();

    fResultPndRiemannTrack.clear();
    fResultPndTrack.clear();
    fResultPndTrackCand.clear();
    fResultPzData.clear();
    fResultSkewedSttHits.clear();
    fResultMvdPixelHits.clear();
    fResultMvdStripHits.clear();
    fVectorGemHits.clear();

    if (fVerbose > 0) {
      cout << "PndSttSkewStrawPzFinder::Reset()" << endl;
    }
  }

 private:
  bool fWithRiemann;
  int fVerbose;
  int fMethod;
  Double_t fStepTheta;
  Int_t fStepR;
  Double_t LineCombiAngleThreshold;
  TClonesArray *fTubeArray;
  TClonesArray *fSTTHits;
  //PndSttStrawMap *fStrawMap; // for getting more information about the tubes

  TH2D *fHoughHisto; // Line Hough histogram

  // Vectors of initial tracks and hits
  vector<PndRiemannTrack> fVectorPndRiemannTrack;
  vector<PndTrack> fVectorPndTrack;
  vector<PndTrackCand> fVectorPndTrackCand;

  vector<vector<PndSttHit>> fVectorSkewedSttHits;
  vector<vector<FairHit>> fVectorMvdPixelHits;
  vector<vector<FairHit>> fVectorMvdStripHits;
  vector<vector<FairHit>> fVectorGemHits;

  // Vectors of final tracks and hits that extracted Pz
  vector<PndRiemannTrack> fResultPndRiemannTrack;
  vector<PndTrack> fResultPndTrack;
  vector<PndTrackCand> fResultPndTrackCand;

  vector<vector<PndSttHit>> fResultSkewedSttHits;
  vector<vector<FairHit>> fResultMvdPixelHits;
  vector<vector<FairHit>> fResultMvdStripHits;
  vector<vector<FairHit>> fResultGemHits;

  // Additional data produced by PzFinder
  vector<PndSttSkewStrawPzFinderData> fResultPzData;

  //	void InitSkewed();

  void GetCenterOfTrack(PndTrack temp, TVector2 &center, double &radius);

  Int_t ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2);

  TVector2 ComputeTangentInPoint(double xc, double yc, TVector2 point);

  //	vector<vector<TVector2>> LineCombiIsoFinder(vector<vector<TVector2>> SZPairVector);
  //	vector<pair<unsigned int,unsigned int>> LineCombiIsoFinderIndex(vector<vector<TVector2>> SZPairVector);
  vector<pair<unsigned int, unsigned int>>
  LineCombiIsoFinderIndex2(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem);

  // TODO: refine Hough transform part
  //	vector<vector<TVector2>> HoughTrueIsoFinder(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2> SZPairVectorMvdStrip, TH2D
  //*HoughSpace);
  vector<pair<unsigned int, unsigned int>> HoughTrueIsoFinderIndex(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel,
                                                                   vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem, bool draw);

  //	vector<vector<TVector2>> HoughTrueIsoFinder2(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2> SZPairVectorMvdStrip, TH2D
  //*HoughSpace);

  //	TVector2 TheilSen(vector<vector<TVector2>> SZPairVector);

  vector<vector<PndSttHit>> MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad, vector<PndSttHit> skewhits, vector<vector<PndSttHit>> &corrxy);

  void NestedFor(vector<int> &times, vector<int> &current, pair<vector<int>, double> &best, double sum, vector<vector<pair<vector<int>, double>>> matrix, unsigned int depth);

  //	TVector2 PzLineFitExtract(vector<TVector2> TrueSZ);

  //	TVector2 PzLineFitExtract2(vector<vector<TVector2>> TrueSZPairVector);

  double PzLineFitExtract3(vector<vector<TVector2>> TrueSZPairVector, vector<TVector2> TrueSZPairVectorMvdPixel, vector<TVector2> TrueSZPairVectorMvdStrip,
                           vector<TVector2> TrueSZPairVectorGem, TVector2 &lineparams);

  void LineFitRecursiveAnnealFinder(vector<vector<TVector2>> SZPairVector, vector<double> DriftRadVector, vector<TVector2> SZPairVectorMvdPixel,
                                    vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem, vector<pair<unsigned int, unsigned int>> &TrueSZPairVectorindex,
                                    vector<unsigned int> &TrueSZPairVectorMvdPixelindex, vector<unsigned int> &TrueSZPairVectorMvdStripindex,
                                    vector<unsigned int> &TrueSZPairVectorGemindex);

  FairTrackParP GetTrackParam(FairTrackParP oldParam, TVector2 center, double radius, TVector2 lineparams, double S0, int charge);

  Double_t CalculateResidual(Double_t lineSlope, Double_t lineIntercept, TVector2 SZPoint);

  void LineFit(vector<vector<TVector2>> SZPairVector, vector<double> DriftRadVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2> SZPairVectorMvdStrip,
               vector<TVector2> SZPairVectorGem, double &k, double &m);
  void DrawOpt2D(TH2D *ShitSpace);

  ClassDef(PndSttSkewStrawPzFinder, 1);
};

#endif /* PNDSTTSKEWSTRAWPZFINDER_H_ */
