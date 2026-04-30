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

//----------------------------------------------------------------------
// File and Version Information:
//      $Id: Exp $
//
// Description:
//      Class PndMdtPointsToWaveform. Module to take the point list for the
//      mdt induced current.
//
//
// Author List:
//  Jifeng Hu, hu@to.infn.it, Torino University
//----------------------------------------------------------------------
//#pragma once
#ifndef PndMdtPointsToWaveform_H
#define PndMdtPointsToWaveform_H

#include <PndPersistencyTask.h>
#include "PndMdtWaveform.h"
#include "TVector3.h"
#include <TFile.h>
#include <TTree.h>

class TClonesArray;
// class PndMdtDigiPar;
// class PndMdtGeoPar;
class PndMdtWaveformWriteoutBuffer;
class PndMdtParamDigi;
class PndMdtIGeometry;

class PndMdtPointsToWaveform : public PndPersistencyTask {

 public:
  // Constructors

  PndMdtPointsToWaveform(Int_t verbose = 0, Bool_t storewaves = kTRUE);

  // Destructor

  virtual ~PndMdtPointsToWaveform();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetStorageOfData(Bool_t val); // Method to specify whether waveforms are stored or not.

  void RunTimeBased() { fTimeOrderedWaveform = kTRUE; }

  void FinishTask();

 private:
  TClonesArray *fMcTrackArray;

  /** Input array of PndMdtHits **/
  TClonesArray *fPointArray;

  /* output array for general sim */
  TClonesArray *fWaveformArray;

  /* output array for time-based sim */
  Bool_t fTimeOrderedWaveform;
  PndMdtWaveformWriteoutBuffer *fDataBuffer;

  PndMdtParamDigi *fParamDigiModel;
  PndMdtIGeometry *fGeoIF;

  //    PndMdtDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  //    PndMdtGeoPar*     fGeoPar;       /** Geometry parameter container **/
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void exec_t();
  virtual void exec_e();
  Int_t PdgToIndex(Int_t pdg);

  /** Verbosity level **/
  Int_t fVerbose;
  // counters for task
  Int_t HowManyPoint;
  Int_t nWaveformProduced;

  PndMdtPointsToWaveform(const PndMdtPointsToWaveform &L);
  PndMdtPointsToWaveform &operator=(const PndMdtPointsToWaveform &);

  Bool_t Digitize(PndMdtWaveform *theWf, Double_t &time, Double_t &amp, Bool_t isWire);

  struct key {
    key() {}
    key(Int_t _tid, Int_t _detid) : TrkID(_tid), DetID(_detid) {}
    bool operator<(const key &rhs) const
    {
      if (TrkID < rhs.TrkID)
        return true;
      if (TrkID == rhs.TrkID)
        return DetID < rhs.DetID;
      return false;
    }
    bool operator==(const key &rhs) const { return TrkID == rhs.TrkID && DetID == rhs.DetID; }
    Int_t TrkID; // track id
    Int_t DetID; // detector id
  };

  TFile *fFile;
  TTree *tTree;
  Double_t fWirpT;
  Double_t fStripT;
  Double_t fEvtT;
  Double_t fLength;
  Double_t fDis;
  Int_t fMod;
  Int_t fPid;

  ClassDef(PndMdtPointsToWaveform, 2);
};

#endif
