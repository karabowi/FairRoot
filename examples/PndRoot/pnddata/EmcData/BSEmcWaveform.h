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

//==========================================================================
//	BSEmcWaveform.h
//
// 	Class to hold waveforms created from Emc Hits
//
//	Hits will be converted to waveforms using the standard
//	formula for an exponential decay convoluted with CR-RC (or CR-2RC)
//	shaping.
//
// Public functions:
//
//      AddElecNoise(Double_t)
//                                      Adds gaussian noise with width
//                                      given by the Double_t parameter.
//
//      Digitise(Double_t)                Simple digitisation, given
//                                      the value of one bit in energy
//                                      equivalent units.
//
//     AddElecNoiseAndDigitse(Double_t, Double_t)  Do both
//
//     AddShapedElecNoiseAndDigitse(Double_t, Double_t)  Do both, but with
//            noise tyhat is not just plain gaussian, but passed through the
//            shaping.  Only use for detailed studies- this is like
//            adding a Hit to *every bin* and is very slow if you are doing
//            a calorimiter's worth.
//
//     GetScale()  Return the maximum peak of a 1 GeV hit.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// 	P.D.Strother 	Imperial College
// Dima Melnichuk - adaption for PANDA
//=======================================================================
//#pragma once
#ifndef BSEMCWAVEFORM_HH
#define BSEMCWAVEFORM_HH

#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TGraphErrors.h"
#include "TObject.h"

#include "FairTimeStamp.h"

#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TGraphErrors;
class TMemberInspector;

/**
 * @brief represents a simulated waveform in an emc crystal
 * @author P.D.Strother 	Imperial College
 * @author Dima Melnichuk - adaption for PANDA
 * @ingroup EmcData
 */
class BSEmcWaveform : public FairTimeStamp {

  friend class BSEmcAbsWaveformModifier;

 public:
  BSEmcWaveform();
  BSEmcWaveform(Int_t t_trackId, long t_detId, const std::vector<Double_t> &t_signal, Int_t t_hitIndex = -1);
  virtual ~BSEmcWaveform();

  long GetDetectorId() const { return fDetectorId; }
  Int_t GetTrackId() const { return fTrackId; }
  Short_t GetModule() const { return (fDetectorId / 100000000); };
  virtual std::vector<Double_t> GetSignal() const { return fSignal; };
  virtual std::vector<Double_t> GetSignalError() const { return fSignalError; }

  Int_t GetHitIndex() const { return fHitIndex; }

  // Operators
  virtual Bool_t operator==(const BSEmcWaveform &t_otherWave) const;
  virtual Bool_t operator<(const BSEmcWaveform &t_otherWave) const;
  virtual Bool_t operator!=(const BSEmcWaveform &t_otherWave) const;
  virtual Bool_t equal(FairTimeStamp *t_data);
  BSEmcWaveform &operator+=(const BSEmcWaveform &t_otherWave);

  Double_t GetSampleRate() const { return fSampleRate; } // in s^(-1)

  Int_t GetWaveformLength() const { return fWaveformLength; }

  // Modifiers
  void SetSampleRate(Double_t t_rate) { fSampleRate = t_rate; }; // in s^(-1)

  void SetWaveform(std::vector<Double_t> &t_signal, Int_t t_length);
  Double_t Max();

  virtual void clearAndReset();
  virtual void Clear(Option_t * /*unused*/) { fSignal.clear(); };

  Double_t GetActiveTime() const { return GetTimeStamp() + (fWaveformLength - 1) / fSampleRate * 1.0e9; } // nano seconds
  Int_t GetPileupCount() const { return fEvt.size() - 1; }

  void AddEvt(Int_t t_evtNo) { fEvt.push_back(t_evtNo); }
  const std::vector<Int_t> &GetEvtList() const { return fEvt; }

  TGraphErrors *ToTGraph() const;

  Double_t GetBaseline() const { return fBaselineValue; }
  Double_t Integral() const;

  void SetDetectorId(UInt_t t_detId) { fDetectorId = t_detId; }
  void SetOverlapTime(const Double_t t_overlap) { fOverlapTime = t_overlap; }

 protected:
  Int_t fTrackId{-1};
  Int_t fDetectorId{-1};
  Int_t fWaveformLength{-1};
  std::vector<Double_t> fSignal{};      // Signal after FADC
  std::vector<Double_t> fSignalError{}; // Signal after FADC
  Int_t fHitIndex{-1};

  Double_t fSampleRate{-1}; // in s^(-1)
  Double_t fBaselineValue{-1};

  std::vector<Int_t> fEvt{}; // combined waveforms from which events, for check.
  Double_t fOverlapTime{};

  ClassDef(BSEmcWaveform, 7)
};
#endif /*BSEMCWAVEFORM_HH*/
