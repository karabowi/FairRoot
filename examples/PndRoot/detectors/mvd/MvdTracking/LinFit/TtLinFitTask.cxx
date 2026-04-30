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

// Simone Bianco
// 15.07.2010

// This Class' Header ------------------
#include "TtLinFitTask.h"
#include "TtFitRes.h"

// C/C++ Headers ----------------------
#include <iostream>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "FairLogger.h"

//#include "PndLinTrack.h"
#include "PndTrackCand.h"
#include "PndSdsHit.h"
#include "PndMvdHit.h"
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

using namespace ROOT::Math;
using namespace std;

TtLinFitTask::TtLinFitTask() : FairTask("3D-Straight-Line-Fit")
{
  fTCandBranchName = "MVDHitsStrip";
}

TtLinFitTask::~TtLinFitTask() {}

InitStatus TtLinFitTask::Init()
{
  // Get ROOT Manager
  FairRootManager *ioman = FairRootManager::Instance();

  if (ioman == 0) {
    Error("TtLinFitTask::Init", "RootManager not instantiated!");
    return kERROR;
  }

  // Get input collection

  fTCandArray = (TClonesArray *)ioman->GetObject(fTCandBranchName);

  if (fTCandArray == 0) {
    Error("TtLinFitTask::Init", "trackcand-array not found!");
    return kERROR;
  }

  fTrackArray = new TClonesArray("TtFitRes");
  ioman->Register("TTFit", "PndMvd", fTrackArray, kTRUE);

  LOG(info) << " TtLinFitTask: Initialisation successfull";

  fEvent = 0;

  return kSUCCESS;
}

void TtLinFitTask::Exec(Option_t *)
{

  fTrackcount = 0;

  // resetting energy losses

  for (Int_t qq = 0; qq < 6; qq++) {
    fEloss[qq] = 0;
  }

  Int_t ntcand = fTCandArray->GetEntriesFast();

  if (ntcand == 6) {

    //  std::cout << "Entries: " << ntcand << std::endl;

    std::map<Double_t, Int_t> SensorsPos;

    //    std::cout<<"TtLinFitTask::Exec"<<std::endl;
    // Reset output Array
    if (fTrackArray == 0)
      Fatal("TtLinFitTask::Exec", "No TrackArray");

    Int_t name;

    for (Int_t itr = 0; itr < ntcand; ++itr) {

      PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(itr);

      name = theHit->GetSensorID();

      if (SensorsPos.size() < 6)
        SensorsPos[theHit->GetZ()] = name;
    }

    const Int_t sizeMap = SensorsPos.size();

    Int_t DetNames[sizeMap];
    Double_t Pos[sizeMap];
    Int_t jj = 0;

    if (SensorsPos.size() >= 6) {
      // std::cout << "Number of Sensors:" << SensorsPos.size() << std::endl;
      for (std::map<Double_t, Int_t>::iterator it = SensorsPos.begin(); it != SensorsPos.end(); ++it) {
        //	      if (fEvent < 5) std::cout << "position: " << it->first << " name: " << (it->second) << std::endl;

        DetNames[jj] = it->second;
        Pos[jj] = it->first;
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
      Double_t Erz[6];

      Int_t track = 0;

      Double_t buffErrX = -0.099999;
      Double_t buffErrY = -0.099999;

      //	  std::cout << "Event: " << fEvent << std::endl;

      Int_t ihit = 0;

      for (Int_t it1 = 0; it1 < ntcand; it1++) {

        PndSdsHit *theHit = (PndSdsHit *)fTCandArray->At(it1);

        if ((theHit->GetSensorID()) == DetNames[0]) {
          x[0] = theHit->GetX();
          y[0] = theHit->GetY();
          z[0] = theHit->GetZ();
          Erx[0] = theHit->GetDx();
          Ery[0] = theHit->GetDy();
          Erz[0] = theHit->GetDz();
          fEloss[0] = theHit->GetEloss();
        }

        if ((theHit->GetSensorID()) == DetNames[1]) {
          x[1] = theHit->GetX();
          y[1] = theHit->GetY();
          z[1] = theHit->GetZ();
          Erx[1] = theHit->GetDx();
          Ery[1] = theHit->GetDy();
          Erz[1] = theHit->GetDz();
          fEloss[1] = theHit->GetEloss();
        }
        if ((theHit->GetSensorID()) == DetNames[2]) {
          x[2] = theHit->GetX();
          y[2] = theHit->GetY();
          z[2] = theHit->GetZ();
          Erx[2] = theHit->GetDx();
          Ery[2] = theHit->GetDy();
          Erz[2] = theHit->GetDz();
          fEloss[2] = theHit->GetEloss();
        }

        if ((theHit->GetSensorID()) == DetNames[3]) {
          x[3] = theHit->GetX();
          y[3] = theHit->GetY();
          z[3] = theHit->GetZ();
          Erx[3] = theHit->GetDx();
          Ery[3] = theHit->GetDy();
          Erz[3] = theHit->GetDz();
          fEloss[3] = theHit->GetEloss();
        }
        if ((theHit->GetSensorID()) == DetNames[4]) {
          x[4] = theHit->GetX();
          y[4] = theHit->GetY();
          z[4] = theHit->GetZ();
          Erx[4] = theHit->GetDx();
          Ery[4] = theHit->GetDy();
          Erz[4] = theHit->GetDz();
          fEloss[4] = theHit->GetEloss();
        }

        if ((theHit->GetSensorID()) == DetNames[5]) {
          x[5] = theHit->GetX();
          y[5] = theHit->GetY();
          z[5] = theHit->GetZ();
          Erx[5] = theHit->GetDx();
          Ery[5] = theHit->GetDy();
          Erz[5] = theHit->GetDz();
          fEloss[5] = theHit->GetEloss();
        }

        ihit++;

      } // end loop on points

      Int_t uu = 0;

      for (Int_t ww = 0; ww < 6; ww++) { // setting big errors for the bottom side
        {
          if (TMath::Abs(Erx[ww]) > 0.5)
            Erx[ww] = 1000000;
          if (TMath::Abs(Ery[ww]) > 0.5)
            Ery[ww] = 1000000;
        }

        //	      std::cout << "(" << x[ww] << "," <<  y[ww] << "," <<  z[ww] << ")" << std::endl;
      }

      Double_t parFit[4]; // fit-parameter

      Double_t chiX, chiY;

      MyFit(x, y, z, Erx, Ery, Erz, parFit, chiX, chiY);

      Double_t pointX, pointY, pointZ;

      TtFitRes *trackfit = new TtFitRes(parFit[0], parFit[1], parFit[2], parFit[3], fEloss, chiX, chiY, 6);

      new ((*fTrackArray)[fTrackcount]) TtFitRes(*(trackfit)); // save Track
      fTrackcount++;

    } // if 6 hits
  }

  fEvent++;

  return;
}

void TtLinFitTask::MyFit(Double_t *x, Double_t *y, Double_t *z, Double_t *Erx, Double_t *Ery, Double_t *Erz, Double_t *par, Double_t &chiX, Double_t &chiY)
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

ClassImp(TtLinFitTask);
