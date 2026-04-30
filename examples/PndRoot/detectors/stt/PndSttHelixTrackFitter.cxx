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

//

#include "PndSttHelixTrackFitter.h"

#include "PndSttTrackMatch.h"
#include "PndSttHoughDefines.h"
#include "PndSttGeomPoint.h"
#include "PndSttHit.h"
#include "PndSttPoint.h"
#include "PndSttTube.h"

#include "PndTrackCandHit.h"
#include "PndTrackCand.h"

#include "FairRootManager.h"
#include "FairTask.h"
#include "FairLogger.h"

#include "TArc.h"
#include "TH2.h"
#include "TClonesArray.h"

#include "TMatrixD.h"
#include "TMarker.h"
#include "TLine.h"
#include "TPolyLine.h"
#include "TMinuit.h"
#include "TMath.h"

#include <string>
#include <sstream>
#include <iostream>
//#include <cmath>

using std::cin;
using std::cout;
using std::endl;
using std::map;
using TMath::ATan;
using TMath::ATan2;
using TMath::Cos;
using TMath::Sin;
using TMath::Sqrt;

Int_t h = 0; // negative charge = 1; positive charge = -1
Double_t vote[201][1001], votecon[201];
TArrayD marray(200);

PndSttHelixTrackFitter::PndSttHelixTrackFitter()
{
  fEventCounter = 0;
  fVerbose = 1;
  fConstraint = 0;
  fDisplayLevel = 0;
}

PndSttHelixTrackFitter::PndSttHelixTrackFitter(Int_t verbose)
{
  fVerbose = verbose;
  fEventCounter = 0;
  fConstraint = 0;
  fDisplayLevel = 0;
}

PndSttHelixTrackFitter::~PndSttHelixTrackFitter() {}

void PndSttHelixTrackFitter::Init()
{
  fEventCounter = 0;

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();

  if (!ioman) {
    cout << "-E- PndSttHelixTrackFitter::Init: "
         << "RootManager not instantised!" << endl;
    //      return kFATAL;
  }

  // Get hit Array
  fHitArray = (TClonesArray *)ioman->GetObject("STTHit");
  if (!fHitArray) {
    LOG(warn) << " PndSttHelixTrackFitter::Init: No Hit array!";
  }

  // Get point Array
  fPointArray = (TClonesArray *)ioman->GetObject("STTPoint");
  if (!fPointArray) {
    LOG(warn) << " PndSttHelixTrackFitter::Init: No Point array!";
  }

  if (fDisplayLevel > 0)
    InitEventDisplay();
}

Int_t PndSttHelixTrackFitter::DoFit(PndTrackCand *pTrackCand, PndSttTrack *pTrack, Int_t pidHypo)
{

  if (fDisplayLevel > 0)
    RunEventDisplay(pTrackCand);

  if (fConstraint == 0)
    return DoFitPlain(pTrackCand, pTrack, pidHypo);
  else if (fConstraint == 1) {
    cout << "PndSttHelixTrackFitter::DoFit, Constraint 1 not usable now. A big change is needed to use start point from PndTrack and not PndTrackCand anymore" << endl;
    return 1;
    // return DoFitThroughOrigin(pTrackCand, pTrack, pidHypo);
  } else {
    cout << "PndSttHelixTrackFitter::DoFit, Constraint " << fConstraint << " not implemented" << endl;
    return 1;
  }
}

// ===================================================================================
// CONSTRAINT 0
Int_t PndSttHelixTrackFitter::DoFitPlain(PndTrackCand *pTrackCand, PndSttTrack *pTrack, Int_t) // pidHypo //[R.K.03/2017] unused variable(s)
{
  // cout << "track fitting event # " << fEventCounter << endl;

  if (!pTrackCand)
    return 0;
  fTrack = pTrack; // CHECK canc
  fTrackCand = pTrackCand;
  if (fDisplayLevel > 0) {
    RunEventDisplay(pTrackCand);
    char goOnChar;
    cout << "press any key to continue: " << endl;
    cin >> goOnChar;
  }

  Int_t fit = 0;

  fit = XYFit(pTrackCand, 1);

  if (fit == 0 || fTrack->GetRad() == 0 || !(fTrack->GetRad()) || fTrack->GetRad() > 3000) {
    if (fVerbose == 2)
      LOG(error) << " pre prefit FAILED " << fit << " " << fTrack->GetRad();
    fTrack->SetRad(-999);
    return 0;
  }

  // cout << "prefit-------------------------------" << endl;
  fit = 0;
  fit = MinuitFit(pTrackCand, 1);

  // if the fit fails
  if (fit == 0 || fTrack->GetRad() == 0 || !(fTrack->GetRad()) || fTrack->GetRad() > 3000) {
    fTrack->SetRad(-999);
    if (fVerbose == 2)
      LOG(error) << " prefit FAILED";
    return 0;
  } else {
    pTrack->SetFlag(1); // prefit done
    Bool_t Rint = IntersectionFinder(pTrackCand);
    //    cout << "refit" << endl;
    // if refit is OK
    if (Rint == kTRUE) {
      fit = XYFit(pTrackCand, 2);
      Rint = IntersectionFinder(pTrackCand);
      if (Rint == kTRUE)
        fit = MinuitFit(pTrackCand, 2);
      Rint = IntersectionFinder(pTrackCand);
      if (Rint == kTRUE)
        fit = MinuitFit(pTrackCand, 2);

    } else {
      return 0;
    }
    if (fit == 1 && fTrack->GetRad() > 0 && fTrack->GetRad() < 3000) {

      pTrack->SetFlag(2); // refit done
      Bool_t zint = ZFinder(pTrackCand, 1);

      if (zint == kTRUE) {
        Int_t zfit = ZFit(pTrackCand, 1);
        if (zfit == 1) {
          pTrack->SetFlag(3); // z fit done
        }
      } else if (fVerbose == 2)
        LOG(error) << " zfinder FAILED";
    }
  }

  if (fVerbose == 2) {
    cout << "param last d    : " << fTrack->GetDist() << endl;
    cout << "param last phi  : " << fTrack->GetPhi() << endl;
    cout << "param last R    : " << fTrack->GetRad() << endl;
    cout << "param last tanL : " << fTrack->GetTanL() << endl;
    cout << "param last q    : " << fTrack->GetCharge() << endl;
    double pt, pl;
    pt = fTrack->GetRad() * 0.006;
    pl = fTrack->GetRad() * fTrack->GetTanL() * 0.006;
    cout << "pT              : " << pt << endl;
    cout << "pL              : " << pl << endl;
    cout << "px, py, pz      : " << pt * cos(-h * TMath::Pi() / 2. + fTrack->GetPhi()) << " " << pt * sin(-h * TMath::Pi() / 2. + fTrack->GetPhi()) << " " << pl << endl;
  }

  if (fDisplayLevel > 0)
    FinishEventDisplay(fTrack);

  return 0;
}

// XYFit was Fit4b
Int_t PndSttHelixTrackFitter::XYFit(PndTrackCand *pTrackCand, Int_t whatToFit)
{

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();
  Bool_t first = kFALSE;
  if (hitcounter == 0)
    return 0;
  if (hitcounter < 5) { // hitcounter > 50
    if (fVerbose == 2)
      cout << "Bad No of hits in STT " << hitcounter << endl;
    return 0;
  }
  if (fVerbose == 2)
    cout << "FIT xy ********************" << endl;

  // traslation and rotation -----------
  // traslation near the first point
  Double_t s = 0.001;
  Double_t trasl[2];
  // rotation
  Double_t alpha;
  PndSttHit *hitfirst = nullptr, *hitlast = nullptr;
  if (fVerbose == 2)
    cout << "hitcounter: " << hitcounter << endl;
  for (Int_t k = 0; k < hitcounter; k++) {

    PndTrackCandHit candhit = pTrackCand->GetSortedHit(k);
    Int_t iHit = candhit.GetHitId();
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit) {
      cout << "PndSttHelixTrackFitter::XYFit: no hit at " << iHit << endl;
      continue;
    }

    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;
    Int_t refindex = currenthit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::XYFit: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wiredirection = tube->GetWireDirection();
    if (wiredirection != TVector3(0., 0., 1.))
      continue;
    else if (first == kFALSE) {
      hitfirst = (PndSttHit *)fHitArray->At(iHit);
      first = kTRUE;
      trasl[0] = hitfirst->GetX();
      trasl[1] = hitfirst->GetY();
    } else {
      hitlast = (PndSttHit *)fHitArray->At(iHit);
      alpha = TMath::ATan2(hitlast->GetY() - hitfirst->GetY(), hitlast->GetX() - hitfirst->GetX());
    }
  }

  // error <--> resolution
  Double_t sigr, sigx, sigy; // = 0.14; //sigxy, //[R.K. 01/2017] unused variable?

  // FITTING IN X-Y PLANE:
  // v = a + bu + cu^2

  Double_t Suu, Su, Sv, Suv, S1, Suuu, Suuv, Suuuu;

  Su = 0.;
  Sv = 0.;
  Suu = 0.;
  Suv = 0.;
  Suuu = 0.;
  S1 = 0.;
  Suuv = 0.;
  Suuuu = 0.;

  Int_t digicounter = 0;
  TArrayD uarray(hitcounter);
  TArrayD varray(hitcounter);
  TArrayD sigv2array(hitcounter);
  TArrayD sigu2array(hitcounter);

  for (Int_t i = 0; i < hitcounter; i++) {
    uarray.AddAt(-999, i);
    varray.AddAt(-999, i);
    sigv2array.AddAt(-999, i);
    sigu2array.AddAt(-999, i);
  }
  for (Int_t i = 0; i < hitcounter; i++) {

    PndTrackCandHit candhit = pTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit) {
      cout << "PndSttHelixTrackFitter::XYFit: no hit at " << iHit << endl;
      continue;
    }
    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;
    Int_t refindex = currenthit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::XYFit: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wiredirection = tube->GetWireDirection();
    if (wiredirection != TVector3(0., 0., 1.))
      continue;

    if (whatToFit == 2) {
      // Double_t resx = iPoint->GetX() - currenthit->GetX(); //[R.K. 01/2017] unused variable?
      // Double_t resy = iPoint->GetY() - currenthit->GetY(); //[R.K. 01/2017] unused variable?
      // Double_t resdist = TMath::Sqrt((iPoint->GetY() - currenthit->GetY())*(iPoint->GetY() - currenthit->GetY()) + (iPoint->GetX() - currenthit->GetX())*(iPoint->GetX() -
      // currenthit->GetX())); //[R.K. 01/2017] unused variable?
    }

    Double_t xtrasl, ytrasl;
    // traslation
    xtrasl = currenthit->GetX() - trasl[0];
    ytrasl = currenthit->GetY() - trasl[1];

    Double_t xrot, yrot;
    // rotation
    xrot = TMath::Cos(alpha) * xtrasl + TMath::Sin(alpha) * ytrasl;
    yrot = -TMath::Sin(alpha) * xtrasl + TMath::Cos(alpha) * ytrasl;

    // re-traslation
    xtrasl = xrot + s;
    ytrasl = yrot;

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(1);
      TMarker *pt = new TMarker(xrot, yrot, 6);
      pt->SetMarkerColor(3);
      //  if(whatToFit == 2)
      //  pt->Draw("SAME");
    }

    // change coordinate
    Double_t u, v, sigv2, sigu2;
    u = xtrasl / (xtrasl * xtrasl + ytrasl * ytrasl);
    v = ytrasl / (xtrasl * xtrasl + ytrasl * ytrasl);

    if (fDisplayLevel >= 4) {
      eventDetails->cd(1);
      TMarker *uv = new TMarker(u, v, 6);
      // uv->SetMarkerColor(3);
      if (whatToFit == 2)
        uv->Draw("SAME");
      eventDetails->Update();
      eventDetails->Modified();
    }

    if (whatToFit == 1) {
      if (currenthit->GetIsochrone() == 0)
        sigr = sqrt(2.) / sqrt(12.);
      else
        sigr = sqrt(2.) * currenthit->GetIsochrone() / sqrt(12.);
      sigx = sigr;
      sigy = sigr;
    } else {
      if (currenthit->GetIsochrone() == 0) {
        sigr = sqrt(2.) / sqrt(12.);
        sigx = sigr;
        sigy = sigr;
      } else {
        sigr = 0.0150;
        sigx = fabs(sigr * TMath::Cos(TMath::ATan(marray.At(i))));
        sigy = fabs(sigr * TMath::Sin(TMath::ATan(marray.At(i))));
        // sigxy = sigr * TMath::Sqrt(fabs(TMath::Cos(TMath::ATan(marray.At(i))) * TMath::Sin(TMath::ATan(marray.At(i)))));; //[R.K. 01/2017] unused variable?
      }
    }

    Double_t dvdx = (-2 * xtrasl * ytrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);
    Double_t dvdy = (xtrasl * xtrasl - ytrasl * ytrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);
    Double_t dudx = (ytrasl * ytrasl - xtrasl * xtrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);
    Double_t dudy = (-2 * xtrasl * ytrasl) / pow((xtrasl * xtrasl + ytrasl * ytrasl), 2);

    sigu2 = dudx * dudx * sigx * sigx + dudy * dudy * sigy * sigy + 2 * dudx * dudy * sigx * sigy;
    sigv2 = dvdx * dvdx * sigx * sigx + dvdy * dvdy * sigy * sigy + 2 * dvdx * dvdy * sigx * sigy;

    uarray.AddAt(u, digicounter);
    varray.AddAt(v, digicounter);
    sigu2array.AddAt(sigu2, digicounter);
    sigv2array.AddAt(sigv2, digicounter);

    Su = Su + (u / sigv2);
    Sv = Sv + (v / sigv2);

    Suv = Suv + ((u * v) / sigv2);
    Suu = Suu + ((u * u) / sigv2);

    Suuu = Suuu + ((u * u * u) / sigv2);
    Suuv = Suuv + ((u * u * v) / sigv2);

    Suuuu = Suuuu + ((u * u * u * u) / sigv2);

    S1 = S1 + 1 / sigv2;

    digicounter++;
  }

  TMatrixD matrix(3, 3);
  matrix[0][0] = S1;
  matrix[0][1] = Su;
  matrix[0][2] = Suu;

  matrix[1][0] = Su;
  matrix[1][1] = Suu;
  matrix[1][2] = Suuu;

  matrix[2][0] = Suu;
  matrix[2][1] = Suuu;
  matrix[2][2] = Suuuu;

  Double_t determ;

  determ = matrix.Determinant();

  if (determ != 0) {
    matrix.Invert();
  } else {
    return 0;
    if (fVerbose == 2)
      cout << "DET 0" << endl;
  }

  TMatrixD column(3, 1);
  column[0][0] = Sv;
  column[1][0] = Suv;
  column[2][0] = Suuv;

  TMatrixD column2(3, 1);
  column2.Mult(matrix, column);

  Double_t a, b, c;
  a = column2[0][0];
  b = column2[1][0];
  c = column2[2][0];

  if (fVerbose == 2) {
    std::cout << "1) parabolic parameters:\n";
    std::cout << "a = " << column2[0][0] << "\n";
    std::cout << "b = " << column2[1][0] << "\n";
    std::cout << "c = " << column2[2][0] << "\n";
  }
  if (fabs(a) < 0.000001)
    return 0;

  Double_t chi2;
  chi2 = 0.;
  for (Int_t i = 0; i < digicounter; i++) {
    if (uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999)
      continue;
    chi2 = chi2 + pow(((varray.At(i) - (a + b * uarray.At(i) + c * uarray.At(i) * uarray.At(i))) / sqrt(sigv2array.At(i))), 2);
  }

  if (fVerbose == 2)
    cout << "digicounter: " << digicounter << endl;

  //------------------ with right errors
  Su = 0.;
  Sv = 0.;
  Suu = 0.;
  Suv = 0.;
  Suuu = 0.;
  S1 = 0.;
  Suuv = 0.;
  Suuuu = 0.;

  TArrayD sigE2array(digicounter);
  Double_t sigE2;
  for (Int_t i = 0; i < digicounter; i++) {
    if (uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999)
      continue;
    sigE2 = sigv2array.At(i) + sigu2array.At(i) * pow((b + 2 * c * uarray.At(i)), 2);
    sigE2array.AddAt(sigE2, i);

    Su = Su + (uarray.At(i) / sigE2);
    Sv = Sv + (varray.At(i) / sigE2);

    Suv = Suv + ((uarray.At(i) * varray.At(i)) / sigE2);
    Suu = Suu + ((uarray.At(i) * uarray.At(i)) / sigE2);

    Suuu = Suuu + ((uarray.At(i) * uarray.At(i) * uarray.At(i)) / sigE2);
    Suuv = Suuv + ((uarray.At(i) * uarray.At(i) * varray.At(i)) / sigE2);

    Suuuu = Suuuu + ((uarray.At(i) * uarray.At(i) * uarray.At(i) * uarray.At(i)) / sigE2);

    S1 = S1 + 1 / sigE2;
  }

  TMatrixD matrixb(3, 3);
  matrixb[0][0] = S1;
  matrixb[0][1] = Su;
  matrixb[0][2] = Suu;

  matrixb[1][0] = Su;
  matrixb[1][1] = Suu;
  matrixb[1][2] = Suuu;

  matrixb[2][0] = Suu;
  matrixb[2][1] = Suuu;
  matrixb[2][2] = Suuuu;

  determ = matrixb.Determinant();

  if (determ != 0) {
    matrixb.Invert();
  } else {
    return 0;
    if (fVerbose == 2)
      cout << "DET 0" << endl;
  }

  TMatrixD columnb(3, 1);
  columnb[0][0] = Sv;
  columnb[1][0] = Suv;
  columnb[2][0] = Suuv;

  TMatrixD column2b(3, 1);
  column2b.Mult(matrixb, columnb);

  a = column2b[0][0];
  b = column2b[1][0];
  c = column2b[2][0];

  //  std::cout << "*************\n";
  //     std::cout << "2) parabolic parameters:\n";
  //     std::cout << "a = " << column2b[0][0] <<  "\n";
  //     std::cout << "b = " << column2b[1][0] <<  "\n";
  //     std::cout << "c = " << column2b[2][0] <<  "\n";

  // v = a + bu + cu^2
  chi2 = 0.;
  for (Int_t i = 0; i < digicounter; i++) {
    if (uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999)
      continue;
    chi2 = chi2 + pow(((varray.At(i) - (a + b * uarray.At(i) + c * uarray.At(i) * uarray.At(i))) / sqrt(sigE2array.At(i))), 2);
  }

  if (fDisplayLevel >= 4) {
    eventDetails->cd(1);
    Double_t uu[100];
    Double_t vv[100];
    for (Int_t p = 0; p < 100; p++) {
      uu[p] = -1.5 + p * 3 * 0.01;
      vv[p] = a + b * uu[p] + c * uu[p] * uu[p];
    }
    TPolyLine *uvline = new TPolyLine(100, uu, vv);
    if (whatToFit == 2)
      uvline->Draw("SAME");
    eventDetails->Update();
    eventDetails->Modified();
  }

  // center and radius
  Double_t xcrot, ycrot, xc, yc, epsilon, R;
  ycrot = 1 / (2 * a);
  xcrot = -b / (2 * a);
  epsilon = -c * pow((1 + (b * b)), -3 / 2);
  R = epsilon + sqrt((xcrot * xcrot) + (ycrot * ycrot));

  // // errors on parameters -------------------
  //     // a, b, c
  //     Double_t Da[MAXNUMSTTHITS], Db[MAXNUMSTTHITS], Dc[MAXNUMSTTHITS];
  //     Double_t p1[MAXNUMSTTHITS], pu[MAXNUMSTTHITS], puu[MAXNUMSTTHITS];
  //     Double_t erra2, errb2, errc2;
  //     erra2 = 0.;
  //     errb2 = 0.;
  //     errc2 = 0.;

  //     for(Int_t i=0; i<fTrackNumSttHits[fitTrack]; i++){
  //       p1[i] = 1/sigE2[i];
  //       pu[i] = u[i]*p1[i];
  //       puu[i] = u[i]*pu[i];

  //       Da[i] = matrixb[0][0] * p1[i] + matrixb[0][1] * pu[i] + matrixb[0][2] * puu[i];
  //       Db[i] = matrixb[1][0] * p1[i] + matrixb[1][1] * pu[i] + matrixb[1][2] * puu[i];
  //       Dc[i] = matrixb[2][0] * p1[i] + matrixb[2][1] * pu[i] + matrixb[2][2] * puu[i];

  //       erra2 = erra2 + (Da[i]*Da[i]*sigE2[i]);
  //       errb2 = errb2 + (Db[i]*Db[i]*sigE2[i]);
  //       errc2 = errc2 + (Dc[i]*Dc[i]*sigE2[i]);
  //     }

  //     //  std::cout << "**************\n";
  //     //   std::cout << "erra = " <<  sqrt(erra2) << "\n";
  //     //   std::cout << "errb = " <<  sqrt(errb2)<< "\n";
  //     //   std::cout << "errc = " <<  sqrt(errc2)<< "\n";

  //     // errors on xc, yc, R
  //     Double_t errxcrot2, errycrot2, errR2;
  //     errxcrot2 = (1./(4*pow(a,4))) * (b*b*erra2 + a*a*errb2 - 2*a*b*sqrt(erra2*errb2)) ;
  //     errycrot2 = (1./4)*(erra2/pow(a,4)) ;
  //     errR2 = (1./(R*R)) * (ycrot*ycrot*errxcrot2 + xcrot*xcrot*errycrot2 + 2*xcrot*ycrot*sqrt(errxcrot2*errycrot2)) ;

  // re-rotation and re-traslation of xc and yc
  // translation
  xcrot = xcrot - s;
  // rotation
  xc = TMath::Cos(alpha) * xcrot - TMath::Sin(alpha) * ycrot;
  yc = TMath::Sin(alpha) * xcrot + TMath::Cos(alpha) * ycrot;
  // traslation
  xc = xc + trasl[0];
  yc = yc + trasl[1];
  Double_t phi = TMath::ATan2(yc, xc);
  Double_t d;
  d = ((xc + yc) - R * (TMath::Cos(phi) + TMath::Sin(phi))) / (TMath::Cos(phi) + TMath::Sin(phi));

  fTrack->SetDist(d);
  fTrack->SetPhi(phi);
  //    Double_t newZ = -999.; // CHECK da cambiare
  //    fTrack->SetZ(newZ);
  fTrack->SetRad(R);
  //    Double_t newTheta = -999.; // CHECK da cambiare
  //    fTrack->SetTanL(newTheta);
  h = -GetCharge(d, phi, R);
  fTrack->SetCharge(-h);

  //   cout << "FIT: " << xc << " " << yc << endl;
  //   cout << "RAGGIO: " << R << endl;

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(1);
    TArc *fitarc = new TArc(((fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi())), ((fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi())), fTrack->GetRad());
    if (whatToFit == 2)
      fitarc->SetLineColor(kRed - 9);
    fitarc->SetFillStyle(0);
    fitarc->Draw("SAME");
    eventCanvas->Update();
    eventCanvas->Modified();
  }
  return 1;
}

// -------------- IntersectionFinder  --------------------------------------
Bool_t PndSttHelixTrackFitter::IntersectionFinder(PndTrackCand *pTrackCand)
{

  ResetMArray();

  // calculation of the curvature from the helix prefit
  if (fTrack->GetRad() == 0)
    return kFALSE;

  // vec = (xc, yc)
  TVector2 vec((fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi()), (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi()));

  //==========
  // POINT ----------------------------------------------------
  // 1. find the cooordinates of the point fired wire of the track
  TVector2 point; // point
  Double_t radius;

  Int_t counter = 0;
  // loop over input points
  Int_t hitcounter = pTrackCand->GetNHits();
  for (Int_t k = 0; k < hitcounter; k++) {
    // get hit
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(k);
    Int_t iHit = candhit.GetHitId();

    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::IntersectionFinder: no hit at " << iHit << endl;
      continue;
    }

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::IntersectionFinder: no point at " << refindex << " associated  to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TVector3 wiredirection = tube->GetWireDirection();

    if (wiredirection != TVector3(0., 0., 1.))
      continue;

    // [xp, yp] point = coordinates xy of the centre of the firing tube
    point.Set(tube->GetPosition().X(), tube->GetPosition().Y());
    radius = pMhit->GetIsochrone();

    // the coordinates of the point are taken from the intersection
    // between the circumference from the drift time and the R radius of
    // curvature. -------------------------------------------------------
    // 2. find the intersection between the little circle and the line // R
    // 2.a
    // find the line passing throught [xc, yc] (centre of curvature) and [xp, yp] (first wire)
    // y = mx + q
    Double_t m = (point.Y() - vec.Y()) / (point.X() - vec.X());
    Double_t q = point.Y() - m * point.X();

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(1);
      //       TArc *archetto = new TArc(point.X(), point.Y(), radius);
      //  archetto->SetFillStyle(0);//       archetto->Draw("SAME");
      TLine *line = new TLine(-50, -50 * m + q, 50, 50 * m + q);
      line->SetLineColor(5);
      // line->Draw("SAME");
      eventCanvas->Update();
      eventCanvas->Modified();
    }

    // cut on radius CHECK
    // if the simulated radius is too small, the pMhit
    // is not used for the fit
    if (radius < 0.1) {
      marray.AddAt(-999, k);
      pMhit->SetX(-999);
      pMhit->SetY(-999);
      continue; // CHECK
    }

    Double_t x1 = 0, y1 = 0, x2 = 0, y2 = 0, xb1 = 0, yb1 = 0, xb2 = 0, yb2 = 0;

    // CHECK the vertical track
    if (fabs(point.X() - vec.X()) < 1e-6) {

      // 2.b
      // intersection little circle and line --> [x1, y1]
      // + and - refer to the 2 possible intersections
      // +
      x1 = point.X();
      y1 = point.Y() + sqrt(radius * radius - (x1 - point.X()) * (x1 - point.X()));
      // -
      x2 = x1;
      y2 = point.Y() - sqrt(radius * radius - (x2 - point.X()) * (x2 - point.X()));

      // 2.c intersection between line and circle
      // +
      xb1 = vec.X();
      yb1 = vec.Y() + sqrt(fTrack->GetRad() * fTrack->GetRad() - (xb1 - vec.X()) * (xb1 - vec.X()));
      // -
      xb2 = xb1;
      yb2 = vec.Y() - sqrt(fTrack->GetRad() * fTrack->GetRad() - (xb2 - vec.X()) * (xb2 - vec.X()));

    } // END CHECK
    else {

      // 2.b
      // intersection little circle and line --> [x1, y1]
      // + and - refer to the 2 possible intersections
      // +
      x1 = (-(m * (q - point.Y()) - point.X()) + sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                      (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius))) /
           (m * m + 1);
      y1 = m * x1 + q;
      // -
      x2 = (-(m * (q - point.Y()) - point.X()) - sqrt((m * (q - point.Y()) - point.X()) * (m * (q - point.Y()) - point.X()) -
                                                      (m * m + 1) * ((q - point.Y()) * (q - point.Y()) + point.X() * point.X() - radius * radius))) /
           (m * m + 1);
      y2 = m * x2 + q;

      // 2.c intersection between line and circle
      // +
      xb1 = (-(m * (q - vec.Y()) - vec.X()) + sqrt((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) -
                                                   (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - (fTrack->GetRad()) * (fTrack->GetRad())))) /
            (m * m + 1);
      yb1 = m * xb1 + q;
      // -
      xb2 = (-(m * (q - vec.Y()) - vec.X()) - sqrt((m * (q - vec.Y()) - vec.X()) * (m * (q - vec.Y()) - vec.X()) -
                                                   (m * m + 1) * ((q - vec.Y()) * (q - vec.Y()) + vec.X() * vec.X() - (fTrack->GetRad()) * (fTrack->GetRad())))) /
            (m * m + 1);
      yb2 = m * xb2 + q;
    }

    // calculation of the distance between [xb, yb] and [xp, yp]
    Double_t distb1 = sqrt((yb1 - point.Y()) * (yb1 - point.Y()) + (xb1 - point.X()) * (xb1 - point.X()));
    Double_t distb2 = sqrt((yb2 - point.Y()) * (yb2 - point.Y()) + (xb2 - point.X()) * (xb2 - point.X()));

    // choice of [xb, yb]
    TVector2 xyb;
    if (distb1 > distb2)
      xyb.Set(xb2, yb2);
    else
      xyb.Set(xb1, yb1);

    // calculation of the distance between [x, y] and [xb. yb]
    Double_t dist1 = sqrt((xyb.Y() - y1) * (xyb.Y() - y1) + (xyb.X() - x1) * (xyb.X() - x1));
    Double_t dist2 = sqrt((xyb.Y() - y2) * (xyb.Y() - y2) + (xyb.X() - x2) * (xyb.X() - x2));

    // choice of [x, y]
    TVector2 *xy;
    if (dist1 > dist2)
      xy = new TVector2(x2, y2);
    else
      xy = new TVector2(x1, y1); // <========= THIS IS THE NEW POINT to be used for the fit

    // SET AS DEBUG
    //    if (TMath::Sqrt((xy->X() - point.X())*(xy->X() - point.X()) + (xy->Y() - point.Y())*(xy->Y() - point.Y())) - radius > 0.000001) {
    //      cout << "ATTENZIONE: " << "differenza = " << TMath::Sqrt((xy->X() - point.X())*(xy->X() - point.X()) + (xy->Y() - point.Y())*(xy->Y() - point.Y())) - radius << endl;
    //    }

    //    cout << "x hit prima: " << tube->GetPosition().X() << " " << pMhit->GetX() << endl;
    //    cout << prima: " << pMhit->GetX() << " " << pMhit->GetY() << " " << pMhit->GetZ() << endl;

    marray.AddAt(m, k);
    pMhit->SetX(xy->X());
    pMhit->SetY(xy->Y());

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(1);
      TMarker *np = new TMarker(pMhit->GetX(), pMhit->GetY(), 6);
      np->SetMarkerColor(3);
      //      np->Draw("SAME");
      eventCanvas->Update();
      eventCanvas->Modified();
    }
    delete xy;
    counter++;
  }

  if (counter == 0)
    return kFALSE;
  else
    return kTRUE;
}

// ZFinder was ZFinderbb3
// -------- ZFinder --------------------------------------------
Bool_t PndSttHelixTrackFitter::ZFinder(PndTrackCand *pTrackCand, Int_t)
{ // whatToFit //[R.K.03/2017] unused variable(s)
  // the z finding procedure uses the hough transform to find the line
  // in the plane z - track length on which the correct points lie.

  if (fVerbose == 2)
    cout << "ZFINDER" << endl;

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();

  // cut on number of hits
  //   if(hitcounter > 50) {
  //     cout << "more than 50 hits " << hitcounter << endl;
  //     return 0;
  //   }
  if (hitcounter < 5) {
    if (fVerbose == 2)
      cout << "less than 5 hits" << endl;
    return 0;
  }

  ZPointsArray = new TObjArray(2 * hitcounter);

  // ZFIT =======
  if (hitcounter == 0)
    return kFALSE;

  // Double_t Sxx, Sx, Sz, Sxz, S1z; //[R.K.01/2017]unused variable?
  // Double_t Detz = 0.; //[R.K. 01/2017] unused variable?
  // Double_t fitm, fitp; //[R.K. 01/2017] unused variable?
  Double_t sigz = 1.; // CHECK

  // Sx = 0.; //[R.K.01/2017]unused variable?
  // Sz = 0.; //[R.K.01/2017]unused variable?
  // Sxx = 0.; //[R.K.01/2017]unused variable?
  // Sxz = 0.; //[R.K.01/2017]unused variable?
  // S1z = 0.; //[R.K.01/2017]unused variable?
  //// ============

  TVector3 *tofit, *tofit2;

  // centre of curvature
  Double_t x_0 = (fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi());
  Double_t y_0 = (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi());

  // radius of curvature
  Double_t R = fTrack->GetRad();
  Int_t wireOk = 0;
  // Bool_t first = kTRUE; //[R.K.01/2017]unused variable?

  for (Int_t i = 0; i < hitcounter; i++) {

    // get index of hit
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFinder: no hit at " << iHit << endl;
      continue;
    }

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFinder: no point at " << refindex << " associated  to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wiredirection = tube->GetWireDirection();
    TVector3 wiredirection2;

    wiredirection2 = tube->GetHalfLength() * wiredirection;
    TVector3 cenposition = tube->GetPosition();
    //    if(tube->GetPosition().Z() != 35) continue; // to throw away short tubes

    TVector3 min, max;
    min = cenposition - wiredirection2;
    max = cenposition + wiredirection2;

    // first extremity
    Double_t x_1 = min.X();
    Double_t y_1 = min.Y();
    Double_t z_1 = min.Z();

    // second extremity
    Double_t x_2 = max.X();
    Double_t y_2 = max.Y();
    Double_t z_2 = max.Z();

    Double_t rcur = pMhit->GetIsochrone();

    Double_t x1 = -9999.;
    Double_t y1 = -9999.;
    Double_t x2 = -9999.;
    Double_t y2 = -9999.;

    // from xy plane fit
    Double_t phi0 = fTrack->GetPhi();
    Double_t d0 = fTrack->GetDist();
    Double_t x0 = d0 * TMath::Cos(phi0);
    Double_t y0 = d0 * TMath::Sin(phi0);
    // in xy plane: angle of the PCA to the origin
    // with respect to the curvature center
    Double_t Phi0 = TMath::ATan2((y0 - y_0), (x0 - x_0));

    if (wiredirection != TVector3(0., 0., 1.)) {
      Double_t a = -999;
      Double_t b = -999;

      // intersection point between the reconstructed
      // circumference and the line joining the centres
      // of the reconstructed circle and the i_th drift circle
      if (fabs(x_2 - x_1) > 0.0001) {
        a = (y_2 - y_1) / (x_2 - x_1);
        b = (y_1 - a * x_1);
        Double_t A = a * a + 1;
        Double_t B = x_0 + a * y_0 - a * b;
        Double_t C = x_0 * x_0 + y_0 * y_0 + b * b - R * R - 2 * y_0 * b;
        if ((B * B - A * C) > 0) {
          x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        }
      } else if (fabs(y_2 - y_1) > 0.0001) {
        Double_t A = 1;
        Double_t B = y_0;
        Double_t C = y_0 * y_0 + (x_1 - x_0) * (x_1 - x_0) - R * R;

        if ((B * B - A * C) > 0) {
          y1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          y2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          x1 = x2 = x_1;
        }
      } else {
        if (fVerbose == 2)
          LOG(error) << " intersection point not found";
        continue;
      }

      // x1 and x2 are the 2 intersection points
      Double_t d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
      Double_t d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

      Double_t x_ = x1;
      Double_t y_ = y1;

      // the intersection point nearest to the drift circle's centre is taken
      if (d2 < d1) {
        x_ = x2;
        y_ = y2;
      }

      if (fDisplayLevel >= 3) {
        eventCanvas->cd(1);
        TMarker *pt = new TMarker(x_, y_, 6);
        pt->SetMarkerColor(6);
        pt->Draw("SAME");

        // intersection lines
        TLine *intline = new TLine(x_, y_, x_0, y_0);
        intline->SetLineColor(5);
        intline->Draw("SAME");
      }

      // now we need to find the actual centre of the drift circle,
      // by translating the drift circle until it becomes tangent
      // to the reconstructed circle.
      // Two solutions are possible (left rigth abiguity),
      // they are both kept, only the following zed fit will discard the wrong ones.
      // Using the parametric equation of the 3d-straigth line and taking the
      // x points just obtained, the zed coordinate of the skewed tube centre is calculated.

      if (x_1 == x_2) {
        // if the skewed layer lies in yz plane
        x1 = x_;
        y1 = y_ + rcur;
        x2 = x_;
        y2 = y_ - rcur;
      } else {
        // solving the equation to find out the centre of the tangent circle
        Double_t A = a * a + 1;
        Double_t B = -(a * b - a * y_ - x_);
        Double_t C = x_ * x_ + y_ * y_ + b * b - 2 * b * y_ - rcur * rcur;
        if ((B * B - A * C) > 0) {
          x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        } else if ((B * B - A * C) == 0) {
          x1 = B / A;
          x2 = x1;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        } else {
          if (fVerbose == 2)
            cout << "NO WAY2" << endl;
          continue;
        }
      }
      if (fDisplayLevel >= 3) {
        eventCanvas->cd(1);
        TArc *drftarc = new TArc(x1, y1, rcur);
        drftarc->SetLineColor(7);
        drftarc->SetFillStyle(0);
        drftarc->Draw("SAME");
        TArc *drftarc2 = new TArc(x2, y2, rcur);
        drftarc2->SetLineColor(7);
        drftarc2->SetFillStyle(0);
        drftarc2->Draw("SAME");
      }

      d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
      d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

      Double_t xcen0 = x1;
      Double_t xcen1 = x2;
      Double_t ycen0 = y1;
      Double_t ycen1 = y2;

      if (d2 < d1) { // z2 contains the points nearest (in x-y) to the initial centre of the skewed tube
        xcen0 = x2;
        xcen1 = x1;
        ycen0 = y2;
        ycen1 = y1;
      }

      // zed association
      Double_t t_, z_, t_bis, z_bis;
      if (fabs(x_2 - x_1) < 0.001) {
        t_ = (ycen0 - y_1) / (y_2 - y_1);    // k= a_y*t + y_1 [t=1 y=y_2]
        z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
        t_bis = (ycen1 - y_1) / (y_2 - y_1); // from y_'s (the 2 solutions of the 2nd order equation)
        z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
      } else {
        t_ = (xcen0 - x_1) / (x_2 - x_1);    // x= a_x*t + x_1 [t=1 x=x_2]
        z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
        t_bis = (xcen1 - x_1) / (x_2 - x_1); // from x_'s (the 2 solutions of the 2nd order equation)
        z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
      }

      //	tofit = new TVector3(xcen0,ycen0,z_);
      tofit = new TVector3(x_, y_, z_);
      ZPointsArray->Add(tofit);
      //	tofit2 = new TVector3(xcen1,ycen1,z_bis);
      tofit2 = new TVector3(x_, y_, z_bis);
      ZPointsArray->Add(tofit2);

      if (fDisplayLevel >= 4) {
        eventDetails->cd(3);

        // xz plane
        // found point 1
        TMarker *mrkt2 = new TMarker(x_, z_, 6);
        mrkt2->SetMarkerColor(wireOk);
        mrkt2->Draw("SAME");
        // found point2
        TMarker *mrkt2bis = new TMarker(x_, z_bis, 6);
        mrkt2bis->SetMarkerColor(wireOk);
        mrkt2bis->Draw("SAME");

        eventDetails->cd(4);
        // yz plane
        // found point 1
        TMarker *mrkt2b = new TMarker(y_, z_, 6);
        mrkt2b->SetMarkerColor(wireOk);
        mrkt2b->Draw("SAME");
        // found point 2
        TMarker *mrkt2bbis = new TMarker(y_, z_bis, 6);
        mrkt2bbis->SetMarkerColor(wireOk);
        mrkt2bbis->Draw("SAME");
      }

      if (fDisplayLevel >= 2) {
        eventCanvas->cd(2);
        // MC point in z - track length plane
        TMarker *MCmrk = new TMarker(h * R *
                                       TMath::ATan2((iPoint->GetY() - y0) * TMath::Cos(Phi0) - (iPoint->GetX() - x0) * TMath::Sin(Phi0),
                                                    R + (iPoint->GetX() - x0) * TMath::Cos(Phi0) + (iPoint->GetY() - y0) * TMath::Sin(Phi0)),
                                     iPoint->GetZ(), 6);
        MCmrk->SetMarkerColor(4);
        MCmrk->Draw("SAME");
      }

      // ------- HOUGH TRANSFORM ------------
      // FIRST CHOICE
      if (tofit)
        Hough(tofit, Phi0, x0, y0, R);

      // SECOND CHOICE
      if (tofit2)
        Hough(tofit2, Phi0, x0, y0, R);
      wireOk++;
    }
  }

  //  cout << "skewed: " << wireOk << endl;

  TVector3 outz = GetHoughResponse();

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(2);
    // found line in z track length plane
    TLine *line = new TLine(-40, -40 * outz.X() + outz.Y(), 40, (40 * outz.X() + outz.Y()));
    line->Draw("SAME");
  }

  //  pTrack->GetParamFirst()->SetX(outz.Z()); // CHECK what is this?!!!!!

  // if votes < 6 consider the fit failed CHECK
  //  if(outz.Z() < 6) return kFALSE;

  Int_t counter = 0;

  if (outz.X() == 0. && outz.Y() == 0.)
    return kFALSE;

  if (wireOk < 2) {
    return kFALSE;
  }

  // first = kTRUE; //[R.K.01/2017]unused variable?
  wireOk = 0;
  Int_t okcounter = 0;
  for (Int_t i = 0; i < (2 * hitcounter); i += 2) {
    // get index of hit
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(counter);
    Int_t iHit = candhit.GetHitId();
    counter++;

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFinder: no hit at " << iHit << endl;
      continue;
    }

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFinder: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TVector3 wiredirection = tube->GetWireDirection();

    if (wiredirection == TVector3(0., 0., 1.))
      continue;
    wireOk++;

    sigz = 1.; // CHECK

    // FIRST CHOICE .................
    TVector3 *vi = (TVector3 *)ZPointsArray->At(okcounter);

    if (vi == nullptr)
      continue;
    Double_t scos = fTrack->CalculateScosl(vi->X(), vi->Y());

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(2);
      // first point z track lenght plane
      TMarker *mrk = new TMarker(scos, vi->Z(), 6);
      mrk->Draw("SAME");
    }

    Bool_t fitdone = kFALSE;

    // distance between the found z and the predicted from the found line one
    Double_t distfirst = pow(((vi->Z() - (outz.Y() + outz.X() * scos)) / sigz), 2);
    if (distfirst < 10) {
      pMhit->SetX(vi->X());
      pMhit->SetY(vi->Y());
      pMhit->SetZ(vi->Z());
      fitdone = kTRUE;
    }

    // SECOND CHOICE ...................
    vi = (TVector3 *)ZPointsArray->At(okcounter + 1);
    if (vi == nullptr)
      continue;
    scos = fTrack->CalculateScosl(vi->X(), vi->Y());

    Double_t distsecond = pow(((vi->Z() - (outz.Y() + outz.X() * scos)) / sigz), 2);

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(2);
      TMarker *mrk2 = new TMarker(scos, vi->Z(), 6);
      mrk2->Draw("SAME");
    }

    // Xint Yin Zint set
    if (fitdone == kTRUE) {
      if (distsecond < 10 && distsecond < distfirst) {
        pMhit->SetX(vi->X());
        pMhit->SetY(vi->Y());
        pMhit->SetZ(vi->Z());
      }
    } else {
      if (distsecond < 10) {
        pMhit->SetX(vi->X());
        pMhit->SetY(vi->Y());
        pMhit->SetZ(vi->Z());
        fitdone = kTRUE;
      } else {
        pMhit->SetX(-999);
        pMhit->SetY(-999);
        pMhit->SetZ(-999);
      }
    }

    if (fDisplayLevel >= 3) {
      if (pMhit->GetZ() != -999) {
        eventCanvas->cd(2);
        TMarker *mrk3 = new TMarker(scos, pMhit->GetZ(), 6);
        mrk3->SetMarkerColor(3);
        mrk3->Draw("SAME");
      }
    }

    okcounter = okcounter + 2;
  }

  return kTRUE;
  //   fiterrp = sqrt(fiterrp2);
  //   fiterrm = sqrt(fiterrm2);
}

// ZFit was Zfitbb2
// ----- Zfit  ----------------------------------------
Int_t PndSttHelixTrackFitter::ZFit(PndTrackCand *pTrackCand, Int_t)
{ // whatToFit //[R.K.03/2017] unused variable(s)

  if (fVerbose == 2)
    cout << "ZFIT" << endl;

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();

  // cut on number of hits
  //   if(hitcounter > 50) return 0;
  if (hitcounter < 5)
    return 0;

  // CHECK: ATTENTION
  // refit for error correction missing

  // SCOSL ======
  // get 1st hit
  // PndSttHit *fMhit = (PndSttHit*) fHitArray->At(pTrackCand->GetSortedHit(0).GetHitId()); //[R.K. 01/2017] unused variable?

  Double_t Sxx, Sx, Sz, Sxz, S1z;
  Double_t Detz = 0.;
  Double_t fitm, fitp;
  Double_t sigz = 1.; // CHECK

  Sx = 0.;
  Sz = 0.;
  Sxx = 0.;
  Sxz = 0.;
  S1z = 0.;

  // centre of curvature
  // Double_t x_0 = (fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi()); //[R.K. 01/2017] unused variable?
  // Double_t y_0 = (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi()); //[R.K. 01/2017] unused variable?
  // radius of curvature
  // Double_t R  = fTrack->GetRad(); //[R.K. 01/2017] unused variable?
  Int_t counter = 0;
  Int_t wireOk = 0;
  // Bool_t first = kTRUE; //[R.K. 01/2017] unused variable?
  for (Int_t i = 0; i < hitcounter; i++) {

    // get index of hit
    PndTrackCandHit candhit = fTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFit: no hit at " << iHit << endl;
      continue;
    }

    if (pMhit->GetX() == -999 || pMhit->GetY() == -999 || pMhit->GetZ() == -999)
      continue; // CHECK

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFit: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wiredirection = tube->GetWireDirection();

    if (wiredirection == TVector3(0., 0., 1.))
      continue;
    //    if(tube->GetPosition().Z() != 35) continue; // for the moment I throw away short tubes

    TVector3 *vi = new TVector3(pMhit->GetX(), pMhit->GetY(), pMhit->GetZ());

    // if the found z is > 75 cm or < -75 cm continue: this has to be fixed
    if (pMhit->GetZ() < (tube->GetPosition().Z() - tube->GetHalfLength()) || pMhit->GetZ() > (tube->GetPosition().Z() + tube->GetHalfLength()))
      continue; // CHECK

    Double_t scos = fTrack->CalculateScosl(pMhit->GetX(), pMhit->GetY());

    Sx = Sx + (scos / (sigz * sigz));
    Sz = Sz + (vi->Z() / (sigz * sigz));
    Sxz = Sxz + ((scos * vi->Z()) / (sigz * sigz));
    Sxx = Sxx + ((scos * scos) / (sigz * sigz));
    S1z = S1z + 1 / (sigz * sigz);
    counter++;
    wireOk++;
  }

  if (counter == 0)
    return 0;

  Detz = S1z * Sxx - Sx * Sx;
  if (Detz == 0) {
    return 0;
  }

  fitp = (1 / Detz) * (Sxx * Sz - Sx * Sxz);
  fitm = (1 / Detz) * (S1z * Sxz - Sx * Sz);

  // y = m*x + p
  TVector2 outz(fitm, fitp);

  // fill in parameters
  fTrack->SetTanL(fitm); // tan(lambda)
  fTrack->SetZ(fitp);    // z0

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(2);

    // fit line
    TLine *line2 = new TLine(-40, -40 * fitm + fitp, 40, (40 * fitm + fitp));
    line2->SetLineColor(kGreen - 9);
    line2->Draw("SAME");
  }
  return 1;
  //   fiterrp = sqrt(fiterrp2);
  //   fiterrm = sqrt(fiterrm2);
}
// -------------------------------------------------------------------------------

void PndSttHelixTrackFitter::Hough(TVector3 *choice, Double_t Phi0, Double_t x0, Double_t y0, Double_t R)
{

  // since the points stay on a line in z track length plane
  // I can use the hough transform to transform the points in
  // z track length plane to lines in the parameters plane ab
  // (scos = a * z + b) and find their intersections.
  //
  // grid:
  // a [-pi/2 , pi/2]; 200 steps
  // b [-150, 150];    1000 steps  // CHECK deve essere -75, 75

  Double_t y = choice->Z();
  Double_t x =
    h * R *
    TMath::ATan2((choice->Y() - y0) * TMath::Cos(Phi0) - (choice->X() - x0) * TMath::Sin(Phi0), R + (choice->X() - x0) * TMath::Cos(Phi0) + (choice->Y() - y0) * TMath::Sin(Phi0));

  // Hough  -------------------------- angle [-90, 90]
  Double_t a = -TMath::Pi() / 2.;
  Double_t tga;
  Double_t b = -150;

  // 200 step per -pi/2 < a < pi/2
  for (Int_t i = 0; i < 200; i++) {
    b = -150;
    tga = TMath::Tan(a);
    // 1000 step per -150 < b < 150
    for (Int_t j = 0; j <= 1000; j++) {
      Double_t ycalc = x * tga + b;

      //  if(fabs(ycalc - y) < (0.3 + TMath::Tan(TMath::Pi()/200.) * x)) {
      if (fabs(ycalc - y) < 0.2) {
        vote[i][j] += 1;
      }

      b += 0.3;
    }
    a += (TMath::Pi() / 200.);
  }
}

TVector3 PndSttHelixTrackFitter::GetHoughResponse()
{
  Double_t a = -TMath::Pi() / 2.;
  Double_t tga;
  Double_t b = -150;
  TMarker *mrk2;

  Double_t aref, bref, vref = 0;
  // Double_t aref2, bref2, vref2 = 0, cref2 = 0; //[R.K. 01/2017] unused variable?
  for (Int_t i = 0; i <= 200; i++) {
    tga = TMath::Tan(a);
    b = -150;
    for (Int_t j = 0; j <= 1000; j++) {
      if (vote[i][j] != 0) {
        if (fDisplayLevel >= 4) {
          eventDetails->cd(2);
          mrk2 = new TMarker(tga, b, 6);
          mrk2->SetMarkerColor(3);
          mrk2->Draw("SAME");
        }

        if (vote[i][j] >= vref) {
          aref = tga;
          bref = b;
          vref = vote[i][j];

          // 	      if(vote[i][j] > vref) {
          // 		aref2 = tga;
          // 		bref2 = b;
          // 		vref2 = vote[i][j];
          // 		cref2 = 1;
          // 	      }
          // 	      else if(vote[i][j] > vref) {
          // 		aref2 += tga;
          // 		bref2 += b;
          // 		cref2++;
          // 	      }
        }
      }
      b += 0.3;
    }
    a += (TMath::Pi() / 200.);
  }
  //   if(vref < vref2)
  // 	{
  // 	  aref = aref2/cref2;
  // 	  bref = bref2/cref2;
  //      vref = vref2;
  //    }

  TVector3 hough(aref, bref, vref);

  // reset votes
  for (Int_t i = 0; i <= 200; i++)
    for (Int_t j = 0; j <= 1000; j++)
      vote[i][j] = 0;
  return hough;
}

// ------ Extrapolate ------------------------------------------------------------
void PndSttHelixTrackFitter::Extrapolate(PndSttTrack *, Double_t, FairTrackParam *) // track, r, param //[R.K.03/2017] unused variable(s)
{
  LOG(warn) << " PndSttMinuitTrackFitter::Extrapolate: Not yet implemented, sorry!";
}

void PndSttHelixTrackFitter::ResetMArray()
{
  for (Int_t i = 0; i < 100; i++)
    marray.AddAt(-999, i);
}

Int_t PndSttHelixTrackFitter::SetUpFitVector(PndTrackCand *pTrackCand, TMatrixT<Double_t> &fitvect)
{
  int nhits = pTrackCand->GetNHits();
  fitvect.ResizeTo(nhits, 4); // x y r errr

  PndSttHit *currenthit = nullptr;
  int counter = 0;
  for (int i = 0; i < nhits; i++) {
    PndTrackCandHit candhit = fTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit) {
      cout << "PndSttHelixTrackFitter::SetUpFitVector: no hit at " << iHit << endl;
      continue;
    }

    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    if (tube->GetWireDirection() != TVector3(0., 0., 1.))
      continue;
    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;

    fitvect[counter][0] = currenthit->GetX();
    fitvect[counter][1] = currenthit->GetY();
    fitvect[counter][2] = currenthit->GetIsochrone();
    fitvect[counter][3] = currenthit->GetIsochroneError();

    counter++;
  }
  if (nhits != counter) {
    fitvect.ResizeTo(counter, 4); // x y r errr
  }

  return counter;
}

Int_t PndSttHelixTrackFitter::MinuitFit(PndTrackCand *pTrackCand, Int_t whatToFit)
{
  if (fVerbose == 2)
    cout << "MINUIT FIT " << pTrackCand->GetNHits() << endl;

  fEventCounter++;
  // Double_t hitcounter = pTrackCand->GetNHits(); //[R.K. 01/2017] unused variable?

  Double_t rstart = fTrack->GetRad();
  Double_t xcstart = (fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi());
  Double_t ycstart = (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi());

  TMinuit minimizer(3);

  // set to quiet
  if (fVerbose < 2)
    minimizer.SetPrintLevel(-1);

  if (fVerbose == 2) {
    cout << "*******MINUIT********" << endl;
    cout << "D   SEED: " << fTrack->GetDist() << endl;
    cout << "PHI SEED: " << fTrack->GetPhi() << endl;
    cout << "R   SEED: " << fTrack->GetRad() << endl;
    cout << "********************" << endl;
  }

  // set the object to be fitted:
  // TMatrixT<Double_t> [x][y][r][err_r]
  TMatrixT<Double_t> fitvect;
  SetUpFitVector(pTrackCand, fitvect);

  if (whatToFit == 1)
    minimizer.SetFCN(fcnHelix);
  else
    minimizer.SetFCN(fcnHelix2);
  //  minimizer.SetErrorDef(1);  // ???

  minimizer.DefineParameter(0, "xc", xcstart, 0.1, -3000., 3000.); // ???
  minimizer.DefineParameter(1, "yc", ycstart, 0.1, -3000., 3000.); // ??? LIMITS ???
  minimizer.DefineParameter(2, "r", rstart, 0.1, 0., 3000.);       // ???
  if (fVerbose == 2)
    cout << "xcstart: " << xcstart << " ycxtart: " << ycstart << " rstart: " << rstart << endl;
  minimizer.SetObjectFit(&fitvect);

  minimizer.SetPrintLevel(-1);

  minimizer.SetMaxIterations(500);

  minimizer.Migrad();

  // Double_t chisquare;  //[R.K. 01/2017] unused variable?
  Double_t resultsRadial[3], errorsRadial[3];

  minimizer.GetParameter(0, resultsRadial[0], errorsRadial[0]);
  minimizer.GetParameter(1, resultsRadial[1], errorsRadial[1]);
  minimizer.GetParameter(2, resultsRadial[2], errorsRadial[2]);

  //  minimizer.Eval(3, nullptr, chisquare, resultsRadial, 0); // ???

  if (fVerbose == 2) {
    cout << "xc: " << resultsRadial[0] << endl;
    cout << "yc: " << resultsRadial[1] << endl;
    cout << "R:  " << resultsRadial[2] << endl;
  }

  Double_t phi = TMath::ATan2(resultsRadial[1], resultsRadial[0]); // CHECK
  Double_t d;
  d = ((resultsRadial[0] + resultsRadial[1]) - resultsRadial[2] * (TMath::Cos(phi) + TMath::Sin(phi))) / (TMath::Cos(phi) + TMath::Sin(phi)); // CHECK

  //   pTrack->SetChi2Rad(chisquare);          // CHECK how to handle this
  //   pTrack->SetNDF(fTrackCand->GetNHits()); // CHECK how to handle this

  fTrack->SetDist(d);
  fTrack->SetPhi(phi);
  //    fTrack->SetZ(z0);
  fTrack->SetRad(resultsRadial[2]);
  //    fTrack->SetTanL(alpha);
  h = -GetCharge(d, phi, resultsRadial[2]);
  fTrack->SetCharge(-h);

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(1);
    TArc *fitarc = new TArc(((fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi())), ((fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi())), fTrack->GetRad());
    fitarc->SetLineColor(kGreen - 9);
    fitarc->SetFillStyle(0);
    fitarc->Draw("SAME");
    eventCanvas->Update();
    eventCanvas->Modified();
  }

  return 1;
}

void fcnHelix(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) // npar, gin, iflag //[R.K.03/2017] unused variable(s)
{

  TMatrixT<Double_t> *mama = (TMatrixT<Double_t> *)gMinuit->GetObjectFit();

  Double_t chisq = 0;
  Double_t delta = 0;
  Int_t hitcounter = mama->GetNrows();
  for (Int_t i = 0; i < hitcounter; i++) {
    delta = sqrt((mama[0][i][0] - par[0]) * (mama[0][i][0] - par[0]) + (mama[0][i][1] - par[1]) * (mama[0][i][1] - par[1])) - par[2];
    if (mama[0][i][2] == 0)
      chisq += (delta * delta * 12.);
    else
      chisq += (delta * delta) / (mama[0][i][2] * mama[0][i][2] / 12.);
  }
  f = chisq;
}

void fcnHelix2(Int_t &, Double_t *, Double_t &f, Double_t *par, Int_t) // npar, gin, iflag //[R.K.03/2017] unused variable(s)
{
  TMatrixT<Double_t> *mama = (TMatrixT<Double_t> *)gMinuit->GetObjectFit();

  Double_t chisq = 0;
  Double_t delta = 0;
  Int_t hitcounter = mama->GetNrows();
  for (Int_t i = 0; i < hitcounter; i++) {
    delta = sqrt((mama[0][i][0] - par[0]) * (mama[0][i][0] - par[0]) + (mama[0][i][1] - par[1]) * (mama[0][i][1] - par[1])) - par[2];
    if (mama[0][i][2] == 0)
      chisq += (delta * delta * 12.);
    else
      chisq += (delta * delta) / (pow(mama[0][i][3], 2));
  }

  f = chisq;
}

PndSttHit *PndSttHelixTrackFitter::GetHitFromCollections(Int_t hitCounter) const
{
  PndSttHit *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      retval = (PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
      break;
    } else {
      relativeCounter -= size;
    }
  }

  return retval;
}

Double_t PndSttHelixTrackFitter::GetHitAngle(Int_t hitNo, Double_t dCenter, Double_t phiCenter, Double_t radius)
{
  PndSttHit *pMhit = nullptr;

  pMhit = GetHitFromCollections(hitNo);

  // CHECK added
  Int_t tubeID = pMhit->GetTubeID();
  PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

  Double_t xCenter = (dCenter + radius) * cos(phiCenter), yCenter = (dCenter + radius) * sin(phiCenter);

  // get XY of wire
  Double_t deltaX = tube->GetPosition().X() - xCenter, deltaY = tube->GetPosition().Y() - yCenter;

  // CHECK: the old way is method 1, but I thinkl method 2 is better. Check what
  // kind of angle exactly is used and choose one of the two methods (anyway, up to now
  // both works, so it is not urgent).

  // method 1
  Double_t angle = 0;
  if (deltaX != 0)
    angle = atan(deltaY / deltaX);
  else
    angle = TMath::Pi() / 2.;
  if (deltaY < 0.)
    angle += dPi;

  // bring angle into the usual frame of reference
  if (deltaX < 0.) {
    if (deltaY < 0.)
      angle -= dPi;
    else
      angle += dPi;
  }

  // method 2
  //    Double_t
  //      angle = TMath::ATan2(deltaY , deltaX); // CHECK
  //    if (deltaY < 0.) angle += (2 * dPi); // CHECK

  return angle;
}

void PndSttHelixTrackFitter::OrderHitsByR(map<Double_t, Int_t> &hitMap)
{
  // sort the hits by r position
  PndSttHit *pMhit = nullptr;

  if (fVerbose == 2)
    cout << "n of hits: " << fTrackCand->GetNHits() << endl;

  for (size_t i = 0; i < fTrackCand->GetNHits(); i++) {
    // get index of hit
    PndTrackCandHit candhit = fTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    // get hit
    pMhit = GetHitFromCollections(iHit);
    if (!pMhit)
      continue;

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    if (tube->GetWireDirection() != TVector3(0., 0., 1.))
      continue;
    //	hitMap[sqrt(pMhit->GetX() * pMhit->GetX() + pMhit->GetY() * pMhit->GetY())] = iHit;
    hitMap[tube->GetPosition().Perp()] = iHit;
  }
}

Int_t PndSttHelixTrackFitter::GetCharge(Double_t dCenter, Double_t phiCenter, Double_t radius)
{
  map<Double_t, Int_t> hitMap;
  Int_t charge = 0;

  OrderHitsByR(hitMap);

  map<Double_t, Int_t>::iterator hitMapStart = hitMap.begin(), hitMapEnd = hitMap.end();

  hitMapEnd--;

  PndSttHit *startHit = GetHitFromCollections(hitMapStart->second), *endHit = GetHitFromCollections(hitMapEnd->second);

  // tubeID  CHECK added
  Int_t starttubeID = startHit->GetTubeID();
  Int_t endtubeID = endHit->GetTubeID();

  PndSttTube *startTube = (PndSttTube *)fTubeArray->At(starttubeID), *endTube = (PndSttTube *)fTubeArray->At(endtubeID);

  PndSttGeomPoint vertex(0., 0., 0.), firstPoint(startTube->GetPosition().X(), startTube->GetPosition().Y(), startTube->GetPosition().Z()),
    lastPoint(endTube->GetPosition().X(), endTube->GetPosition().Y(), endTube->GetPosition().Z());

  // Bool_t
  // retval = kTRUE; //[R.K. 01/2017] unused variable?

  Double_t angle, oldAngle, difAngle;

  Int_t votesForPositive = 0, votesForNegative = 0, hitNo = 0;

  map<Double_t, Int_t>::iterator hitMapIter = hitMap.begin();

  while (hitMapIter != hitMap.end()) {
    // get hit's angle
    angle = GetHitAngle(hitMapIter->second, dCenter, phiCenter, radius);

    // store the first hit's angle as a reference (hit with smallest z coordinate)
    if (hitNo == 0) {
      refAngle = angle;
    }

    // calculate the angle difference between the reference hit and this hit
    difAngle = angle - refAngle;

    while (difAngle < 0) {
      difAngle += 2 * dPi;
    }

    // cast votes for criterium 1
    if (hitNo > 0) {
      if (difAngle > oldAngle)
        votesForPositive++;
      else
        votesForNegative++;
    }

    oldAngle = difAngle;

    hitNo++;
    hitMapIter++;
  }

  // majority is enough to decide on track
  if (votesForPositive > votesForNegative) // 2*
  {

    // when swimming the track from -z to z we get a positive rotation
    if (firstPoint.DistanceTo(vertex) < lastPoint.DistanceTo(vertex)) {
      charge = -1;
    } else {
      charge = 1;
    }
  } else if (votesForNegative > votesForPositive) // 2*
  {
    // when swimming the track from -z to z we get a positive rotation
    if (firstPoint.DistanceTo(vertex) < lastPoint.DistanceTo(vertex)) {
      charge = 1;
    } else {
      charge = -1;
    }
  } else {
    charge = -1; // most likely an electron which makes a spiral
  }

  return charge;
}

// ===================================================================================
// CONSTRAINT 1

// XYFit spposing the track passes through (0, 0)
Int_t PndSttHelixTrackFitter::XYFitThroughOrigin(PndTrackCand *pTrackCand, Int_t whatToFit)
{

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();
  Bool_t first = kFALSE;
  if (hitcounter == 0)
    return 0;
  if (hitcounter < 5) { // hitcounter > 50
    if (fVerbose == 2)
      cout << "Bad No of hits in STT " << hitcounter << endl;
    return 0;
  }
  if (fVerbose == 2)
    cout << "FIT xy ********************" << endl;

  // traslation and rotation -----------
  // traslation near the first point
  Double_t trasl[2];
  // rotation
  Double_t alpha;
  PndSttHit *hitfirst = nullptr, *hitlast = nullptr;
  if (fVerbose == 2)
    cout << "hitcounter: " << hitcounter << endl;
  for (Int_t k = 0; k < hitcounter; k++) {

    PndTrackCandHit candhit = pTrackCand->GetSortedHit(k);
    Int_t iHit = candhit.GetHitId();
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit)
      continue;
    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;

    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    TVector3 wiredirection = tube->GetWireDirection();
    if (wiredirection != TVector3(0., 0., 1.))
      continue;
    else if (first == kFALSE) {
      hitfirst = (PndSttHit *)fHitArray->At(iHit);
      first = kTRUE;
      trasl[0] = hitfirst->GetX();
      trasl[1] = hitfirst->GetY();
    } else {
      hitlast = (PndSttHit *)fHitArray->At(iHit);
      alpha = TMath::ATan2(hitlast->GetY() - hitfirst->GetY(), hitlast->GetX() - hitfirst->GetX());
    }
  }

  // error <--> resolution
  Double_t sigr, sigx, sigy; // = 0.14;   sigxy,  //[R.K.01/2017]unused variable?

  // FITTING IN X-Y PLANE:
  // v = a + bu

  Double_t Suu, Su, Sv, Suv, S1;
  Su = 0.;
  Sv = 0.;
  Suu = 0.;
  Suv = 0.;
  S1 = 0.;

  Int_t digicounter = 0;
  TArrayD uarray(hitcounter);
  TArrayD varray(hitcounter);
  TArrayD sigv2array(hitcounter);
  TArrayD sigu2array(hitcounter);

  for (Int_t i = 0; i < hitcounter; i++) {
    uarray.AddAt(-999, i);
    varray.AddAt(-999, i);
    sigv2array.AddAt(-999, i);
    sigu2array.AddAt(-999, i);
  }
  for (Int_t i = 0; i < hitcounter; i++) {

    PndTrackCandHit candhit = pTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit) {
      cout << "PndSttHelixTrackFitter::XYFit: no hit at " << iHit << endl;
      continue;
    }
    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;

    // tubeID  CHECK added
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    Int_t refindex = currenthit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::XYFit: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }
    TVector3 wiredirection = tube->GetWireDirection();
    if (wiredirection != TVector3(0., 0., 1.))
      continue;

    if (whatToFit == 2) {
      // Double_t resx = iPoint->GetX() - currenthit->GetX(); //[R.K. 01/2017] unused variable?
      // Double_t resy = iPoint->GetY() - currenthit->GetY(); //[R.K. 01/2017] unused variable?
      // Double_t resdist = TMath::Sqrt((iPoint->GetY() - currenthit->GetY())*(iPoint->GetY() - currenthit->GetY()) + (iPoint->GetX() - currenthit->GetX())*(iPoint->GetX() -
      // currenthit->GetX())); //[R.K. 01/2017] unused variable?
    }

    Double_t xtrasl, ytrasl;
    // Double_t xi, yi; //[R.K. 01/2017] unused variable?
    // traslation
    xtrasl = currenthit->GetX() - trasl[0];
    ytrasl = currenthit->GetY() - trasl[1];

    if (xtrasl == 0 && ytrasl == 0)
      continue;

    Double_t xrot, yrot;
    // rotation
    xrot = TMath::Cos(alpha) * xtrasl + TMath::Sin(alpha) * ytrasl;
    yrot = -TMath::Sin(alpha) * xtrasl + TMath::Cos(alpha) * ytrasl;

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(1);
      TMarker *pt = new TMarker(xrot, yrot, 6);
      pt->SetMarkerColor(3);
      //  if(whatToFit == 2)
      // pt->Draw("SAME");
    }

    // change coordinate
    Double_t u, v, sigv2, sigu2;
    u = xrot / (xrot * xrot + yrot * yrot);
    v = yrot / (xrot * xrot + yrot * yrot);

    if (fDisplayLevel >= 4) {
      eventDetails->cd(1);
      TMarker *uv = new TMarker(u, v, 6);
      uv->SetMarkerColor(3);
      if (whatToFit == 2)
        uv->Draw("SAME");
      eventDetails->Update();
      eventDetails->Modified();
    }

    if (whatToFit == 1) {
      if (currenthit->GetIsochrone() == 0)
        sigr = sqrt(2.) / sqrt(12.);
      else
        sigr = sqrt(2.) * currenthit->GetIsochrone() / sqrt(12.);
      sigx = sigr;
      sigy = sigr;
    } else {
      if (currenthit->GetIsochrone() == 0) {
        sigr = sqrt(2.) / sqrt(12.);
        sigx = sigr;
        sigy = sigr;
      } else {
        sigr = 0.0150;
        sigx = fabs(sigr * TMath::Cos(TMath::ATan(marray.At(i))));
        sigy = fabs(sigr * TMath::Sin(TMath::ATan(marray.At(i))));
        // sigxy = sigr * TMath::Sqrt(fabs(TMath::Cos(TMath::ATan(marray.At(i))) * TMath::Sin(TMath::ATan(marray.At(i)))));; //[R.K.01/2017]unused variable?
      }
    }

    Double_t dvdx = (-2 * xrot * yrot) / pow((xrot * xrot + yrot * yrot), 2);
    Double_t dvdy = (xrot * xrot - yrot * yrot) / pow((xrot * xrot + yrot * yrot), 2);
    Double_t dudx = (yrot * yrot - xrot * xrot) / pow((xrot * xrot + yrot * yrot), 2);
    Double_t dudy = (-2 * xrot * yrot) / pow((xrot * xrot + yrot * yrot), 2);

    sigu2 = dudx * dudx * sigx * sigx + dudy * dudy * sigy * sigy + 2 * dudx * dudy * sigx * sigy;
    sigv2 = dvdx * dvdx * sigx * sigx + dvdy * dvdy * sigy * sigy + 2 * dvdx * dvdy * sigx * sigy;

    uarray.AddAt(u, digicounter);
    varray.AddAt(v, digicounter);
    sigu2array.AddAt(sigu2, digicounter);
    sigv2array.AddAt(sigv2, digicounter);

    Su = Su + (u / sigv2);
    Sv = Sv + (v / sigv2);

    Suv = Suv + ((u * v) / sigv2);
    Suu = Suu + ((u * u) / sigv2);

    S1 = S1 + 1 / sigv2;

    digicounter++;
  }

  Double_t determ = S1 * Suu - Su * Su;
  if (determ == 0) {
    return 0;
  }

  // v = a + bu
  double fita = (1 / determ) * (Suu * Sv - Su * Suv);
  double fitb = (1 / determ) * (S1 * Suv - Su * Sv);

  if (fVerbose == 2) {
    std::cout << "1) linear parameters:\n";
    std::cout << "a = " << fita << "\n";
    std::cout << "b = " << fitb << "\n";
  }

  Double_t chi2;
  chi2 = 0.;
  for (Int_t i = 0; i < digicounter; i++) {
    if (uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999)
      continue;
    chi2 = chi2 + pow(((varray.At(i) - (fita + fitb * uarray.At(i))) / sqrt(sigv2array.At(i))), 2);
  }

  if (fVerbose == 2)
    cout << "digicounter: " << digicounter << endl;

  /**
   //------------------ with right errors
   Su = 0.;
   Sv = 0.;
   Suu = 0.;
   Suv = 0.;
   S1 = 0.;

   TArrayD sigE2array(digicounter);
   Double_t sigE2;
   for(Int_t i=0; i< digicounter; i++){
   if(uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999) continue;
   //   sigE2 = sigv2array.At(i) + sigu2array.At(i) * pow((b + 2*c*uarray.At(i)),2);   // CHECK calcolalo giusto!

   sigE2array.AddAt(sigE2, i);

   Su = Su + (uarray.At(i)/sigE2);
   Sv = Sv + (varray.At(i)/sigE2);

   Suv = Suv + ((uarray.At(i)*varray.At(i))/sigE2);
   Suu = Suu + ((uarray.At(i)*uarray.At(i))/sigE2);

   S1 = S1 + 1/sigE2;
   }


   determ = S1*Suu - Su*Su;
   if(determ == 0) {
   return 0;
   }

   // v = a + bu
   double fita = (1/determ)*(Suu*Sv - Su*Suv);
   double fitb = (1/determ)*(S1*Suv - Su*Sv);

   if(fVerbose == 2) {
   std::cout << "2) linear parameters:\n";
   std::cout << "a = " << fita <<  "\n";
   std::cout << "b = " << fitb <<  "\n";
   }

   chi2 = 0.;
   for(Int_t i=0; i<digicounter; i++){
   if(uarray.At(i) == -999 || varray.At(i) == -999 || sigv2array.At(i) == -999) continue;
   chi2 = chi2 + pow(((varray.At(i) - (a + b*uarray.At(i)))/sqrt(sigE2array.At(i))), 2);
   }
  **/

  if (fDisplayLevel >= 4) {
    eventDetails->cd(1);
    TLine *uvline = new TLine(-0.1, fita - 0.1 * fitb, 0.1, fita + 0.1 * fitb);
    if (whatToFit == 2)
      uvline->Draw("SAME");
    eventDetails->Update();
    eventDetails->Modified();
  }

  // center and radius
  Double_t xc, yc, xcrot, ycrot, R;
  //  if(fabs(a)<0.000001) return 0; // CHECK proteggi dalla divisione per 0
  ycrot = 1 / (2 * fita);
  xcrot = -fitb * ycrot;
  R = sqrt(xcrot * xcrot + ycrot * ycrot);

  // re-rotation and re-traslation of xc and yc
  // rotation
  xc = TMath::Cos(alpha) * xcrot - TMath::Sin(alpha) * ycrot;
  yc = TMath::Sin(alpha) * xcrot + TMath::Cos(alpha) * ycrot;
  // traslation
  xc = xc + trasl[0];
  yc = yc + trasl[1];

  // // errors on parameters -------------------
  // MISSING
  // =============================================

  Double_t phi = TMath::ATan2(yc, xc);
  Double_t d;
  d = ((xc + yc) - R * (TMath::Cos(phi) + TMath::Sin(phi))) / (TMath::Cos(phi) + TMath::Sin(phi));

  fTrack->SetDist(d);
  fTrack->SetPhi(phi);
  //    Double_t newZ = -999.; // CHECK da cambiare
  //    fTrack->SetZ(newZ);
  fTrack->SetRad(R);
  //    Double_t newTheta = -999.; // CHECK da cambiare
  //    fTrack->SetTanL(newTheta);
  h = -GetCharge(d, phi, R);
  fTrack->SetCharge(-h);

  //   cout << "FIT: " << xc << " " << yc << endl;
  //   cout << "RAGGIO: " << R << endl;

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(1);
    TArc *fitarc = new TArc(((fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi())), ((fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi())), fTrack->GetRad());
    if (whatToFit == 2)
      fitarc->SetLineColor(kRed - 9);
    fitarc->SetFillStyle(0);
    fitarc->Draw("SAME");
    eventCanvas->Update();
    eventCanvas->Modified();
  }
  return 1;
}

void PndSttHelixTrackFitter::HoughThroughOrigin(TVector3 *choice, Double_t Phi0, Double_t x0, Double_t y0, Double_t R)
{

  // since the points stay on a line in z track length plane
  // I can use the hough transform to transform the points in
  // z track length plane to lines in the parameters plane ab
  // (scos = a * z) and find their intersections.
  //
  // grid:
  // a [-pi/2 , pi/2]; 200 steps
  // b = 0

  Double_t y = choice->Z();
  Double_t x =
    h * R *
    TMath::ATan2((choice->Y() - y0) * TMath::Cos(Phi0) - (choice->X() - x0) * TMath::Sin(Phi0), R + (choice->X() - x0) * TMath::Cos(Phi0) + (choice->Y() - y0) * TMath::Sin(Phi0));

  // Hough  -------------------------- angle [-90, 90]
  Double_t a = -TMath::Pi() / 2.;
  Double_t tga;

  // 200 step per -pi/2 < a < pi/2
  for (Int_t i = 0; i < 200; i++) {
    tga = TMath::Tan(a);
    Double_t ycalc = x * tga;

    //  if(fabs(ycalc - y) < (0.3 + TMath::Tan(TMath::Pi()/200.) * x)) {
    if (fabs(ycalc - y) < 0.2) {
      votecon[i] += 1;
    }
    a += (TMath::Pi() / 200.);
  }
}

TVector3 PndSttHelixTrackFitter::GetHoughResponseThroughOrigin()
{
  Double_t a = -TMath::Pi() / 2.;
  Double_t tga;

  // TMarker *mrk2; //[R.K. 01/2017] unused variable?

  Double_t aref, /*bref,*/ vref = 0; //[R.K. 01/2017] unused variable?
  // Double_t aref2, bref2, vref2 = 0, cref2 = 0; //[R.K. 01/2017] unused variable?
  for (Int_t i = 0; i <= 200; i++) {
    tga = TMath::Tan(a);

    if (votecon[i] != 0) {
      if (fDisplayLevel >= 3) {
        hougcon->Fill(tga);
      }

      if (votecon[i] >= vref) {
        aref = tga;
        vref = votecon[i];
      }
    }
    a += (TMath::Pi() / 200.);
  }

  if (fDisplayLevel >= 4) {
    eventDetails->cd(2);
    hougcon->Draw();
  }
  TVector3 hough(aref, 0, vref);

  // reset votes
  for (Int_t i = 0; i <= 200; i++)
    votecon[i] = 0;
  return hough;
}

// ZFinder was ZFinderbb3
// -------- ZFinder --------------------------------------------
Bool_t PndSttHelixTrackFitter::ZFinderThroughOrigin(PndTrackCand *pTrackCand, Int_t)
{ // whatToFit //[R.K.03/2017] unused variable(s)
  // the z finding procedure uses the hough transform to find the line
  // in the plane z - track length on which the correct points lie.

  if (fVerbose == 2)
    cout << "ZFINDER" << endl;

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();

  // cut on number of hits
  //   if(hitcounter > 50) {
  //     cout << "more than 50 hits " << hitcounter << endl;
  //     return 0;
  //   }
  if (hitcounter < 5) {
    if (fVerbose == 2)
      cout << "less than 5 hits" << endl;
    return 0;
  }

  ZPointsArray = new TObjArray(2 * hitcounter);

  // ZFIT =======
  if (hitcounter == 0)
    return kFALSE;

  // Double_t Sxx, Sxz; //[R.K. 01/2017] unused variable?
  // Double_t fitm, fitp; //[R.K. 01/2017] unused variable?
  Double_t sigz = 1.; // CHECK

  // Sxx = 0.; //[R.K. 01/2017] unused variable?
  // Sxz = 0.; //[R.K. 01/2017] unused variable?
  // ============

  TVector3 *tofit, *tofit2;

  // from xy plane fit
  Double_t phi0 = fTrack->GetPhi();
  Double_t d0 = fTrack->GetDist();
  Double_t x0 = d0 * TMath::Cos(phi0);
  Double_t y0 = d0 * TMath::Sin(phi0);

  // centre of curvature
  Double_t x_0 = (fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi());
  Double_t y_0 = (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi());

  // radius of curvature
  Double_t R = fTrack->GetRad();
  Int_t wireOk = 0;
  // Bool_t first = kTRUE; //[R.K. 01/2017] unused variable?

  for (Int_t i = 0; i < hitcounter; i++) {

    // get index of hit
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFinder: no hit at " << iHit << endl;
      continue;
    }
    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFinder: no point at " << refindex << " associated  to hit " << iHit << endl;
      continue;
    }

    TVector3 wiredirection = tube->GetWireDirection();
    TVector3 wiredirection2;

    wiredirection2 = tube->GetHalfLength() * wiredirection;
    TVector3 cenposition = tube->GetPosition();
    //    if(cenposition.Z() != 35) continue; // to throw away short tubes

    TVector3 min, max;
    min = cenposition - wiredirection2;
    max = cenposition + wiredirection2;

    // first extremity
    Double_t x_1 = min.X();
    Double_t y_1 = min.Y();
    Double_t z_1 = min.Z();

    // second extremity
    Double_t x_2 = max.X();
    Double_t y_2 = max.Y();
    Double_t z_2 = max.Z();

    Double_t rcur = pMhit->GetIsochrone();

    Double_t x1 = -9999.;
    Double_t y1 = -9999.;
    Double_t x2 = -9999.;
    Double_t y2 = -9999.;

    // in xy plane: angle of the PCA to the origin
    // with respect to the curvature center
    Double_t Phi0 = TMath::ATan2((y0 - y_0), (x0 - x_0));

    if (wiredirection != TVector3(0., 0., 1.)) {
      Double_t a = -999;
      Double_t b = -999;

      // intersection point between the reconstructed
      // circumference and the line joining the centres
      // of the reconstructed circle and the i_th drift circle
      if (fabs(x_2 - x_1) > 0.0001) {
        a = (y_2 - y_1) / (x_2 - x_1);
        b = (y_1 - a * x_1);
        Double_t A = a * a + 1;
        Double_t B = x_0 + a * y_0 - a * b;
        Double_t C = x_0 * x_0 + y_0 * y_0 + b * b - R * R - 2 * y_0 * b;
        if ((B * B - A * C) > 0) {
          x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        }
      } else if (fabs(y_2 - y_1) > 0.0001) {
        Double_t A = 1;
        Double_t B = y_0;
        Double_t C = y_0 * y_0 + (x_1 - x_0) * (x_1 - x_0) - R * R;

        if ((B * B - A * C) > 0) {
          y1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          y2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          x1 = x2 = x_1;
        }
      } else {
        if (fVerbose == 2)
          LOG(error) << " intersection point not found";
        continue;
      }

      // x1 and x2 are the 2 intersection points
      Double_t d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
      Double_t d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

      Double_t x_ = x1;
      Double_t y_ = y1;

      // the intersection point nearest to the drift circle's centre is taken
      if (d2 < d1) {
        x_ = x2;
        y_ = y2;
      }

      if (fDisplayLevel >= 3) {
        eventCanvas->cd(1);
        TMarker *pt = new TMarker(x_, y_, 6);
        pt->SetMarkerColor(6);
        pt->Draw("SAME");

        // intersection lines
        TLine *intline = new TLine(x_, y_, x_0, y_0);
        intline->SetLineColor(5);
        intline->Draw("SAME");
      }

      // now we need to find the actual centre of the drift circle,
      // by translating the drift circle until it becomes tangent
      // to the reconstructed circle.
      // Two solutions are possible (left rigth abiguity),
      // they are both kept, only the following zed fit will discard the wrong ones.
      // Using the parametric equation of the 3d-straigth line and taking the
      // x points just obtained, the zed coordinate of the skewed tube centre is calculated.

      if (x_1 == x_2) {
        // if the skewed layer lies in yz plane
        x1 = x_;
        y1 = y_ + rcur;
        x2 = x_;
        y2 = y_ - rcur;
      } else {
        // solving the equation to find out the centre of the tangent circle
        Double_t A = a * a + 1;
        Double_t B = -(a * b - a * y_ - x_);
        Double_t C = x_ * x_ + y_ * y_ + b * b - 2 * b * y_ - rcur * rcur;
        if ((B * B - A * C) > 0) {
          x1 = (B + TMath::Sqrt(B * B - A * C)) / A;
          x2 = (B - TMath::Sqrt(B * B - A * C)) / A;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        } else if ((B * B - A * C) == 0) {
          x1 = B / A;
          x2 = x1;
          y1 = a * x1 + b;
          y2 = a * x2 + b;
        } else {
          if (fVerbose == 2)
            cout << "NO WAY2" << endl;
          continue;
        }
      }
      if (fDisplayLevel >= 3) {
        eventCanvas->cd(1);
        TArc *drftarc = new TArc(x1, y1, rcur);
        drftarc->SetFillStyle(0);
        drftarc->SetLineColor(3);
        drftarc->Draw("SAME");
        TArc *drftarc2 = new TArc(x2, y2, rcur);
        drftarc2->SetFillStyle(0);
        drftarc2->SetLineColor(3);
        drftarc2->Draw("SAME");
      }

      d1 = TMath::Sqrt((x1 - cenposition.X()) * (x1 - cenposition.X()) + (y1 - cenposition.Y()) * (y1 - cenposition.Y()));
      d2 = TMath::Sqrt((x2 - cenposition.X()) * (x2 - cenposition.X()) + (y2 - cenposition.Y()) * (y2 - cenposition.Y()));

      Double_t xcen0 = x1;
      Double_t xcen1 = x2;
      Double_t ycen0 = y1;
      Double_t ycen1 = y2;

      if (d2 < d1) { // z2 contains the points nearest (in x-y) to the initial centre of the skewed tube
        xcen0 = x2;
        xcen1 = x1;
        ycen0 = y2;
        ycen1 = y1;
      }

      // zed association
      Double_t t_, z_, t_bis, z_bis;
      if (fabs(x_2 - x_1) < 0.001) {
        t_ = (ycen0 - y_1) / (y_2 - y_1);    // k= a_y*t + y_1 [t=1 y=y_2]
        z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
        t_bis = (ycen1 - y_1) / (y_2 - y_1); // from y_'s (the 2 solutions of the 2nd order equation)
        z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
      } else {
        t_ = (xcen0 - x_1) / (x_2 - x_1);    // x= a_x*t + x_1 [t=1 x=x_2]
        z_ = (z_2 - z_1) * t_ + z_1;         // z= a_z*t + z_1 [t=1 z=z_2]
        t_bis = (xcen1 - x_1) / (x_2 - x_1); // from x_'s (the 2 solutions of the 2nd order equation)
        z_bis = (z_2 - z_1) * t_bis + z_1;   // and the 2 parametric equations the z coord. are obtained
      }

      //	tofit = new TVector3(xcen0,ycen0,z_);
      tofit = new TVector3(x_, y_, z_);
      ZPointsArray->Add(tofit);
      //	tofit2 = new TVector3(xcen1,ycen1,z_bis);
      tofit2 = new TVector3(x_, y_, z_bis);
      ZPointsArray->Add(tofit2);

      if (fDisplayLevel >= 4) {
        eventDetails->cd(3);
        // xz plane
        // found point 1
        TMarker *mrkt2 = new TMarker(x_, z_, 6);
        mrkt2->SetMarkerColor(wireOk);
        mrkt2->Draw("SAME");
        // found point2
        TMarker *mrkt2bis = new TMarker(x_, z_bis, 6);
        mrkt2bis->SetMarkerColor(wireOk);
        mrkt2bis->Draw("SAME");

        eventDetails->cd(4);
        // yz plane
        // found point 1
        TMarker *mrkt2b = new TMarker(y_, z_, 6);
        mrkt2b->SetMarkerColor(wireOk);
        mrkt2b->Draw("SAME");
        // found point 2
        TMarker *mrkt2bbis = new TMarker(y_, z_bis, 6);
        mrkt2bbis->SetMarkerColor(wireOk);
        mrkt2bbis->Draw("SAME");
      }

      if (fDisplayLevel >= 2) {
        eventCanvas->cd(2);
        // MC point in z - track length plane
        TMarker *MCmrk = new TMarker(h * R *
                                       TMath::ATan2((iPoint->GetY() - y0) * TMath::Cos(Phi0) - (iPoint->GetX() - x0) * TMath::Sin(Phi0),
                                                    R + (iPoint->GetX() - x0) * TMath::Cos(Phi0) + (iPoint->GetY() - y0) * TMath::Sin(Phi0)),
                                     iPoint->GetZ(), 6);
        MCmrk->SetMarkerColor(4);
        MCmrk->Draw("SAME");
      }

      // ------- HOUGH TRANSFORM ------------
      // 	// FIRST CHOICE
      // 	if(tofit) HoughThroughOrigin(tofit, Phi0, x0, y0, R); // CHECK

      // 	// SECOND CHOICE
      // 	if(tofit2) HoughThroughOrigin(tofit2, Phi0, x0, y0, R); // CHECK

      wireOk++;
    }
  }

  //  cout << "skewed: " << wireOk << endl;

  //   TVector3 outz = GetHoughResponseThroughOrigin(); // CHECK
  TVector3 outz = FindCorrectZ(ZPointsArray, x_0, y_0, x0, y0, R); // CHEC
  if (fDisplayLevel >= 3) {
    eventCanvas->cd(2);

    // found line in z track length plane
    TLine *line = new TLine(-40, -40 * outz.X() + outz.Y(), 40, (40 * outz.X() + outz.Y()));
    line->Draw("SAME");
  }

  //  pTrack->GetParamFirst()->SetX(outz.Z()); // CHECK what is this?!!!!!

  // if votes < 6 consider the fit failed CHECK
  //  if(outz.Z() < 6) return kFALSE;

  Int_t counter = 0;

  if (outz.X() == 0. && outz.Y() == 0.)
    return kFALSE;

  if (wireOk < 2) {
    return kFALSE;
  }

  // first = kTRUE; //[R.K. 01/2017] unused variable?
  wireOk = 0;
  Int_t okcounter = 0;
  for (Int_t i = 0; i < (2 * hitcounter); i += 2) {
    // get index of hit
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(counter);
    Int_t iHit = candhit.GetHitId();
    counter++;

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFinder: no hit at " << iHit << endl;
      continue;
    }
    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFinder: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    TVector3 wiredirection = tube->GetWireDirection();

    if (wiredirection == TVector3(0., 0., 1.))
      continue;
    wireOk++;

    sigz = 1.; // CHECK

    // FIRST CHOICE .................
    TVector3 *vi = (TVector3 *)ZPointsArray->At(okcounter);

    if (vi == nullptr)
      continue;
    Double_t scos = fTrack->CalculateScosl(vi->X(), vi->Y());

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(2);
      // first point z track lenght plane
      TMarker *mrk = new TMarker(scos, vi->Z(), 6);
      mrk->Draw("SAME");
    }

    Bool_t fitdone = kFALSE;

    // distance between the found z and the predicted from the found line one
    Double_t distfirst = pow(((vi->Z() - (outz.Y() + outz.X() * scos)) / sigz), 2);
    if (distfirst < 10) {
      pMhit->SetX(vi->X());
      pMhit->SetY(vi->Y());
      pMhit->SetZ(vi->Z());
      fitdone = kTRUE;
    }

    // SECOND CHOICE ...................
    vi = (TVector3 *)ZPointsArray->At(okcounter + 1);
    if (vi == nullptr)
      continue;
    scos = fTrack->CalculateScosl(vi->X(), vi->Y());

    Double_t distsecond = pow(((vi->Z() - (outz.Y() + outz.X() * scos)) / sigz), 2);

    if (fDisplayLevel >= 3) {
      eventCanvas->cd(2);
      TMarker *mrk2 = new TMarker(scos, vi->Z(), 6);
      mrk2->Draw("SAME");
    }

    // Xint Yin Zint set
    if (fitdone == kTRUE) {
      if (distsecond < 10 && distsecond < distfirst) {
        pMhit->SetX(vi->X());
        pMhit->SetY(vi->Y());
        pMhit->SetZ(vi->Z());
      }
    } else {
      if (distsecond < 10) {
        pMhit->SetX(vi->X());
        pMhit->SetY(vi->Y());
        pMhit->SetZ(vi->Z());
        fitdone = kTRUE;
      } else {
        pMhit->SetX(-999);
        pMhit->SetY(-999);
        pMhit->SetZ(-999);
      }
    }

    if (fDisplayLevel >= 3) {
      if (pMhit->GetZ() != -999) {
        eventCanvas->cd(2);
        TMarker *mrk3 = new TMarker(scos, pMhit->GetZ(), 6);
        mrk3->SetMarkerColor(3);
        mrk3->Draw("SAME");
      }
    }

    okcounter = okcounter + 2;
  }

  return kTRUE;
  //   fiterrp = sqrt(fiterrp2);
  //   fiterrm = sqrt(fiterrm2);
}

// ZFit was Zfitbb2
// ----- Zfit  ----------------------------------------
Int_t PndSttHelixTrackFitter::ZFitThroughOrigin(PndTrackCand *pTrackCand, Int_t)
{ // whatToFit //[R.K.03/2017] unused variable(s)

  if (fVerbose == 2)
    cout << "ZFIT" << endl;

  if (!pTrackCand)
    return 0;

  Int_t hitcounter = pTrackCand->GetNHits();

  // cut on number of hits
  //   if(hitcounter > 50) return 0;
  if (hitcounter < 5)
    return 0;

  // CHECK: ATTENTION
  // refit for error correction missing

  // SCOSL ======
  // get 1st hit
  // PndSttHit *fMhit = (PndSttHit*) fHitArray->At(pTrackCand->GetSortedHit(0).GetHitId()); //[R.K. 01/2017] unused variable?

  Double_t Sxx, Sxz;
  Double_t fitm, fitp;
  Double_t sigz = 1.; // CHECK

  Sxx = 0.;
  Sxz = 0.;

  // centre of curvature
  // Double_t x_0 = (fTrack->GetDist() + fTrack->GetRad()) * cos(fTrack->GetPhi()); //[R.K. 01/2017] unused variable?
  // Double_t y_0 = (fTrack->GetDist() + fTrack->GetRad()) * sin(fTrack->GetPhi()); //[R.K. 01/2017] unused variable?
  // radius of curvature
  // Double_t R  = fTrack->GetRad(); //[R.K. 01/2017] unused variable?
  Int_t counter = 0;
  Int_t wireOk = 0;
  // Bool_t first = kTRUE; //[R.K. 01/2017] unused variable?
  for (Int_t i = 0; i < hitcounter; i++) {

    // get index of hit
    PndTrackCandHit candhit = fTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();

    // get hit
    PndSttHit *pMhit = (PndSttHit *)fHitArray->At(iHit);
    if (!pMhit) {
      cout << "PndSttHelixTrackFitter::ZFit: no hit at " << iHit << endl;
      continue;
    }
    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);

    if (pMhit->GetX() == -999 || pMhit->GetY() == -999 || pMhit->GetZ() == -999)
      continue; // CHECK

    Int_t refindex = pMhit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint) {
      cout << "PndSttHelixTrackFitter::ZFit: no point at " << refindex << " associated to hit " << iHit << endl;
      continue;
    }

    TVector3 wiredirection = tube->GetWireDirection();

    if (wiredirection == TVector3(0., 0., 1.))
      continue;

    //    if(tube->GetPosition().Z() != 35) continue; // for the moment I throw away short tubes

    TVector3 *vi = new TVector3(pMhit->GetX(), pMhit->GetY(), pMhit->GetZ());

    // if the found z is > 75 cm or < -75 cm continue: this has to be fixed
    if (pMhit->GetZ() < (tube->GetPosition().Z() - tube->GetHalfLength()) || pMhit->GetZ() > (tube->GetPosition().Z() + tube->GetHalfLength()))
      continue; // CHECK
    Double_t scos = fTrack->CalculateScosl(pMhit->GetX(), pMhit->GetY());

    Sxz = Sxz + ((scos * vi->Z()) / (sigz * sigz));
    Sxx = Sxx + ((scos * scos) / (sigz * sigz));

    counter++;
    wireOk++;
  }

  if (counter == 0)
    return 0;

  fitp = 0.;
  fitm = Sxz / Sxx;

  // y = m*x + p
  TVector2 outz(fitm, fitp);

  // fill in parameters
  fTrack->SetTanL(fitm); // tan(lambda)
  fTrack->SetZ(fitp);    // z0

  if (fDisplayLevel >= 3) {
    eventCanvas->cd(2);

    // fit line
    TLine *line2 = new TLine(-40, -40 * fitm + fitp, 40, (40 * fitm + fitp));
    line2->SetLineColor(kGreen - 9);
    line2->Draw("SAME");
  }
  return 1;
  //   fiterrp = sqrt(fiterrp2);
  //   fiterrm = sqrt(fiterrm2);
}
// -------------------------------------------------------------------------------
//    ***************** CHECK *******************
// TO USE THIS A BIG CHANGE IS NEEDED TO RETRIEVE THE START
// POINT FROM PndTrack
/**

Int_t PndSttHelixTrackFitter::DoFitThroughOrigin(PndTrackCand* pTrackCand, PndSttTrack* pTrack, Int_t pidHypo)
{
// TO BE USED ONLY when PndSttTrackFinderReal is applied:
// the PR finds only PRIMARY TRACKS ==>
// the track can be forced to come from the IP (0, 0, 0)
//
// the starting point is the PR found track seed


if(!pTrackCand) return 0;
fTrack = pTrack; // CHECK canc
fTrackCand = pTrackCand;
if(fDisplayLevel > 0) {
RunEventDisplay(pTrackCand);
char goOnChar;
cout << "press any key to continue: " << endl;
cin >> goOnChar;
}

Int_t fit = 0;

//  fit = XYFitThroughOrigin(pTrackCand, 1);
//    ***************** CHECK *******************
// TO USE THIS A BIG CHANGE IS NEEDED TO RETRIEVE THE START
// POINT FROM PndTrack

// take start point from PR ===================================================
TVector3 foundMom = pTrackCand->getDirSeed();
Double_t momMag = fabs(1./pTrackCand->getQoverPseed());
foundMom.SetMag(momMag);
TVector3 foundVtx = pTrackCand->getPosSeed();
int foundCharge = (int) (pTrackCand->getQoverPseed()/fabs(pTrackCand->getQoverPseed()));
Double_t foundRad = foundMom.Perp() / 0.006;
// track from tangent ---------------------
double found_m1 = foundMom.Y() / foundMom.X();
double found_q1 = foundVtx.Y() - foundVtx.X() * found_m1;
double found_m2 = -1./found_m1;
double found_q2 = foundVtx.Y() - foundVtx.X() * found_m2;

double alpha = TMath::ATan2(foundMom.X(), foundMom.Y());
double foundX0, foundY0;
if(foundCharge > 0) {
foundX0 = foundVtx.X() + foundRad * TMath::Cos(alpha);
foundY0 = foundVtx.Y() - foundRad * TMath::Sin(alpha);
}
else {
foundX0 = foundVtx.X() - foundRad * TMath::Cos(alpha);
foundY0 = foundVtx.Y() + foundRad * TMath::Sin(alpha);
}

Double_t foundDist, foundPhi;

foundDist = TMath::Sqrt(foundX0 * foundX0 + foundY0 * foundY0) - foundRad;
foundPhi = atan2(foundY0, foundX0);

Double_t foundTanL, foundZ = 0; // CHECK
foundTanL = foundMom.Z()/foundMom.Perp();

fTrack->SetDist(foundDist);
fTrack->SetPhi(foundPhi);
fTrack->SetRad(foundRad);
fTrack->SetTanL(foundTanL);
fTrack->SetZ(foundZ);
fTrack->SetCharge(foundCharge);

if(fDisplayLevel >= 4) {
TArc *foundcir = new TArc(foundX0, foundY0, foundRad);
eventCanvas->cd(1);
foundcir->SetLineColor(8);
foundcir->SetFillStyle(0);
foundcir->Draw("SAME");
}

// ==========================================================


// if the track finding is ok
if(fTrack->GetRad() == 0 || !(fTrack->GetRad()) || fTrack->GetRad() > 3000) {
//  fTrack->SetRad(-999);
//    if(fVerbose == 2)
LOG(error) << " track finding FAILED" ;
return 0;
}
else {
pTrack->SetFlag(1); // prefit done
Bool_t Rint = IntersectionFinder(pTrackCand);
//    cout << "refit" << endl;
// if refit is OK
if(Rint == kTRUE) {
fit = XYFitThroughOrigin(pTrackCand, 2);
Rint = IntersectionFinder(pTrackCand);
if(Rint == kTRUE)  fit = XYFitThroughOrigin(pTrackCand, 2); // MinuitFit(pTrackCand, 2);
Rint = IntersectionFinder(pTrackCand);
if(Rint == kTRUE)  fit = XYFitThroughOrigin(pTrackCand, 2); // MinuitFit(pTrackCand, 2);

}
else {
return 0;
}
if(fit == 1 && fTrack->GetRad()>0&& fTrack->GetRad() < 3000) {

pTrack->SetFlag(2); // refit done
Bool_t zint = ZFinderThroughOrigin(pTrackCand, 1);

if(zint == kTRUE) {
Int_t zfit = ZFitThroughOrigin(pTrackCand, 1);
if(zfit == 1) {
pTrack->SetFlag(3); // z fit done
}
}
else if(fVerbose == 2) cout << "-E- zfinder FAILED" << endl;
}
}

if(fVerbose == 2) {
cout << "param last d    : "  << fTrack->GetDist() << endl;
cout << "param last phi  : "  << fTrack->GetPhi() << endl;
cout << "param last R    : " << fTrack->GetRad() << endl;
cout << "param last tanL : " << fTrack->GetTanL() << endl;
cout << "param last q    : " << fTrack->GetCharge() << endl;
double pt, pl;
pt = fTrack->GetRad() * 0.006;
pl = fTrack->GetRad() * fTrack->GetTanL() * 0.006;
cout << "pT              : " << pt << endl;
cout << "pL              : " << pl << endl;
cout << "px, py, pz      : " << pt * cos(-h * TMath::Pi()/2. + fTrack->GetPhi()) << " " << pt * sin(-h * TMath::Pi()/2. + fTrack->GetPhi()) << " " << pl << endl;
}


if(fDisplayLevel > 0) FinishEventDisplay(fTrack);

return 0;
}
**/
void PndSttHelixTrackFitter::InitEventDisplay()
{
  // 0 no display
  // 1 only final result
  // 2 final + mc points
  // 3 also details

  h1 = new TH2F("h1", "xy plane", 100, -50, 50, 100, -50, 50);
  h2 = new TH2F("h2", "z cos#{lambda} plane", 100, -75, 75, 100, -45, 115);
  h3 = new TH2F("h3", "conformal plane", 100, -1.5, 1.5, 100, -1.5, 1.5);
  h4 = new TH2F("h4", "tubes", 100, -45, 45, 100, -45, 115);

  houg = new TH2F("houg", "houg", 100, -1.001, 1.001, 100, -150.001, 151.001);
  hougcon = new TH1F("hougcon", "hougcon", 200, -1.001, 1.001);

  switch (fDisplayLevel) {
  case 0: break;
  case 1:
  case 2:
  case 3:
    eventCanvas = new TCanvas("eventCanvas", "eventcanvas", 900, 500);
    eventCanvas->Divide(2, 1);
    break;
  case 4:
    eventCanvas = new TCanvas("eventCanvas", "eventcanvas", 0, 0, 400, 600);
    eventCanvas->Divide(1, 2);
    eventDetails = new TCanvas("eventDetails", "eventdetails", 400, 0, 600, 600);
    eventDetails->Divide(2, 2);
    break;
  }
}

Bool_t PndSttHelixTrackFitter::RunEventDisplay(PndTrackCand *pTrackCand)
{
  // 0 no display
  // 1 only final result
  // 2 final + mc points
  // 3 also details

  eventCanvas->cd(1);
  h1->Draw();
  eventCanvas->cd(2);
  h2->Draw();
  if (fDisplayLevel >= 4) {
    eventDetails->cd(1);
    h3->Draw();
    eventDetails->cd(3);
    h4->Draw();
    eventDetails->cd(4);
    h4->Draw();
    eventDetails->cd(2);
    houg->Draw();
  }
  //.....................
  if (!pTrackCand)
    return kFALSE;
  Int_t hitcounter = pTrackCand->GetNHits();

  for (Int_t i = 0; i < hitcounter; i++) {
    PndTrackCandHit candhit = pTrackCand->GetSortedHit(i);
    Int_t iHit = candhit.GetHitId();
    PndSttHit *currenthit = (PndSttHit *)fHitArray->At(iHit);
    if (!currenthit)
      continue;
    if (currenthit->GetX() == -999 || currenthit->GetY() == -999)
      continue;
    Int_t refindex = currenthit->GetRefIndex();
    // get point
    PndSttPoint *iPoint = (PndSttPoint *)fPointArray->At(refindex);
    if (!iPoint)
      continue;
    Int_t tubeID = currenthit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    TVector3 wiredirection = tube->GetWireDirection();

    // draw MC points (BLUE)
    if (fDisplayLevel >= 2) {
      eventCanvas->cd(1);
      TMarker *cir0 = new TMarker(iPoint->GetX(), iPoint->GetY(), 6);
      cir0->SetMarkerColor(4);
      cir0->Draw("SAME");

      if (fDisplayLevel >= 4) {
        eventDetails->cd(3);
        TMarker *mcmrkt2bis = new TMarker(iPoint->GetX(), iPoint->GetZ(), 6);
        mcmrkt2bis->SetMarkerColor(4);
        mcmrkt2bis->Draw("SAME");

        eventDetails->cd(4);
        TMarker *mcmrkt2bbis = new TMarker(iPoint->GetY(), iPoint->GetZ(), 6);
        mcmrkt2bbis->SetMarkerColor(4);
        mcmrkt2bbis->Draw("SAME");
      }
    }

    // draw hit (GREEN = NON SKEWED / YELLOW = SKEWED)
    eventCanvas->cd(1);
    TArc *cir1 = new TArc(tube->GetPosition().X(), tube->GetPosition().Y(), currenthit->GetIsochrone());
    cir1->SetLineColor(3);
    if (wiredirection != TVector3(0., 0., 1.))
      cir1->SetLineColor(5);
    cir1->SetFillStyle(0);
    cir1->Draw("SAME");

    if (fDisplayLevel >= 4) {

      // draw tube (BLACK LINE / PURPLE EXTREMITIES)
      TVector3 wiredirection2;
      wiredirection2 = tube->GetHalfLength() * wiredirection;
      TVector3 cenposition = tube->GetPosition();

      TVector3 min, max;
      min = cenposition - wiredirection2;
      max = cenposition + wiredirection2;

      // first extremity
      Double_t x_1 = min.X();
      Double_t y_1 = min.Y();
      Double_t z_1 = min.Z();

      // second extremity
      Double_t x_2 = max.X();
      Double_t y_2 = max.Y();
      Double_t z_2 = max.Z();

      if (wiredirection != TVector3(0., 0., 1.)) {
        eventDetails->cd(3);

        // first wire extremity
        TMarker *pt1z = new TMarker(x_1, z_1, 6);
        pt1z->SetMarkerColor(6);
        pt1z->Draw("SAME");
        // last wire extremity
        TMarker *pt2z = new TMarker(x_2, z_2, 6);
        pt2z->SetMarkerColor(6);
        pt2z->Draw("SAME");
        // tube line
        TLine *ztubeline = new TLine(x_1, z_1, x_2, z_2); // wire position
        ztubeline->Draw("SAME");

        eventDetails->cd(4);

        // first wire extremity
        TMarker *pt1z2 = new TMarker(y_1, z_1, 6);
        pt1z2->SetMarkerColor(6);
        pt1z2->Draw("SAME");
        // last wire extremity
        TMarker *pt2z2 = new TMarker(y_2, z_2, 6);
        pt2z2->SetMarkerColor(6);
        pt2z2->Draw("SAME");
        // tube line
        TLine *ztubeline2 = new TLine(y_1, z_1, y_2, z_2); // wire position
        ztubeline2->Draw("SAME");

        // xy
        eventCanvas->cd(1);
        TMarker *pt1 = new TMarker(x_1, y_1, 6);
        pt1->SetMarkerColor(6);
        pt1->Draw("SAME");
        TMarker *pt2 = new TMarker(x_2, y_2, 6);
        pt2->SetMarkerColor(6);
        pt2->Draw("SAME");
        TLine *tubeline = new TLine(x_1, y_1, x_2, y_2);
        tubeline->Draw("SAME");
      }
    }
  }

  return kTRUE;
}

void PndSttHelixTrackFitter::FinishEventDisplay(PndSttTrack *track)
{
  if (!track)
    return;

  //   cout << "param last x: "  << track->GetDist() << endl;
  //   cout << "param last y: "  << track->GetPhi() << endl;
  //   cout << "param last tx: " << track->GetRad() << endl;
  //   cout << "param last ty: " << track->GetTanL() << endl;
  //   cout << "param last qp: " << track->GetCharge() << endl;

  eventCanvas->cd(1);
  TArc *fitarc = new TArc(((track->GetDist() + track->GetRad()) * cos(track->GetPhi())), ((track->GetDist() + track->GetRad()) * sin(track->GetPhi())), track->GetRad());
  fitarc->SetLineColor(2);
  fitarc->SetFillStyle(0);
  fitarc->Draw("SAME");

  eventCanvas->cd(2);

  TLine *line = new TLine(-40, -40 * track->GetTanL() + track->GetZ(), 40, 40 * track->GetTanL() + track->GetZ());
  line->SetLineColor(2);
  if (track->GetFlag() >= 3)
    line->Draw("SAME");

  eventCanvas->Update();
  eventCanvas->Modified();
  if (fDisplayLevel >= 4) {
    eventDetails->Update();
    eventDetails->Modified();
  }
}

TVector3 PndSttHelixTrackFitter::FindCorrectZ(TObjArray *choices, Double_t x_0, Double_t y_0, Double_t x0, Double_t y0, Double_t R)
{

  int hitcounter = choices->GetEntriesFast();
  TH1F hlocal("hlocal", "", 200, -0.001, 6.281);
  Double_t Phi0 = TMath::ATan2((y0 - y_0), (x0 - x_0));

  TMatrixT<double> found_atan(hitcounter, 1);
  for (int i = 0; i < hitcounter; i++) {

    TVector3 *choice = (TVector3 *)choices->At(i);

    Double_t y = choice->Z();

    Double_t x = h * R *
                 TMath::ATan2((choice->Y() - y0) * TMath::Cos(Phi0) - (choice->X() - x0) * TMath::Sin(Phi0),
                              R + (choice->X() - x0) * TMath::Cos(Phi0) + (choice->Y() - y0) * TMath::Sin(Phi0));

    //     TVector2 pos(choice->X() - x_0, choice->Y() - y_0);
    //     TVector2 piv(x0 - x_0, y0 - y_0);
    //     double alpha = TMath::ACos((pos.X() * piv.X() + pos.Y() * piv.Y()) / (pos.Mod() * piv.Mod()));
    //     Double_t x = R * alpha;

    // the straingt line starting from (0, 0) is y = m * x;
    double atang = TMath::ATan2(y, x);
    if (y < 0)
      atang += (2 * TMath::Pi());
    found_atan[i][0] = atang;

    if (fDisplayLevel >= 4)
      hougcon->Fill(found_atan[i][0]);
    hlocal.Fill(found_atan[i][0]);
    //    cout << "m " << i << " " << found_atan[i][0] * TMath::RadToDeg() << endl;
  }

  if (fDisplayLevel >= 4) {
    eventDetails->cd(2);
    hougcon->Draw();
  }

  double foundatan = hlocal.GetBinCenter(hlocal.GetMaximumBin());
  //  cout << "found atan " << foundatan << " " <<  hlocal.GetMaximumBin() << endl;

  TVector3 foundz(TMath::Tan(foundatan), 0, hlocal.GetMaximumBin());

  return foundz;
}

ClassImp(PndSttTrackFitter)
