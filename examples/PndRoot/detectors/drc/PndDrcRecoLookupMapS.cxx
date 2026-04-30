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

// -------------------------------------------------------------------------
// -----                   PndDrcRecoLookupMapS source file                -----
// -----               Created 10/11/10  by Maria Patsyuk              -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndDrcRecoLookupMapS.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcPDPoint.h"
#include "PndDrcHit.h"
#include "PndDrcPDHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "FairLogger.h"

#include "PndGeoDrcPar.h"
#include "TFormula.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TF1.h"
#include "TExec.h"
#include "TLine.h"
#include "TPolyLine.h"
//#include "PndChPho.h"
#include "PndDrcLutInfo.h"
#include "PndDrcDigiPar.h"
#include "TVectorD.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
PndDrcRecoLookupMapS::PndDrcRecoLookupMapS() : FairTask("PndDrcRecoLookupMapS")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fDigiPar = nullptr;
}
// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcRecoLookupMapS::PndDrcRecoLookupMapS(Int_t verbose) : FairTask("PndDrcRecoLookupMapS")
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fDigiPar = nullptr;
}
// -----   Destructor   ----------------------------------------------------
PndDrcRecoLookupMapS::~PndDrcRecoLookupMapS()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
  fHistoList->Delete();
  delete fHistoList;
}

// -----   Initialization   -----------------------------------------------
InitStatus PndDrcRecoLookupMapS::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcRecoLookupMapS::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndDrcRecoLookupMapS::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcRecoLookupMapS::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }
  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndDrcRecoLookupMapS::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  /*  // Get input array
    fHitArray = (TClonesArray*) ioman->GetObject("DrcHit");
    if ( ! fHitArray ) {
      cout << "-W- PndDrcRecoLookupMapS::Init: "
           << "No DrcHit array!" << endl;
     return kERROR;
     }
  */

  // Get digi array
  fDigiArray = (TClonesArray *)ioman->GetObject("DrcDigi");
  if (!fDigiArray) {
    cout << "-W- PndDrcRecoLookupMapS::Init: "
         << "No DrcDigi array!" << endl;
    return kERROR;
  }

  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- PndDrcRecoLookupMapS::Init: "
         << "No DrcPDHit array!" << endl;
    return kERROR;
  }

  // Create and register output array
  // fChPhoArray = new TClonesArray("PndChPho");
  // ioman->Register("ChPho","Drc",fChPhoArray, kTRUE);
  fDrcLutInfoArray = new TClonesArray("PndDrcLutInfo");
  ioman->Register("DrcLutInfo", "Drc", fDrcLutInfoArray, kTRUE);

  // Get parameters:
  fpi = TMath::Pi();

  fR = fGeo->radius();
  fzup = fGeo->barBoxZUp();
  fzdown = fGeo->barBoxZDown();
  fHThick = fGeo->barHalfThick();
  fBboxNum = fGeo->BBoxNum();
  fBarNum = fGeo->barNum();
  fPipehAngle = fGeo->PipehAngle();
  fBarBoxGap = fGeo->BBoxGap();
  // fLength     = (180. - 2.*fPipehAngle - fBarBoxGap/fR*(fBboxNum/2. - 1.)/fpi*180.)/(fBboxNum/2.) * fR/ 180.*fpi;
  fDphi = 2. * (180. - 2 * fPipehAngle) / fBboxNum; //[degrees]
  fLSide = fGeo->Lside();                           //(180. - 2.*fPipehAngle - fBarBoxGap/fR*(fBboxNum/2. - 1.)/fpi*180.)/(fBboxNum/2.) * fR/ 180.*fpi;
  fBarWidth = fGeo->BarWidth();                     // fLSide/fBarNum;

  // vectors of bar surfaces:
  fnX1.SetXYZ(-1., 0., 0.);
  fnY1.SetXYZ(0., 1., 0.);

  LOG(info) << " PndDrcRecoLookupMapS: Intialization successfull";
  CreateHisto();
  return kSUCCESS;
}

// -----   Private method SetParContainers   -------------------------------
void PndDrcRecoLookupMapS::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get DIRC digitisation parameter container
  fDigiPar = (PndDrcDigiPar *)(db->getContainer("DIRCLookupTable"));
  LOG(info) << " PndDrcRecoLookupMapS::SetParContainers(). read parameters";
  cout << "read them!" << endl;
  // cout << "-I- PndDrcRecoLookupMapS: Number of hit pixels "<< fDigiPar->GetNHitPixels()<< endl;
  LOG(info) << " PndDrcRecoLookupMapS: Number of hit pixels " << fDigiPar->GetNAmbiguities();

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();
  if (fVerbose > 1)
    Info("SetParContainers", "done.");
  return;
}
// -------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------
void PndDrcRecoLookupMapS::Exec(Option_t *ion)
{
  // if ( ! fChPhoArray ) Fatal("Exec", "No fChPhoArray");
  // fChPhoArray->Clear();
  fGeoH->SetVerbose(fVerbose);

  nevents++;
  fDetectorID = 0;

  cout << "EVENT # " << nevents << endl;
  // fNHits = 0;
  // ProcessBarHit();
  ProcessPhotonHit();
  // ProcessPhotonMC();
}

//--------------Process Photon MC Points----------------------------------------------------
void PndDrcRecoLookupMapS::ProcessPhotonMC()
{
  if (fVerbose > 0) {
    cout << "PndDrcRecoLookupMapS: Number of Detected MC Tracks : " << fMCArray->GetEntries() << endl;
    PndMCTrack *ptr = nullptr;
    PndMCTrack *trMr = nullptr;
  }
}
//--------------Process Bar Hits----------------------------------------------------
void PndDrcRecoLookupMapS::ProcessBarHit()
{
  PndDrcBarPoint *pt = nullptr;
  PndDrcHit *hit = nullptr;
  PndMCTrack *tr = nullptr;

  for (Int_t j = 0; j < fHitArray->GetEntriesFast(); j++) {
    hit = (PndDrcHit *)fHitArray->At(j);
    Int_t mcRef = hit->GetRefIndex();
    pt = (PndDrcBarPoint *)fBarPointArray->At(mcRef);

    Int_t chtrID = pt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(chtrID);

    cout << "nother track px = " << pt->GetPx() << ", py = " << pt->GetPy() << endl;
  }
}

//--------------Process Photon Hits----------------------------------------------------
void PndDrcRecoLookupMapS::ProcessPhotonHit()
{
  // Int_t nofChPho = 0;
  fDrcLutInfoArray->Clear();
  PndDrcLutInfo lutinfo;

  PndDrcPDPoint *Ppt = nullptr;
  PndDrcPDHit *pdhit = nullptr;
  PndMCTrack *tr = nullptr;
  PndMCTrack *trMr;

  if (fVerbose > 0) {
    cout << "PndDrcRecoLookupMapS: Number of Detected Photon MCPDPoints : " << fPDPointArray->GetEntries() << endl;
    cout << "PndDrcRecoLookupMapS: Number of Detected Photon PDHits : " << fPDHitArray->GetEntries() << endl;
  }

  // Loop over PndDrcPDHits
  for (Int_t k = 0; k < fPDHitArray->GetEntriesFast(); k++) {

    pdhit = (PndDrcPDHit *)fPDHitArray->At(k);

    Int_t mcPDRef = pdhit->GetRefIndex();

    if (mcPDRef < 0)
      continue;
    Ppt = (PndDrcPDPoint *)fPDPointArray->At(mcPDRef);

    if (Ppt->GetBarPointID() < 0)
      continue;
    PndDrcBarPoint *fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(Ppt->GetBarPointID());
    fBarId = fBarPoint->GetDetectorID();
    if (fBarId < 0)
      continue;
    // cout<<"bar name - "<<fGeoH->GetPath(fBarId)<<endl;

    Int_t trID = Ppt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(trID);

    Int_t trMID = tr->GetMotherID();

    // if(trMID > -1){
    trMr = (PndMCTrack *)fMCArray->At(trMID);
    Int_t trMpdg = trMr->GetPdgCode();

    if (trMr->GetMotherID() == -1) { // charged track is a primary

      lutinfo.SetChPartPdg(trMpdg);

      // expected cherenkov angle
      Double_t Mrmass;
      Int_t MoSign;
      if (fabs(trMpdg) == 11) {
        Mrmass = 0.000511;
        MoSign = 1;
      }
      if (fabs(trMpdg) == 13) {
        Mrmass = 0.105658;
        MoSign = 1;
      }
      if (fabs(trMpdg) == 211) {
        Mrmass = 0.139570;
        MoSign = -1;
      }
      if (fabs(trMpdg) == 321) {
        Mrmass = 0.49368;
        MoSign = -1;
      }
      if (fabs(trMpdg) == 2212) {
        Mrmass = 0.938;
        MoSign = -1;
      }
      if (fabs(trMpdg) == 50000050) {
        Mrmass = 0.0;
        MoSign = -1;
      }
      Double_t Mrmom = trMr->GetMomentum().Mag();
      CHexp = acos(sqrt(pow(Mrmom, 2) + pow(Mrmass, 2)) / Mrmom / fGeo->nQuartz());
      lutinfo.SetCherenkovMC(CHexp);
      // cout<<"+++++++++++++++++++++++++++"<<endl;
      // cout<<"CH expected = "<<CHexp<<endl;

      fxPHit = pdhit->GetX();
      fyPHit = pdhit->GetY();
      fzPHit = pdhit->GetZ();
      ftime = pdhit->GetTime();
      // ftime = Ppt->GetTime();

      // get the pixel id
      fpixID = pdhit->GetDetectorID();
      if (fpixID < 0)
        continue;

      // cout<<"hit id "<<fpixID<<", x = "<<pdhit->GetX()<<", y = "<<pdhit->GetY()<<", time = "<<ftime<<endl;

      Double_t xPPoi = Ppt->GetX();
      Double_t yPPoi = Ppt->GetY();

      lutinfo.AddTruePath(Ppt->GetLength());

      // momentum of a photon on the PD Plane
      fPphoPD.SetXYZ(Ppt->GetPx(), Ppt->GetPy(), Ppt->GetPz());

      {
        // mother momentum/direction
        fPMo.SetXYZ(trMr->GetMomentum().X(), trMr->GetMomentum().Y(), trMr->GetMomentum().Z());
        // fPMo.Print();
        lutinfo.SetChPartDir(fPMo);
        // cout<<"mother phi = "<<fPMo.Phi()/3.1415*180.<<endl;
        if (fB > 0.) {
          Double_t PtMo = sqrt(pow(trMr->GetMomentum().X(), 2) + pow(trMr->GetMomentum().Y(), 2));
          Double_t Rratio = fR * fR / 2. / pow((PtMo / 0.29979 / fB) * 100., 2);
          Double_t phi_extra = fPMo.Phi() + MoSign * trMpdg / fabs(trMpdg) * acos(1. - Rratio);
          fHAngleInBDeg = 0.5 * MoSign * trMpdg / fabs(trMpdg) * (acos(1. - Rratio) / TMath::Pi()) * 180.;
          // cout<<"B = "<<fB<<", R = "<<fR<<", trMpdg = "<<trMpdg<<", half angle in B = "<<fHAngleInBDeg<<endl;
          fPMo.SetPhi(phi_extra);
        }
        lutinfo.SetChPartDirInBar2(fPMo);
      }

      {
        // mother momentum/direction from the bar point
        fBarPoint->Momentum(fPMo);
        lutinfo.SetChPartDirInBar(fPMo);
      }

      // cout<<"Mother vector (global cs) : "<<endl;
      TVector3 motherMom = fPMo.Unit();
      // motherMom.Print();

      // initial momentum of the photon
      fPphoInit = tr->GetMomentum();
      // cout<<"Initial momentum of the photon :"<<endl;
      // fPphoInit.Print();

      // real = generated cherenkov angle
      CHreal = fPphoInit.Angle(fPMo);
      lutinfo.SetCherenkovReal(CHreal);
      // cout<<"CH real (generated) = "<<CHreal<<endl;

      Double_t etot = sqrt(pow(fPphoPD.X(), 2) + pow(fPphoPD.Y(), 2) + pow(fPphoPD.Z(), 2));
      flambdah = 197.0 * 2.0 * TMath::Pi() / (etot * 1.0E9); // wavelength of photon in nm
      lutinfo.AddLambda(flambdah);

      // production point of the photon
      fStartVertex = tr->GetStartVertex();

      // cout<<"start: phi = "<<fStartVertex.Phi()<<endl;
      // fStartVertex.Print();
      ftime0 = tr->GetStartTime() / 1.0E9;
      lutinfo.AddHitTime(ftime - ftime0);

      // cout<<"fstart time = "<<ftime0<<endl;
      //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      // proceed only if photon was born in the bar for which the LUT is used!!!
      // if(fStartVertex.Phi() >0./180.*fpi && fStartVertex.Phi() < 6./180.*fpi){
      if (fPphoInit.Z() > 0. || fPphoInit.Z() < 0.) { // reflected photons

        // Transformation of the photon initial direction to the local bar coodr system:
        // to get to the bar coord system.
        fPhiRot = InBarCoordSyst(fStartVertex, &fBBver1, &fBBver2, &fBBver3, &fBBver4);

        // Photon initial momentum in the bar coord. syst.
        fPphoB = (fGeoH->MasterToLocalShortId(fPphoInit, fBarId) - fGeoH->MasterToLocalShortId((0., 0., 0.), fBarId)).Unit(); // vector
        // fPphoB.Print();

        // mother momentum in the bar' (and bar) coord syst:
        fPMoB = (fGeoH->MasterToLocalShortId(fPMo, fBarId) - fGeoH->MasterToLocalShortId((0., 0., 0.), fBarId)).Unit(); // vector
        // PMBar.Print();

        // to find Cherenkov Phi (NOT THETA!!!):
        TVector3 kB;
        kB.SetXYZ(fkxBar, fkyBar, fkzBar);

        // use lookups to get the kXbar and kYbar of photons:
        NHitPix = fDigiPar->GetNHitPixels();
        // cout<<"N pixels = "<<NHitPix<<endl;
        NAmb = fDigiPar->GetNAmbiguities();
        // cout<<"N amb = "<<NAmb<<endl;
        NPixPar = fDigiPar->GetNPixelParam();
        Double_t par[NPixPar];

        if (fDigiPar->GetParamsForPixel(fpixID, par) == kFALSE) {
          cout << "NO SUCH HIT IN LUT!!!" << endl;
          continue;
        }

        // cout<<"LOOKUP!!!!"<<endl;
        // cout<<"N Hit Pix = "<<NHitPix<<endl;
        // cout<<"N Amb = "<<NAmb<<endl;
        // cout<<"NPixPar = "<<NPixPar<<endl;
        /*      cout<<"par 0 = "<<par[0]<<endl; // kx
              cout<<"par 1 = "<<par[1]<<endl; // ky
              cout<<"par 2 = "<<par[2]<<endl; // kxB
              cout<<"par 3 = "<<par[3]<<endl; // kyB
              cout<<"par 4 = "<<par[4]<<endl; // kxU1
              cout<<"par 5 = "<<par[5]<<endl; // kyU1
              cout<<"par 6 = "<<par[6]<<endl; // kxU2
              cout<<"par 7 = "<<par[7]<<endl; // kyU2
              cout<<"par 8 = "<<par[8]<<endl; // kxU3
              cout<<"par 9 = "<<par[9]<<endl; // kyU3
              cout<<"par 10 = "<<par[10]<<endl; // kxBU1
              cout<<"par 11 = "<<par[11]<<endl; // kyBU1
              cout<<"par 12 = "<<par[12]<<endl; // kxBU2
              cout<<"par 13 = "<<par[13]<<endl; // kyBU2
              cout<<"par 14 = "<<par[14]<<endl; // kxBU3
              cout<<"par 15 = "<<par[15]<<endl; // kyBU3
              cout<<"par 16 = "<<par[16]<<endl; // kxUB
              cout<<"par 17 = "<<par[17]<<endl; // kyUB
              cout<<"par 18= "<<par[18]<<endl; // kxUU
              cout<<"par 19= "<<par[19]<<endl; // kyUU
              cout<<"par 20= "<<par[20]<<endl; // kxUUU
              cout<<"par 21= "<<par[21]<<endl; // kyUUU
              cout<<"par 22= "<<par[22]<<endl; // kxBUU
              cout<<"par 23= "<<par[23]<<endl; // kyBUU
              cout<<"par 24= "<<par[24]<<endl; // kxBUB
              cout<<"par 25= "<<par[25]<<endl; // kyBUB
              cout<<"par 26= "<<par[26]<<endl; // kxUBU
              cout<<"par 27= "<<par[27]<<endl; // kyUBU
        */
        // fill in NPixPar/2 * 1 ambiguities:
        Double_t kX, kY, kZ;
        std::vector<TVector3> ambig;
        std::vector<Double_t> CHreco;
        std::vector<Double_t> Tamb;
        std::vector<Double_t> Path;

        // length of the vector should be the same as NumAmb*8   !!!!!
        Int_t NumAmb = NAmb; //=14
        TVectorD CHdiff(8 * NumAmb);
        TVectorD Adiff(8 * NumAmb); // vector to store angle diff btw real and reco photons

        for (Int_t i = 0; i < NumAmb; i++) {
          // in bar' coordinate system
          kX = par[i * 2];     // kXD
          kY = par[i * 2 + 1]; // kYD
          kZ = -sqrt(1. - pow(kX, 2.) - pow(kY, 2.));
          // cout<<"mom:  x = "<<fPMoB.X()<<",  y = "<<fPMoB.Y()<<", z = "<<fPMoB.Z()<<endl;
          fkBar.SetXYZ(kX, kY, kZ);
          // cout<<"photon from LUT in bar' coordinate system: "<<endl;
          // fkBar.Print();

          for (Int_t jamb = 0; jamb < 8; jamb++) {
            if (jamb == 1) {
              fkBar.SetXYZ(-kX, kY, kZ);
            }
            if (jamb == 2) {
              fkBar.SetXYZ(kX, -kY, kZ);
            }
            if (jamb == 3) {
              fkBar.SetXYZ(kX, kY, -kZ);
            }
            if (jamb == 4) {
              fkBar.SetXYZ(-kX, -kY, kZ);
            }
            if (jamb == 5) {
              fkBar.SetXYZ(kX, -kY, -kZ);
            }
            if (jamb == 6) {
              fkBar.SetXYZ(-kX, kY, -kZ);
            }
            if (jamb == 7) {
              fkBar.SetXYZ(-kX, -kY, -kZ);
            }

            // check if there is such enrty in the LUT and if the ambiguity fulfills the total internal reflection requitrement
            if ((kX != 0. && kY != 0.) && ((fkBar.Cross(fnX1)).Mag() > 1.00028 / fGeo->nQuartz() || (fkBar.Cross(fnY1)).Mag() > 1.00028 / fGeo->nQuartz())) {
              ambig.push_back(fkBar);
              CHreco.push_back(fkBar.Angle(fPMoB));
              CHdiff(8 * i + jamb) = fabs(CHreco[8 * i + jamb] - CHexp);
              Tamb.push_back(RecoAmbigTime(fkBar, fStartVertex, &fPath, 0));
              Path.push_back(fPath);
              Adiff(8 * i + jamb) = fkBar.Angle(fPphoB);
              // cout<<"reco ch angle = "<<fkBar.Angle(fPMoB)<<endl;
              // cout<<"number of bounces = "<<NumberOfBounces(fStartVertex, fPphoB, fBarId)<<endl;
              // cout<<"CHdiff = "<<CHdiff(8*i+jamb)<<", CHreco"<<8*i+jamb<<" = "<<CHreco[8*i+jamb]<<endl;

              // fill   only with credible information:
              lutinfo.AddAngle(fkBar.Angle(fPMoB));
              lutinfo.AddTime(RecoAmbigTime(fkBar, fStartVertex, &fPath, 0));
              lutinfo.AddPath(fPath);
              lutinfo.AddChDiff(CHreco[8 * i + jamb] - CHexp);
              // cout<<"fkBar = "<<endl;
              // fkBar.Print();
              lutinfo.AddNOfBounces(NumberOfBounces(fStartVertex, fkBar /*fPphoB*/, fBarId));

              // fkBarXHist->Fill(kX, fPphoB.X());
              // fkBarYHist->Fill(kY, fPphoB.Y());
            } else {
              ambig.push_back((0., 0., 0.));
              CHreco.push_back(-999.);
              CHdiff(8 * i + jamb) = -999.;
              Tamb.push_back(-999.);
              Path.push_back(-999.);
              Adiff(8 * i + jamb) = -999.;
            }
          } // for
        }

        fPhiMap = TMath::Nint(fPMo.Phi() / fpi * 180.);
        fThetaMap = TMath::Nint(fPMo.Theta() / fpi * 180.);
        // cout<<"phi = "<<fPMo.Phi()/fpi*180.<<", phi map = "<<fPhiMap<<", theta map = "<<fThetaMap<<endl;

        if (fB > 0.) {
          // fMapHist->Fill(fThetaMap,fPhiMap-fHAngleInBDeg);  // assuming that R = 50 cm
        }
        if (fB == 0.) {
          // fMapHist->Fill(fThetaMap,fPhiMap);
        }
      } // if start vertex lies within the right bar!
    }   // photon from primary particle

  } // photon hits
  new ((*fDrcLutInfoArray)[fDrcLutInfoArray->GetEntriesFast()]) PndDrcLutInfo(lutinfo);
}
//----------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMapS::InBarCoordSyst(TVector3 start, TVector3 *v1, TVector3 *v2, TVector3 *v3, TVector3 *v4)
{
  Double_t startPhi = start.Phi() / fpi * 180.; // [degrees]
  // cout<<"-I- InBarCoordinateSystem: start phi = "<<start.Phi()/fpi*180.<<endl;
  if (startPhi < 0.) {
    startPhi = 360. + startPhi;
  }
  // if(startPhi > 360.){startPhi = startPhi - 360.;}
  // cout<<"-I- InBarCoordinateSystem: start phi = "<<startPhi<<endl;
  // Double_t dphi = 2.*(180. - 2*fPipehAngle)/ fBboxNum; //[degrees]
  // cout<<"-I- InBarCoordinateSystem: dphi = "<<dphi<<endl;
  Double_t PhiRot = 0.; //[degrees]
  if (startPhi >= 0. && startPhi < 90.) {
    PhiRot = TMath::Floor(startPhi / fDphi) * fDphi + fDphi / 2.;
  }
  if (startPhi >= 90. && startPhi < 270.) {
    PhiRot = 90. + fPipehAngle + TMath::Floor((startPhi - 90. - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;
  }
  if (startPhi >= 270. && startPhi < 360.) {
    PhiRot = 270. + fPipehAngle + TMath::Floor((startPhi - 270. - fPipehAngle) / fDphi) * fDphi + fDphi / 2.;
  }
  // cout<<"-I- InBarCoordinateSystem: PhiRot = "<<PhiRot<<endl;

  TVector3 ver1, ver2, ver3, ver4;
  ver1.SetXYZ(fR - (fHThick), fLSide / 2., 0.);
  ver2.SetXYZ(fR + (fHThick), fLSide / 2., 0.);
  ver3.SetXYZ(fR + (fHThick), -fLSide / 2., 0.);
  ver4.SetXYZ(fR - (fHThick), -fLSide / 2., 0.);

  ver1.RotateZ(PhiRot / 180. * fpi);
  ver2.RotateZ(PhiRot / 180. * fpi);
  ver3.RotateZ(PhiRot / 180. * fpi);
  ver4.RotateZ(PhiRot / 180. * fpi);

  *v1 = ver1;
  *v2 = ver2;
  *v3 = ver3;
  *v4 = ver4;

  return PhiRot / 180. * fpi;
}

//------   Find Nubmer of Bounces     --------------------------------------
Int_t PndDrcRecoLookupMapS::NumberOfBounces(TVector3 start, TVector3 dir, Int_t barId)
{
  // start - photon production point in global coord system
  // dir - photon direction in bar coord system

  // cout<<"-I- NumberOfBounces: dir"<<endl;
  // dir.Print();
  // cout<<"-I- NumberOfBounces: start"<<endl;
  // start.Print();

  // Find coordinates of X0, Y0:
  Double_t Z0, X0, Y0;
  if (dir.Theta() < 3.1415 / 2.) {
    Z0 = -(fabs(fzup) + 2. * fzdown - start.Z());
  }
  if (dir.Theta() >= 3.1415 / 2.) {
    Z0 = -(start.Z() - fzup);
  }
  X0 = Z0 * TMath::Tan(dir.Theta()) * TMath::Cos(dir.Phi());
  Y0 = Z0 * TMath::Tan(dir.Theta()) * TMath::Sin(dir.Phi());
  // cout<<"-I- NumberOfBounces: tan th = "<<TMath::Tan(dir.Theta())<<", sin ph = "<<TMath::Sin(dir.Phi())<<", cos ph = "<<TMath::Cos(dir.Phi())<<endl;
  // cout<<"-I- NumberOfBounces: X0 = "<<X0<<", Y0 = "<<Y0<<", Z0 = "<<Z0<<endl;

  // Find the start position of the photon with respect to the middle of the bar:
  TVector3 startLocal = fGeoH->MasterToLocalShortId(start, barId); // point
  // cout<<"-I- NumberOfBounces: Xen = "<<startLocal.X() + fBarWidth/2.<<", Yen = "<<startLocal.Y() + fHThick<<endl;

  // Find the number of bounces in each direction
  Double_t N1, N2;
  FindOutPoint(X0, startLocal.X() + fBarWidth / 2., fBarWidth, &N1, 0);
  FindOutPoint(Y0, startLocal.Y() + fHThick, 2. * fHThick, &N2, 0);
  // cout<<"-I- NumberOfBounces: N1 = "<<N1<<", N2 = "<<N2<<endl;

  return (Int_t)N1 + N2;
}

//----------------------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMapS::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t printt)
{
  Double_t m = 99.;
  Double_t n = TMath::Floor(x0 / a);
  m = n;
  if (printt) {
    std::cout << "n = " << n << ", NN = " << *NN << ", x0 = " << x0 << ", a = " << a << std::endl;
  }
  Double_t x1 = x0 - n * a;
  if (x0 < 0.) {
    x1 = x0 - (n + 1) * a;
  }
  if (printt) {
    std::cout << "xy = " << x1 << std::endl;
  }
  Double_t xK = 0.;
  if ((m / 2. - TMath::Floor(m / 2.)) == 0.) { // 4etnoe
    // if(print){std::cout<<"odd==0"<<std::endl;}
    if (x0 >= 0. && x1 + xEn <= a) {
      xK = x1 + xEn;
    }
    if (x0 >= 0. && x1 + xEn > a) {
      xK = 2 * a - x1 - xEn;
      n = 1. + n;
    }
    if (x0 < 0. && x1 + xEn >= 0.) {
      xK = a - (x1 + xEn);
      n = -1. - n;
    }
    if (x0 < 0. && x1 + xEn < 0.) {
      xK = a + x1 + xEn;
      n = -n;
    }
    if (printt) {
      std::cout << "xK = " << xK << ", n = " << n << std::endl;
    }
  }

  if ((m / 2. - TMath::Floor(m / 2.)) != 0.) { // ne4etnoe
    // if(print){std::cout<<"even!=0"<<std::endl;}
    if (x0 >= 0. && x1 + xEn <= a) {
      xK = a - (x1 + xEn);
    }
    if (x0 >= 0. && x1 + xEn > a) {
      xK = x1 + xEn - a;
      n = 1. + n;
    }
    if (x0 < 0. && x1 + xEn >= 0.) {
      xK = x1 + xEn;
      n = -1. - n;
    }
    if (x0 < 0. && x1 + xEn < 0.) {
      xK = -(x1 + xEn);
      n = -n;
    }
    if (printt) {
      std::cout << "xK = " << xK << ", n = " << n << std::endl;
    }
  }

  *NN = n;
  return xK;
}

//-------------------------------------------------------------------------------------------
void PndDrcRecoLookupMapS::DrawBarBox(TVector3 v1, TVector3 v2, TVector3 v3, TVector3 v4)
{
  Double_t xx[5];
  Double_t yy[5];
  xx[0] = v1.X();
  xx[1] = v2.X();
  xx[2] = v3.X();
  xx[3] = v4.X();
  xx[4] = v1.X();
  yy[0] = v1.Y();
  yy[1] = v2.Y();
  yy[2] = v3.Y();
  yy[3] = v4.Y();
  yy[4] = v1.Y();
  TPolyLine *p6 = new TPolyLine(5, xx, yy);
  p6->SetLineWidth(2);
  p6->Draw("same");
}

//--------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMapS::RecoAmbigTime(TVector3 kb, TVector3 start, Double_t *l, Bool_t printout)
{ // [ns]

  // group velocities for oil and quartz for Noil and Nquartz assuming that mean lambda = 410 nm:
  // Double_t nOil = fGeo->nEV();
  Double_t u_oil = 19.8; // 30./nOil;
  // Double_t u_quartz = 30./1.46907; // for lambda = 410 nm
  // Double_t u_quartz = 30./1.47012; // for lambda = 400 nm
  // Double_t u_quartz = 30./1.46838; // for lambda = 417 nm
  // Double_t u_quartz = 30./1.47125; // for lambda = 390 nm
  // Double_t u_quartz = 30./1.47248; // for lambda = 380 nm
  Double_t u_quartz = 20.; // 30./fNquartz;//1.47805; // for lambda = 400 nm

  // kb - photon momentum at the production point, first calculate path in QUARTZ BAR:
  // assume that there are only DIRECT photons!!!

  // path in quartz:
  Double_t Z0 = 0.;
  Double_t kz = 0.;
  if (kb.Z() < 0.) {
    Z0 = start.Z() - fGeo->barBoxZUp();
    kz = kb.Z();
  }
  if (kb.Z() >= 0.) {
    Z0 = 2. * fGeo->barBoxZDown() - fGeo->barBoxZUp() - start.Z();
    kz = -kb.Z();
  }

  if (printout) {
    LOG(info) << " Reco ambig Time: Zstart = " << start.Z() << ", Z0 = " << Z0;
  }

  Double_t L0 = Z0 / fabs(cos(kb.Theta()));
  kb.SetZ(kz);
  Double_t thetaOil = asin(fGeo->nQuartz() / fGeo->nEV() * sin(kb.Theta()));
  if (printout) {
    cout << "RECO ambig TIME: thetas are: quartz = " << kb.Theta() / fpi * 180. << ", oil = " << thetaOil / fpi * 180. << endl;
  }
  kb.SetTheta(thetaOil);
  Double_t Z1 = fGeo->EVlen();
  Double_t L1 = Z1 / fabs(cos(kb.Theta())); // path in oil

  if (printout) {
    LOG(info) << " Reco ambig Time: Zev = " << fGeo->EVlen();
    LOG(info) << " Reco ambig Time: L0 = " << L0 << ", L1 = " << L1;
    LOG(info) << " Reco ambig Time: time is " << L0 / u_quartz + L1 / u_oil; // 19.5 cm/ns - average (lambda) group speed
  }

  *l = L0 + L1;

  return L0 / u_quartz + L1 / u_oil; //[ns]
}
//---------------------------------------------------------------------------
void PndDrcRecoLookupMapS::CreateHisto() {}

//------------------Write to File----------------------------------------------
void PndDrcRecoLookupMapS::WriteToFile() {}

// -----   Finish Task   ---------------------------------------------------
void PndDrcRecoLookupMapS::Finish()
{
  LOG(info) << " PndDrcRecoLookupMapS: Finish";
  WriteToFile();
  DrawHisto();
}

//----------------------------------------------------------------
void PndDrcRecoLookupMapS::DrawHisto() {}

// -------------------------------------------------------------------------
ClassImp(PndDrcRecoLookupMapS)
