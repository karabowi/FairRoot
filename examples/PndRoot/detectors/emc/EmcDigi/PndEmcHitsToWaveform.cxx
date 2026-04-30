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
//      $Id: //
// Description:
//      Class PndEmcHitsToWaveform. Module to take the hit list for the
//      calorimeter and make ADC waveforms from them.
//
//	 Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//      Phil Strother                  Original author
// 		 Dima Melnichuk - adaption for PANDA
// Copyright Information:
//      Copyright (C) 1996             Imperial College
//
//----------------------------------------------------------------------

#include "PndEmcHitsToWaveform.h"
#include "FairEventHeader.h"
#include "FairLogger.h"
#include "PndEmcHit.h"
#include "PndEmcWaveform.h"
#include "PndEmcAsicPulseshape.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcDigiPar.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDataTypes.h"
#include "TRandom.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "TGraph.h"
#include "TFile.h"
#include "TStopwatch.h"
#include "TROOT.h"
#include "TClonesArray.h"

#include <iostream>
#include <cassert>
#include "PndEmcWaveformWriteoutBuffer.h"
//#include <map>
//#include <string>

// typedef std::pair<Int_t, Double_t> Element;
// std::map<Int_t, std::vector<Element> > WaveformMap;

using std::cout;
using std::endl;
using std::fstream;
//#define TIMEBASEDSIM

PndEmcHitsToWaveform::PndEmcHitsToWaveform(Int_t verbose, Bool_t storewaves)
  : fHitArray(nullptr), fWaveformArray(nullptr), fDataBuffer(nullptr), fTimeOrderedWaveform(kFALSE), fOneBitResolution(0), fOneBitResolutionBW(0), fOneBitResolutionPMT(0),
    fNBits(0), fDetectedPhotonsPerMeV(0), fDetectedPhotonsPerMeV_PMT(0), fNPhotoElectronsPerMeVAPDBarrel(0), fNPhotoElectronsPerMeVAPDBWD(0), fNPhotoElectronsPerMeVVPT(0),
    fNPhotoElectronsPerMeVPMT(0), fSensitiveAreaAPD(0), fSensitiveAreaVPT(0), fQuantumEfficiencyAPD(0), fQuantumEfficiencyVPT(0), fQuantumEfficiencyPMT(0),
    fExcessNoiseFactorAPD(0), fExcessNoiseFactorVPT(0), fExcessNoiseFactorPMT(0), fIncoherent_elec_noise_width_GeV_APD(0), fIncoherent_elec_noise_width_GeV_VPT(0), fEnergyRange(0),
    fEnergyRangeBW(0), fFirstSamplePhase(0), fNumber_of_samples_in_waveform(0), fNumber_of_samples_in_waveform_pmt(0), fASIC_Shaping_int_time(0), fPMT_Shaping_int_time(0),
    fPMT_Shaping_diff_time(0), fCrystal_time_constant(0), fShashlyk_time_constant(0), fShashlykSamplingFactor(0), fSampleRate(0), fSampleRate_PMT(0), fUse_shaped_noise(0),
    fUse_photon_statistic(0), fNoiseAllChannels(0), fMapVersion(0), fFirstADCBinTime(0), fGevPeakAnalogue(0), fGevPeakAnalogue_PMT(0), fDigiPar(new PndEmcDigiPar()),
    fGeoPar(new PndEmcGeoPar()), fVerbose(verbose)
{ 
  HowManyHit = 0;
  SetPersistency(storewaves);
}

//--------------
// Destructor --
//--------------

PndEmcHitsToWaveform::~PndEmcHitsToWaveform()
{
  //	delete fw_endcap_noise;
  delete pulseshape1;
  delete pulseshape2;
  delete pulseshape3;
}

/**
 * @brief Init Task
 *
 * Prepares the TClonesArrays of PndEmcHit for reading and of PndEmcWaveform for writing.
 * Also reads all relevant EMC parameters and initializes the mapper (PndEmcMapper).
 * Prepares the pulseshapes (PndEmcAbsPulseshape) for the different parts of the EMC and
 * calculates their normalization.
 *
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcHitsToWaveform::Init()
{
  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcHitsToWaveform::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fHitArray = (TClonesArray *)ioman->GetObject("EmcHit");
  if (!fHitArray) {
    cout << "-W- PndEmcHitsToWaveform::Init: "
         << "No EmcHit array!" << endl;
    return kERROR;
  }
  if (fTimeOrderedWaveform) {
    // Create and register output buffer
    fDataBuffer = new PndEmcWaveformWriteoutBuffer("EmcWaveform", "Emc", GetPersistency());
    fDataBuffer->ActivateBuffering(fTimeOrderedWaveform);
    fDataBuffer->SetVerbose(fVerbose);
    ioman->RegisterWriteoutBuffer("EmcWaveform", fDataBuffer);
    fDataBuffer->SaveToTree(kTRUE);
  } else {
    // Create and register output array
    fWaveformArray = new TClonesArray("PndEmcWaveform");
    ioman->Register("EmcWaveform", "Emc", fWaveformArray, GetPersistency());
  }

  LOG(info) << " PndEmcHitsToWaveform: Intialization successfull";

  fNBits = fDigiPar->GetNBits();
  fDetectedPhotonsPerMeV = fDigiPar->GetDetectedPhotonsPerMeV();
  fDetectedPhotonsPerMeV_PMT = fDigiPar->GetDetectedPhotonsPerMeV_PMT();
  fSensitiveAreaAPD = fDigiPar->GetSensitiveAreaAPD();
  fSensitiveAreaVPT = fDigiPar->GetSensitiveAreaVPT();
  fQuantumEfficiencyAPD = fDigiPar->GetQuantumEfficiencyAPD();
  fQuantumEfficiencyVPT = fDigiPar->GetQuantumEfficiencyVPT();
  fQuantumEfficiencyPMT = fDigiPar->GetQuantumEfficiencyPMT();
  fExcessNoiseFactorAPD = fDigiPar->GetExcessNoiseFactorAPD();
  fExcessNoiseFactorVPT = fDigiPar->GetExcessNoiseFactorVPT();
  fExcessNoiseFactorPMT = fDigiPar->GetExcessNoiseFactorPMT();
  fIncoherent_elec_noise_width_GeV_APD = fDigiPar->GetIncoherent_elec_noise_width_GeV_APD(); // GeV
  fIncoherent_elec_noise_width_GeV_VPT = fDigiPar->GetIncoherent_elec_noise_width_GeV_VPT(); // GeV
  fEnergyRange = fDigiPar->GetEnergyRange();                                                 // GeV
  fEnergyRangeBW = fDigiPar->GetEnergyRangeBW();                                             // GeV
  fFirstSamplePhase = fDigiPar->GetFirstSamplePhase();
  fNumber_of_samples_in_waveform = fDigiPar->GetNumber_of_samples_in_waveform();
  fNumber_of_samples_in_waveform_fwd = fDigiPar->GetNumber_of_samples_in_waveform_fwd();
  fNumber_of_samples_in_waveform_pmt = fDigiPar->GetNumber_of_samples_in_waveform_pmt();
  fASIC_Shaping_int_time = fDigiPar->GetASIC_Shaping_int_time();   // s
  fPMT_Shaping_int_time = fDigiPar->GetPMT_Shaping_int_time();     // s
  fPMT_Shaping_diff_time = fDigiPar->GetPMT_Shaping_diff_time();   // s
  fFWD_Shaping_int_time = fDigiPar->GetFWD_Shaping_int_time();     // s
  fFWD_time_constant = fDigiPar->GetFWD_time_constant();           // s
  fCrystal_time_constant = fDigiPar->GetCrystal_time_constant();   // s
  fShashlyk_time_constant = fDigiPar->GetShashlyk_time_constant(); // s
  fShashlykSamplingFactor = fDigiPar->GetShashlykSamplingFactor();
  fSampleRate = fDigiPar->GetSampleRate();
  fSampleRate_PMT = fDigiPar->GetSampleRate_PMT();
  fSampleRate_FWD = fDigiPar->GetSampleRate_FWD();
  fUse_shaped_noise = fDigiPar->GetUse_shaped_noise();
  fUse_photon_statistic = fDigiPar->GetUse_photon_statistic();
  fNoiseAllChannels = fDigiPar->GetNoiseAllChannels();

  fDigiPar->printParams();
  // Test how parameters were read from DB.
  cout << "EMC digitisation parameters " << endl;
  cout << "  nBits " << fNBits << endl;
  cout << "  detectedPhotonsPerMeV " << fDetectedPhotonsPerMeV << endl;
  cout << "  detectedPhotonsPerMeV_PMT " << fDetectedPhotonsPerMeV_PMT << endl;
  cout << "  excessNoiseFactor APD" << fExcessNoiseFactorAPD << endl;
  cout << "  excessNoiseFactor VPT" << fExcessNoiseFactorVPT << endl;
  cout << "  excessNoiseFactor PMT" << fExcessNoiseFactorPMT << endl;
  cout << "  incoherent_elec_noise_width_GeV_APD " << fIncoherent_elec_noise_width_GeV_APD << endl;
  cout << "  incoherent_elec_noise_width_GeV_VPT " << fIncoherent_elec_noise_width_GeV_VPT << endl;
  cout << "  energyRange " << fEnergyRange << endl;
  cout << "  energyRangeBW " << fEnergyRangeBW << endl;
  cout << "  firstSamplePhase " << fFirstSamplePhase << endl;
  cout << "  number_of_samples_in_waveform " << fNumber_of_samples_in_waveform << endl;
  cout << "  number_of_samples_in_waveform_pmt " << fNumber_of_samples_in_waveform_pmt << endl;
  cout << "  number_of_samples_in_waveform_fwd " << fNumber_of_samples_in_waveform_fwd << endl;
  cout << "  ASIC_Shaping_int_time " << fASIC_Shaping_int_time << endl;
  cout << "  PMT_Shaping_int_time " << fPMT_Shaping_int_time << endl;
  cout << "  PMT_Shaping_diff_time " << fPMT_Shaping_diff_time << endl;
  cout << "  crystal_time_constant " << fCrystal_time_constant << endl;
  cout << "  shashlyk_time_constant " << fShashlyk_time_constant << endl;
  cout << "  ShashlykSamplingFactor " << fShashlykSamplingFactor << endl;
  cout << "  sampleRate " << fSampleRate << endl;
  cout << "  sampleRate_PMT " << fSampleRate_PMT << endl;
  cout << "  use_shaped_noise " << fUse_shaped_noise << endl;
  cout << "  use_photon_statistic " << fUse_photon_statistic << endl;
  cout << "  EMC mapper " << fGeoPar->GetMapperVersion() << endl;

  fGeoPar->InitEmcMapper();
  PndEmcStructure::Instance();

  // fUse_shaped_noise = 0;
  // fNumber_of_samples_in_waveform = 64;
  // fNumber_of_samples_in_waveform_FWD = 64;
  // Calculate 1 bit resolution (in units of FADC amplitude)
  PndEmcWaveform *tmpwaveform1 = new PndEmcWaveform(0, 101010001, fNumber_of_samples_in_waveform);
  pulseshape1 = new PndEmcAsicPulseshape(fASIC_Shaping_int_time, fCrystal_time_constant);

  PndEmcWaveform *tmpwaveform2 = new PndEmcWaveform(0, 101010001, fNumber_of_samples_in_waveform_pmt);
  pulseshape2 = new PndEmcCRRCPulseshape(fPMT_Shaping_int_time, fPMT_Shaping_diff_time, fShashlyk_time_constant);

  PndEmcWaveform *tmpwaveform3 = new PndEmcWaveform(0, 101010001, fNumber_of_samples_in_waveform_fwd); // length 10
  pulseshape3 = new PndEmcAsicPulseshape(fFWD_Shaping_int_time, fFWD_time_constant);                   // LNP raw signal, decay time constant 25 microseconds

  // temp parameters;
  // fSampleRate_FWD = 100e6;//100 MHz

  fGevPeakAnalogue = tmpwaveform1->GetScale(fSampleRate, pulseshape1);
  fGevPeakAnalogue_PMT = tmpwaveform2->GetScale(fSampleRate_PMT, pulseshape2);
  fGevPeakAnalogue_FWD = tmpwaveform3->GetScale(fSampleRate_FWD, pulseshape3); // forward endcap,100 MHz

  //	cout<<" -I- PndEmcHitsToWaveform::Init:=========== "<<endl;
  fOneBitResolution = fEnergyRange / ((double)(1 << fNBits)) * fGevPeakAnalogue;
  fOneBitResolutionBW = fEnergyRangeBW / ((double)(1 << fNBits)) * fGevPeakAnalogue;
  fOneBitResolutionPMT = fEnergyRange / ((double)(1 << fNBits)) * fGevPeakAnalogue_PMT;
  fOneBitResolutionFWD = fEnergyRange / ((double)(1 << fNBits)) * fGevPeakAnalogue_FWD;

  cout << "  fGevPeakAnalogue= " << fGevPeakAnalogue << endl;
  cout << "  fGevPeakAnalogue_PMT= " << fGevPeakAnalogue_PMT << endl;
  cout << "  fGevPeakAnalogue_FWD= " << fGevPeakAnalogue_FWD << endl;
  cout << "  fOneBitResolution= " << fOneBitResolution << endl;
  cout << "  fOneBitResolutionBW= " << fOneBitResolutionBW << endl;
  cout << "  fOneBitResolutionPMT= " << fOneBitResolutionPMT << endl;

  fFirstADCBinTime = fFirstSamplePhase / fSampleRate;
  //	cout<<"  fFirstADCBinTime= "<<fFirstADCBinTime<<endl;

  // Calculate number of photoelectrons for APD and VPT
  // The number fDetectedPhotonsPerMeV is the measured number of photoelectrons with PM covering the whole rear surface divided by quantum efficiency of PM (18%)
  // To estimate Number of photoelectrons in barrel the rare surface is taken equal for all the crystals 745 mm^2, which is average surface, hovewer it varies depending on the type
  // of the crystal For forward and backward endcap rear surface is equal 26x26=676 mm^2 Therefore the different number of photoelectrons are used with APD for barrel and backward
  // endcap
  fNPhotoElectronsPerMeVAPDBarrel = fDetectedPhotonsPerMeV * fSensitiveAreaAPD / 745. * fQuantumEfficiencyAPD;
  fNPhotoElectronsPerMeVAPDBWD = fDetectedPhotonsPerMeV * fSensitiveAreaAPD / 676. * fQuantumEfficiencyAPD;
  fNPhotoElectronsPerMeVVPT = fDetectedPhotonsPerMeV * fSensitiveAreaVPT / 676. * fQuantumEfficiencyVPT;
  fNPhotoElectronsPerMeVPMT = fDetectedPhotonsPerMeV_PMT * fQuantumEfficiencyPMT;
  cout << "  fNPhotoElectronsPerMeVAPDBarrel= " << fNPhotoElectronsPerMeVAPDBarrel << endl;
  cout << "  fNPhotoElectronsPerMeVAPDBWD= " << fNPhotoElectronsPerMeVAPDBWD << endl;
  cout << "  fNPhotoElectronsPerMeVVPT= " << fNPhotoElectronsPerMeVVPT << endl;
  cout << "  fNPhotoElectronsPerMeVPMT= " << fNPhotoElectronsPerMeVPMT << endl;
  // delete pulseshape1;
  delete tmpwaveform1;
  // delete pulseshape2;
  delete tmpwaveform2;
  // delete pulseshape3;
  delete tmpwaveform3;

  nWaveformProduced = 0;

  return kSUCCESS;
}

/**
 * @brief Runs the task.
 *
 * The tasks consists of using the PndEmcHit to create PndEmcWaveform by using the pulseshape
 * generators (PndEmcAbsPulseshape derived) for the different parts of the EMC. Also the
 * noise is added to the pulseshape here.
 *
 * @param opt unused
 * @return void
 */
void PndEmcHitsToWaveform::Exec(Option_t *)
{
  TStopwatch timer;
  if (fVerbose > 2) {
    timer.Start();
  }
  // Reset output array
  if (fTimeOrderedWaveform) {
    if (!fDataBuffer)
      Fatal("Exec", "No Waveform Data Buffer");
  } else {
    if (!fWaveformArray)
      Fatal("Exec", "No Waveform Array");
    fWaveformArray->Delete();
  }

  Double_t EventTime = FairRootManager::Instance()->GetEventTime(); // nano seconds
  Int_t nHits = fHitArray->GetEntriesFast();
  Int_t evtNo = FairRun::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;
  if (fVerbose > 1) {
    cout << "**************************************" << endl;
    cout << "Event No. #" << evtNo << ", EvtTime #" << EventTime << std::endl;
    cout << "PndEmcHitsToWaveform:: Hit array contains " << nHits << " hits" << endl;
    cout << "fDataBuffer size #" << (fTimeOrderedWaveform ? fDataBuffer->GetNData() : fWaveformArray->GetEntriesFast()) << std::endl;
    cout << "**************************************" << endl;
  }

  // Variable declaration
  PndEmcHit *theHit = nullptr;
  PndEmcWaveform *theWaveform = nullptr;
  PndEmcHit *tmpHit = nullptr;
  std::set<Int_t> waveformInd;
  Int_t NumOfSamples;

  // Loop over PndEmcHits to add them to correspondent waveforms
  // <set> fWaveformInd contains indexes of detectors for which Waveforms are created
  // PndEmcAsicPulseshape *pulseshape= new PndEmcAsicPulseshape(fASIC_Shaping_int_time,fCrystal_time_constant);
  // PndEmcAbsPulseshape *pulseshape2=new PndEmcCRRCPulseshape(fPMT_Shaping_int_time,fPMT_Shaping_diff_time,fShashlyk_time_constant);
  // PndEmcAbsPulseshape *pulseshape3=new PndEmcAsicPulseshape(10.e-9,25e-6);//fCrystal_time_constant = decay time constant 25 micorseconds

  Double_t sampleRate;
  Double_t TimeMin = 99999.;
  Double_t TimeMax = 0.;
  Double_t WaveformTimeStamp(0.);
  Double_t TimeError(0.);
  Double_t EnergyError(0.);
  Int_t detId, module, MCTrackID;

  HowManyHit += nHits;
  for (Int_t iHit = 0; iHit < nHits; iHit++) {
    theHit = (PndEmcHit *)fHitArray->At(iHit);
    module = theHit->GetModule();

    if (module > 5) {
      std::cout << " UpdateWaveform: Unknown module number " << module << " in EMC digitization. Detector ID = " << detId << std::endl;
      continue;
    }
    detId = theHit->GetDetectorID();
    // if(theHit->GetEnergy() < 0.001) continue;//1MeV
    TimeError = gRandom->Gaus(0, 0.55 + 5.5 * TMath::Exp(-27.7 * theHit->GetEnergy())); // ns
    EnergyError = sqrt(0.01 * 0.01 + 0.02 * 0.02 / theHit->GetEnergy());                ////emc_tdr page 33
    const std::vector<Int_t> &mcTrack = theHit->GetMcList();
    MCTrackID = mcTrack.size() > 0 ? mcTrack[0] : -1;

    waveformInd.insert(detId);
    if (module == 5) {
      sampleRate = fSampleRate_PMT;
      NumOfSamples = fNumber_of_samples_in_waveform_pmt;
    } else if (module == 3) {
      sampleRate = fSampleRate_FWD; // 100 MHz
      NumOfSamples = fNumber_of_samples_in_waveform_fwd;
    } else {
      sampleRate = fSampleRate;
      NumOfSamples = fNumber_of_samples_in_waveform;
    }
    WaveformTimeStamp = EventTime + theHit->GetTime() * 1.0e9; // to nano seconds
    theWaveform = AddWaveform(detId, iHit, NumOfSamples, WaveformTimeStamp, sampleRate, MCTrackID);
    theWaveform->AddEvt(evtNo); // very important
    theWaveform->SetTimeStampError(TimeError);

    // WaveformMap[detId].push_back(Element(evtNo, theWaveform->GetTimeStamp()));
    if (theWaveform->GetTimeStamp() > TimeMax)
      TimeMax = theWaveform->GetTimeStamp();
    if (theWaveform->GetTimeStamp() < TimeMin)
      TimeMin = theWaveform->GetTimeStamp();
    //		Int_t module_wf = theWaveform->GetModule();

    switch (module) {
    case 1: // Barrel
      theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstADCBinTime, fSampleRate, pulseshape1, EnergyError);
      if (fUse_shaped_noise == 0) {
        theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, EnergyError);
      } else {
        theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, pulseshape1, fFirstADCBinTime, fSampleRate,
                                                   EnergyError);
      }
      break;
    case 2: // Barrel
      theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVAPDBarrel, fUse_photon_statistic, fExcessNoiseFactorAPD, fFirstADCBinTime, fSampleRate, pulseshape1, EnergyError);
      if (fUse_shaped_noise == 0) {
        theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, EnergyError);
      } else {
        theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, pulseshape1, fFirstADCBinTime, fSampleRate,
                                                   EnergyError);
      }
      break;
    case 3: // Fwd endcap
      theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstADCBinTime, fSampleRate_FWD, pulseshape3,
                                  EnergyError); // use a different shape
      if (fUse_shaped_noise == 0) {
        theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT * fGevPeakAnalogue_FWD, fOneBitResolutionFWD, EnergyError);
      } else {
        theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT * fGevPeakAnalogue_FWD, fOneBitResolutionFWD, pulseshape3, fFirstADCBinTime,
                                                   fSampleRate_FWD, EnergyError);
      }
      break;
    case 4: // Bwd endcap
      theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVVPT, fUse_photon_statistic, fExcessNoiseFactorVPT, fFirstADCBinTime, fSampleRate, pulseshape1, EnergyError);
      if (fUse_shaped_noise == 0) {
        theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolutionBW, EnergyError);
      } else {
        theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolutionBW, pulseshape1, fFirstADCBinTime, fSampleRate,
                                                   EnergyError);
      }
      break;
    case 5: // Shashlyk calorimetr
      tmpHit = theHit;
      tmpHit->SetEnergy(tmpHit->GetEnergy() * fShashlykSamplingFactor);
      EnergyError = sqrt(0.01 * 0.01 + 0.02 * 0.02 / tmpHit->GetEnergy());
      theWaveform->UpdateWaveform(theHit, fNPhotoElectronsPerMeVPMT, fUse_photon_statistic, fExcessNoiseFactorPMT, fFirstADCBinTime, fSampleRate_PMT, pulseshape2, EnergyError);
      if (fUse_shaped_noise == 0) {
        theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue_PMT, fOneBitResolutionPMT, EnergyError);
      } else {
        theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue_PMT, fOneBitResolutionPMT, pulseshape2, fFirstADCBinTime,
                                                   fSampleRate_PMT, EnergyError);
      }
      break;
    default:
      std::cout << " UpdateWaveform: Unknown module number " << module << " in EMC digitization. Detector ID = " << detId << std::endl;
      // abort();
    }
    if (fTimeOrderedWaveform) {
      fDataBuffer->FillNewData(theWaveform, theWaveform->GetTimeStamp(), theWaveform->GetActiveTime());
    }
  }
  // Produce waveforms in all the crystals, not only where hits took place
  // Since it is time consuming, by default it is off
  if (fNoiseAllChannels) {
    Int_t detId_tmp, modId_tmp;
    std::map<Int_t, PndEmcTwoCoordIndex *> intTwoCoordMap = PndEmcMapper::Instance()->GetTciMap();
    for (std::map<Int_t, PndEmcTwoCoordIndex *>::iterator iter = intTwoCoordMap.begin(); iter != intTwoCoordMap.end(); ++iter) {
      detId_tmp = (*iter).first;
      modId_tmp = detId_tmp / 100000000;
      if (modId_tmp == 5) {
        NumOfSamples = fNumber_of_samples_in_waveform_pmt;
        sampleRate = fSampleRate_PMT;
      } else if (modId_tmp == 3) {
        NumOfSamples = fNumber_of_samples_in_waveform_fwd;
        sampleRate = fSampleRate_FWD;
      } else {
        NumOfSamples = fNumber_of_samples_in_waveform;
        sampleRate = fSampleRate;
      }

      if (waveformInd.insert(detId_tmp).second) {
        WaveformTimeStamp = gRandom->Uniform(TimeMin, TimeMax);
        theWaveform = AddWaveform(detId_tmp, -1, NumOfSamples, WaveformTimeStamp, sampleRate, -1); // -1 correponds to Waveform produced not from EmcHit but from Noise
        switch (modId_tmp) {
        case 1: // Barrel
          if (fUse_shaped_noise == 0) {
            theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution);
          } else {
            theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, pulseshape1, fFirstSamplePhase, fSampleRate);
          }
          break;
        case 2: // Barrel
          if (fUse_shaped_noise == 0) {
            theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution);
          } else {
            theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolution, pulseshape1, fFirstSamplePhase, fSampleRate);
          }
          break;
        case 3: // Fwd endcap
          if (fUse_shaped_noise == 0) {
            theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT * fGevPeakAnalogue_FWD, fOneBitResolutionFWD);
          } else {
            theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT * fGevPeakAnalogue_FWD, fOneBitResolutionFWD, pulseshape3, fFirstSamplePhase,
                                                       fSampleRate_FWD);
          }
          break;
        case 4: // Bwd endcap
          if (fUse_shaped_noise == 0) {
            theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolutionBW);
          } else {
            theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue, fOneBitResolutionBW, pulseshape1, fFirstSamplePhase, fSampleRate);
          }
          break;
        case 5: // Shashlyk calorimetr
          if (fUse_shaped_noise == 0) {
            theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue_PMT, fOneBitResolutionPMT);
          } else {
            theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD * fGevPeakAnalogue_PMT, fOneBitResolutionPMT, pulseshape2, fFirstSamplePhase,
                                                       fSampleRate_PMT);
          }
          break;
        default:
          std::cout << " UpdateWaveform: Unknown module number " << modId_tmp << " in EMC digitization. Detector ID = " << detId_tmp << std::endl;
          // abort();
        }
        if (fTimeOrderedWaveform) {
          fDataBuffer->FillNewData(theWaveform, theWaveform->GetTimeStamp(), theWaveform->GetActiveTime());
        }
      }
    }
  }

  // Add electronic noise
  // There are two options how to add noise (before and after shaping)

  /*Int_t nWf = fWaveformArray->GetEntriesFast();
    if (fVerbose>2){
    cout << "Number of waveforms processed= "<<nWf<<endl;
    }

    for (Int_t iWf=0; iWf<nWf; iWf++) {
    theWaveform = (PndEmcWaveform*) fWaveformArray->At(iWf);
    Int_t module = theWaveform->GetModule();
    switch (module){
    case 1: // Barrel
    if (fUse_shaped_noise==0)
    {
    theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
    }
    else {
    theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
    }
    break;
    case 2: // Barrel
    if (fUse_shaped_noise==0)
    {
    theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution);
    }
    else {
    theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
    }
    break;
    case 3: // FWD Endcap
    if (fUse_shaped_noise==0)
    {
    theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution);
    }
    else {
    theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_VPT*fGevPeakAnalogue,fOneBitResolution, pulseshape, fFirstSamplePhase, fSampleRate);
    }
    break;
    case 4: // BWD Endcap
    if (fUse_shaped_noise==0)
    {
    theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW);
    }
    else {
    theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue,fOneBitResolutionBW, pulseshape, fFirstSamplePhase, fSampleRate);
    }
    break;
    case 5: // shashlyk calorimetr
    if (fUse_shaped_noise==0)
    {
    theWaveform->AddElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue_PMT,fOneBitResolutionPMT);
    }
    else {
    theWaveform->AddShapedElecNoiseAndDigitise(fIncoherent_elec_noise_width_GeV_APD*fGevPeakAnalogue_PMT,fOneBitResolutionPMT, pulseshape2, fFirstSamplePhase, fSampleRate_PMT);
    }
    break;
    default:
    std::cout<<"Add Noise: Unknown module number in EMC digitization"<<std::endl;
  //abort();
  }
  }*/

  // std::cout<<"nWaveformProduced = "<<nWaveformProduced<<std::endl;
  // delete pulseshape;
  // delete pulseshape2;
  // delete pulseshape3;

  if (fVerbose > 2) {
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << "PndEmcHitsToWaveform, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  }
}

void PndEmcHitsToWaveform::SetParContainers()
{

  // Get run and runtime database
  FairRun *run = FairRun::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar *)db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar *)db->getContainer("PndEmcDigiPar");
}

// -----   Private method AddWaveform   --------------------------------------------
/*PndEmcWaveform* PndEmcHitsToWaveform::AddWaveform(Int_t detID, Int_t iHit,Int_t numOfSamples){
  TClonesArray& clref = *fWaveformArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndEmcWaveform(0,detID,numOfSamples,iHit);
  }*/
/**
 * @brief Create a new PndEmcWaveform from the passed parameters
 *
 * @param detID Detector ID
 * @param iHit Index of PndEmcHit in hit TClonesArray
 * @param numOfSamples Number of samples in waveform
 * @param timeStamp Timestamp from the hit
 * @param sampleRate Sample rate (in Hz)
 * @param MCTrackID MC track ID if available, else -1
 * @return PndEmcWaveform* The created waveform object
 */
PndEmcWaveform *PndEmcHitsToWaveform::AddWaveform(Int_t detID, Int_t iHit, Int_t numOfSamples, Double_t timeStamp, Double_t sampleRate, Int_t MCTrackID)
{
  nWaveformProduced++;
  PndEmcWaveform *thisWave(0);
  if (fTimeOrderedWaveform) {
    thisWave = new PndEmcWaveform(MCTrackID, detID, sampleRate, numOfSamples, iHit, timeStamp);
  } else {
    TClonesArray &clref = *fWaveformArray;
    Int_t size = clref.GetEntriesFast();
    thisWave = new (clref[size]) PndEmcWaveform(MCTrackID, detID, sampleRate, numOfSamples, iHit, timeStamp);
  }
  return thisWave;
}

void PndEmcHitsToWaveform::SetStorageOfData(Bool_t val)
{
  SetPersistency(val);
  return;
}
/**
 * @brief Called at end of task.
 *
 * Outputs statistics and writes PndEmcWaveformWriteoutBuffer.
 *
 * @return void
 */

void PndEmcHitsToWaveform::FinishTask()
{
  std::cout << "===================================================" << std::endl;
  std::cout << "PndEmcHitsToWaveform::FinishTask" << std::endl;
  std::cout << "***************************************************" << std::endl;
  std::cout << "Read Hits# " << HowManyHit << std::endl;
  std::cout << "Produc waveforms# " << nWaveformProduced << std::endl;
  std::cout << "***************************************************" << std::endl;

  if (fTimeOrderedWaveform) {
    fDataBuffer->Write();
  }
}

ClassImp(PndEmcHitsToWaveform)
