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
 * PndSttSkewStrawPzFinder.cxx
 *
 *  Created on: Mar 17, 2016
 *      Author: walan603
 */

#include "PndSttSkewStrawPzFinder.h"
//#include "PndHelixPropagator.h"

// ROOT includes	REMOVE WHEN DONE DEVELOPING
#include "TCanvas.h"
#include "TH2D.h"

// For sorting skew straws according to layer
struct sort_pred {

  bool operator()(const std::pair<PndSttHit, int> &left, const std::pair<PndSttHit, int> &right) { return left.second < right.second; }
};

/*
 * Public member functions
 */

PndSttSkewStrawPzFinder::PndSttSkewStrawPzFinder(TClonesArray *sttTubeArray, TClonesArray *sttHitArray)
  : fVerbose(0), fTubeArray(sttTubeArray), fSTTHits(sttHitArray), LineCombiAngleThreshold(90.), fStepTheta(90), fStepR(50), fWithRiemann(true)
{
  // default is LineCombiAngleThreshold(90.), fStepTheta(90.), fStepR(50)
  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder initialized" << endl;
  }
  //fStrawMap = new PndSttStrawMap(sttTubeArray);
}

void PndSttSkewStrawPzFinder::AddPndRiemannTracks(vector<PndRiemannTrack> AllRiemannTracks)
{
  fVectorPndRiemannTrack = AllRiemannTracks;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddPndRiemannTracks() added " << AllRiemannTracks.size() << " Riemann tracks." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddPndTracks(vector<PndTrack> AllTracks)
{
  fVectorPndTrack = AllTracks;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddPndTracks() added " << AllTracks.size() << " tracks." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddPndTrackCands(vector<PndTrackCand> AllTrackCands)
{
  fVectorPndTrackCand = AllTrackCands;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddPndTrackCands() added " << AllTrackCands.size() << " tracks." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddPndSttHits(vector<vector<PndSttHit>> AllSkewedHits)
{
  fVectorSkewedSttHits = AllSkewedHits;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddPndSttHits() added " << AllSkewedHits.size() << " vectors of PndSttHits." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddMvdPixelHits(vector<vector<FairHit>> AllMvdPixelHits)
{
  fVectorMvdPixelHits = AllMvdPixelHits;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddMvdPixelHits() added " << AllMvdPixelHits.size() << " vectors of MvdPixelHits." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddMvdStripHits(vector<vector<FairHit>> AllMvdStripHits)
{
  fVectorMvdStripHits = AllMvdStripHits;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddMvdStripHits() added " << AllMvdStripHits.size() << " vectors of MvdStripHits." << endl;
  }
}

void PndSttSkewStrawPzFinder::AddGemHits(vector<vector<FairHit>> AllGemHits)
{
  fVectorGemHits = AllGemHits;

  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::AddGemHits() added " << AllGemHits.size() << " vectors of GemHits." << endl;
  }
}

void PndSttSkewStrawPzFinder::ExtractPz()
{
  if (fVerbose > 0) {
    cout << "PndSttSkewStrawPzFinder::ExtractPz()" << endl;
  }

  // Fill fVectorSkewedSttHits with only SkewedSttHits
  // This method should be run if list of skewed straws are not provided by user
  // InitSkewed();

  vector<vector<PndSttHit>> output;
  /*
   * Index i runs over all Track candidates. Following input TCloneArray's
   * are assumed to have the same index i:
   *
   * fVectorPndTrackCand
   * fVectorPndTrack
   * fVectorPndRiemannTrack
   */
  for (unsigned int i = 0; i < fVectorPndTrackCand.size(); ++i) {

    double riemannR;
    TVector2 riemanncenter;
    if (fWithRiemann) {
      riemannR = fVectorPndRiemannTrack.at(i).r();
      riemanncenter.SetX(fVectorPndRiemannTrack.at(i).orig()[0]);
      riemanncenter.SetY(fVectorPndRiemannTrack.at(i).orig()[1]);
    } else {
      GetCenterOfTrack(fVectorPndTrack.at(i), riemanncenter, riemannR);
    }
    Double_t charge = (Double_t)fVectorPndTrack.at(i).GetParamFirst().GetQ(); // q=1 => clockwise, q=-q 0> anti-clockwise

    if (fVerbose > 0)
      cout << "ExtractSkewedHits - Correcting hits for track: " << i << endl;
    vector<vector<PndSttHit>> Corrxy;
    output = MoveSkewedHitsToCircle(riemanncenter, riemannR, fVectorSkewedSttHits.at(i), Corrxy);
    //		if (output.empty()) {
    //			if (fVerbose > 0 ) cout<<"ExtractSkewedHits - No hits in track: "<<i<<" were adjusted!"<<endl;
    //			continue;
    //		}

    /*
     * Code to calculate starting phi value of track
     */
    //		PndHelixPropagator proptoz0(1.,fVectorPndTrack.at(i).GetParamFirst().GetPosition(),fVectorPndTrack.at(i).GetParamFirst().GetMomentum(),charge);
    //		FairTrackPar parorig = proptoz0.PropagateToZ(0.);
    TVector2 pos0(fVectorPndTrack.at(i).GetParamFirst().GetX() - riemanncenter.X(), fVectorPndTrack.at(i).GetParamFirst().GetY() - riemanncenter.Y());
    //		TVector2 pos0(parorig.GetX() - riemanncenter.X(),parorig.GetY() - riemanncenter.Y());
    Double_t Phi0 = pos0.Phi();
    Double_t S0 = Phi0 * riemannR;
    vector<vector<TVector2>> SZPairVector;
    vector<double> DriftRadVector;
    for (unsigned int j = 0; j < output.size(); ++j) // Loop over all stt hits
    {
      vector<TVector2> SZPair;
      if (output.at(j).empty()) {
        cout << "ExtractSkewedHits - Hit: " << j << " in track: " << i << "was not adjusted adjusted!" << endl;
        continue;
      }
      // cout<<"ExtractSkewedHits - New list of adjusted hits"<<endl;
      for (unsigned int k = 0; k < output.at(j).size(); ++k) // Loop over all possible adjusted hits for one hit
      {
        Double_t x0 = output.at(j).at(k).GetX() - riemanncenter.X();
        Double_t y0 = output.at(j).at(k).GetY() - riemanncenter.Y();
        TVector2 pos(x0, y0);
        Double_t phi = pos.Phi();
        Double_t S = (phi * riemannR - S0) * charge * -1.;
        if (S < 0.)
          S += riemannR * 2. * TMath::Pi();
        TVector2 SZ(S, output.at(j).at(k).GetZ());
        if (fVerbose > 1)
          cout << " S: " << S << "ExtractSkewedHits - SkewHit Z: " << output.at(j).at(k).GetZ() << endl;

        SZPair.push_back(SZ);
      }
      SZPairVector.push_back(SZPair);
      DriftRadVector.push_back(fVectorSkewedSttHits.at(i).at(j).GetIsochrone());
    }

    vector<TVector2> SZPairVectorMvdPixel;
    for (unsigned int j = 0; j < fVectorMvdPixelHits.at(i).size(); ++j) {
      Double_t x0 = fVectorMvdPixelHits.at(i).at(j).GetX() - riemanncenter.X();
      Double_t y0 = fVectorMvdPixelHits.at(i).at(j).GetY() - riemanncenter.Y();
      TVector2 pos(x0, y0);
      Double_t phi = pos.Phi();
      Double_t S = (phi * riemannR - S0) * charge * -1.;
      // Changed by Anna Alicke: in this case the first hit was not found correctly. Therefore the hit is not used for the pz calculation
      // if ((phi - Phi0) < 0)
      //  continue;
      // if (S < 0.)
      //  S += riemannR * 2. * TMath::Pi();
      TVector2 SZ(S, fVectorMvdPixelHits.at(i).at(j).GetZ());
      SZPairVectorMvdPixel.push_back(SZ);
    }
    vector<TVector2> SZPairVectorMvdStrip;
    for (unsigned int j = 0; j < fVectorMvdStripHits.at(i).size(); ++j) {
      Double_t x0 = fVectorMvdStripHits.at(i).at(j).GetX() - riemanncenter.X();
      Double_t y0 = fVectorMvdStripHits.at(i).at(j).GetY() - riemanncenter.Y();
      TVector2 pos(x0, y0);
      Double_t phi = pos.Phi();
      Double_t S = (phi * riemannR - S0) * charge * -1.;
      // if (S < 0.)
      // S += riemannR * 2. * TMath::Pi();
      // Changed by Anna Alicke: in this case the first hit was not found correctly. Therefore the hit is not used for the pz calculation
      // if ((phi - Phi0) < 0)
      //  continue;

      TVector2 SZ(S, fVectorMvdStripHits.at(i).at(j).GetZ());
      SZPairVectorMvdStrip.push_back(SZ);
    }

    vector<TVector2> SZPairVectorGem;
    for (unsigned int j = 0; j < fVectorGemHits.at(i).size(); ++j) {
      Double_t x0 = fVectorGemHits.at(i).at(j).GetX() - riemanncenter.X();
      Double_t y0 = fVectorGemHits.at(i).at(j).GetY() - riemanncenter.Y();
      TVector2 pos(x0, y0);
      Double_t phi = pos.Phi();
      Double_t S = (phi * riemannR - S0) * charge * -1.;
      TVector2 SZ(S, fVectorGemHits.at(i).at(j).GetZ());
      SZPairVectorGem.push_back(SZ);
    }

    bool sttonly = false, mvdgemonly = false, sttmvdgemmix = false;
    if (SZPairVector.size() >= 3) {
      sttonly = true;
    }

    if (SZPairVectorMvdPixel.size() + SZPairVectorMvdStrip.size() + SZPairVectorGem.size() >= 2) {
      mvdgemonly = true;
    }

    if (SZPairVector.size() >= 1 && SZPairVectorMvdPixel.size() + SZPairVectorMvdPixel.size() + SZPairVectorGem.size() >= 1) {
      sttmvdgemmix = true;
    }

    //		if (SZPairVector.size() < 3) {
    //			if (fVerbose > 0) cout<<"<=2 skewed straw hits, Cannot continue!"<<endl;
    //			continue;
    //		}

    if ((!sttonly) && (!mvdgemonly) && (!sttmvdgemmix)) {
      if (fVerbose > 0)
        cout << "Too few hits with z information, cannot continue!" << endl;
      continue;
    }

    if (SZPairVector.size() > 12 && fMethod == 2) {
      if (fVerbose > 0)
        cout << "Too many STT hits to perform combi line finder, cannot continue!" << endl;
      continue;
    }

    /*
     * From here, choose a Z-finding method; Line combi, Hough or RecLinFit
     */

    vector<pair<unsigned int, unsigned int>> TrueSZindex; // indices of selected SZ positions in SZPairVector.
    vector<unsigned int> TrueSZMvdPixelindex;             // indices of selected SZ positions in SZPairVectorMvdPixel.
    vector<unsigned int> TrueSZMvdStripindex;             // indices of selected SZ positions in SZPairVectorMvdStrip.
    vector<unsigned int> TrueSZGemindex;                  // indices of selected SZ positions in SZPairVectorGem.

    vector<TVector2> TrueSZMvdPixel;
    vector<TVector2> TrueSZMvdStrip;
    vector<TVector2> TrueSZGem;
    vector<vector<TVector2>> TrueSZ;
    vector<double> TrueDriftRadVector;
    if (sttonly) {
      // Recursive Annealing Linear Fit
      if (fMethod == 1) {
        LineFitRecursiveAnnealFinder(SZPairVector, DriftRadVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem, TrueSZindex, TrueSZMvdPixelindex,
                                     TrueSZMvdStripindex, TrueSZGemindex);
      }
      // Line combi code
      if (fMethod == 2) {
        //    TrueSZ = LineCombiIsoFinder(SZPairVector);
        //    TrueSZindex = LineCombiIsoFinderIndex(SZPairVector);
        TrueSZindex = LineCombiIsoFinderIndex2(SZPairVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem);
      }

      // Hough transform code
      if (fMethod == 3) {
        //    vector<vector<TVector2>> TrueSZ = HoughTrueIsoFinder(SZPairVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, HoughSpace);
        TrueSZindex = HoughTrueIsoFinderIndex(SZPairVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem, false);
      }

      // use obtained indices from chosen method to create TrueSZ lists
      for (unsigned int j = 0; j < SZPairVector.size(); j++) {
        vector<TVector2> TrueSZentry;
        for (unsigned int k = 0; k < SZPairVector.at(j).size(); k++) {
          for (unsigned int l = 0; l < TrueSZindex.size(); l++) {
            if (j == TrueSZindex.at(l).first && k == TrueSZindex.at(l).second) {
              TrueSZentry.push_back(SZPairVector.at(j).at(k));
            }
          }
        }
        if (TrueSZentry.size() > 0) {
          TrueSZ.push_back(TrueSZentry);
          TrueDriftRadVector.push_back(DriftRadVector.at(j));
        }
      }
    }

    /*
     * Use all skew SZ hits
     */

    TrueSZMvdPixel = SZPairVectorMvdPixel;
    TrueSZMvdStrip = SZPairVectorMvdStrip;
    TrueSZGem = SZPairVectorGem;

    if ((mvdgemonly || sttmvdgemmix) && (!sttonly)) {
      TrueSZ = SZPairVector;
      TrueDriftRadVector = DriftRadVector;
    }

    /*
     * Check that the number of SZ hits within the true lists are enough to actually fit a line
     */

    //		if (TrueSZ.empty()) {
    //			cout<<"No true Zphi was selected"<<endl;
    //			continue;
    //		}

    //		if (TrueSZMvdPixel.size()+TrueSZMvdStrip.size()<2) {
    //			cout<<"No true Zphi was selected"<<endl;
    //			continue;
    //		}

    // Perform a straight line fit with the selected z-phi coordinates
    TVector2 lineparams;
    PzLineFitExtract3(TrueSZ, TrueSZMvdPixel, TrueSZMvdStrip, TrueSZGem, lineparams);
    // Create new track parameters with same x, y, px, py but new pz and z
    FairTrackParP trackParamFirst = GetTrackParam(fVectorPndTrack.at(i).GetParamFirst(), riemanncenter, riemannR, lineparams, S0, charge);
    FairTrackParP trackParamLast = GetTrackParam(fVectorPndTrack.at(i).GetParamLast(), riemanncenter, riemannR, lineparams, S0, charge);

    // check track integrity
    if (isnan(trackParamFirst.GetMomentum().Mag()) || isnan(trackParamFirst.GetPosition().Mag())) {
      std::cout << "*** PndSttSkewStrawPzFinder::ExtractPz : Momentum or position contains NaN element(s). Skipping track ***" << std::endl;
      continue;
    }

    // Generate new PndTrack
    PndTrackCand newCand = fVectorPndTrackCand.at(i);
    PndTrack newTrack(trackParamFirst, trackParamLast, newCand);
    
    // Store track info
    if (fWithRiemann)
      fResultPndRiemannTrack.push_back(fVectorPndRiemannTrack.at(i));
    fResultPndTrackCand.push_back(newCand);
    fResultPndTrack.push_back(newTrack);

    // Store additional PzFinder information
    PndSttSkewStrawPzFinderData resultData;
    resultData.setSttHits(fVectorSkewedSttHits.at(i));
    resultData.setMethod(fMethod);
    resultData.setLineSlope(lineparams.X());
    resultData.setLineIntercept(lineparams.Y());
    resultData.setCenter(riemanncenter);
    resultData.setRadius(riemannR);
    resultData.setS0(S0);

    resultData.setSZPairVector(SZPairVector);
    resultData.setSZPairVectorMvdPixel(SZPairVectorMvdPixel);
    resultData.setSZPairVectorMvdStrip(SZPairVectorMvdStrip);
    resultData.setSZPairVectorGem(SZPairVectorGem);

    resultData.setTrueSZindex(TrueSZindex);

    resultData.setTrueSZ(TrueSZ);
    resultData.setTrueSZMvdPixel(TrueSZMvdPixel);
    resultData.setTrueSZMvdStrip(TrueSZMvdStrip);
    resultData.setTrueSZGem(TrueSZGem);
    resultData.setAlignedSkewStt(Corrxy);
    fResultPzData.push_back(resultData);
  }
}

/*
 * Private member functions
 */

void PndSttSkewStrawPzFinder::GetCenterOfTrack(PndTrack temp, TVector2 &center, double &radius)
{
  Double_t pfirstx = temp.GetParamFirst().GetPx();
  Double_t pfirsty = temp.GetParamFirst().GetPy();
  Double_t plastx = temp.GetParamLast().GetPx();
  Double_t plasty = temp.GetParamLast().GetPy();
  Double_t dpfirstx = pfirsty;  //*temp->GetParamFirst().GetQ();
  Double_t dpfirsty = -pfirstx; //*temp->GetParamFirst().GetQ();
  Double_t dplastx = plasty;    //*temp->GetParamLast().GetQ();
  Double_t dplasty = -plastx;   //*temp->GetParamLast().GetQ();

  Double_t kfirst = dpfirsty / dpfirstx;
  Double_t klast = dplasty / dplastx;

  Double_t mfirst = temp.GetParamFirst().GetY() - kfirst * temp.GetParamFirst().GetX();
  Double_t mlast = temp.GetParamLast().GetY() - klast * temp.GetParamLast().GetX();

  Double_t x0 = (mlast - mfirst) / (kfirst - klast);
  Double_t y0 = klast * x0 + mlast;
  TVector2 r(x0 - temp.GetParamFirst().GetX(), y0 - temp.GetParamFirst().GetY());
  center.SetX(x0);
  center.SetY(y0);
  radius = r.Mod();
}

vector<vector<PndSttHit>> PndSttSkewStrawPzFinder::MoveSkewedHitsToCircle(TVector2 circle, Double_t circlerad, vector<PndSttHit> skewhits, vector<vector<PndSttHit>> &corrxy)
{

  vector<vector<PndSttHit>> result;
  for (unsigned int i = 0; i < skewhits.size(); i++) {
    if (fVerbose > 1)
      cout << "MoveSkewedHitsToCircle() Hit: " << i << " is being adjusted" << endl;
    vector<PndSttHit> tempResult;
    PndSttHit myHit = skewhits.at(i);
    PndSttTube myTube = *(PndSttTube *)fTubeArray->At(myHit.GetTubeID());

    Double_t tubeLengthHalf = myTube.GetHalfLength();
    TVector3 wireDirection = myTube.GetWireDirection();
    TVector3 tubePosition = myTube.GetPosition();

    TVector3 startPoint = tubePosition - (tubeLengthHalf * wireDirection);
    TVector3 endPoint = tubePosition + (tubeLengthHalf * wireDirection);

    TVector2 intersection1, intersection2;
    Int_t nofint = ComputeSegmentCircleIntersection(TVector2(endPoint.X(), endPoint.Y()), TVector2(startPoint.X(), startPoint.Y()), circle.X(), circle.Y(), circlerad,
                                                    intersection1, intersection2);

    if (nofint == 0)
      continue;

    PndSttHit fixhit = myHit;

    if (fVerbose > 1)
      cout << "MoveSkewedHitsToCircle() Intersection1 X: " << intersection1.X() << " Y: " << intersection1.Y() << endl;

    if (nofint == 2) {
      if (fVerbose > 1)
        cout << "MoveSkewedHitsToCircle() Intersection2 X: " << intersection1.X() << " Y: " << intersection1.Y() << endl;
    }

    TVector2 tangent = ComputeTangentInPoint(circle.X(), circle.Y(), intersection1);

    // Rotate the tangent/point/(wire, not explicitly) clockwise
    // in order to have the wire parallel to the x axis.
    // Translate coordinates such that the wire is ON the x-axis
    // From PndTrkTools.cxx, author: Lia Lavezzi
    double beta = wireDirection.Phi();
    if (beta < 0)
      beta += TMath::Pi();

    // ... rotate the tangent
    double rtx = TMath::Cos(beta) * tangent.X() + TMath::Sin(beta) * tangent.Y();
    double rty = TMath::Cos(beta) * tangent.Y() - TMath::Sin(beta) * tangent.X();

    TVector2 rottangent(rtx, rty);
    rottangent = rottangent.Unit();

    // ... rotate the point
    double rx = TMath::Cos(beta) * intersection1.X() + TMath::Sin(beta) * intersection1.Y();
    double ry = TMath::Cos(beta) * intersection1.Y() - TMath::Sin(beta) * intersection1.X();

    // translation
    Double_t deltay = ry;
    rty -= deltay;
    ry -= deltay;

    // rotm, rotp
    Double_t rotm = rottangent.Y() / rottangent.X();
    Double_t rotp = ry - rotm * rx;

    // ellipsis
    //		double a = myHit.GetIsochrone() * TMath::Cos(3*TMath::DegToRad()); // CHECK skew angle hard coded
    double a = myHit.GetIsochrone() * TMath::Cos(2.9 * TMath::DegToRad()); // CHECK skew angle hard coded
    double b = myHit.GetIsochrone();

    // center of ellipsis
    Double_t x0a, x0b, y0;
    y0 = 0.;
    x0a = (-rotp + TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;
    x0b = (-rotp - TMath::Sqrt(b * b + a * a * rotm * rotm)) / rotm;

    // intersection point
    double intxa = (x0a * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
    double intya = rotm * intxa + rotp;
    double intxb = (x0b * b * b - rotm * rotp * a * a) / (b * b + rotm * rotm * a * a);
    double intyb = rotm * intxb + rotp;

    // 4. retraslate/rerotate all back to the original plane
    // retranslate
    y0 += deltay;
    intya += deltay;
    intyb += deltay;

    // rerotate
    double x0anew = TMath::Cos(beta) * x0a - TMath::Sin(beta) * y0;
    double y0anew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0a;
    double x0bnew = TMath::Cos(beta) * x0b - TMath::Sin(beta) * y0;
    double y0bnew = TMath::Cos(beta) * y0 + TMath::Sin(beta) * x0b;

    double intxanew = TMath::Cos(beta) * intxa - TMath::Sin(beta) * intya;
    double intyanew = TMath::Cos(beta) * intya + TMath::Sin(beta) * intxa;
    double intxbnew = TMath::Cos(beta) * intxb - TMath::Sin(beta) * intyb;
    double intybnew = TMath::Cos(beta) * intyb + TMath::Sin(beta) * intxb;

    intxa = intxanew;
    intya = intyanew;
    intxb = intxbnew;
    intyb = intybnew;

    // x0a, y0a - center of the first ellipse
    // x0b, y0b - center of the second ellipse
    x0a = x0anew;
    double y0a = y0anew;
    x0b = x0bnew;
    double y0b = y0bnew;

    double dx = intxa - x0a;
    double dy = intya - y0a;
    TVector3 dxdy(dx, dy, 0.0);

    TVector3 tfirst = endPoint + dxdy;
    TVector3 tsecond = startPoint + dxdy;

    double t = ((intxa + intya) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
    double intza = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

    tfirst = endPoint - dxdy;
    tsecond = startPoint - dxdy;

    t = ((intxb + intyb) - (tfirst.X() + tfirst.Y())) / ((tsecond.X() - tfirst.X()) + (tsecond.Y() - tfirst.Y()));
    double intzb = tfirst.Z() + (tsecond.Z() - tfirst.Z()) * t;

    // Set new coordinates for hits and plot isochrones
    //		fixhit.SetX(x0a);
    //		fixhit.SetY(y0a);
    fixhit.SetX(intxa);
    fixhit.SetY(intya);
    fixhit.SetZ(intza);
    tempResult.push_back(fixhit);

    //		fixhit.SetX(x0b);
    //		fixhit.SetY(y0b);
    fixhit.SetX(intxb);
    fixhit.SetY(intyb);
    fixhit.SetZ(intzb);
    tempResult.push_back(fixhit);

    vector<PndSttHit> tempcorrxy;
    fixhit.SetX(x0a);
    fixhit.SetY(y0a);
    tempcorrxy.push_back(fixhit);
    fixhit.SetX(x0b);
    fixhit.SetY(y0b);
    tempcorrxy.push_back(fixhit);

    corrxy.push_back(tempcorrxy);
    result.push_back(tempResult);
  }
  return result;
}

// Calculate points where a segment of a line (xy-projection of skewed wire, between points ex1 and ex2)
// intersects a circle (xy-projection of track helix) centered at (xc,yc) with radius R.
Int_t PndSttSkewStrawPzFinder::ComputeSegmentCircleIntersection(TVector2 ex1, TVector2 ex2, double xc, double yc, double R, TVector2 &intersection1, TVector2 &intersection2)
{

  double xa, ya, xb, yb;
  double delta;

  if ((ex2.X() - ex1.X()) != 0) {
    double m = (ex2.Y() - ex1.Y()) / (ex2.X() - ex1.X());
    double p = ex2.Y() - m * ex2.X();
    ;

    delta = -(m * xc + p - yc) * (m * xc + p - yc) + R * R * (m * m + 1);

    if (delta < 0 || TMath::IsNaN(delta))
      return 0;
    xa = (-(m * (p - yc) - xc) + sqrt(delta)) / (m * m + 1);
    ya = m * xa + p;
    xb = (-(m * (p - yc) - xc) - sqrt(delta)) / (m * m + 1);
    yb = m * xb + p;
  } else {

    delta = R * R - (ex1.X() - xc) * (ex1.X() - xc);

    if (delta < 0 || TMath::IsNaN(delta))
      return 0;

    xa = ex1.X();
    ya = yc + sqrt(delta);
    xb = ex1.X();
    yb = yc - sqrt(delta);
  }

  TVector2 int1(xa, ya);
  TVector2 int2(xb, yb);

  TVector2 distance11 = int1 - ex1;
  TVector2 distance12 = int1 - ex2;

  TVector2 distance21 = int2 - ex1;
  TVector2 distance22 = int2 - ex2;

  TVector2 length = ex1 - ex2;
  Int_t result = 0;

  if (distance11.Mod() < length.Mod() && distance12.Mod() < length.Mod()) {
    // cout << "intersection 1" << endl;
    result++;
    intersection1 = int1;
  }

  if (distance22.Mod() < length.Mod() && distance21.Mod() < length.Mod()) {
    // cout << "intersection 2" << endl;
    if (result == 1)
      intersection2 = int2;
    else
      intersection1 = int2;
    result++;
  }
  // if delta = 0 we have two identical intersections
  if (delta == 0)
    result--; // CHECK
  return result;
}

TVector2 PndSttSkewStrawPzFinder::ComputeTangentInPoint(double xc, double yc, TVector2 point)
{
  TVector2 center(xc, yc);
  TVector2 fromcentertoint = point - center;
  TVector2 tangent(fromcentertoint.Y(), -fromcentertoint.X());
  return tangent;
}

Double_t PndSttSkewStrawPzFinder::CalculateResidual(Double_t lineSlope, Double_t lineIntercept, TVector2 SZPoint)
{
  Double_t PhiAtPoint = lineSlope * SZPoint.X() + lineIntercept;
  Double_t result = SZPoint.Y() - PhiAtPoint;
  return result;
}

/*
 * Functions for cellular automaton method with lines
 */
// todo:zphi changed to SZ, make sure this function works properly!!
// vector<vector<TVector2>> PndSttSkewStrawPzFinder::LineCombiIsoFinder(vector<vector<TVector2>> SZPairVector){
//	vector<vector<pair<vector<int>,double>>> matrix2;
//	vector<pair<vector<int>,double> > tmp2;
//	vector<int> tmpIndices;
//	TVector2 first, second;
//	TVector2 v1, v2;
//	double CosAngle, Angle;
//	vector<int> nPairArray;
//
//	for (unsigned int i = 1; i < SZPairVector.size()-1; i++){//Loop over straws
//
//		if (fVerbose > 1 ) cout<<"LineCombiIsoFinder - New segment centered around straw: "<<i<<endl;
//
//		for (unsigned int j = 0; j < SZPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
//
//			for (unsigned int k = 0; k < SZPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
//
//				for (unsigned int l = 0; l < SZPairVector.at(i+1).size(); l++){//Loop over all Z-phi in straw i+1
//					v1 = SZPairVector.at(i+1).at(l) - SZPairVector.at(i).at(k);
//					v2 = SZPairVector.at(i-1).at(j) - SZPairVector.at(i).at(k);
//					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
//					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
//
//					//if(Angle > 180 - LineCombiAngleThreshold){//exclude paths
//					tmpIndices.push_back(j);
//					tmpIndices.push_back(k);
//					tmpIndices.push_back(l);
//					tmp2.push_back(pair<vector<int>,double> (tmpIndices,Angle));
//					tmpIndices.clear();
//					if (fVerbose > 4 ) cout<<"LineCombiIsoFinder - Indices j: "
//							<<j<<" k: "<<k<<" l: "<<l<<" Angle: "<<Angle<<endl;
//					//}
//				}
//			}
//		}
//
//		nPairArray.push_back(tmp2.size());
//		matrix2.push_back(tmp2);
//		tmp2.clear();
//
//	}
//
//	vector<int> current;
//	current.assign(matrix2.size(),0);
//	pair<vector<int>,double> bestIndices;
//	bestIndices.first.assign(matrix2.size(),0);
//	bestIndices.second=0.;
//
//	NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
//
//	if (bestIndices.second == 0. && SZPairVector.size() < 10) {
//
//		Double_t temp=LineCombiAngleThreshold;
//		LineCombiAngleThreshold = 180.;
//		NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
//		LineCombiAngleThreshold=temp;
//	}
//
//	if (fVerbose > 1 ) cout<<"Final answer: "<<endl;
//
//	if (fVerbose > 1 ) {
//
//		for (unsigned int i = 0; i < bestIndices.first.size();i++){
//			cout<<bestIndices.first.at(i)<<" ";
//		}
//		cout<<endl;
//	}
//
//
//	if (fVerbose > 0 ) cout<<"sum: "<<bestIndices.second<<endl;
//	int a, b, c;
//
//	vector<TVector2> result;
//
//	if (bestIndices.second > 0.) {
//		for (unsigned int i = 0; i<matrix2.size();i++) {
//
//			a = matrix2.at(i).at(bestIndices.first.at(i)).first.at(0);
//			result.push_back(SZPairVector.at(i).at(a));
//
//			if (i == matrix2.size()-1){
//				b = matrix2.at(i).at(bestIndices.first.at(i)).first.at(1);
//				c = matrix2.at(i).at(bestIndices.first.at(i)).first.at(2);
//				result.push_back(SZPairVector.at(i+1).at(b));
//				result.push_back(SZPairVector.at(i+2).at(c));
//			}
//		}
//	}
//
//	vector<vector<TVector2>> resultsnew;
//
//	/*
//	 *
//	 */
//	for (unsigned int i = 0; i < result.size(); i++) {
//		vector<TVector2> entry;
//		entry.push_back(result.at(i));
//		resultsnew.push_back(entry);
//	}
//
//	return resultsnew;
//
//}

// vector<pair<unsigned int,unsigned int>> PndSttSkewStrawPzFinder::LineCombiIsoFinderIndex(vector<vector<TVector2>> SZPairVector){
//	vector<vector<pair<vector<int>,double>>> matrix2;
//	vector<pair<vector<int>,double> > tmp2;
//	vector<int> tmpIndices;
//	TVector2 first, second;
//	TVector2 v1, v2;
//	double CosAngle, Angle;
//	vector<int> nPairArray;
//
//	for (unsigned int i = 1; i < SZPairVector.size()-1; i++){//Loop over straws
//
//		if (fVerbose > 1 ) cout<<"LineCombiIsoFinder - New segment centered around straw: "<<i<<endl;
//
//		for (unsigned int j = 0; j < SZPairVector.at(i-1).size(); j++){//Loop over all Z-phi in straw i-1
//
//			for (unsigned int k = 0; k < SZPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
//
//				for (unsigned int l = 0; l < SZPairVector.at(i+1).size(); l++){//Loop over all Z-phi in straw i+1
//					v1 = SZPairVector.at(i+1).at(l) - SZPairVector.at(i).at(k);
//					v2 = SZPairVector.at(i-1).at(j) - SZPairVector.at(i).at(k);
//					CosAngle = v1*v2/(TMath::Sqrt(v1*v1)*TMath::Sqrt(v2*v2));
//					Angle = TMath::ACos(CosAngle) * 180./TMath::Pi();
//
//					if(Angle > 180 - LineCombiAngleThreshold){//exclude paths
//						tmpIndices.push_back(j);
//						tmpIndices.push_back(k);
//						tmpIndices.push_back(l);
//						tmp2.push_back(pair<vector<int>,double> (tmpIndices,Angle));
//						tmpIndices.clear();
//						if (fVerbose > 4 ) cout<<"LineCombiIsoFinder - Indices j: "
//								<<j<<" k: "<<k<<" l: "<<l<<" Angle: "<<Angle<<endl;
//					}
//				}
//			}
//		}
//
//		nPairArray.push_back(tmp2.size());
//		matrix2.push_back(tmp2);
//		tmp2.clear();
//
//	}
//
//	vector<int> current;
//	current.assign(matrix2.size(),0);
//	pair<vector<int>,double> bestIndices;
//	bestIndices.first.assign(matrix2.size(),0);
//	bestIndices.second=0.;
//
//	NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
//
//	if (bestIndices.second == 0. && SZPairVector.size() < 10) {
//
//		Double_t temp=LineCombiAngleThreshold;
//		LineCombiAngleThreshold = 180.;
//		NestedFor(nPairArray,current,bestIndices,0.,matrix2,0);
//		LineCombiAngleThreshold=temp;
//	}
//
//	if (fVerbose > 1 ) cout<<"Final answer: "<<endl;
//
//	if (fVerbose > 1 ) {
//
//		for (unsigned int i = 0; i < bestIndices.first.size();i++){
//			cout<<bestIndices.first.at(i)<<" ";
//		}
//		cout<<endl;
//	}
//
//
//	if (fVerbose > 0 ) cout<<"sum: "<<bestIndices.second<<endl;
//	int a, b, c;
//
//	vector<pair<unsigned int,unsigned int>> result;
//
//	if (bestIndices.second > 0.) {
//		for (unsigned int i = 0; i<matrix2.size();i++) {
//
//			a = matrix2.at(i).at(bestIndices.first.at(i)).first.at(0);
//			pair<unsigned int,unsigned int> ent1(i,a);
//			result.push_back(ent1);
//			//			result.push_back(SZPairVector.at(i).at(a));
//
//			if (i == matrix2.size()-1){
//				b = matrix2.at(i).at(bestIndices.first.at(i)).first.at(1);
//				c = matrix2.at(i).at(bestIndices.first.at(i)).first.at(2);
//				//				result.push_back(SZPairVector.at(i+1).at(b));
//				pair<unsigned int,unsigned int> ent2(i+1,b);
//				result.push_back(ent2);
//				//				result.push_back(SZPairVector.at(i+2).at(c));
//				pair<unsigned int,unsigned int> ent3(i+2,c);
//				result.push_back(ent3);
//			}
//		}
//	}
//
//	return result;
//
//}

// Recursive for loop. Loops over angles between all pairs of lines. Calculate the sum of the angles for all possible
// paths from the first to the last skewed straw.
// todo:zphi changed to SZ, make sure this function works properly!!
// void PndSttSkewStrawPzFinder::NestedFor(vector<int> &ntimes, vector<int> &current, pair<vector<int>,double> &best,
//		double sum, vector<vector<pair<vector<int>,double> > > matrix, unsigned int depth) {
//	double tmp;
//	//bool possible = true;								//Unused var, commented away for now
//	if (depth == ntimes.size()) {
//
//		if (sum > best.second) {
//			/*
//    		for (unsigned int i = 0; i<matrix.size();i++) {
//    			int a, b, c;
//    			a = matrix.at(i).at(current.at(i)).first.at(0);
//    			b = matrix.at(i).at(current.at(i)).first.at(1);	//Unused var, commented away for now
//    			c = matrix.at(i).at(current.at(i)).first.at(2);
//    		}
//			 */
//			best.first = current;
//			best.second = sum;
//			if (fVerbose > 1 ) cout<<"NestedFor - Weight: "<<sum<<endl;
//		}
//		return;
//	}
//
//	for (int i = 0; i < ntimes[depth]; ++i) {
//		if (depth>0) {
//			//Check if lines are connected
//			if (matrix.at(depth).at(i).first.at(1) != matrix.at(depth-1).at(current.at(depth-1)).first.at(2)) continue;
//			if (matrix.at(depth).at(i).first.at(0) != matrix.at(depth-1).at(current.at(depth-1)).first.at(1)) continue;
//			//Check if angle is too small
//			if (matrix.at(depth).at(i).second < 180. - LineCombiAngleThreshold) continue;
//		}
//		tmp = sum;
//		current[depth]=i;
//		sum+= matrix.at(depth).at(i).second;
//
//		NestedFor(ntimes, current, best, sum, matrix, depth + 1);
//
//		sum = tmp;
//	}
//}

vector<pair<unsigned int, unsigned int>> PndSttSkewStrawPzFinder::LineCombiIsoFinderIndex2(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel,
                                                                                           vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem)
{
  unsigned int ncombi = 1;
  vector<unsigned int> indlist;
  vector<unsigned int> indlistcurr;
  vector<unsigned int> indlistbest;
  // Calculate number of combinations of Stt Sz points that are possible
  for (unsigned int i = 0; i < SZPairVector.size(); i++) {
    indlist.push_back(SZPairVector.at(i).size());
    indlistcurr.push_back(0);
    ncombi *= SZPairVector.at(i).size();
  }
  // Find the MVD SZ point closest to the STT SZ points
  double SMvdMax = 0;
  TVector2 SZMvdMax;
  bool ismvd = false;
  if (SZPairVectorMvdPixel.size() + SZPairVectorMvdStrip.size() > 0)
    ismvd = true;

  if (ismvd) {
    for (unsigned int i = 0; i < SZPairVectorMvdPixel.size(); i++) {
      if (SZPairVectorMvdPixel.at(i).X() > SMvdMax) {
        SZMvdMax = SZPairVectorMvdPixel.at(i);
        SMvdMax = SZPairVectorMvdPixel.at(i).X();
      }
    }
    for (unsigned int i = 0; i < SZPairVectorMvdStrip.size(); i++) {
      if (SZPairVectorMvdStrip.at(i).X() > SMvdMax) {
        SZMvdMax = SZPairVectorMvdStrip.at(i);
        SMvdMax = SZPairVectorMvdStrip.at(i).X();
      }
    }
  }

  double cosangle, angle;
  double bestsum = 9999999;
  double sum;
  TVector2 v1, v2;
  //	unsigned int iter = 0;
  cout << "vector size" << SZPairVector.size() << endl;
  cout << "n combis" << ncombi << endl;
  for (unsigned int i = 0; i < ncombi; i++) {
    //		cout<<"new combi"<<endl;
    sum = 0;
    for (unsigned int j = 0; j < SZPairVector.size() - 2; j++) {
      v1 = SZPairVector.at(j).at(indlistcurr.at(j)) - SZPairVector.at(j + 1).at(indlistcurr.at(j + 1));
      v2 = SZPairVector.at(j + 2).at(indlistcurr.at(j + 2)) - SZPairVector.at(j + 1).at(indlistcurr.at(j + 1));

      // In the beginning, we also add angle spanned by the last MVD and the first two STT SZ points
      if (j == 0 && ismvd) {
        cosangle = SZMvdMax * v1 / (TMath::Sqrt(SZMvdMax * SZMvdMax) * TMath::Sqrt(v1 * v1));
        angle = TMath::ACos(cosangle) * 180. / TMath::Pi();
        sum += (180. - angle) * (180. - angle);
      }

      cosangle = v1 * v2 / (TMath::Sqrt(v1 * v1) * TMath::Sqrt(v2 * v2));
      angle = TMath::ACos(cosangle) * 180. / TMath::Pi();
      sum += (180. - angle) * (180. - angle);
      //			if(angle < LineCombiAngleThreshold) sum+= 9999999;
      //			cout<<"angle: "<<angle<<endl;
    }
    if (sum < bestsum) {
      bestsum = sum;
      indlistbest = indlistcurr;
    }
    // Plotting used indices
    //		if(SZPairVector.size()==3){
    //			cout<<"used indices: ";
    //			for(unsigned int j = 0; j < indlistcurr.size(); j++) {
    //				cout<<indlistcurr.at(j)<<" ";
    //			}
    //			cout<<endl;
    //		}

    // Update current set of SZ choices (list of indices)
    indlistcurr.at(0)++;
    for (unsigned int j = 1; j < indlistcurr.size(); j++) {
      if (indlistcurr.at(j - 1) == indlist.at(j - 1)) {
        indlistcurr.at(j)++;
        for (unsigned int k = 0; k < j; k++) {
          indlistcurr.at(k) = 0;
        }
      }
    }
  }

  vector<pair<unsigned int, unsigned int>> result;
  for (unsigned int i = 0; i < indlistbest.size(); i++) {
    pair<unsigned int, unsigned int> ent(i, indlistbest.at(i));
    result.push_back(ent);
  }

  return result;
}

/*
 * Functions for linear Hough transform
 */
// todo:zphi changed to SZ, make sure this function works properly!!
// vector<vector<TVector2>> PndSttSkewStrawPzFinder::HoughTrueIsoFinder(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2>
// SZPairVectorMvdStrip, TH2D *HoughSpace){
//	//Generate lines for each SZ point, fill Houghspace with generated line parameters
//	pair<double, double> AngleRange(0.,180.);
//	int binmatrixcnt = 0;
//	vector<vector<vector<int>>> binmatrix;
//	for (unsigned int i = 0; i < SZPairVector.size(); i++){
//		vector<vector<int>> matrixrow;
//		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
//			double theta = AngleRange.first;
//			double StepSize = ( AngleRange.second-AngleRange.first)/fStepTheta;
//			double S = SZPairVector.at(i).at(j).X();
//			double Z = SZPairVector.at(i).at(j).Y();
//			vector<int> matrixentry;
//			for (unsigned int k = 0; k < fStepTheta; k++) {
//				double dist = Z*TMath::Cos(theta*TMath::DegToRad()) + S*TMath::Sin(theta*TMath::DegToRad());
//				int binind = HoughSpace->Fill(theta,dist);
//				theta += StepSize;
//				matrixentry.push_back(binind);
//
//			}
//			matrixrow.push_back(matrixentry);
//		}
//		binmatrix.push_back(matrixrow);
//	}
//	//SZ points for mvd pixels
//	for (unsigned int i = 0; i < SZPairVectorMvdPixel.size(); i++){
//		double theta = AngleRange.first;
//		double StepSize = ( AngleRange.second-AngleRange.first)/fStepTheta;
//		double S = SZPairVectorMvdPixel.at(i).X();
//		double Z = SZPairVectorMvdPixel.at(i).Y();
//		for (unsigned int k = 0; k < fStepTheta; k++) {
//			double dist = Z*TMath::Cos(theta*TMath::DegToRad()) + S*TMath::Sin(theta*TMath::DegToRad());
//			HoughSpace->Fill(theta,dist);
//			theta += StepSize;
//		}
//	}
//	//SZ points for mvd strips
//	for (unsigned int i = 0; i < SZPairVectorMvdStrip.size(); i++){
//		double theta = AngleRange.first;
//		double StepSize = ( AngleRange.second-AngleRange.first)/fStepTheta;
//		double S = SZPairVectorMvdStrip.at(i).X();
//		double Z = SZPairVectorMvdStrip.at(i).Y();
//		for (unsigned int k = 0; k < fStepTheta; k++) {
//			double dist = Z*TMath::Cos(theta*TMath::DegToRad()) + S*TMath::Sin(theta*TMath::DegToRad());
//			HoughSpace->Fill(theta,dist);
//			theta += StepSize;
//		}
//	}
//
//	//Find maximum in Hough space
//	Int_t MaxDist, MaxTheta, MaxBin, BinID;									//Unused var, commented away for now
//	BinID=HoughSpace->GetMaximumBin(MaxDist,MaxTheta,MaxBin);
//	double MaxdDist = HoughSpace->GetXaxis()->GetBinCenter(MaxDist);
//	double MaxdTheta = HoughSpace->GetYaxis()->GetBinCenter(MaxTheta);
//	double MaxdBin = HoughSpace->GetBinContent(MaxDist,MaxTheta);
//
//	vector<TVector2> result;
//	for (unsigned int i = 0; i < SZPairVector.size(); i++){
//		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
//			if(std::find( binmatrix.at(i).at(j).begin(),binmatrix.at(i).at(j).end(),BinID)!=binmatrix.at(i).at(j).end()){
//				result.push_back(SZPairVector.at(i).at(j));
//			}
//
//		}
//	}
//
//	//	BinID = HoughSpace->GetBin(MaxDist,MaxTheta,MaxBin);						//Unused var, commented away for now
//	//	cout<<"bin max: "<<HoughSpace->GetMaximumBin(MaxTheta,MaxDist,MaxBin)<<" "<<MaxdBin<<endl;
//
//	//	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - HoughSpaceMaxima x: "<<MaxdDist<<" y: "<<MaxdTheta<<endl;
//	//	double slope = 1/TMath::Tan(MaxdTheta*TMath::DegToRad());
//	//	//Calculate true line parameters
//	//	double m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
//	//	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
//	//	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
//	//	vector<TVector2> result;
//	//	for (unsigned int i = 0; i < SZPairVector.size(); i++){
//	//		double poca = 999999.;
//	//		Int_t index = 0;
//	//		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
//	//			double temppoca = TMath::Abs(slope*SZPairVector.at(i).at(j).X() - 1*SZPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));
//	//
//	//			if (temppoca < poca) {
//	//				poca = temppoca;
//	//				index = j;
//	//			}
//	//		}
//	//		result.push_back(SZPairVector.at(i).at(index));
//	//	}
//
//	vector<vector<TVector2>> resultsnew;
//
//	/*
//	 *
//	 */
//	for (unsigned int i = 0; i < result.size(); i++) {
//		vector<TVector2> entry;
//		entry.push_back(result.at(i));
//		resultsnew.push_back(entry);
//	}
//
//	return resultsnew;
//}

void PndSttSkewStrawPzFinder::DrawOpt2D(TH2D *HoughSpace)
{
  HoughSpace->GetXaxis()->SetTitle("#theta /Deg");
  HoughSpace->GetXaxis()->SetTitleSize(0.05);
  HoughSpace->GetXaxis()->SetTitleOffset(0.9);
  HoughSpace->GetXaxis()->SetNdivisions(7);
  HoughSpace->GetXaxis()->SetLabelOffset(0.);
  HoughSpace->GetXaxis()->SetLabelSize(0.05);
  HoughSpace->GetYaxis()->SetTitle("R /cm"); // Change back to ("R*#phi /cm")
  HoughSpace->GetYaxis()->SetTitleSize(0.05);
  HoughSpace->GetYaxis()->SetTitleOffset(0.9);
  HoughSpace->GetYaxis()->SetLabelOffset(0);
  HoughSpace->GetYaxis()->SetLabelSize(0.05);
}

vector<pair<unsigned int, unsigned int>> PndSttSkewStrawPzFinder::HoughTrueIsoFinderIndex(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel,
                                                                                          vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem,
                                                                                          bool draw = false)
{
  //	Double_t Zmin = 99999., Zmax = -1., Smin = 99999., Smax = -1.;
  //	for (unsigned int k = 0; k < SZPairVector.size(); k++){
  //		for (unsigned int j = 0; j < SZPairVector.at(k).size(); j++){
  //			if (SZPairVector.at(k).at(j).Y() < Zmin) { Zmin = SZPairVector.at(k).at(j).Y(); };
  //			if (SZPairVector.at(k).at(j).Y() > Zmax) { Zmax = SZPairVector.at(k).at(j).Y(); };
  //
  //			if (SZPairVector.at(k).at(j).X() < Smin) { Smin = SZPairVector.at(k).at(j).X(); };
  //			if (SZPairVector.at(k).at(j).X() > Smax) { Smax = SZPairVector.at(k).at(j).X(); };
  //		}
  //	}
  Double_t Zmax = -1., Smax = -1.;
  for (unsigned int k = 0; k < SZPairVector.size(); k++) {
    for (unsigned int j = 0; j < SZPairVector.at(k).size(); j++) {
      if (TMath::Abs(SZPairVector.at(k).at(j).Y()) > Zmax) {
        Zmax = TMath::Abs(SZPairVector.at(k).at(j).Y());
      };
      if (TMath::Abs(SZPairVector.at(k).at(j).X()) > Smax) {
        Smax = TMath::Abs(SZPairVector.at(k).at(j).X());
      };
    }
  }
  Double_t maxDist = TMath::Sqrt(Smax * Smax + Zmax * Zmax);
  //	Double_t maxDist=60;
  //	if (TMath::Abs(Zmax - Zmin)>TMath::Abs(Smax-Smin)) {
  //		maxDist = (sqrt(2)*TMath::Abs(Zmax - Zmin));
  //	}
  //	else {
  //		maxDist = (sqrt(2)*TMath::Abs(Smax - Smin));
  //	}
  Int_t angbin = fStepTheta;
  TH2D *HoughSpace = new TH2D("HoughSpace", "HoughSpace", angbin, 0, 180, fStepR, -maxDist, maxDist);
  HoughSpace->GetZaxis()->SetRangeUser(1, 21);
  //	TH2D *ShitSpace = new TH2D("HoughSpace","HoughSpace", 90, 0, 180, 50, -maxDist, maxDist);
  DrawOpt2D(HoughSpace);
  // Generate lines for each SZ point, fill Houghspace with generated line parameters
  pair<double, double> AngleRange(0., 180.);
  //	int binmatrixcnt = 0;
  vector<vector<vector<int>>> binmatrix;
  for (unsigned int i = 0; i < SZPairVector.size(); i++) {
    vector<vector<int>> matrixrow;
    for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++) {
      double theta = AngleRange.first;
      double StepSize = (AngleRange.second - AngleRange.first) / fStepTheta;
      double S = SZPairVector.at(i).at(j).X();
      double Z = SZPairVector.at(i).at(j).Y();
      vector<int> matrixentry;
      //			if (i==0&&j==1) cout<<"S, z plotted: "<<S<<" "<<Z<<endl;
      //			if (i==3&&j==1) cout<<"S, z plotted: "<<S<<" "<<Z<<endl;
      //			if (i==6&&j==0) cout<<"S, z plotted: "<<S<<" "<<Z<<endl;
      for (unsigned int k = 0; k < fStepTheta; k++) {
        double dist = Z * TMath::Cos(theta * TMath::DegToRad()) + S * TMath::Sin(theta * TMath::DegToRad());
        int binind;
        binind = HoughSpace->Fill(theta, dist);
        //				if (i==0&&j==1) ShitSpace->Fill(theta,dist);
        //				if (i==3&&j==1) ShitSpace->Fill(theta,dist);
        //				if (i==6&&j==0) ShitSpace->Fill(theta,dist);
        theta += StepSize;
        matrixentry.push_back(binind);
      }
      matrixrow.push_back(matrixentry); // For each SZ point, we push back all bin IDs in the HoughSpace
    }
    binmatrix.push_back(matrixrow);
  }
  // SZ points for mvd pixels
  for (unsigned int i = 0; i < SZPairVectorMvdPixel.size(); i++) {
    double theta = AngleRange.first;
    double StepSize = (AngleRange.second - AngleRange.first) / fStepTheta;
    double S = SZPairVectorMvdPixel.at(i).X();
    double Z = SZPairVectorMvdPixel.at(i).Y();
    for (unsigned int k = 0; k < fStepTheta; k++) {
      double dist = Z * TMath::Cos(theta * TMath::DegToRad()) + S * TMath::Sin(theta * TMath::DegToRad());
      HoughSpace->Fill(theta, dist);
      theta += StepSize;
    }
  }
  // SZ points for mvd strips
  for (unsigned int i = 0; i < SZPairVectorMvdStrip.size(); i++) {
    double theta = AngleRange.first;
    double StepSize = (AngleRange.second - AngleRange.first) / fStepTheta;
    double S = SZPairVectorMvdStrip.at(i).X();
    double Z = SZPairVectorMvdStrip.at(i).Y();
    for (unsigned int k = 0; k < fStepTheta; k++) {
      double dist = Z * TMath::Cos(theta * TMath::DegToRad()) + S * TMath::Sin(theta * TMath::DegToRad());
      HoughSpace->Fill(theta, dist);
      theta += StepSize;
    }
  }
  // SZ points for gems
  for (unsigned int i = 0; i < SZPairVectorGem.size(); i++) {
    double theta = AngleRange.first;
    double StepSize = (AngleRange.second - AngleRange.first) / fStepTheta;
    double S = SZPairVectorGem.at(i).X();
    double Z = SZPairVectorGem.at(i).Y();
    for (unsigned int k = 0; k < fStepTheta; k++) {
      double dist = Z * TMath::Cos(theta * TMath::DegToRad()) + S * TMath::Sin(theta * TMath::DegToRad());
      HoughSpace->Fill(theta, dist);
      theta += StepSize;
    }
  }

  // Find maximum in Hough space
  Int_t MaxDist, MaxTheta, MaxBin, BinID;
  BinID = HoughSpace->GetMaximumBin(MaxDist, MaxTheta, MaxBin);
  Int_t BinMax = HoughSpace->GetBinContent(BinID);

  // Since there could be more than one maximum, go through houghspace again and save all bins with same number of entries as maxima
  Int_t bin, binx, biny, binz;
  Int_t xfirst = HoughSpace->GetXaxis()->GetFirst();
  Int_t xlast = HoughSpace->GetXaxis()->GetLast();
  Int_t yfirst = HoughSpace->GetYaxis()->GetFirst();
  Int_t ylast = HoughSpace->GetYaxis()->GetLast();
  Int_t zfirst = HoughSpace->GetZaxis()->GetFirst();
  Int_t zlast = HoughSpace->GetZaxis()->GetLast();
  Int_t value;
  vector<Int_t> maximas;
  for (binz = zfirst; binz <= zlast; binz++) {
    for (biny = yfirst; biny <= ylast; biny++) {
      for (binx = xfirst; binx <= xlast; binx++) {
        bin = HoughSpace->GetBin(binx, biny, binz);
        value = HoughSpace->GetBinContent(bin);
        if (value == BinMax) {
          maximas.push_back(bin);
        }
      }
    }
  }
  cout << "new list of maximas" << maximas.size() << endl;
  cout << "skewed stt: " << SZPairVector.size() << endl;
  cout << "mvd pixels: " << SZPairVectorMvdPixel.size() << endl;
  cout << "mvd strips: " << SZPairVectorMvdStrip.size() << endl;
  cout << "gems: " << SZPairVectorGem.size() << endl;
  //	for(int i=0;i<maximas.size();i++){
  //		cout<<maximas.at(i)<<endl;
  //	}

  //	double MaxdDist = HoughSpace->GetXaxis()->GetBinCenter(MaxDist);
  //	double MaxdTheta = HoughSpace->GetYaxis()->GetBinCenter(MaxTheta);
  //	double MaxdBin = HoughSpace->GetBinContent(MaxDist,MaxTheta);

  vector<pair<unsigned int, unsigned int>> result;
  //	for (unsigned int i = 0; i < SZPairVector.size(); i++){
  //		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
  //			if(std::find( binmatrix.at(i).at(j).begin(),binmatrix.at(i).at(j).end(),BinID)!=binmatrix.at(i).at(j).end()){
  //				pair<unsigned int,unsigned int> a(i,j);
  //				result.push_back(a);
  //			}
  //
  //		}
  //	}
  double chi2best = 999999;
  for (unsigned int b = 0; b < maximas.size(); b++) {
    vector<pair<unsigned int, unsigned int>> tempresult;
    for (unsigned int i = 0; i < SZPairVector.size(); i++) {
      for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++) {
        if (std::find(binmatrix.at(i).at(j).begin(), binmatrix.at(i).at(j).end(), maximas.at(b)) != binmatrix.at(i).at(j).end()) {
          pair<unsigned int, unsigned int> a(i, j);
          tempresult.push_back(a);
        }
      }
    }
    vector<vector<TVector2>> TrueSZ;
    for (unsigned int j = 0; j < SZPairVector.size(); j++) {
      vector<TVector2> TrueSZentry;
      for (unsigned int k = 0; k < SZPairVector.at(j).size(); k++) {
        for (unsigned int l = 0; l < tempresult.size(); l++) {
          if (j == tempresult.at(l).first && k == tempresult.at(l).second) {
            TrueSZentry.push_back(SZPairVector.at(j).at(k));
          }
        }
      }
      if (TrueSZentry.size() > 0) {
        TrueSZ.push_back(TrueSZentry);
      }
    }
    TVector2 lineparams;
    double chi2 = PzLineFitExtract3(TrueSZ, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem, lineparams);
    cout << "chi2: " << chi2 << endl;
    if (chi2 < chi2best) {
      chi2best = chi2;
      result = tempresult;
    }
  }

  if (draw) {
    TCanvas *HoughCanvas = new TCanvas();
    HoughCanvas->SetCanvasSize(500, 500);
    HoughSpace->Draw("COLZ");
    //		ShitSpace->Draw("COLZ");
    HoughCanvas->Update();
  } else {
    HoughSpace->Delete();
    //		ShitSpace->Delete();
  }

  return result;
}

/*
 * Second function for linear Hough transform
 * only generates alla lines between all pairs of z-phi points
 */
// todo:zphi changed to SZ, make sure this function works properly!!
// vector<vector<TVector2>> PndSttSkewStrawPzFinder::HoughTrueIsoFinder2(vector<vector<TVector2>> SZPairVector, vector<TVector2> SZPairVectorMvdPixel, vector<TVector2>
// SZPairVectorMvdStrip, TH2D *HoughSpace){
//	//Generate only lines between all z-phi candidates, fill Hough space
//	for (unsigned int i = 0; i < SZPairVector.size(); i++){
//		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
//			double z1 = SZPairVector.at(i).at(j).X();
//			double phi1 = SZPairVector.at(i).at(j).Y();
//			for (unsigned int k = i+1; k < SZPairVector.size(); k++){
//				for (unsigned int l = 0; l < SZPairVector.at(k).size(); l++){
//					double z2 = SZPairVector.at(k).at(l).X();
//					double phi2 = SZPairVector.at(k).at(l).Y();
//					double theta = TMath::ATan(-(phi2-phi1)/(z2-z1));
//					double dist = z1*TMath::Cos(theta) + phi1*TMath::Sin(theta);
//					if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - theta: "<<theta*TMath::RadToDeg()<<" dist: "<<dist<<endl;
//					HoughSpace->Fill(theta*TMath::RadToDeg(),dist);
//
//				}
//			}
//		}
//	}
//	//Find maximum in Hough space
//	Int_t MaxDist, MaxTheta, MaxBin;//, BinID;								//Unused var, commented away for now
//	HoughSpace->GetMaximumBin(MaxTheta,MaxDist,MaxBin);
//
//	Double_t MaxdTheta = HoughSpace->GetXaxis()->GetBinCenter(MaxTheta);
//	Double_t MaxdDist = HoughSpace->GetYaxis()->GetBinCenter(MaxDist);
//	//Double_t MaxdBin = HoughSpace->GetBinContent(MaxTheta,MaxDist);		//Unused var, commented away for now
//
//	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - HoughSpaceMaxima x: "<<MaxdTheta<<" y: "<<MaxdDist<<endl;
//	//Calculate true line parameters
//	Double_t slope = -1/TMath::Tan(MaxdTheta*TMath::DegToRad());
//	Double_t m = MaxdDist/TMath::Sin(MaxdTheta*TMath::DegToRad());
//	if (fVerbose > 0 ) cout<<"HoughTrueIsoFinder2 - Z-Phi line Slope: "<<slope<<" Intercept: "<<m<<endl;
//	//Loop over Z-phi pairs and reject the one with larges POCA for each skewed straw
//	vector<TVector2> result;
//	for (unsigned int i = 0; i < SZPairVector.size(); i++){
//		Double_t poca = 999999.;
//		Int_t index = 0;
//		for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
//			Double_t temppoca = TMath::Abs(slope*SZPairVector.at(i).at(j).X() - 1*SZPairVector.at(i).at(j).Y() + m)/TMath::Sqrt(pow(slope,2) + pow(-1.,2));
//
//			if (temppoca < poca) {
//				poca = temppoca;
//				index = j;
//			}
//		}
//		result.push_back(SZPairVector.at(i).at(index));
//	}
//
//	vector<vector<TVector2>> resultsnew;
//
//	for (unsigned int i = 0; i < result.size(); i++) {
//		vector<TVector2> entry;
//		entry.push_back(result.at(i));
//		resultsnew.push_back(entry);
//	}
//
//	return resultsnew;
//}

void PndSttSkewStrawPzFinder::LineFit(vector<vector<TVector2>> SZPairVector, vector<double> DriftRadVector, vector<TVector2> SZPairVectorMvdPixel,
                                      vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem, double &k, double &m)
{
  double A = 0, B = 0, C = 0, D = 0, E = 0; //, F=0;
  double x, y, sig2;
  for (unsigned int i = 0; i < SZPairVector.size(); i++) {
    for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++) {
      //			errors.push_back(0.3);
      //			errors.push_back(1.);
      x = SZPairVector.at(i).at(j).X();
      y = SZPairVector.at(i).at(j).Y();
      // changed by Anna Alicke: bugfix to make the code more stable
      if (TMath::IsNaN(x) || TMath::IsNaN(y) || abs(y) <= 0)
        continue;

      // sig2 = DriftRadVector.at(i) * DriftRadVector.at(i);
      sig2 = 0.3 * 0.3;
      A += x / sig2;
      B += 1 / sig2;
      C += y / sig2;
      D += (x * x) / sig2;
      E += (x * y) / sig2;
      //			F += (y*y)/sig2;
    }
  }
  for (unsigned int i = 0; i < SZPairVectorMvdPixel.size(); i++) {
    //		errors.push_back(0.0058);
    //		errors.push_back(0.5);
    x = SZPairVectorMvdPixel.at(i).X();
    y = SZPairVectorMvdPixel.at(i).Y();
    // changed by Anna Alicke: bugfix to make the code more stable
    if (TMath::IsNaN(x) || TMath::IsNaN(y) || abs(y) <= 0)
      continue;

    sig2 = 0.0058 * 0.0058;
    A += x / sig2;
    B += 1 / sig2;
    C += y / sig2;
    D += (x * x) / sig2;
    E += (x * y) / sig2;
    //		F += (y*y)/sig2;
  }
  for (unsigned int i = 0; i < SZPairVectorMvdStrip.size(); i++) {
    //		errors.push_back(0.0087);
    //		errors.push_back(0.5);
    x = SZPairVectorMvdStrip.at(i).X();
    y = SZPairVectorMvdStrip.at(i).Y();
    if (TMath::IsNaN(x) || TMath::IsNaN(y) || abs(y) <= 0)
      continue;

    sig2 = 0.0087 * 0.0087;
    A += x / sig2;
    B += 1 / sig2;
    C += y / sig2;
    D += (x * x) / sig2;
    E += (x * y) / sig2;
    //		F += (y*y)/sig2;
  }
  for (unsigned int i = 0; i < SZPairVectorGem.size(); i++) {
    //    errors.push_back(0.0087);
    //    errors.push_back(0.5);
    x = SZPairVectorGem.at(i).X();
    y = SZPairVectorGem.at(i).Y();
    if (TMath::IsNaN(x) || TMath::IsNaN(y) || abs(y) <= 0)
      continue;
    // ToDo: the error is not correct for a GEM hit
    sig2 = 0.5 * 0.5;
    A += x / sig2;
    B += 1 / sig2;
    C += y / sig2;
    D += (x * x) / sig2;
    E += (x * y) / sig2;
    //    F += (y*y)/sig2;
  }

  k = (E * B - C * A) / (D * B - A * A);
  m = (D * C - E * A) / (D * B - A * A);
}

void PndSttSkewStrawPzFinder::LineFitRecursiveAnnealFinder(vector<vector<TVector2>> SZPairVector, vector<double> DriftRadVector, vector<TVector2> SZPairVectorMvdPixel,
                                                           vector<TVector2> SZPairVectorMvdStrip, vector<TVector2> SZPairVectorGem,
                                                           vector<pair<unsigned int, unsigned int>> &TrueSZPairVectorindex, vector<unsigned int> &TrueSZPairVectorMvdPixelindex,
                                                           vector<unsigned int> &TrueSZPairVectorMvdStripindex, vector<unsigned int> &TrueSZPairVectorGemindex)
{

  unsigned int nSkews = 0;
  // First, fill TrueSZPairVectorindex with all indices of SZPairVector
  for (unsigned int i = 0; i < SZPairVector.size(); i++) {
    nSkews++;
    for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++) {
      pair<unsigned int, unsigned int> a(i, j);
      TrueSZPairVectorindex.push_back(a);
    }
  }
  // Same for TrueSZPairVectorMvdPixelindex
  for (unsigned int i = 0; i < SZPairVectorMvdPixel.size(); i++) {
    unsigned int a = i;
    TrueSZPairVectorMvdPixelindex.push_back(a);
  }
  // Same for TrueSZPairVectorMvdStripindex
  for (unsigned int i = 0; i < SZPairVectorMvdStrip.size(); i++) {
    unsigned int a = i;
    TrueSZPairVectorMvdStripindex.push_back(a);
  }
  // Same for TrueSZPairVectorGemindex
  for (unsigned int i = 0; i < SZPairVectorGem.size(); i++) {
    unsigned int a = i;
    TrueSZPairVectorGemindex.push_back(a);
  }

  // Make a first line fit using all SZ hits
  double k = 0, m = 0;
  LineFit(SZPairVector, DriftRadVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem, k, m);
  double zpred;
  double res;
  double resmax;
  unsigned int imax, jmax;
  bool aselect;
  vector<vector<TVector2>> SZPairVectorCopy = SZPairVector;
  while (nSkews > 0) {
    aselect = false;
    resmax = -1.;
    for (unsigned int i = 0; i < SZPairVectorCopy.size(); i++) {
      if (SZPairVectorCopy.at(i).size() == 1)
        continue;

      for (unsigned int j = 0; j < SZPairVectorCopy.at(i).size(); j++) {
        zpred = SZPairVectorCopy.at(i).at(j).X() * k + m;
        res = TMath::Abs(SZPairVectorCopy.at(i).at(j).Y() - zpred);
        if (res > resmax) {
          resmax = res;
          imax = i;
          jmax = j;
          aselect = true;
        }
      }
    }
    if (aselect) {
      SZPairVectorCopy.at(imax).erase(SZPairVectorCopy.at(imax).begin() + jmax);
      pair<unsigned int, unsigned int> a(imax, jmax);
      for (unsigned int i = 0; i < TrueSZPairVectorindex.size(); i++) {
        if (TrueSZPairVectorindex.at(i) == a) {
          TrueSZPairVectorindex.erase(TrueSZPairVectorindex.begin() + i);
        }
      }
    } else {
      // The aselect variable is used since sometimes, the iso alignment function
      // does not give proper SZ hits.
      // This should be fixed by restructuring the code from vector<vector<TVector2>>
      // to multimap<PndSttHit,TVector2>
      //			cout<<endl;
      //			cout<<endl;
      //			cout<<endl;
      //			cout<<"NO SELECTION WAS MADE IN THE RECURSIVE FIT STARTEGY"<<endl;
      //			cout<<resmax<<endl;
      //			cout<<endl;
      //			cout<<endl;
      //			cout<<endl;
      //			for (unsigned int i = 0; i < SZPairVector.size(); i++){
      //				for (unsigned int j = 0; j < SZPairVector.at(i).size(); j++){
      //					cout<<"S: "<<SZPairVector.at(i).at(j).X()<<" Z: "<<SZPairVector.at(i).at(j).Y()<<endl;
      //				}
      //			}
    }

    LineFit(SZPairVectorCopy, DriftRadVector, SZPairVectorMvdPixel, SZPairVectorMvdStrip, SZPairVectorGem, k, m);
    nSkews--;
  }
}

/*
 * FUnctions for linear regression
 */
// todo:zphi changed to SZ, make sure this function works properly!!
// TVector2 PndSttSkewStrawPzFinder::TheilSen(vector<vector< TVector2> > SZPairVector){
//	TVector2 result;
//
//	vector<Double_t> SlopeVector;
//
//	for (unsigned int i = 0; i < SZPairVector.size()-1; i++){//Loop over all straws minus last one
//
//		for (unsigned int j = i+1; j < SZPairVector.size(); j++){//Loop over straws from i+1 to and including last one
//
//			for (unsigned int k = 0; k < SZPairVector.at(i).size(); k++){//Loop over all Z-phi in straw i
//
//				for (unsigned int l = 0; l < SZPairVector.at(j).size(); l++){//Loop over all Z-phi in straw j
//
//
//					Double_t slope = (SZPairVector.at(j).at(l).Y() - SZPairVector.at(i).at(k).Y())/(SZPairVector.at(j).at(l).X() - SZPairVector.at(i).at(k).X());
//					SlopeVector.push_back(slope);
//
//				}
//
//			}
//
//		}
//
//	}
//
//	sort(SlopeVector.begin(),SlopeVector.end());
//
//	cout<<"Slopes ordered: "<<endl;
//	for (unsigned int i = 0; i < SlopeVector.size(); i++) {
//		cout<<"Slope: "<<SlopeVector.at(i)<<endl;
//	}
//
//	Double_t MedianSlope;
//	if (SlopeVector.size() % 2 == 0) {
//		auto middle = SlopeVector.begin();
//		advance(middle, SlopeVector.size()/2);//TODO: Calculate median as mean of two middle elements
//		MedianSlope = *middle;
//	}
//	else {
//		auto middle = SlopeVector.begin();
//		advance(middle, SlopeVector.size()/2);
//		MedianSlope = *middle;
//	}
//	cout<<"Median slope: "<<MedianSlope<<endl;
//
//	return result;
//}

// todo:zphi changed to SZ, make sure this function works properly!!
// TVector2 PndSttSkewStrawPzFinder::PzLineFitExtract(vector< TVector2> TrueSZ){
//	//Simple linear regression, Z on X-axis Phi on Y-axis
//	//Errors not taken into account
//	Double_t meanZ = 0.;
//	Double_t meanPhi = 0.;
//	for (unsigned int i = 0; i < TrueSZ.size(); i++){
//		if (fVerbose > 0 ) cout<<"PzLineFitExtract - True hit Z: "<<TrueSZ.at(i).X()<<" Phi*R: "<<TrueSZ.at(i).Y()<<endl;
//		meanZ += TrueSZ.at(i).X();
//		meanPhi += TrueSZ.at(i).Y();
//	}
//	meanZ /=(TrueSZ.size());
//	meanPhi /=(TrueSZ.size());
//	Double_t betanume = 0.;
//	Double_t betadeno = 0.;
//	for (unsigned int i = 0; i < TrueSZ.size(); i++){
//		betanume += (TrueSZ.at(i).X() - meanZ)*(TrueSZ.at(i).Y() - meanPhi);
//		betadeno += pow(TrueSZ.at(i).X() - meanZ,2);
//	}
//	Double_t beta = betanume/betadeno;
//	Double_t alpha = meanPhi - beta*meanZ;
//	if (fVerbose > 0 ) cout<<"PzLineFitExtract - Slope: "<<beta<<" Intercept: "<<alpha<<endl;
//	TVector2 result(alpha,beta);
//	return result;
//}

// todo:zphi changed to SZ, make sure this function works properly!!
// TVector2 PndSttSkewStrawPzFinder::PzLineFitExtract2(vector<vector< TVector2> > TrueSZPairVector){
//	//Simple linear regression, Z on X-axis Phi on Y-axis
//	//Errors not taken into account
//	Double_t meanZ = 0.;
//	Double_t meanPhi = 0.;
//	Int_t N = 0;
//	for (unsigned int i = 0; i < TrueSZPairVector.size(); i++){
//		for (unsigned int j = 0; j < TrueSZPairVector.at(i).size(); j++){
//			meanZ += TrueSZPairVector.at(i).at(j).X();
//			meanPhi += TrueSZPairVector.at(i).at(j).Y();
//			N++;
//		}
//	}
//	meanZ /= N;
//	meanPhi /= N;
//	Double_t betanume = 0.;
//	Double_t betadeno = 0.;
//	for (unsigned int i = 0; i < TrueSZPairVector.size(); i++){
//		for (unsigned int j = 0; j < TrueSZPairVector.at(i).size(); j++){
//			betanume += (TrueSZPairVector.at(i).at(j).X() - meanZ)*(TrueSZPairVector.at(i).at(j).Y() - meanPhi);
//			betadeno += pow(TrueSZPairVector.at(i).at(j).X() - meanZ,2);
//		}
//	}
//	Double_t beta = betanume/betadeno;
//	Double_t alpha = meanPhi - beta*meanZ;
//	if (fVerbose > 0 ) cout<<"PzLineFitExtract - Slope: "<<beta<<" Intercept: "<<alpha<<endl;
//	TVector2 result(alpha,beta);
//	return result;
//}

double PndSttSkewStrawPzFinder::PzLineFitExtract3(vector<vector<TVector2>> TrueSZPairVector, vector<TVector2> TrueSZPairVectorMvdPixel, vector<TVector2> TrueSZPairVectorMvdStrip,
                                                  vector<TVector2> TrueSZPairVectorGem, TVector2 &lineparams)
{
  // Simple linear regression, S on X-axis Z on Y-axis
  // Errors taken into account!
  // vectors points and errors contain information about all hits (stt and mvd). Run with same indices
  vector<TVector2> points;
  vector<double> errors;
  for (unsigned int i = 0; i < TrueSZPairVector.size(); i++) {
    for (unsigned int j = 0; j < TrueSZPairVector.at(i).size(); j++) {
      points.push_back(TrueSZPairVector.at(i).at(j));
      errors.push_back(0.3);
      //			errors.push_back(1.);
    }
  }
  for (unsigned int i = 0; i < TrueSZPairVectorMvdPixel.size(); i++) {
    points.push_back(TrueSZPairVectorMvdPixel.at(i));
    errors.push_back(0.0058);
    //		errors.push_back(0.5);
  }
  for (unsigned int i = 0; i < TrueSZPairVectorMvdStrip.size(); i++) {
    points.push_back(TrueSZPairVectorMvdStrip.at(i));
    errors.push_back(0.0087);
    //		errors.push_back(0.5);
  }
  for (unsigned int i = 0; i < TrueSZPairVectorGem.size(); i++) {
    points.push_back(TrueSZPairVectorGem.at(i));
    // errors.push_back(0.0087);
    // ToDo: errors must be set
    errors.push_back(0.5);
    //         errors.push_back(0.5);
  }

  double A = 0, B = 0, C = 0, D = 0, E = 0, F = 0;
  double x = 0, y = 0, sig2 = 0, chi2 = 0;
  for (unsigned int i = 0; i < points.size(); i++) {
    x = points.at(i).X();
    y = points.at(i).Y();
    if (TMath::IsNaN(x) || TMath::IsNaN(y))
      continue;
    sig2 = errors.at(i) * errors.at(i);
    A += x / sig2;
    B += 1 / sig2;
    C += y / sig2;
    D += (x * x) / sig2;
    E += (x * y) / sig2;
    F += (y * y) / sig2;
  }

  Double_t k = (E * B - C * A) / (D * B - A * A);
  Double_t m = (D * C - E * A) / (D * B - A * A);
  double predy;

  for (unsigned int i = 0; i < points.size(); i++) {
    x = points.at(i).X();
    y = points.at(i).Y();
    if (TMath::IsNaN(x) || TMath::IsNaN(y) || abs(y) <= 0)
      continue;

    predy = k * x + m;
    chi2 += ((predy - y) / errors.at(i)) * ((predy - y) / errors.at(i));
  }

  if (fVerbose > 0)
    cout << "PzLineFitExtract - Slope: " << k << " Intercept: " << m << endl;
  lineparams.Set(k, m);
  return chi2;
}

FairTrackParP PndSttSkewStrawPzFinder::GetTrackParam(FairTrackParP oldParam, TVector2 center, double radius, TVector2 lineparams, double S0, int charge)
{
  TVector2 r2(oldParam.GetX() - center.X(), oldParam.GetY() - center.Y());
  double S = (r2.Phi() * radius - S0) * charge * -1.;
  //	TVector3 Hit(oldParam.GetX(),oldParam.GetY(),(S - lineparams.X())/lineparams.Y());
  TVector3 Hit(oldParam.GetX(), oldParam.GetY(), (S * lineparams.X()) + lineparams.Y());

  TVector3 HitError(oldParam.GetDX(), oldParam.GetDY(), 1.);

  if (fVerbose > 0)
    cout << "TrackPar S: " << S << endl;
  if (fVerbose > 0)
    cout << "New position: " << Hit.X() << " " << Hit.Y() << " " << Hit.Z() << endl;
  // Declare everything needed for new FairTrackParP
  //	TVector3 originFirst(Hit);
  double pz = TMath::Sqrt(oldParam.GetPx() * oldParam.GetPx() + oldParam.GetPy() * oldParam.GetPy());

  pz *= lineparams.X();

  TVector3 Mom(oldParam.GetPx(), oldParam.GetPy(), pz);
  if (fVerbose > 0)
    cout << "New momentum vector: " << Mom.X() << " " << Mom.Y() << " " << Mom.Z() << endl;
  TVector3 momError(0.02, 0.02, 0.05);
  TVector3 dj(1, 0, 0);
  TVector3 dk(0, 1, 0);
  FairTrackParP result(Hit, Mom, HitError, momError, oldParam.GetQ(), Hit, dj, dk);

  return result;
}
