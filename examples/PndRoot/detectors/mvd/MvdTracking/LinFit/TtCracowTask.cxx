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

// This Class' Header ------------------
#include "TtCracowTask.h"
#include "TtFitRes.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TGraph2DErrors.h"
#include "FairLogger.h"

#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndTrackCandHit.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"

// Fit Classes -----------
#include <TMath.h>
#include <TVector3.h>
#include <TRandom.h>
//#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TVirtualFitter.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

#include <fstream>

// using namespace ROOT::Math;
using namespace std;

TtCracowTask::TtCracowTask() : FairTask("Cracow"), fTCandArray(nullptr), fTCandBranchName("MVDHitsStrip"), fTrackArray(nullptr), fTrackcount(0), fEvent(0), fEloss()
{
  for (int i = 0; i < 6; i++)
    fEloss[i] = 0.;
}

TtCracowTask::~TtCracowTask() {}

InitStatus TtCracowTask::Init()
{

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("TtCracowTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection

  fTCandArray = (TClonesArray *)ioman->GetObject(fTCandBranchName);

  if (fTCandArray == 0) {
    Error("TtCracowTask::Init", "trackcand-array not found!");
    return kERROR;
  }

  std::cout << "Input: " << fTCandArray->GetClass()->GetName() << std::endl;

  fTrackArray = new TClonesArray("TtFitRes");
  ioman->Register("TTFit", "PndMvd", fTrackArray, kTRUE);

  LOG(info) << " TtCracowTask: Initialisation successfull";

  fEvent = 0;

  return kSUCCESS;
}

// line parametrization
// x = par[1]*t +par[0]
// y = par[3]*t +par[2]
// z = t

Double_t TtCracowTask::distance2(double x, double y, double z, double *par)
{
  // distance line point is D= | (xp-x0) cross  ux |
  // where ux is direction of line and x0 is a point in the line (like t = 0)
  TVector3 xp(x, y, z);
  TVector3 x0(par[0], par[2], 0);
  TVector3 x1(par[0] + par[1], par[2] + par[3], 1);
  TVector3 u = (x1 - x0).Unit();
  double d2 = ((xp - x0).Cross(u)).Mag2();

  // std::cout << "Root of the distance: " << TMath::Sqrt(d2) << std::endl;

  return d2;
}

Double_t TtCracowTask::distance2Single(double x, double y, double z, double ex, double ey, double, double *par)
{ // ez //[R.K.03/2017] unused variable(s)
  // distance line point is D= | (xp-x0) cross  ux |
  // where ux is direction of line and x0 is a point in the line (like t = 0)

  TVector3 ddd(0, 0, 0);
  double d2 = -999999.;

  if (ex < 10000.) //  this means only x
  {
    ddd.SetXYZ(x - (par[1] * z + par[0]), 0., 0.);
    d2 = ddd.Mag2();
  }

  if (ey < 10000.) //  this means only y
  {
    ddd.SetXYZ(0., y - (par[3] * z + par[2]), 0.);
    d2 = ddd.Mag2();
  }

  // std::cout << "Root of the distance: " << TMath::Sqrt(d2) << std::endl;

  return d2;
}

void TtCracowTask::SumDistance2(int &, double *, double &sum, double *par, int)
{

  TGraph2D *gr = dynamic_cast<TGraph2D *>((TVirtualFitter::GetFitter())->GetObjectFit());
  assert(gr != 0);
  double *x = gr->GetX();
  double *y = gr->GetY();
  double *z = gr->GetZ();
  double *Ex = gr->GetEX();
  double *Ey = gr->GetEY();
  double *Ez = gr->GetEZ();

  int npoints = gr->GetN();
  sum = 0;
  double d = 0.;

  // for (int i  = 0; i < 3 ; ++i) {
  for (int i = 0; i < npoints; ++i) {
    // std::cout << "Sensor: " << i;
    if ((Ex[i] < 10000.) && (Ey[i] < 10000.)) {
      // std::cout << " - Double Sided" << std::endl;
      d = distance2(x[i], y[i], z[i], par);
    }

    if ((Ex[i] > 10000.) && (Ey[i] > 10000.)) {
      // std::cout << " - Wrong Errors!" << std::endl;
      d = 0.;
    }

    if ((Ex[i] > 10000.) || (Ey[i] > 10000.)) {
      // std::cout << " - Single Sided" << std::endl;
      d = distance2Single(x[i], y[i], z[i], Ex[i], Ey[i], Ez[i], par);
    }

    sum += d;
  }
}

void TtCracowTask::Exec(Option_t *)
{

  fTrackcount = 0;

  Bool_t s0 = kFALSE;
  Bool_t s1 = kFALSE;
  Bool_t s2 = kFALSE;
  Bool_t s3 = kFALSE;
  Bool_t s4 = kFALSE;
  Bool_t s5 = kFALSE;

  // resetting energy losses

  for (Int_t qq = 0; qq < 6; qq++) {
    fEloss[qq] = 0;
  }

  Int_t ntcand = fTCandArray->GetEntriesFast();

  if (ntcand == 6) {

    //  std::cout << "Entries: " << ntcand << std::endl;

    std::map<Double_t, Int_t> SensorsPos;

    std::cout << "Event: " << fEvent << std::endl;
    std::cout << "Initial map size: " << SensorsPos.size() << std::endl;

    //    std::cout<<"TtCracowTask::Exec"<<std::endl;
    // Reset output Array
    if (fTrackArray == 0)
      Fatal("TtCracowTask::Exec", "No TrackArray");

    Int_t name;

    for (Int_t itr = 0; itr < ntcand; ++itr) {

      PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(itr);

      name = theHit->GetSensorID();

      if (SensorsPos.size() < 6)
        SensorsPos[theHit->GetZ()] = name;
    }

    const Int_t sizeMap = (Int_t)SensorsPos.size();

    Int_t DetNames[sizeMap];
    // Double_t Pos[sizeMap]; //[R.K.02/2017] Unused variable?

    Int_t jj = 0;

    if (SensorsPos.size() >= 6) {

      std::cout << "Map size after looping: " << SensorsPos.size() << std::endl;
      // std::cout << "Number of Sensors:" << SensorsPos.size() << std::endl;
      for (std::map<Double_t, Int_t>::iterator it = SensorsPos.begin(); it != SensorsPos.end(); ++it) {

        std::cout << "position: " << it->first << " name: " << (it->second) << std::endl;

        DetNames[jj] = it->second;
        // Pos[jj] = it -> first; //[R.K.02/2017] Unused variable?
        jj++;
      }
    }

    if (fEvent < 5) // just to check
    {

      for (Int_t l = 0; l < sizeMap; l++) {
        // std::cout << DetNames[l] << std::endl;
      }
    }

    if (SensorsPos.size() == 6) {

      Double_t x[6];
      Double_t y[6];
      Double_t z[6];

      Double_t Erx[6];
      Double_t Ery[6];
      // Double_t Erz[6]; //[R.K.02/2017] Unused variable?

      // Int_t track = 0; //[R.K. 01/2017] unused variable?

      // Double_t buffErrX = -0.099999; //[R.K. 01/2017] unused variable?
      // Double_t buffErrY = -0.099999; //[R.K. 01/2017] unused variable?

      //	  std::cout << "Event: " << fEvent << std::endl;

      Int_t ihit = 0;

      TGraph2DErrors *gr = new TGraph2DErrors();

      for (Int_t it1 = 0; it1 < ntcand; it1++) {

        PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(it1);

        if ((theHit->GetSensorID()) == DetNames[0]) {
          x[0] = theHit->GetX();
          y[0] = theHit->GetY();
          z[0] = theHit->GetZ();
          Erx[0] = theHit->GetDx();
          Ery[0] = theHit->GetDy();
          // Erz[0] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[0] = theHit->GetEloss();
          s0 = kTRUE;
        }

        if ((theHit->GetSensorID()) == DetNames[1]) {
          x[1] = theHit->GetX();
          y[1] = theHit->GetY();
          z[1] = theHit->GetZ();
          Erx[1] = theHit->GetDx();
          Ery[1] = theHit->GetDy();
          // Erz[1] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[1] = theHit->GetEloss();
          s1 = kTRUE;
        }
        if ((theHit->GetSensorID()) == DetNames[2]) {
          x[2] = theHit->GetX();
          y[2] = theHit->GetY();
          z[2] = theHit->GetZ();
          Erx[2] = theHit->GetDx();
          Ery[2] = theHit->GetDy();
          // Erz[2] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[2] = theHit->GetEloss();
          s2 = kTRUE;
        }

        if ((theHit->GetSensorID()) == DetNames[3]) {
          x[3] = theHit->GetX();
          y[3] = theHit->GetY();
          z[3] = theHit->GetZ();
          Erx[3] = theHit->GetDx();
          Ery[3] = theHit->GetDy();
          // Erz[3] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[3] = theHit->GetEloss();
          s3 = kTRUE;
        }
        if ((theHit->GetSensorID()) == DetNames[4]) {
          x[4] = theHit->GetX();
          y[4] = theHit->GetY();
          z[4] = theHit->GetZ();
          Erx[4] = theHit->GetDx();
          Ery[4] = theHit->GetDy();
          // Erz[4] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[4] = theHit->GetEloss();
          s4 = kTRUE;
        }

        if ((theHit->GetSensorID()) == DetNames[5]) {
          x[5] = theHit->GetX();
          y[5] = theHit->GetY();
          z[5] = theHit->GetZ();
          Erx[5] = theHit->GetDx();
          Ery[5] = theHit->GetDy();
          // Erz[5] = theHit->GetDz(); //[R.K.02/2017] Unused variable?
          fEloss[5] = theHit->GetEloss();
          s5 = kTRUE;
        }

        ihit++;

      } // end loop on points

      // Int_t uu = 0; //[R.K. 01/2017] unused variable?

      for (Int_t ww = 0; ww < 6; ww++) { // setting big errors for the bottom side
        {
          if (TMath::Abs(Erx[ww]) > 0.5)
            Erx[ww] = 10000.;
          if (TMath::Abs(Ery[ww]) > 0.5)
            Ery[ww] = 10000.;
        }

        //	      std::cout << "(" << x[ww] << "," <<  y[ww] << "," <<  z[ww] << ")" << std::endl;
      }

      // Double_t parFit[4]; //fit-parameter //[R.K. 01/2017] unused variable?

      if (s0 && s1 && s2 && s3 && s4 && s5) {
        for (Int_t ss = 0; ss < 6; ss++) {
          gr->SetPoint(ss, x[ss], y[ss], z[ss]);
          gr->SetPointError(ss, x[ss], y[ss], z[ss]);
        }
      }

      Int_t Npoint = gr->GetN();

      Double_t *xx = gr->GetX();
      Double_t *yy = gr->GetY();
      Double_t *zz = gr->GetZ();

      TVector3 dir;
      dir.SetXYZ(xx[Npoint - 1] - xx[0], yy[Npoint - 1] - yy[0], zz[Npoint - 1] - zz[0]);

      TVirtualFitter::SetDefaultFitter("Minuit");
      TVirtualFitter *min = TVirtualFitter::Fitter(0, 4);
      min->SetObjectFit(gr);
      min->SetFCN(*SumDistance2); // using local coordinate in FCN

      Double_t arglist[100];

      arglist[0] = 0;
      min->ExecuteCommand("SET NOWarnings", arglist, 1);
      arglist[0] = -1;
      min->ExecuteCommand("SET PRINT", arglist, 1);

      // double pStart[4] = {xx[0],(dir.X())/(dir.Y()),zz[0],(dir.Z())/(dir.Y())}; //[R.K. 01/2017] unused variable?
      // double pStartErr[4] = {gr->GetErrorX(0),0.1*(dir.X())/(dir.Y()),gr->GetErrorZ(0),0.1*(dir.Z())/(dir.Y())}; //[R.K. 01/2017] unused variable?

      /*min->SetParameter(0,"y0",pStart[0],pStartErr[0],0,0);
      min->SetParameter(1,"A",pStart[1],pStartErr[1],0,0);
      min->SetParameter(2,"z0",pStart[2],pStartErr[2],0,0);
      min->SetParameter(3,"B",pStart[3],pStartErr[3],0,0);*/

      min->SetParameter(0, "y0", 0, 1, 0, 0);
      min->SetParameter(1, "A", 0, 1, 0, 0);
      min->SetParameter(2, "z0", 0, 1, 0, 0);
      min->SetParameter(3, "B", 0, 1, 0, 0);

      arglist[0] = 1000;  // number of functiona calls
      arglist[1] = 0.001; // tolerance
      min->ExecuteCommand("MIGRAD", arglist, 2);

      int nvpar, nparx;
      double amin, edm, errdef;
      min->GetStats(amin, edm, errdef, nvpar, nparx);
      if (fVerbose > 1)
        min->PrintResults(1, amin);

      // Double_t fitPar[4]; //[R.K. 01/2017] unused variable?
      // Double_t fitParErr[4]; //[R.K. 01/2017] unused variable?

      Double_t chi2 = amin / (2. * Npoint - 4);

      Double_t SumEn = 0.; // FIXME is that a good initialisation value?

      for (Int_t gg = 0; gg < 6; gg++)
        SumEn += fEloss[gg];

      std::cout << "Scrivo" << endl;
      new ((*fTrackArray)[0]) TtFitRes(min->GetParameter(0), min->GetParameter(1), min->GetParameter(2), min->GetParameter(3), SumEn, chi2, chi2, Npoint);
      //	  new ((*fTrackArray)[0]) TtFitRes(min->GetParameter(0), min->GetParameter(1), min->GetParameter(2), min->GetParameter(3), fEloss, chi2, chi2, 3) ;
      delete gr;

    } // if 6 hits
  }

  fEvent++;

  //  delete gr;

  return;
}

void TtCracowTask::MyFit(Double_t *x, Double_t *y, Double_t *z, Double_t *Erx, Double_t *Ery, Double_t *Erz, Double_t *par, Double_t &chiX, Double_t &chiY)
{

  TGraphErrors grX;
  TGraphErrors grY;

  Int_t ix = 0, iy = 0;

  for (Int_t j = 0; j < 6; j++) {
    // std::cout << j << "@(" << Erx[j] << "," <<  Ery[j] << "," <<  Erz[j] << ")" << std::endl;

    if (TMath::Abs(Erx[j]) < 0.5) {
      grX.SetPoint(ix, z[j], x[j]);
      grX.SetPointError(ix, TMath::Abs(Erz[j]), TMath::Abs(Erx[j]));
      // cout << "erz : " << Erz[j] << "  erx : " << Erx[j] << endl;
      ix++;
    }

    if (TMath::Abs(Ery[j]) < 0.5) {
      grY.SetPoint(iy, z[j], y[j]);
      grY.SetPointError(iy, TMath::Abs(Erz[j]), TMath::Abs(Ery[j]));
      // cout << "erz : " << Erz[j] << "  ery : " << Ery[j] << endl;
      iy++;
    }
  }

  //  std::cout << "POINTS  " << ix << "   " << iy << std::endl;

  Double_t mx, nx, my, ny;

  grX.Fit("pol1", "");

  nx = (grX.GetFunction("pol1"))->GetParameter(0);
  mx = (grX.GetFunction("pol1"))->GetParameter(1);

  grY.Fit("pol1", ""); //"Q"

  ny = (grY.GetFunction("pol1"))->GetParameter(0);
  my = (grY.GetFunction("pol1"))->GetParameter(1);

  par[0] = nx;
  par[1] = mx;
  par[2] = ny;
  par[3] = my;

  chiX = (grX.GetFunction("pol1"))->GetChisquare();

  chiY = (grY.GetFunction("pol1"))->GetChisquare();
}

void TtCracowTask::FinishEvent()
{
  fTrackArray->Clear();
  FinishEvents();
}

ClassImp(TtCracowTask);
