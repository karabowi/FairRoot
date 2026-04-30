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
// -----                 HypStatDecay header file                  -----
// -----               Created 30/10/08  by A.Sanchez                 -----
// -------------------------------------------------------------------------

/** HypStatDecay.h
 *@author A.Sanchez <A.Sanchez@gsi.de>
 *
 The HypStatDecay call the Statistical decay subrutines(fortran format)
 for the production of hypernuclei(Fermi Break-Up)
 and calculates the electromagnetic decay of excited hyperfragments.

**/

#ifndef HYPSTATDECAY_HH
#define HYPSTATDECAY_HH

//#include "THParticle.h"
#include <vector>
#include "TRandom.h"

class TClonesArray;
class TFile;
class TTree;

class HypStatDecay {

 public:
  /** Default constructor (should not be used) **/
  HypStatDecay();

  /** Standard constructor
   ** @param fileName The input file name
   **/
  HypStatDecay(const Char_t *fileName);

  /** Destructor **/
  virtual ~HypStatDecay();

  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   **
   **/
  void GetFragment(Int_t primGen); //! search fragment in event
  void GetData(Int_t tr, TVector3 &p, Int_t &pid, Double_t &mass);
  void GetAZH(Int_t ion, Int_t &AI, Int_t &ZI, Int_t &L);

  /** Accessors **/
  Int_t GetNtr() const { return fNtr; } //! return multiplicity
  // TClonesArray* GetFrag() const { return fFrag; }

  /** Modifiers **/
  void SetNtr(Int_t t) { fNtr = t; };
  // void SetFrag(TClonesArray* frag)          { fFrag    = frag; };

 private:
  Int_t iEvent;
  Int_t fev;               //! Event number
  const Char_t *fFileName; //! Input file name

  double she;
  int an, zn, hn;

  Int_t fNtr;
  std::vector<int> fPid;
  TVector3 fMom;
  TLorentzVector v, vN;
  TVector3 tvL, tNvL;

  std::vector<double> fPx;
  std::vector<double> fPy;
  std::vector<double> fPz;
  // Double fPhx[1000],fPhy[1000],fPhz[1000];
  std::vector<double> fMass;
  std::vector<double> fEx;
  std::vector<int> A;
  std::vector<int> Z;
  std::vector<int> H;

  TLorentzVector GetPgCMSLab(float mass, float &Delta, TVector3 &PL);
  TLorentzVector GetPNuCMSLab(float mass, float &Delta, TVector3 &PL);
  Double_t GetEtot(Float_t mass, TVector3 P);

  /** Private method CloseInput. Just for convenience. Closes the
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();

  ClassDef(HypStatDecay, 2);
};

#endif
