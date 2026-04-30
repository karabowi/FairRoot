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

// Simone Bianco 14/07/2010

// This Class' Header ------------------
#include "TtAliTask.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "FairLogger.h"

#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndTrackCandHit.h"

#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "TLorentzVector.h"
#include "TVector2.h"

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

using namespace ROOT::Math;
using namespace std;

TtAliTask::TtAliTask()
  : FairTask("Alignment"), fTCandArray(nullptr), fTCandBranchName("MVDHitsStrip"), fTrackcount(), fEvent(), fExclBox(), fPrint(0), sX(), sY(), sigX(), sigY(), m_X(), m_Y(),
    hx(nullptr), hy(nullptr)
{
  for (Int_t gg = 0; gg < 4; gg++) {
    sX[gg] = 0.;
    sY[gg] = 0.;
    sigX[gg] = 0.;
    sigY[gg] = 0.;
    m_X[gg] = 0.;
    m_Y[gg] = 0.;
  }
}

TtAliTask::TtAliTask(Int_t ExcludedBox)
  : FairTask("3D-Straight-Line-Fit"), fTCandArray(nullptr), fTCandBranchName("MVDHitsStrip"), fTrackcount(), fEvent(), fExclBox(ExcludedBox), fPrint(0), sX(), sY(), sigX(), sigY(),
    m_X(), m_Y(), hx(nullptr), hy(nullptr)
{
  if (fExclBox < 1 || fExclBox > 6) {
    std::cout << "Excluded box: Wrong value, setting as default 2!" << std::endl;
    fExclBox = 2;
  }

  for (Int_t gg = 0; gg < 6; gg++) {
    sX[gg] = 0.;
    sY[gg] = 0.;
    sigX[gg] = 0.;
    sigY[gg] = 0.;
    m_X[gg] = 0.;
    m_Y[gg] = 0.;
  }
}

TtAliTask::~TtAliTask() {}

InitStatus TtAliTask::Init()
{

  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("TtAliTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection

  fTCandArray = (TClonesArray *)ioman->GetObject(fTCandBranchName);

  if (fTCandArray == 0) {
    Error("TtAliTask::Init", "trackcand-array not found!");
    return kERROR;
  }

  LOG(info) << " TtAliTask: Initialisation successfull";

  fEvent = 0;

  hx = new TH1F("hx", "hx", 50000, -5., +5.);
  hy = new TH1F("hy", "hy", 50000, -5., +5.);

  return kSUCCESS;
}

void TtAliTask::Exec(Option_t *)
{

  //  std::cout << "Event: " << fEvent << std::endl;

  Int_t ntcand = fTCandArray->GetEntriesFast();

  if (ntcand == 6) {

    //  std::cout << "Entries: " << ntcand << std::endl;

    std::map<Double_t, Int_t> SensorsPos;

    //    std::cout<<"TtAliTask::Exec"<<std::endl;
    // Reset output Array
    if (fTCandArray == 0)
      Fatal("TtAliTask::Exec", "No Points");

    Int_t name;

    // Detailed output
    //   std::cout<<" -I- TtAliTask: contains "<<ntcand<<" TCandidates"<<std::endl;

    // std::cout<< " Detailed Debug info on the candidates:"<<std::endl;

    for (Int_t itr = 0; itr < ntcand; ++itr) {

      PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(itr);

      name = theHit->GetSensorID();

      if (SensorsPos.size() < 6)
        SensorsPos[theHit->GetZ()] = name;
    }

    //  const Int_t sizeMap = Sensors.size();

    const Int_t sizeMap = SensorsPos.size();

    Int_t DetNames[sizeMap];
    // Double_t Pos[sizeMap]; //[R.K.03/2017] unused variable
    Int_t jj = 0;

    if (SensorsPos.size() >= 6) {
      // std::cout << "Number of Sensors:" << SensorsPos.size() << std::endl;
      for (std::map<Double_t, Int_t>::iterator it = SensorsPos.begin(); it != SensorsPos.end(); ++it) {
        if (fEvent < 5)
          std::cout << "position: " << it->first << " name: " << (it->second) << std::endl;

        DetNames[jj] = it->second;
        // Pos[jj] = it -> first; //[R.K.03/2017] unused variable
        jj++;
      }
    }

    //   if (fEvent < 5)
    // 	{

    // 	  for (Int_t l = 0 ; l < sizeMap ; l++)
    // 	    {
    // 	      std::cout << DetNames[l] << std::endl;
    // 	    }
    // 	}

    ///////////////

    if (SensorsPos.size() == 6) {
      // Int_t DetRem[sizeMap-1]; //[R.K.02/2017] Unused variable?
      // for (Int_t ss = 0 ; ss < (sizeMap-1) ; ss++) //[R.K.02/2017] Unused variable?
      //{ //[R.K.02/2017] Unused variable?

      // DetRem[ss] = 0; //[R.K.02/2017] Unused variable?

      //} //[R.K.02/2017] Unused variable?

      // Int_t counter = 0; //[R.K.02/2017] Unused variable?

      // Double_t BuffZ = -9999.;

      // for (Int_t kk = 0 ; kk < (sizeMap) ; kk++) //[R.K.02/2017] Unused variable?
      //{ //[R.K.02/2017] Unused variable?

      // if ((fExclBox-1) == kk)  //[R.K.02/2017] Unused variable?
      //{ //[R.K.02/2017] Unused variable?
      // BuffZ = Pos[kk]; //[R.K.02/2017] Unused variable?
      // continue; //[R.K.02/2017] Unused variable?
      //} //[R.K.02/2017] Unused variable?
      // else //[R.K.02/2017] Unused variable?
      //{ //[R.K.02/2017] Unused variable?
      // DetRem[counter] = kk; //[R.K.02/2017] Unused variable?
      // counter++; //[R.K.02/2017] Unused variable?
      //} //[R.K.02/2017] Unused variable?

      //} //[R.K.02/2017] Unused variable?

      Double_t x[6];
      Double_t y[6];
      Double_t z[6];

      Double_t Erx[6];
      Double_t Ery[6];
      Double_t Erz[6];

      // Int_t track = 0; //[R.K. 01/2017] unused variable?

      // Double_t RealX = -1999999., RealY = -1999999;

      // Double_t buffErrX = -0.099999; //[R.K. 01/2017] unused variable?
      // Double_t buffErrY = -0.099999; //[R.K. 01/2017] unused variable?

      Int_t ihit = 0;

      for (Int_t it1 = 0; it1 < ntcand; it1++)

      {

        PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(it1);

        if ((theHit->GetSensorID()) == DetNames[0]) {
          x[0] = theHit->GetX();
          y[0] = theHit->GetY();
          z[0] = theHit->GetZ();
          Erx[0] = theHit->GetDx();
          Ery[0] = theHit->GetDy();
          Erz[0] = theHit->GetDz();
        }

        if ((theHit->GetSensorID()) == DetNames[1]) {
          x[1] = theHit->GetX();
          y[1] = theHit->GetY();
          z[1] = theHit->GetZ();
          Erx[1] = theHit->GetDx();
          Ery[1] = theHit->GetDy();
          Erz[1] = theHit->GetDz();
        }
        if ((theHit->GetSensorID()) == DetNames[2]) {
          x[2] = theHit->GetX();
          y[2] = theHit->GetY();
          z[2] = theHit->GetZ();
          Erx[2] = theHit->GetDx();
          Ery[2] = theHit->GetDy();
          Erz[2] = theHit->GetDz();
        }

        if ((theHit->GetSensorID()) == DetNames[3]) {
          x[3] = theHit->GetX();
          y[3] = theHit->GetY();
          z[3] = theHit->GetZ();
          Erx[3] = theHit->GetDx();
          Ery[3] = theHit->GetDy();
          Erz[3] = theHit->GetDz();
        }
        if ((theHit->GetSensorID()) == DetNames[4]) {
          x[4] = theHit->GetX();
          y[4] = theHit->GetY();
          z[4] = theHit->GetZ();
          Erx[4] = theHit->GetDx();
          Ery[4] = theHit->GetDy();
          Erz[4] = theHit->GetDz();
        }

        if ((theHit->GetSensorID()) == DetNames[5]) {
          x[5] = theHit->GetX();
          y[5] = theHit->GetY();
          z[5] = theHit->GetZ();
          Erx[5] = theHit->GetDx();
          Ery[5] = theHit->GetDy();
          Erz[5] = theHit->GetDz();
        }

        ihit++;

      } // end loop on points

      // setting the points

      Int_t uu = 0;

      // to check alignment...

      for (Int_t ww = 0; ww < 6; ww++) {
        if (TMath::Abs(Erx[ww]) < 0.5) {
          x[ww] = x[ww] + sX[ww];
        }
        if (TMath::Abs(Ery[ww]) < 0.5) {
          y[ww] = y[ww] + sY[ww];
        }
      }

      for (Int_t ww = 0; ww < 6; ww++) {
        if ((fExclBox - 1) != ww) {
          if (TMath::Abs(Erx[ww]) > 0.5)
            Erx[ww] = 1000000;
          if (TMath::Abs(Ery[ww]) > 0.5)
            Ery[ww] = 1000000;
          uu++;
        }

        //	      std::cout << "(" << x[ww] << "," <<  y[ww] << "," <<  z[ww] << ")" << std::endl;
      }

      Double_t DX, DY;

      MyFit(x, y, z, Erx, Ery, Erz, x[fExclBox - 1], y[fExclBox - 1], z[fExclBox - 1], DX, DY);

      // Double_t pointX,pointY,pointZ; //[R.K. 01/2017] unused variable?

      hx->Fill(DX);
      hy->Fill(DY);

    } // if 6 hits

    fEvent++;
  }

  return;
}

void TtAliTask::MyFit(Double_t *x, Double_t *y, Double_t *z, Double_t *Erx, Double_t *Ery, Double_t *, Double_t realX, Double_t realY, Double_t realZ, Double_t &DELTAX,
                      Double_t &DELTAY) // Erz //[R.K.03/2017] unused variable(s)
{

  TGraphErrors grX;
  TGraphErrors grY;

  Int_t ix = 0, iy = 0;

  for (Int_t j = 0; j < 6; j++) {
    // std::cout << j << "@(" << Erx[j] << "," <<  Ery[j] << "," <<  Erz[j] << ")" << std::endl;
    if (j == fExclBox - 1)
      continue;

    if (TMath::Abs(Erx[j]) < 0.5)
    // if (j==0 || j==1 || j==3 || j==5)
    {
      grX.SetPoint(ix, z[j], x[j]);
      ix++;
    }
    // if(j==0 || j==2 || j==4 || j==5)
    if (TMath::Abs(Ery[j]) < 0.5) {
      grY.SetPoint(iy, z[j], y[j]);
      iy++;
    }
  }

  //  std::cout << "POINTS  " << ix << "   " << iy << std::endl;

  Double_t mx, nx, my, ny;

  grX.Fit("pol1", "Q");

  nx = (grX.GetFunction("pol1"))->GetParameter(0);
  mx = (grX.GetFunction("pol1"))->GetParameter(1);

  grY.Fit("pol1", "Q");

  ny = (grY.GetFunction("pol1"))->GetParameter(0);
  my = (grY.GetFunction("pol1"))->GetParameter(1);

  Double_t buffX, buffY;

  buffX = mx * realZ + nx;
  buffY = my * realZ + ny;

  DELTAX = buffX - realX;
  DELTAY = buffY - realY;
}

void TtAliTask::FinishTask()
{

  Double_t mX, mY, siX, siY;

  TF1 *fun = new TF1("fun", "gaus");

  hx->Fit(fun, "Q", "", (hx->GetMean() - 2 * (hx->GetRMS())), (hx->GetMean() + 2 * (hx->GetRMS())));

  // hx->Fit(fun,"Q");

  mX = fun->GetParameter(1);
  siX = fun->GetParameter(2);

  cout << "RMS: " << hx->GetRMS() << " sigma " << siX << endl;

  hy->Fit(fun, "Q", "", (hy->GetMean() - 2 * (hy->GetRMS())), (hy->GetMean() + 2 * (hy->GetRMS())));

  // hy->Fit(fun,"Q");

  mY = fun->GetParameter(1);
  siY = fun->GetParameter(2);

  cout << "X, mean: " << mX << ", sig: " << siX << endl;
  cout << "Y, mean: " << mY << ", sig: " << siY << endl;

  if ((TMath::Abs(siX)) < 1.) {
    sX[fExclBox - 1] += mX;
    m_X[fExclBox - 1] = mX;
    sigX[fExclBox - 1] = siX;
  } else {
    sX[fExclBox - 1] = 0.;
    m_X[fExclBox - 1] = 0.;
    sigX[fExclBox - 1] = 0.;
  }

  if ((TMath::Abs(siY)) < 1.) {
    sY[fExclBox - 1] += mY;
    m_Y[fExclBox - 1] = mY;
    sigY[fExclBox - 1] = siY;
  } else {
    sY[fExclBox - 1] = 0.;
    m_Y[fExclBox - 1] = 0.;
    sigY[fExclBox - 1] = 0.;
  }

  /*  switch(fExclBox)
      {
      case 1:
  sX[0] += mX;
  sY[0] += mY;
  m_X[0] = mX;
  m_Y[0] = mY;
  sigX[0] = siX;
  sigY[0] = siY;
  break;

      case 2:
  sX[1] += mX;
  sigX[1] = siX;
  m_X[1] = mX;

  break;

      case 3:
  sY[1] += mY;
  sigY[1] = siY;
  m_Y[1] = mY;
  break;

      case 4:
  sX[2] += mX;
  sigX[2] = siX;
  m_X[2] = mX;
  break;

      case 5:
  sY[2] += mY;
  sigY[2] = siY;
  m_Y[2] = mY;
  break;

      case 6:
  sX[3] += mX;
  sY[3] += mY;
  sigX[3] = siX;
  sigY[3] = siY;
  m_X[3] = mX;
  m_Y[3] = mY;
  break;
      }

  */
  if (fPrint == 1) {
    PrintHistos();
  }

  hx->Reset();
  hy->Reset();

  //  fEvent = 0;
}

void TtAliTask::PrintVal()
{

  cout << "SHIFTS" << endl;
  for (Int_t k = 0; k < 6; k++) {
    cout << "X: " << sX[k] << "   Y: " << sY[k] << endl;
  }
}

void TtAliTask::PrintMeanResiduals()
{

  cout << "RESIDUALS" << endl;
  for (Int_t k = 0; k < 6; k++) {
    cout << "X: " << m_X[k] << "   Y: " << m_Y[k] << endl;
  }
}

void TtAliTask::PrintSigmaResiduals()
{

  cout << "SIGMA-RESIDUALS" << endl;
  for (Int_t k = 0; k < 6; k++) {
    cout << "sigX: " << sigX[k] << "   sigY: " << sigY[k] << endl;
  }
}

TVector2 TtAliTask::GetRes()
{

  Double_t resx = TMath::Power((sigX[0] * sigX[1] * sigX[2] * sigX[3]), 1. / 4.);
  Double_t resy = TMath::Power((sigY[0] * sigY[1] * sigY[2] * sigY[3]), 1. / 4.);

  return TVector2(resx, resy);
}

void TtAliTask::PrintHistos()
{

  TCanvas *can = new TCanvas();

  can->cd();

  hx->Draw();

  hx->GetXaxis()->SetRangeUser((hx->GetMean()) - 8 * (hx->GetRMS()), (hx->GetMean()) + 8 * (hx->GetRMS()));

  std::string nameX = Form("HistResidualsX_%d.png", fExclBox);

  can->Print(nameX.c_str(), "png");

  hy->Draw();

  hy->GetXaxis()->SetRangeUser((hy->GetMean()) - 8 * (hy->GetRMS()), (hy->GetMean()) + 8 * (hy->GetRMS()));

  std::string nameY = Form("HistResidualsY_%d.png", fExclBox);

  can->Print(nameY.c_str(), "png");
}

ClassImp(TtAliTask);
