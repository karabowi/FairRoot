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
// -----                     CbmHyp header file                        -----
// -----               created by A. Sanchez                  -----
// -------------------------------------------------------------------------

#ifndef PNDHYP_H
#define PNDHYP_H

#include "TClonesArray.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairDetector.h"
#include "PndGeoHypPar.h"

#include "TRandom.h"

#include "PndHypGeoHandling.h"
//#include "PndHypDecayer.h"
#include "HypStatDecay.h"
using namespace std;

class TClonesArray;
class PndHypPoint;
class PndHypSecTarPoint;
// class PndHypSTMatBudPoint;
class FairVolume;

class PndHyp : public FairDetector {

 public:
  /** Default constructor **/
  PndHyp();

  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  PndHyp(const char *name, Bool_t active);

  /** Destructor **/
  virtual ~PndHyp();

  /** Virtual method Initialize
   ** Initialises detector. Stores volume IDs for MUO detector and mirror.
   **/
  virtual void Initialize();

  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates PndHypPoints and PndHypMirrorPoints and adds
   ** them to the collections.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume *vol = 0);

  virtual void SetSpecialPhysicsCuts();
  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();

  virtual void BeginEvent();
  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();

  /** Accessor to the hit collection **/
  virtual TClonesArray *GetCollection(Int_t iColl) const;

  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print() const;

  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();

  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray *cl1, TClonesArray *cl2, Int_t offset);

  /** Virtual method Construct geometry
   **
   **/
  virtual void ConstructGeometry();
  // void ConstructASCIIGeometry();
  virtual void FinishRun();

  PndHypPoint *AddHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout,
                      // TVector3 posInLocal,
                      // TVector3 posOutLocal,
                      Double_t tof, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout);

  PndHypPoint *AddSecTarHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout,
                            // TVector3 posInLocal,
                            // TVector3 posOutLocal,
                            Double_t tof, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout);

  PndHypPoint *AddSTMatBudHit(Int_t trackID, Int_t evtID, Int_t detID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout,
                              // TVector3 posInLocal,
                              // TVector3 posOutLocal,
                              Double_t tof, Double_t length, Double_t eLoss, Double_t charge, Double_t mass, Int_t pdgCode, Double_t dist, Double_t PLin, Double_t PLout);

  void PreTrack();

  void SetHypSDtoFile(bool onf, bool val)
  {
    fUseRAZHOption = onf; // **switch On/Off HypStatDecay**
    fUseFileOption = val;
  };

  void SetHypGamFEm(bool gam) { fUseGamOption = gam; }

  void SetMatbud(bool mbud) { fMatBud = mbud; }

  void SetTreeFName(const Char_t *Name) { fFileName = Name; };

  void SetSensorVol(TString VolSi) { fVolNamSi = VolSi.Data(); }
  void SetAbsorberVol(TString VolAb) { fVolNamAb = VolAb.Data(); }

  void SetStartEvID(Int_t EvID = 0)
  {
    fStartEvID = EvID;
    cout << " starting value for EvId " << fStartEvID << endl;
  }

  void SetGeoVersion(TString vers = "standard") { fVers = vers; }

  void SetAbsMat(TString abmat = "HYPdiamond") { fAbsMat = abmat; }

  void SetBPipeMat(TString bpmat = "HYPcarbon") { fBPipeMat = bpmat; }

  void SetListMat(TString mat = "carbon")
  {
    fListMat = kTRUE;
    fListOfMaterials.push_back(mat.Data());
  }

 private:
  std::vector<std::string> fListOfSensitives;
  std::vector<TString> fListOfMaterials;

  bool CheckIfSensitive(std::string name);

  TString fVolNamSi, fVolNamAb, fVers, fAbsMat, fSiMat, fBPipeMat;
  Bool_t fRootSensVol, fListMat, fStandard, fCurrent;
  Int_t fStartEvID;
  // PndGeoHypPar *par;
  Int_t fTrackID;         //  track index
  Int_t fVolumeID;        //  volume id
  Int_t fEventID;         //  event id
  TLorentzVector fPosIn;  //   position
  TLorentzVector fMomIn;  //  momentum
  TLorentzVector fPosOut; //  position
  TLorentzVector fMomOut; //  momentum
  /*  TLorentzVector fPosInLocal;        // entry position in module frame */
  /*   TLorentzVector fPosOutLocal;       //  exit position in module frame */
  Double_t fPLout, fPLin; //  total momentum

  TClonesArray *fEvt; //!
  TFile *fFile;       //!
  TTree *ft;          //!

  Int_t activeCnt;
  Double_t weight;
  Double_t seed;
  Int_t fcount;

  Double32_t fTime;   //   time
  Double32_t fLength; //   length
  Double32_t fELoss;  //   energy loss
  Int_t fPosIndex;    //
  Int_t fpdgCode;     //  MC volume ID of MUO
  Int_t SiId, CId, alId, beId, CpipeId;

  Double_t fcharge;
  Double_t fmass, fdist;

  const Char_t *fFileName; //! Input file name

  PndHypGeoHandling *fGeoH; //! Gives Access to the Path info of a hit

  // PndHypDecayer* fread;          //! Gives Access to the Statistical decay products
  // HypStatDecay* fread;          //! Gives Access to the Statistical decay products

  TClonesArray *fHypCollection;         //! Hit collection
  TClonesArray *fHypSecTarCollection;   // Hit collection(Absorver)
  TClonesArray *fHypSTMatBudCollection; // Hit collection(pipehyp)

  // reset all parameters
  void ResetParameters();
  Bool_t fTrackStopNxtStep;
  Bool_t fUseFileOption;
  Bool_t fUseRAZHOption;
  Bool_t fUseGamOption;
  Bool_t fMatBud;
  void SetHypStatDecay(bool cal, bool active);

  ClassDef(PndHyp, 15)
};

inline void PndHyp::ResetParameters()
{
  fTrackID = fVolumeID = fEventID = -1;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);
  /*  fPosInLocal.SetXYZM(0.0, 0.0, 0.0, 0.0); */
  /*   fPosOutLocal.SetXYZM(0.0, 0.0, 0.0, 0.0); */
  fTime = fLength = fELoss = 0;
};

#endif
