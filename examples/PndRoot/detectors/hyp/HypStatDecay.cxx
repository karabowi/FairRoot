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
// -----                HypStatDecay source file                  -----
// -----          Created 10/06/08  by A. Sanchez           -----
// -------------------------------------------------------------------------

#include <iostream>
#include "TClonesArray.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "TTree.h"
//#include "TRandom.h"
#include "TVector3.h"
//#include "THParticle.h"
#include "HypStatDecay.h"

using namespace std;
#include "cfortran.h"
extern "C" {
extern struct {
  int n, AG[1000], ZG[1000], HG[1000];
  float PXs[1000], PYs[1000], PZs[1000], EP[1000], GM[1000];
} frag_;
}

extern "C" void initia_(double *she, int *an, int *zn, int *hn); // install DPM
extern "C" void razhyp_gen__(double *shell);                     // to generate events

// -----   Default constructor   ------------------------------------------
HypStatDecay::HypStatDecay()
{
  iEvent = fev = 0;
  // fMass = 0.;
  fNtr = 0;
  fMom.SetXYZ(0., 0., 0.);
  // fInputFile = nullptr;
  // fInputTree = nullptr;
  // fFrag = nullptr;
}
// ------------------------------------------------------------------------

// -----   Standard constructor   -----------------------------------------
HypStatDecay::HypStatDecay(const Char_t *fileName)
{
  iEvent = fev = 0;
  // fMass = 0.;
  fNtr = 0;
  fMom.SetXYZ(0., 0., 0.);
  fFileName = fileName;
  // fInputFile = new TFile(fFileName);
  //   fInputTree = (TTree*) fInputFile->Get("data");
  //   fParticles = new TClonesArray("THParticle",100);
  //   fInputTree->SetBranchAddress("Particles", &fParticles);
  she = 3.;
  if (strcmp(fFileName, "12C") == 0) {
    std::cout << " 12C + Xi- --> 13BLL " << std::endl;
    an = 13;
    zn = 5;
    hn = 2;
  } else if (strcmp(fFileName, "13C") == 0) {
    std::cout << " 13C + Xi- --> 14BLL " << std::endl;
    an = 14;
    zn = 5;
    hn = 2;
  } else if (strcmp(fFileName, "10B") == 0) {
    std::cout << " 10B + Xi- --> 11BeLL " << std::endl;
    an = 11;
    zn = 4;
    hn = 2;
  } else if (strcmp(fFileName, "11B") == 0) {
    std::cout << " 11B + Xi- --> 12BeLL " << std::endl;
    an = 12;
    zn = 4;
    hn = 2;
  } else if (strcmp(fFileName, "9Be") == 0) {
    std::cout << " 9Be + Xi- --> 10LiLL " << std::endl;
    an = 10;
    zn = 3;
    hn = 2;
  }

  initia_(&she, &an, &zn, &hn);

  //
}
// ------------------------------------------------------------------------

// -----   Destructor   ---------------------------------------------------
HypStatDecay::~HypStatDecay()
{
  CloseInput();
}
// ------------------------------------------------------------------------

// -----   Public method ReadEvent   --------------------------------------
void HypStatDecay::GetFragment(Int_t) // primGen //[R.K.03/2017] unused variable(s)
{
  fPx.clear();
  fPy.clear();
  fPz.clear();
  A.clear();
  Z.clear();
  H.clear();
  fMass.clear();
  fEx.clear();
  fPid.clear();

  razhyp_gen__(&she);
  int npart;
  npart = frag_.n;
  cout << " npart razhyp " << npart << endl;

  // SetFrag(fFrag);
  // return kTRUE;

  for (int i = 0; i < frag_.n; ++i) {

    if (frag_.EP[i] > 0.) { // gamma emission

      //----photon emitted from excited hypfragment

      tvL.SetXYZ(frag_.PXs[i], frag_.PYs[i], frag_.PZs[i]);

      v = GetPgCMSLab((frag_.GM[i]) * 1000, frag_.EP[i], tvL);

      fPx.push_back(v.Px() / 1000);
      fPy.push_back(v.Py() / 1000);
      fPz.push_back(v.Pz() / 1000);
      fMass.push_back(0.);
      fPid.push_back(22);

      // cout<<" pid 22 "<<v.Px()/1000<<" "<<v.Py()/1000<<" "<<v.Pz()/1000<<endl;

      //----deexcited Hypfragment

      tNvL.SetXYZ(frag_.PXs[i], frag_.PYs[i], frag_.PZs[i]);
      vN = GetPNuCMSLab((frag_.GM[i]) * 1000, frag_.EP[i], tNvL);

      fPx.push_back(vN.Px() / 1000);
      fPy.push_back(vN.Py() / 1000);
      fPz.push_back(vN.Pz() / 1000);
      fMass.push_back(frag_.GM[i]);

      // hypernucleus or ion
      // if((frag_.HG[i]>0&&frag_.ZG[i]>0&&frag_.AG[i]>0)||
      // (frag_.HG[i]==0 &&(frag_.ZG[i]>1)&&frag_.AG[i]>1))
      fPid.push_back(1000000000 + 10000000 * frag_.HG[i] + 10000 * frag_.ZG[i] + 10 * frag_.AG[i]);

      // cout<<" pid frag22 "<<vN.Px()/1000<<" "<<vN.Py()/1000<<" "<<vN.Pz()/1000<<endl;
    }

    if (frag_.EP[i] == 0.) {

      fPx.push_back(frag_.PXs[i] / 1000);
      fPy.push_back(frag_.PYs[i] / 1000);
      fPz.push_back(frag_.PZs[i] / 1000);
      fMass.push_back(frag_.GM[i]);

      // hypernucleus or ion
      if (frag_.HG[i] > 0 && frag_.ZG[i] > 0 && frag_.AG[i] > 0) {
        fPid.push_back(1000000000 + 10000000 * frag_.HG[i] + 10000 * frag_.ZG[i] + 10 * frag_.AG[i]);
      }

      if (frag_.HG[i] == 0 && frag_.ZG[i] > 0 && frag_.AG[i] > 0) {

        if (frag_.ZG[i] == 1 && frag_.AG[i] == 1)
          fPid.push_back(2212); // proton

        else
          fPid.push_back(1000000000 + 10000000 * frag_.HG[i] + 10000 * frag_.ZG[i] + 10 * frag_.AG[i]);
      }

      if (frag_.HG[i] == 0 && frag_.ZG[i] == 0 && frag_.AG[i] == 1) {

        fPid.push_back(2112); // neutron
      }

      //  if(frag_.HG[i]==0&&frag_.ZG[i]==1&&frag_.AG[i]==1){

      // 	fPid.push_back(2212);//proton
      //       }

      if (frag_.HG[i] == 1 && frag_.ZG[i] == 0 && frag_.AG[i] == 1) {

        fPid.push_back(3122); // lambda
      }

      // cout<<"pid fra "<<frag_.PXs[i]/1000<<" "<<frag_.PYs[i]/1000<<" "<<frag_.PZs[i]/1000<<endl;
    }
  }

  cout << " after gamma emision npart razhyp " << fPx.size() << endl;
  SetNtr(fPx.size()); // after electromagnetic decay(frag +photon)
}
void HypStatDecay::GetData(Int_t tr, TVector3 &p, Int_t &pid, Double_t &mass)
{

  pid = fPid[tr];

  Double_t px = fPx[tr];
  Double_t py = fPy[tr];
  Double_t pz = fPz[tr];

  p.SetXYZ(px, py, pz);
  mass = fMass[tr];

  //  Loop over particle in event
}
// ------------------------------------------------------------------------

void HypStatDecay::GetAZH(Int_t ion, Int_t &AI, Int_t &ZI, Int_t &L)
{
  // Int_t A,Z,L;

  if (ion > 1000000000 && (ion < 1010000000)) {
    ion -= 1000000000;
    ZI = ion / 10000;
    ion -= 10000 * ZI;
    AI = ion / 10;
    L = 0;
    cout << " ion charge " << ZI << " " << AI << " " << L << endl;
    // return Z;
  }
  if ((ion > 1010000000 || ion > 1020000000)) {
    ion -= 1000000000;
    L = ion / 10000000;
    ion -= 10000000 * L;
    ZI = ion / 10000;
    ion -= 10000 * ZI;
    AI = ion / 10;
    cout << L << " hypernuclei charge " << ZI << " " << AI << " " << L << endl;

    // return Z;
  }
  if (ion == 2212) {
    AI = 1;
    ZI = 1;
    L = 0;
  }
  if (ion == 2112) {
    AI = 1;
    ZI = 0;
    L = 0;
  }
  if (ion == 3122) {
    AI = 1;
    ZI = 0;
    L = 1;
  }
  if (ion == 22) {
    AI = 0;
    ZI = 0;
    L = 0;
  }
}

TLorentzVector HypStatDecay::GetPgCMSLab(float mass, float &Delta, TVector3 &PL)
{
  Double_t Mex, Pgam, Xgam, Ygam, Zgam, EL, PLmag; //, MC; //[R.K.03/2017] unused variable
  // Double_t theta,phi; //[R.K. 01/2017] unused variable

  TLorentzVector res, PNlab, resLab;
  // mASS OF EXCITED HYPERNUCLEUS
  // cout<<mass<<" "<<Delta<<endl;
  Mex = mass + Delta;
  // cout<<Mex<<" "<<Mex*Mex<<" "<<mass*mass<<endl;

  // MC = (Mex*Mex) - (mass*mass); //[R.K.03/2017] unused variable
  // cout<<Mex*Mex-mass*mass<<" "<<MC<<" "<<MC/(2*Mex)<<endl;
  // --pgam in CM after decay
  Pgam = ((Mex * Mex) - (mass * mass)) / (2 * Mex);

  // cout<<"pgamCM "<<Pgam<<" "<<endl;
  Xgam = Ygam = Zgam = 0.;

  // isotropically distributed
  gRandom->Sphere(Xgam, Ygam, Zgam, Pgam);
  // theta = acos(r->Uniform(cos(0.* TMath::DegToRad()),cos(180.* TMath::DegToRad())));
  // phi = r->Uniform(0,360) * TMath::DegToRad();

  /* Xgam = Pgam*TMath::Sin(theta)*TMath::Cos(phi);
  Ygam = Pgam*TMath::Sin(theta)*TMath::Sin(phi);
  Zgam = Pgam*TMath::Cos(theta);
  */
  // cout<<"pgamCM "<<Xgam<<" "<<Ygam<<" "<<Zgam<<" "<<endl;

  res.SetPxPyPzE(Xgam, Ygam, Zgam, Pgam);
  //-- hypernucleus impuls in LabS
  PLmag = PL.Mag2();

  EL = TMath::Sqrt(Mex * Mex + PLmag);
  PNlab.SetPxPyPzE(PL.X(), PL.Y(), PL.Z(), EL);
  //-- general boost in the Pl direction
  //--Pgam in the LabS.
  res.Boost(PL.X() / EL, PL.Y() / EL, PL.Z() / EL);
  // cout<<"particle 1"<<endl;

  // printf("%e %e  %e \n",PL.X()/EL,PL.Y()/EL,PL.Z()/EL);

  return res;
}

TLorentzVector HypStatDecay::GetPNuCMSLab(float mass, float &Delta, TVector3 &PL)
{
  Double_t Mex, Pgam, Xgam, Ygam, Zgam, EL, PLmag; //, MC; //[R.K.03/2017] unused variable
  TLorentzVector result, PNlab, resLab;
  // Double_t theta, phi; //[R.K.03/2017] unused variable
  // mASS OF EXCITED HYPERNUCLEUS
  // cout<<mass<<" "<<Delta<<endl;
  Mex = mass + Delta;
  // cout<<Mex<<" "<<Mex*Mex<<" "<<mass*mass<<endl;

  // MC = (Mex*Mex) - (mass*mass); //[R.K.03/2017] unused variable
  // cout<<Mex*Mex-mass*mass<<" "<<MC<<" "<<MC/(2*Mex)<<endl;
  // --pgam in CM after decay
  Pgam = ((Mex * Mex) - (mass * mass)) / (2 * Mex);

  // cout<<"pgamCM "<<Pgam<<" "<<endl;
  Xgam = Ygam = Zgam = 0.;
  // isotropically distributed
  gRandom->Sphere(Xgam, Ygam, Zgam, -Pgam);

  // theta = acos(rd.Uniform(cos(0.* TMath::DegToRad()),cos(180.* TMath::DegToRad()))); //[R.K.03/2017] unused variable
  // phi = rd.Uniform(0,360) * TMath::DegToRad(); //[R.K.03/2017] unused variable
  /*Xgam = -Pgam*TMath::Sin(theta)*TMath::Cos(phi);
 Ygam = -Pgam*TMath::Sin(theta)*TMath::Sin(phi);
 Zgam = -Pgam*TMath::Cos(theta);*/
  // cout<<"pgamCM "<<Xgam<<" "<<Ygam<<" "<<Zgam<<" "<<endl;

  result.SetPxPyPzE(Xgam, Ygam, Zgam, TMath::Sqrt((Pgam * Pgam) + (mass * mass)));
  //-- hypernucleus impuls in LabS
  PLmag = PL.Mag2();

  EL = TMath::Sqrt(Mex * Mex + PLmag);
  PNlab.SetPxPyPzE(PL.X(), PL.Y(), PL.Z(), EL);
  //-- general boost in the Pl direction
  //--Pgam in the LabS.
  result.Boost(PL.X() / EL, PL.Y() / EL, PL.Z() / EL);
  // cout<<"particle 1"<<endl;

  return result;
}

Double_t HypStatDecay::GetEtot(Float_t mass, TVector3 P)
{
  Double_t E;
  E = TMath::Sqrt(TMath::Power(mass, 2) + TMath::Power(P.Mag() / 1000., 2));

  return E;
}

// -----   Private method CloseInput   ------------------------------------
void HypStatDecay::CloseInput()
{
  //  if ( fInputFile ) {
  //     cout << "-I HypStatDecay: Closing input file " << fFileName
  // 	 << endl;
  //     fInputFile->Close();
  //     delete fInputFile;
  //   }
  //   fInputFile = nullptr;
}
// ------------------------------------------------------------------------

ClassImp(HypStatDecay)
