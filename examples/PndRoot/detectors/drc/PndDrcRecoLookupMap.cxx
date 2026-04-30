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
// -----                   PndDrcRecoLookupMap source file                -----
// -----               Created 10/11/10  by Maria Patsyuk              -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndDrcRecoLookupMap.h"
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
PndDrcRecoLookupMap::PndDrcRecoLookupMap() : FairTask("PndDrcRecoLookupMap")
{
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fDigiPar = nullptr;
}
// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcRecoLookupMap::PndDrcRecoLookupMap(Int_t verbose) : FairTask("PndDrcRecoLookupMap")
{
  fVerbose = verbose;
  fGeo = new PndGeoDrc();
  fGeoH = nullptr;
  fDigiPar = nullptr;
}
// -----   Destructor   ----------------------------------------------------
PndDrcRecoLookupMap::~PndDrcRecoLookupMap()
{
  if (fGeo)
    delete fGeo;
  if (fGeoH)
    delete fGeoH;
  fHistoList->Delete();
  delete fHistoList;
}

// -----   Initialization   -----------------------------------------------
InitStatus PndDrcRecoLookupMap::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndDrcRecoLookupMap::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fMCArray = (TClonesArray *)ioman->GetObject("MCTrack");
  if (!fMCArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No MCTrack array!" << endl;
    return kERROR;
  }
  // Get input array
  fBarPointArray = (TClonesArray *)ioman->GetObject("DrcBarPoint");
  if (!fBarPointArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }
  // Get Photon point array
  fPDPointArray = (TClonesArray *)ioman->GetObject("DrcPDPoint");
  if (!fPDPointArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No DrcPDPoint array!" << endl;
    return kERROR;
  }
  /*  // Get input array
    fHitArray = (TClonesArray*) ioman->GetObject("DrcHit");
    if ( ! fHitArray ) {
      cout << "-W- PndDrcRecoLookupMap::Init: "
           << "No DrcHit array!" << endl;
     return kERROR;
     }
  */

  // Get digi array
  fDigiArray = (TClonesArray *)ioman->GetObject("DrcDigi");
  if (!fDigiArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
         << "No DrcDigi array!" << endl;
    return kERROR;
  }

  // Get input array
  fPDHitArray = (TClonesArray *)ioman->GetObject("DrcPDHit");
  if (!fPDHitArray) {
    cout << "-W- PndDrcRecoLookupMap::Init: "
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

  // Time resolution as a function of photon path
  timeres = new TF1("timeres", "[0] + [1]*x", 0., 1000.);
  timeres->SetParameters(23.2773, 1.11008);

  LOG(info) << " PndDrcRecoLookupMap: Intialization successfull";
  CreateHisto();
  return kSUCCESS;
}

// -----   Private method SetParContainers   -------------------------------
void PndDrcRecoLookupMap::SetParContainers()
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
  LOG(info) << " PndDrcRecoLookupMap::SetParContainers(). read parameters";
  cout << "read them!" << endl;
  // cout << "-I- PndDrcRecoLookupMap: Number of hit pixels "<< fDigiPar->GetNHitPixels()<< endl;
  LOG(info) << " PndDrcRecoLookupMap: Number of hit pixels " << fDigiPar->GetNAmbiguities();

  if (fGeoH == nullptr)
    fGeoH = PndGeoHandling::Instance();
  fGeoH->SetParContainers();
  if (fVerbose > 1)
    Info("SetParContainers", "done.");
  return;
}
// -------------------------------------------------------------------------

// -----   Execution of Task   ---------------------------------------------
void PndDrcRecoLookupMap::Exec(Option_t *ion)
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
void PndDrcRecoLookupMap::ProcessPhotonMC()
{
  if (fVerbose > 0) {
    cout << "PndDrcRecoLookupMap: Number of Detected MC Tracks : " << fMCArray->GetEntries() << endl;
    PndMCTrack *ptr = nullptr;
    PndMCTrack *trMr = nullptr;
  }
}
//--------------Process Bar Hits----------------------------------------------------
void PndDrcRecoLookupMap::ProcessBarHit()
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
void PndDrcRecoLookupMap::ProcessPhotonHit()
{
  // Int_t nofChPho = 0;
  PndDrcLutInfo lutinfo;

  PndDrcPDPoint *Ppt = nullptr;
  PndDrcPDHit *pdhit = nullptr;
  PndMCTrack *tr = nullptr;
  PndMCTrack *trMr;

  if (fVerbose > 0) {
    cout << "PndDrcRecoLookupMap: Number of Detected Photon MCPDPoints : " << fPDPointArray->GetEntries() << endl;
    cout << "PndDrcRecoLookupMap: Number of Detected Photon PDHits : " << fPDHitArray->GetEntries() << endl;
  }

  // Loop over PndDrcPDHits
  for (Int_t k = 0; k < fPDHitArray->GetEntriesFast(); k++) {

    pdhit = (PndDrcPDHit *)fPDHitArray->At(k);

    Int_t mcPDRef = pdhit->GetRefIndex();

    Ppt = (PndDrcPDPoint *)fPDPointArray->At(mcPDRef);

    PndDrcBarPoint *fBarPoint = (PndDrcBarPoint *)fBarPointArray->At(Ppt->GetBarPointID());
    fBarId = fBarPoint->GetDetectorID();
    // cout<<"bar name - "<<fGeoH->GetPath(fBarId)<<endl;

    Int_t trID = Ppt->GetTrackID();
    tr = (PndMCTrack *)fMCArray->At(trID);

    Int_t trMID = tr->GetMotherID();

    // if(trMID > -1){
    trMr = (PndMCTrack *)fMCArray->At(trMID);
    trMr->GetMomentum().Print();

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
      cout << "+++++++++++++++++++++++++++" << endl;
      cout << "CH expected = " << CHexp << endl;

      fxPHit = pdhit->GetX();
      fyPHit = pdhit->GetY();
      fzPHit = pdhit->GetZ();
      ftime = pdhit->GetTime();

      // get the pixel id
      fpixID = pdhit->GetDetectorID();
      cout << "hit id " << fpixID << ", x = " << pdhit->GetX() << ", y = " << pdhit->GetY() << ", time = " << ftime << endl;

      Double_t xPPoi = Ppt->GetX();
      Double_t yPPoi = Ppt->GetY();

      lutinfo.SetPath(Ppt->GetLength());

      // momentum of a photon on the PD Plane
      fPphoPD.SetXYZ(Ppt->GetPx(), Ppt->GetPy(), Ppt->GetPz());
      // cout<<"Photon momentum on the PDplane: "<<endl;
      // fPphoPD.Print();

      {
        // mother momentum/direction
        fPMo.SetXYZ(trMr->GetMomentum().X(), trMr->GetMomentum().Y(), trMr->GetMomentum().Z());
        // fPMo.Print();
        lutinfo.SetChPartDir(fPMo);
        cout << "mother phi = " << fPMo.Phi() / 3.1415 * 180. << endl;
        if (fB > 0.) {
          Double_t PtMo = sqrt(pow(trMr->GetMomentum().X(), 2) + pow(trMr->GetMomentum().Y(), 2));
          Double_t Rratio = fR * fR / 2. / pow((PtMo / 0.29979 / fB) * 100., 2);
          // Double_t phi_extra = fPMo.Phi() + trMpdg/fabs(trMpdg) * acos(1. - Rratio); // muon - ; proton +
          Double_t phi_extra = fPMo.Phi() + MoSign * trMpdg / fabs(trMpdg) * acos(1. - Rratio); // muon - ; proton +
          // fHAngleInBDeg = 0.5 * trMpdg/fabs(trMpdg) * (acos(1. - Rratio) /TMath::Pi())*180.;
          fHAngleInBDeg = 0.5 * MoSign * trMpdg / fabs(trMpdg) * (acos(1. - Rratio) / TMath::Pi()) * 180.;
          // cout<<"B = "<<fB<<", R = "<<fR<<", trMpdg = "<<trMpdg<<", half angle in B = "<<fHAngleInBDeg<<endl;
          fPMo.SetPhi(phi_extra);
        }
        lutinfo.SetChPartDirInBar2(fPMo);
      }

      {
        fBarPoint->Momentum(fPMo);
        lutinfo.SetChPartDirInBar(fPMo);
      }

      cout << "Mother vector (global cs) : " << endl;
      TVector3 motherMom = fPMo.Unit();
      motherMom.Print();

      // initial momentum of the photon
      fPphoInit.SetXYZ(tr->GetMomentum().X(), tr->GetMomentum().Y(), tr->GetMomentum().Z());
      cout << "Initial momentum of the photon :" << endl;
      fPphoInit.Print();

      // real = generated cherenkov angle
      CHreal = fPphoInit.Angle(fPMo);
      lutinfo.SetCherenkovReal(CHreal);
      // cout<<"CH real (generated) = "<<CHreal<<endl;

      Double_t etot = sqrt(pow(fPphoPD.X(), 2) + pow(fPphoPD.Y(), 2) + pow(fPphoPD.Z(), 2));
      flambdah = 197.0 * 2.0 * TMath::Pi() / (etot * 1.0E9); // wavelength of photon in nm
      lutinfo.SetLambda(flambdah);

      // production point of the photon
      fStartVertex = tr->GetStartVertex();

      // cout<<"start: phi = "<<fStartVertex.Phi()<<endl;
      // fStartVertex.Print();
      ftime0 = tr->GetStartTime() / 1.0E9;
      lutinfo.SetTime(ftime - ftime0);

      // cout<<"fstart time = "<<ftime0<<endl;
      //$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      // proceed only if photon was born in the bar for which the LUT is used!!!
      // if(fStartVertex.Phi() >0./180.*fpi && fStartVertex.Phi() < 6./180.*fpi){
      if (fPphoInit.Z() > 0. || fPphoInit.Z() < 0.) { // reflected photons

        // Transformation of the photon initial direction to the local bar coodr system:
        // to get to the bar coord system.
        fPhiRot = InBarCoordSyst(fStartVertex, &fBBver1, &fBBver2, &fBBver3, &fBBver4);
        cout << "PhiRot = " << fPhiRot / fpi * 180. << ", Mother phi = " << fPMo.Phi() / fpi * 180. << endl;

        // Photon initial momentum in the bar coord. syst.
        fPphoB = fPphoInit.Unit();
        fPphoB.RotateZ(-fPhiRot);
        fkxBar = -fPphoB.Y();
        fkyBar = fPphoB.X();
        fkzBar = fPphoB.Z();
        fPphoB.SetXYZ(fkxBar, fkyBar, fkzBar);
        cout << "photon momentum in the bar = " << endl;
        fPphoB.Print();
        TVector3 fPpB = (fGeoH->MasterToLocalShortId(fPphoInit, fBarId) - fGeoH->MasterToLocalShortId((0., 0., 0.), fBarId)).Unit();
        fPpB.Print();

        // mother momentum in the bar' (and bar) coord syst:
        fPMoB = fPMo.Unit();
        fPMoB.RotateZ(-fPhiRot);
        cout << "Mother vector (bar' cs): " << endl;
        fPMoB.Print();
        fPxMoBar = -fPMoB.Y();
        fPyMoBar = fPMoB.X();
        fPzMoBar = fPMoB.Z();
        TVector3 PMoBar;
        PMoBar.SetXYZ(fPxMoBar, fPyMoBar, fPzMoBar);
        TVector3 PMBar = (fGeoH->MasterToLocalShortId(fPMo, fBarId) - fGeoH->MasterToLocalShortId((0., 0., 0.), fBarId)).Unit();
        PMBar.Print();

        // to find Cherenkov Phi (NOT THETA!!!):
        TVector3 kB;
        kB.SetXYZ(fkxBar, fkyBar, fkzBar);

        // use lookups to get the kXbar and kYbar of photons:
        NHitPix = fDigiPar->GetNHitPixels();
        // cout<<"N pixels = "<<NHitPix<<endl;
        NAmb = fDigiPar->GetNAmbiguities();
        cout << "N amb = " << NAmb << endl;
        NPixPar = fDigiPar->GetNPixelParam();
        Double_t par[NPixPar];

        if (fDigiPar->GetParamsForPixel(fpixID, par) == kFALSE) {
          cout << "NO SUCH HIT IN LUT!!!" << endl;
          continue;
        }

        cout << "LOOKUP!!!!" << endl;
        // cout<<"N Hit Pix = "<<NHitPix<<endl;
        // cout<<"N Amb = "<<NAmb<<endl;
        cout << "NPixPar = " << NPixPar << endl;
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
          fkBar.Print();

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
              CHreco.push_back(fkBar.Angle(PMoBar));
              CHdiff(8 * i + jamb) = fabs(CHreco[8 * i + jamb] - CHexp);
              Tamb.push_back(RecoAmbigTime(fkBar, fStartVertex, &fPath, 0));
              Path.push_back(fPath);
              Adiff(8 * i + jamb) = fkBar.Angle(fPphoB);
              // cout<<"reco ch angle = "<<fkBar.Angle(PMoBar)<<endl;
              // cout<<"number of bounces = "<<NumberOfBounces(fStartVertex, fPphoB, fBarId)<<endl;
              // cout<<"CHdiff = "<<CHdiff(8*i+jamb)<<", CHreco"<<8*i+jamb<<" = "<<CHreco[8*i+jamb]<<endl;

              // fill lutinfo only with credible information:
              lutinfo.AddAngle(fkBar.Angle(PMoBar));
              lutinfo.AddTime(RecoAmbigTime(fkBar, fStartVertex, &fPath, 0));
              lutinfo.AddPath(fPath);
              lutinfo.AddChDiff(CHreco[8 * i + jamb] - CHexp);
              // cout<<"fkBar = "<<endl;
              // fkBar.Print();
              lutinfo.AddNOfBounces(NumberOfBounces(fStartVertex, fkBar /*fPphoB*/, fBarId));

              fkBarXHist->Fill(kX, fPphoB.X());
              fkBarYHist->Fill(kY, fPphoB.Y());
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

        // F I L L     T H E      H I S T O S
        fhPDHits->Fill(xPPoi, yPPoi);   //(fxPHit, fyPHit);
        fhPDTime->Fill(ftime - ftime0); // substract production time of a photon

        fhRecoT1->Fill(flambdah, Ppt->GetLength() / (ftime - ftime0));

        fhCHreal->Fill(CHreal);
        fhLam->Fill(flambdah);

        if (fB > 0.) {
          fMapHist->Fill(fPMo.Theta() / fpi * 180., fPMo.Phi() / fpi * 180. - fHAngleInBDeg); // assuming that R = 50 cm
        }
        if (fB == 0.) {
          fMapHist->Fill(fPMo.Theta() / fpi * 180., fPMo.Phi() / fpi * 180.);
        }
        cout << "fMAPHIST is filled in theta=" << fPMo.Theta() / fpi * 180. << ", phi =" << fPMo.Phi() / fpi * 180. - fHAngleInBDeg << endl;
        cout << "mother phi angle = " << fPMo.Phi() / fpi * 180. << endl;
        cout << "mother phi in the name of histo = " << (Int_t)(fPMo.Phi() / fpi * 180. * 100.) << endl;
        //#############################################
        TString PhiT = Form("t%g_phi%d", fPMo.Theta() / fpi * 180., (Int_t)(fPMo.Phi() / fpi * 180. * 100.));
        TString PhiTCut = Form("t%g_phi%d_cut", fPMo.Theta() / fpi * 180., (Int_t)(fPMo.Phi() / fpi * 180. * 100.));
        TString PhiTWeight = Form("t%g_phi%d_weight", fPMo.Theta() / fpi * 180., (Int_t)(fPMo.Phi() / fpi * 180. * 100.));
        TString Nbo = Form("t%g_phi%d_nbo", fPMo.Theta() / fpi * 180., (Int_t)(fPMo.Phi() / fpi * 180. * 100.));

        Int_t currPhiTh = PhiThetaPoints.size();
        for (Int_t ihi = 0; ihi < PhiThetaPoints.size(); ihi++) {
          if (PhiT == PhiThetaPoints[ihi]->GetName()) {
            currPhiTh = ihi;
            break;
          }
        }
        if (currPhiTh == PhiThetaPoints.size()) {
          Int_t nbins = (Int_t)(2. * fWidth / 0.005);
          TH1F *newThetaPhiPoint = new TH1F(PhiT.Data(), "Cherenkov angle difference for theta", nbins, -fWidth, fWidth);
          PhiThetaPoints.push_back(newThetaPhiPoint);
          TH1F *newThetaPhiPointCut = new TH1F(PhiTCut.Data(), "Cherenkov angle difference for theta with time cut", nbins, -fWidth, fWidth);
          PhiThetaPointsCut.push_back(newThetaPhiPointCut);
          TH1F *newThetaPhiPointWeight = new TH1F(PhiTWeight.Data(), "Cherenkov angle difference for theta with time weights", nbins, -fWidth, fWidth);
          PhiThetaPointsWeight.push_back(newThetaPhiPointWeight);
          TH1F *newNbo = new TH1F(Nbo.Data(), "Number of bounces", 1000, 0., 500.);
          NboPoints.push_back(newNbo);
        }
        // PhiThetaPoints[currPhiTh]->Fill();
        //#############################################

        // fill N bounces for all photons:
        NboPoints[currPhiTh]->Fill(NumberOfBounces(fStartVertex, fPphoB, fBarId));
        fhNboLam->Fill(flambdah, NumberOfBounces(fStartVertex, fPphoB, fBarId));
        // fill all the ambiguities with weights:
        // WEIGHTS ARE BASED ON TIMING!!!!!
        for (Int_t j = 0; j < CHreco.size(); j++) {
          fWeight = exp(-pow(Tamb[j] - ftime + ftime0, 2.) / 2. / pow(timeres->Eval(Path[j]) / 1000., 2.));
          // cout<<"CHreco = "<<CHreco[j]<<", d time = "<<Tamb[j]-ftime + ftime0<<", weight = "<<fWeight<<endl;

          // using time cuts
          if (fabs(CHdiff(j)) < fWidth) {
            PhiThetaPoints[currPhiTh]->Fill(CHreco[j] - CHexp, 1.);
            // fhDiff->Fill(CHreco(j)-CHexp,1./fInvSum/CHdiff(j));
            fhDiff->Fill(CHreco[j] - CHexp, 1.);
            // fhDTime->Fill(Tamb[j] - ftime + ftime0);
            // cout<<"CHreco = "<<CHreco[j]<<", time = "<<Tamb[j]<<", path = "<<Path[j]<<endl;
            // cout<<"time resolution at this path = "<<timeres->Eval(Path[j])/1000.<<", d t = "<<Tamb[j] - ftime + ftime0<<endl;
            // cout<<"histo "<<PhiThetaPoints[currPhiTh]->GetName()<<" filled with"<<CHreco[j]-CHexp<<endl;
            // using time cuts
            if (fabs(Tamb[j] - ftime + ftime0) < fNSigma * timeres->Eval(Path[j]) / 1000.) {
              PhiThetaPointsCut[currPhiTh]->Fill(CHreco[j] - CHexp);
              // fhDTimeCut->Fill(Tamb[j] - ftime + ftime0);
            }
            // using weights based on time
            PhiThetaPointsWeight[currPhiTh]->Fill(CHreco[j] - CHexp, fWeight);
            // fhDTimeWeight->Fill(Tamb[j] - ftime + ftime0, fWeight);

            // cout<<"diff was filled with "<<CHreco(j)-CHexp<<endl;
          }
        }

        // new ((*fChPhoArray)[nofChPho++]) PndChPho(fxPHit,fyPHit,fzPHit,ftime,flambdah,     	//					fPphoB,fPMo,fPphoInit,fPphoPD);
        //} // if kxBar
      } // if start vertex lies within the right bar!
    }   // photon from primary particle

  } // photon hits
  new ((*fDrcLutInfoArray)[fDrcLutInfoArray->GetEntriesFast()]) PndDrcLutInfo(lutinfo);
}
//----------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMap::InBarCoordSyst(TVector3 start, TVector3 *v1, TVector3 *v2, TVector3 *v3, TVector3 *v4)
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
//--------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMap::SectorNum(Double_t xhit, Double_t yhit)
{

  // this function is used for photon hits to create LUTs, so that hits in front of target pipe area
  // are treated as additional small LUTs

  TVector3 hit;
  hit.SetXYZ(xhit, yhit, 0.);
  Double_t phi = hit.Phi() * 180. / fpi;
  if (phi < 0.) {
    phi = 360. + hit.Phi() * 180. / fpi;
  }
  if (phi >= 0. && phi < 90. - fPipehAngle) {
    // return 13.+TMath::Floor(phi/fDphi);
    return 1. + TMath::Floor((90. - fPipehAngle - phi) / fDphi);
  }
  if (phi >= 90. + fPipehAngle && phi < 270. - fPipehAngle) {
    // return TMath::Floor((phi-2.*fPipehAngle)/fDphi) - 3.;
    return 9. + TMath::Floor((270. - fPipehAngle - phi) / fDphi);
  }
  if (phi >= 270. + fPipehAngle) {
    // return TMath::Floor((phi-4.*fPipehAngle)/fDphi) - 3.;
    return 5. + TMath::Floor((360. - phi) / fDphi);
  }
  // small areas in front of the target pipe:
  if (phi >= 90. - fPipehAngle && phi < 90. + fPipehAngle) {
    return 17.;
  }
  if (phi >= 270. - fPipehAngle && phi < 270. - fPipehAngle) {
    return 18.;
  }
}

//------   Find Nubmer of Bounces     --------------------------------------
Int_t PndDrcRecoLookupMap::NumberOfBounces(TVector3 start, TVector3 dir, Int_t barId)
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
  TVector3 startLocal = fGeoH->MasterToLocalShortId(start, barId);
  // cout<<"-I- NumberOfBounces: Xen = "<<startLocal.X() + fBarWidth/2.<<", Yen = "<<startLocal.Y() + fHThick<<endl;

  // Find the number of bounces in each direction
  Double_t N1, N2;
  FindOutPoint(X0, startLocal.X() + fBarWidth / 2., fBarWidth, &N1, 0);
  FindOutPoint(Y0, startLocal.Y() + fHThick, 2. * fHThick, &N2, 0);
  // cout<<"-I- NumberOfBounces: N1 = "<<N1<<", N2 = "<<N2<<endl;

  return (Int_t)N1 + N2;
}

//----------------------------------------------------------------------------------------------------------
Double_t PndDrcRecoLookupMap::FindOutPoint(Double_t x0, Double_t xEn, Double_t a, Double_t *NN, Bool_t printt)
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
    if (print) {
      std::cout << "odd==0" << std::endl;
    }
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
    if (print) {
      std::cout << "even!=0" << std::endl;
    }
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
void PndDrcRecoLookupMap::DrawBarBox(TVector3 v1, TVector3 v2, TVector3 v3, TVector3 v4)
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
Double_t PndDrcRecoLookupMap::RecoAmbigTime(TVector3 kb, TVector3 start, Double_t *l, Bool_t printout)
{ // [ns]

  // group velocities for oil and quartz for Noil and Nquartz assuming that mean lambda = 410 nm:
  // Double_t nOil = fGeo->nEV();
  Double_t u_oil = 19.8; // 30./nOil;
  // Double_t u_quartz = 30./1.46907; // for lambda = 410 nm
  // Double_t u_quartz = 30./1.47012; // for lambda = 400 nm
  // Double_t u_quartz = 30./1.46838; // for lambda = 417 nm
  // Double_t u_quartz = 30./1.47125; // for lambda = 390 nm
  // Double_t u_quartz = 30./1.47248; // for lambda = 380 nm
  Double_t u_quartz = 19.8; // 30./fNquartz;//1.47805; // for lambda = 370 nm

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

void PndDrcRecoLookupMap::CreateHisto()
{
  // Histogram list
  fHistoList = new TList();

  fhPDTime = new TH1D("fhPDTime", "Time of photons in ns", 1000, 0, 100);
  fhRecoT1 = new TH2F("fhRecoT1", "Reconstructed group velocity using MC track path [ns]", 500, 200., 700., 1000, 0., 40.);
  fhPDHits = new TH2D("fhPDHits", "Photon hits on the PD plane", 2500, -250, 250, 2500, -250, 250);

  fhDiff = new TH1F("fhDiff", "Difference btw reco and expected Cherenkov Angle", 50, -fWidth, fWidth);

  fhCHreal = new TH1F("fhCHreal", "Generated cherenkov angle", 100, 0.7, 0.9);
  fhLam = new TH1F("fhLam", "Lambda of detected Cherenkov photons", 480, 200., 800.);

  fhNboLam = new TH2F("fhNboLam", "Nbo as a function of lambda", 600, 200., 800., 100, 0., 500.);

  fHistoList->Add(fhPDTime);
  fHistoList->Add(fhPDHits);
  fHistoList->Add(fhDiff);
  fHistoList->Add(fhCHreal);
  fHistoList->Add(fhLam);
  // also add temp histos
  fHistoList->Add(fhRecoT1);
  fHistoList->Add(fhNboLam);

  fPixelSize = 0.65;

  fMapHist = new TH2F("fMapHist", "Photon yield", 30, 2.5, 152.5, 20, 0.35, 20.35); // 3 bars 41,0.35,20.85); //25,0.5,25.5);
  fSigHist =
    new TH2F("fSigHist", "fSigHist", 30, 2.5, 152.5, 20, 0.35, 20.35); // 3 bars 41,0.35,20.85);//7 bars("fSigHist","fSigHist",30,2.5,152.5, 25,0.5,25.5);// 5 bars in bar box
  fCheHist = new TH2F("fCheHist", "fCheHist", 30, 2.5, 152.5, 20, 0.35, 20.35); // 3 bars 41,0.35,20.85); //("fCheHist","fCheHist",30,2.5,152.5, 25,0.5,25.5);
  fkBarXHist = new TH2F("fkBarXHist", "fkBarXHist", 200, -1., 1., 200, -1., 1.);
  fkBarYHist = new TH2F("fkBarYHist", "fkBarYHist", 200, -1., 1., 200, -1., 1.);
  fHistoList->Add(fMapHist);
  fHistoList->Add(fSigHist);
  fHistoList->Add(fCheHist);
  fHistoList->Add(fkBarXHist);
  fHistoList->Add(fkBarYHist);
}

//------------------Write to File----------------------------------------------
void PndDrcRecoLookupMap::WriteToFile()
{
  fOutputName.Append(":/");
  gDirectory->cd(fOutputName);
  // gDirectory->cd("13_t90_500ev_l0_oil_5_HIT.root:/");

  gDirectory->mkdir("diff");
  gDirectory->cd("diff");
  for (int i = 0; i < PhiThetaPoints.size(); i++) {
    PhiThetaPoints[i]->Write();
  }

  gDirectory->cd("../");
  gDirectory->mkdir("diffCut");
  gDirectory->cd("diffCut");
  for (int i = 0; i < PhiThetaPointsCut.size(); i++) {
    PhiThetaPointsCut[i]->Write();
  }

  gDirectory->cd("../");
  gDirectory->mkdir("diffWeight");
  gDirectory->cd("diffWeight");
  for (int i = 0; i < PhiThetaPointsWeight.size(); i++) {
    PhiThetaPointsWeight[i]->Write();
  }

  gDirectory->cd("../");
  gDirectory->mkdir("Nbounces");
  gDirectory->cd("Nbounces");
  for (int i = 0; i < NboPoints.size(); i++) {
    NboPoints[i]->Write();
  }
  gDirectory->cd("../");
  TIter next(fHistoList);
  while (TH1 *histo = ((TH1 *)next()))
    histo->Write();
}

// -----   Finish Task   ---------------------------------------------------
void PndDrcRecoLookupMap::Finish()
{
  LOG(info) << " PndDrcRecoLookupMap: Finish";

  // creating a map in every pixel
  Double_t sig = 0., chr = 0.;
  TString hnam, num1, num2, num3;
  Float_t xx, yy;
  for (int i = 0; i < PhiThetaPoints.size(); i++) {

    TF1 *fit = new TF1("fit", "gaus(0)+pol1(3)");
    fit->SetParameters(10., -0.001, 0.017, 1., 1.);
    PhiThetaPoints[i]->Fit("fit", "V", "");
    sig = fabs(fit->GetParameter(2));
    chr = fit->GetParameter(1);
    hnam = PhiThetaPoints[i]->GetName();
    // cout<<"hnam - "<<hnam<<endl;
    num1 = hnam;
    num1.Remove(0, 1);
    num1.Remove(num1.Index("_"), (num1.Length() - num1.Index("_")));
    xx = num1.Atof();
    // cout<<"t = "<<xx<<endl;
    num2 = hnam;
    num2.Remove(0, num2.Index("i") + 1);
    yy = num2.Atof() / 100. - fHAngleInBDeg;
    // cout<<"phi = "<<yy<<endl;
    cout << "i = " << i << endl;
    fSigHist->Fill(xx, yy, sig);
    cout << "SigHIST was filled with " << sig << " in phi = " << yy << ", theta - " << xx << endl;
    fCheHist->Fill(xx, yy, chr);
  }
  /*
    // fill diff map with low value to avoid the background color
    for(Int_t i=0; i<fMapHist->GetXaxis()->GetNbins(); i++){
      for(Int_t j=0; j<fMapHist->GetYaxis()->GetNbins(); j++){
        if(fMapHist->GetBinContent(i+1,j+1) == 0.){
          fMapHist->SetBinContent(i+1, j+1, -10.);
        }
      }
    }
  */
  WriteToFile();
  DrawHisto();
}

//----------------------------------------------------------------
void PndDrcRecoLookupMap::DrawHisto()
{
  gStyle->SetOptFit(0111);

  TCanvas *C1 = new TCanvas("C1", "Time", 500, 500);
  C1->Divide(1, 2);
  C1->cd(1);
  fhPDTime->Draw();
  C1->cd(2);
  fhRecoT1->Draw();

  TCanvas *C2 = new TCanvas("C2", "PD Plane", 500, 500);
  fhPDHits->SetMarkerStyle(20);
  fhPDHits->SetMarkerSize(0.25);
  // fhPDHits->SetMarkerColor(2);
  fhPDHits->Draw();

  TCanvas *C3 = new TCanvas("C3", "Diff", 500, 500);
  fhDiff->GetXaxis()->SetTitle("[rad]");
  fhDiff->SetLineWidth(3);
  fhDiff->Draw();

  // C A N V A S     W I T H      M A P S (lambda and NEntries of pixels)

  TCanvas *C8 = new TCanvas("C8", "Nentries map", 500, 500);
  SetPlotStyle();
  gStyle->SetOptStat(11);
  fMapHist->GetXaxis()->SetTitle("#Theta_{track} [degrees]");
  fMapHist->GetYaxis()->SetTitle("#phi_{track} [degrees]");
  fMapHist->GetZaxis()->SetTitle("nEntries");
  // fMapHist->Draw("LEGO2Z");
  fMapHist->Scale(0.05);
  fMapHist->Draw("COL2Z");
  // DrawDetectorLayout();
  // DrawBarBox(fBBver1,fBBver2,fBBver3,fBBver4);

  TCanvas *C9 = new TCanvas("C9", "Mean CHreco map", 500, 500);
  SetPlotStyle();
  gStyle->SetOptStat(11);
  fCheHist->GetXaxis()->SetTitle("#theta track angle [degrees]");
  fCheHist->GetYaxis()->SetTitle("#phi track angle [degrees]");
  fCheHist->GetZaxis()->SetTitle("<CHreco - CHexp> [rad]");
  // fCheHist->Draw("LEGO2Z");
  fCheHist->Draw("COL2Z");
  // DrawDetectorLayout();
  // DrawBarBox(fBBver1,fBBver2,fBBver3,fBBver4);

  TCanvas *C10 = new TCanvas("C10", "Sigma map", 500, 500);
  SetPlotStyle();
  gStyle->SetOptStat(11);
  // fSigHist->SetMinimum(-0.2);
  // fSigHist->SetMaximum(0.2);
  fSigHist->GetXaxis()->SetTitle("#theta track angle [degrees]");
  fSigHist->GetYaxis()->SetTitle("#phi track angle [degrees]");
  fSigHist->GetZaxis()->SetTitle("#sigma_{CHreco - CHexp}");
  fSigHist->Draw("COL2Z");
  // DrawDetectorLayout();
  // DrawBarBox(fBBver1,fBBver2,fBBver3,fBBver4);

  TCanvas *C11 = new TCanvas("C11", "kBar LUT & kBar gen", 500, 500);
  C11->Divide(2, 1);
  C11->cd(1);
  fkBarXHist->GetXaxis()->SetTitle("kxBar from LUT");
  fkBarXHist->GetYaxis()->SetTitle("kxBar generated");
  fkBarXHist->Draw();
  TLine *l1 = new TLine(1., 1., -1., -1.);
  TLine *l2 = new TLine(-1., 1., 1., -1.);
  l1->SetLineColor(8);
  l2->SetLineColor(8);
  l1->Draw("same");
  l2->Draw("same");
  C11->cd(2);
  fkBarYHist->GetXaxis()->SetTitle("kyBar from LUT");
  fkBarYHist->GetYaxis()->SetTitle("kyBar generated");
  fkBarYHist->Draw();
  l1->Draw("same");
  l2->Draw("same");

  TCanvas *C12 = new TCanvas("C12", "Lam & CHreal", 500, 500);
  C12->Divide(1, 2);
  C12->cd(1);
  fhCHreal->Draw();
  C12->cd(2);
  fhLam->Draw();
}

// -------------------------------------------------------------------------
ClassImp(PndDrcRecoLookupMap)
