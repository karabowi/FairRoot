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

#ifndef PNDRICH_H
#define PNDRICH_H

#include "FairDetector.h"
#include "PndGeoHandling.h"
#include "PndRichGeo.h"

#include "TVector3.h"
#include "TLorentzVector.h"

class PndRichPDPoint;
class PndRichBarPoint;
class FairVolume;
class TClonesArray;

class PndRich : public FairDetector {

 public:
  /**      Name :  Detector Name
   *       Active: kTRUE for active detectors (ProcessHits() will be called)
   *               kFALSE for inactive detectors
   */
  PndRich(const char *Name, Bool_t Active);

  /**      default constructor    */
  PndRich();

  /**       destructor     */
  virtual ~PndRich();

  /**      Initialization of the detector is done here    */
  virtual void Initialize();

  /**       this method is called for each step during simulation
   *       (see FairMCApplication::Stepping())
   */
  virtual Bool_t ProcessHits(FairVolume *v = 0);

  /**       Registers the produced collections in FAIRRootManager.     */
  virtual void Register();

  /** Gets the produced collections */
  virtual TClonesArray *GetCollection(Int_t iColl) const;

  /**      has to be called after each event to reset the containers      */
  virtual void Reset();

  /**      Create the detector geometry        */
  void ConstructGeometry();
  void ConstructOpGeometry();

  void SetRunCherenkov(Bool_t const &ch) { fRunCherenkov = ch; };

  void SetGeoVersion(UInt_t const &version) { fGeoVersion = version; };

  void UseProtection(Bool_t const &up) { fUseProtection = up; };

  std::vector<std::string> fListOfSensitives;
  bool CheckIfSensitive(std::string name);

  Double_t lhcbaerindex(Double_t n400, Double_t wl);

  /**      This method is an example of how to add your own point
   *       of type PndRichPoint to the clones array
   */
  PndRichPDPoint *AddPDPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Double_t eLoss, UInt_t EventId = 0);
  PndRichBarPoint *AddBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom, Double_t time, Double_t length, Int_t pdgCode, Double_t thetaC, Int_t eventID, Double_t mass,
                               TVector3 pos0, TVector3 mom0);

  /** The following methods can be implemented if you need to make
   *  any optional action in your detector during the transport.
   */

  virtual void CopyClones(TClonesArray *, TClonesArray *, Int_t) { ; } // cl1 cl2  offset //[R.K.03/2017] unused variable(s)
  virtual void SetSpecialPhysicsCuts() { ; }
  virtual void EndOfEvent();
  virtual void FinishPrimary() { ; }
  virtual void FinishRun();
  virtual void BeginPrimary() { ; }
  virtual void PostTrack() { ; }
  virtual void PreTrack() { ; }
  virtual void BeginEvent();

 private:
  /** Track information to be stored until the track leaves the
  active volume.
  */
  Int_t fTrackID;      //!  track index
  Int_t fVolumeID;     //!  volume id
  TLorentzVector fPos; //!  position at entrance
  TLorentzVector fMom; //!  momentum at entrance
  Double_t fTime;      //!  time
  Double_t fLength;    //!  length
  Double_t fELoss;     //!  energy loss

  PndRichGeo *fGeo;
  PndGeoHandling *fGeoH; //! ///< converter for detector names

  Bool_t fUseProtection;
  Bool_t fRunCherenkov;
  UInt_t fGeoVersion;

  /** container for data points */

  TClonesArray *fPndRichPDPointCollection;
  TClonesArray *fPndRichBarPointCollection;

  PndRich(const PndRich &);
  PndRich &operator=(const PndRich &);

  void DefGeoVersion();

  std::vector<Double_t> fWlPhoton;
  std::vector<Double_t> fPDE;

  std::map<Int_t, Int_t> trackid;

  Double_t fnOpt;
  Double_t fZabar;

  // TGeoElement *C;
  // TGeoElement *H;
  // TGeoElement *B;
  // TGeoIsotope *B10;
  // TGeoIsotope *B11;
  // TGeoMaterial *matRcihProt;
  // TGeoMedium *med;

  ClassDef(PndRich, 1)
};

#endif // PNDRICH_H
