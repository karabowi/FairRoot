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

//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Disc DIRC Implementation for PandaRoot (Giessen)
//-------------------------------------------------------------------------

#ifndef PNDDISC_HH
#define PNDDISC_HH

// Subdetector specific
#include <PndDiscSensorMCPoint.h>
#include <PndDiscParticleMCPoint.h>

// ROOT
#include <Rtypes.h>
#include <TLorentzVector.h>

// FairRoot / PROOT
#include <FairDetector.h>

// cpp
#include <set>
#include <string>
#include <map>

class FairMCEventHeader;

class PndDisc : public FairDetector {
 public:
  // ctor / dtor
  PndDisc();
  PndDisc(const char *name, Bool_t active, Int_t det_id = 0);
  ~PndDisc();

  // interface - initialization
  virtual void Initialize(); //* detector initialization

  // interface - geo (FairModule)
  virtual void ConstructGeometry();
  virtual void ConstructOpGeometry();

  // interface - transport (FairDetector)
  virtual Bool_t ProcessHits(FairVolume *v = 0); //* called for each step during simulation (see FairMCApplication::Stepping())
  // virtual void   BeginPrimary();                      // define actions at the beginning of the primary track (-> TVirtualMCApplication)
  // virtual void   FinishPrimary();                     // define actions at the end of the primary track       (-> TVirtualMCApplication)
  virtual void BeginEvent(); // define actions at the beginning of the event         (-> TVirtualMCApplication)
  // virtual void   FinishEvent();                       // define actions at the end of the event               (-> TVirtualMCApplication)
  virtual void EndOfEvent(); //* called after FinishEvent() and after fRootManager->Fill() in FairMCApplication::FinishEvent (time to free the data in mem?)
  virtual void PreTrack();   // define actions at the beginning of each track        (-> TVirtualMCApplication)
  virtual void PostTrack();  // define actions at the end of each track              (-> TVirtualMCApplication)
  // virtual void   FinishRun();                         // called after sim all events (gMC->ProcessRun(int))   (-> FairMCApplication::FinishRun)
  // virtual void   SetSpecialPhysicsCuts();             //

  virtual bool CheckIfSensitive(std::string name); // Fairroot uses this function to query sensitive volumes

  // interface - collections
  virtual void Register();                                //* register the produced collections in FAIRRootManager
  virtual TClonesArray *GetCollection(Int_t iColl) const; //* return pointer to the produced collections **/
  virtual void Reset();                                   //* reset containers (abstract in FairDetector)
  // virtual void   CopyClones( TClonesArray* cl1,  TClonesArray* cl2 , Int_t offset);

  // --------------------------------
  // Detector specific functionality:
  // --------------------------------
  void StorePhotonTracks(Bool_t bval);
  void SetFilterInterval(Double_t const &wl_min_nm_, Double_t const &wl_max_nm_);

 private:
  Bool_t store_photon_tracks;
  Bool_t track_is_photon;
  TLorentzVector old_momentum; // cache the photon momentum.

  TClonesArray *clarr_sensor_hits;     //! hit on a photodetector surface
  TClonesArray *clarr_photon_tracks;   //! optical photon tracks
  TClonesArray *clarr_particle_tracks; //! tracks of other particles

  // keep track of the number of entries. Looking in Roots GetEntriesFast
  // makes you pulling your hair out.
  int nextid_clarr_sensor_hits;
  int nextid_clarr_photon_tracks;
  int nextid_clarr_particle_tracks;

  FairMCEventHeader *ev_header;                                                     //!
  std::map<int, std::pair<int, double>> last_track_occurence;                       //!
  std::set<std::string> names_of_sensitive_volumes;                                 //!
  std::map<int, double> internal_reflection_angle_of_photons;                       //!
  std::map<int, std::pair<TLorentzVector, TLorentzVector>> photons_entering_optics; //! photon track ids that have been

  int design_id;
  Double_t wl_min_nm;
  Double_t wl_max_nm;

  ClassDef(PndDisc, 1)
};

#endif // PNDDISC_HH
