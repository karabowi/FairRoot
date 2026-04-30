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
// Author List:
//     Guang Zhao
//----------------------------------------------------------------------
//
//
#include <PndPersistencyTask.h>
#include "PndEmcBWEndcapTimebasedWaveforms.h"

#include "FairRun.h"
#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcWaveformBuffer.h"
#include "PndEmcAbsWaveformModifier.h"
#include "PndEmcFullStackedWaveformSimulator.h"
#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcBWEndcapPulseAmplifier.h"
#include "PndEmcBWEndcapNoiseAdder.h"
#include "PndEmcBWEndcapDigitizer.h"
#include "PndEmcHit.h"
#include "PndEmcBWPulseshape.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcBWEndcapDigiPar.h"
#include "PndEmcGeoPar.h"        
#include "PndEmcDataTypes.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLink.h"

#include "TStopwatch.h"
#include "TROOT.h"
#include "TRandom.h"
#include "TClonesArray.h"

#include <iostream>
#include <fstream>
#include <cassert>

using std::cout;
using std::cerr;
using std::endl;
using std::fstream;

PndEmcBWEndcapTimebasedWaveforms::PndEmcBWEndcapTimebasedWaveforms(Int_t verbose, Bool_t storewaves)
  : PndPersistencyTask("PndEmcBWEndcapTimebasedWaveforms", verbose), fHitArray(nullptr), fWaveformBuffer(nullptr), fStoreDataClass(kFALSE), fActivateBuffering(kFALSE),
    fDigiPar(nullptr), fGeoPar(nullptr), fUse_photon_statistic(kFALSE), fNPhotoElectronsPerMeV(0), fExcessNoiseFactor(0.), fExternalSimulator(nullptr), fAPD_LOWHIGH(nullptr)
{
    SetPersistency(storewaves);
    fSingleAPDMode = kFALSE;
    fNoiseMode = 1;
    fTestMode = kFALSE;
}

//--------------
// Destructor --
//--------------
PndEmcBWEndcapTimebasedWaveforms::~PndEmcBWEndcapTimebasedWaveforms() 
{
}


/**
 * @brief Init Task
 * 
 * Prepares the TClonesArray of PndEmcHit for reading and PndEmcMultiWaveform for writing.
 * Also reads the EMC parameters and prepares the waveform simulator (PndEmcMultiWaveformSimulator).
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcBWEndcapTimebasedWaveforms::Init()
{
    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- PndEmcBWEndcapTimebasedWaveforms::Init: "
        << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
    
    // Get input array
    fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
    if (!fHitArray) {
        cout << "-W- PndEmcBWEndcapTimebasedWaveforms::Init: "
        << "No EmcHit array!" << endl;
        return kERROR;
    }

    // Create and activiate output Buffer....choose between PndEmcWaveform and PndEmcMultiWaveform
  #ifndef MULTI
    fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcWaveform", "Emc", GetPersistency());
  #else
    fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcMultiWaveform", "Emc", GetPersistency());
  #endif

    fWaveformBuffer = (PndEmcWaveformBuffer*) ioman ->RegisterWriteoutBuffer("EmcTimebasedWaveform", fWaveformBuffer);
    fWaveformBuffer->ActivateBuffering(fActivateBuffering);

    if (fStoreDataClass) {
        fWaveformBuffer->StoreWaveformData("EmcWaveformData", "Emc", GetPersistency());
    }

    
    if(!fDigiPar) {
        cout << "-E- PndEmcBWEndcapTimebasedWaveforms::Init: "
            << "no DigiPar containter found" << endl;
        return kERROR;
    }

    fUse_photon_statistic = fDigiPar->GetUse_photon_statistic();

    if (fUse_photon_statistic) {
        //rear surface of BWEndcap crystal: 676 mm^2
        fNPhotoElectronsPerMeV = fDigiPar->GetDetectedPhotonsPerMeV() * fDigiPar->GetSensitiveAreaAPD()/ 676. * fDigiPar->GetQuantumEfficiencyAPD();
        fExcessNoiseFactor = fDigiPar->GetExcessNoiseFactorAPD();
    } else {
        fNPhotoElectronsPerMeV = 0;
        fExcessNoiseFactor = 1;
    }

    fGeoPar->InitEmcMapper();
    PndEmcStructure::Instance();

    const Double_t tBefore = fDigiPar->GetTimeBeforeHit();     //ns  //250
    const Double_t tAfter = fDigiPar->GetTimeAfterHit();       //ns  //250
    const Double_t cutoff = fDigiPar->GetWfCutOffEnergy();     //GeV  //0.001
    const Double_t activeTimeIncrement = fDigiPar->GetActiveTimeIncrement(); //ns   //10
    const Double_t sampleRate = fDigiPar->GetSampleRate();     //ns^-1 <-> GHz    //0.08
    const Int_t bits = fDigiPar->GetNBits();                   //1    //14
    const Double_t tau = fDigiPar->GetPulseshapeTau()/sampleRate;         //1->ns   //22 -> 22*12.5
    const Double_t N = fDigiPar->GetPulseshapeN();             //N    //1.667
    const Double_t* ps_freq = fDigiPar->GetPSFreq().GetArray();
    const Double_t* ps_power_HIGH = fDigiPar->GetPSPowerHigh().GetArray();
    const Double_t* ps_power_LOW = fDigiPar->GetPSPowerLow().GetArray();
    Int_t ps_size = fDigiPar->GetPSFreq().GetSize();              //1          
    const Double_t hglg_ratio = fDigiPar->GetHGLGRatio();         //1      //10.5
    const Double_t encalib = fDigiPar->GetEnCalib();              // 0.3 MeV/channel
    const Double_t adc_noise_HIGH = fDigiPar->GetADCNoiseHigh();  //ADC    //3.5
    const Double_t adc_noise_LOW = fDigiPar->GetADCNoiseLow();    //ADC    //3.5
    const Double_t fe_noise_HIGH = fDigiPar->GetFENoiseHigh();    //ADC    //1.89 
    const Double_t fe_noise_LOW = fDigiPar->GetFENoiseLow();      //ADC    //1.89
    const Double_t ps_sigma_HIGH = fDigiPar->GetPsSigmaHigh();    //ADC    //0.21
    const Double_t ps_sigma_LOW = fDigiPar->GetPsSigmaLow();      //ADC    //0.038
    const Double_t ges_sigma_HIGH = fDigiPar->GetGesSigmaHigh();  //ADC    //36.69
    const Double_t ges_sigma_LOW = fDigiPar->GetGesSigmaLow();    //ADC    //7.36

    PndEmcAbsPulseshape* fPulseshape = new PndEmcBWPulseshape(tau, N);
    Int_t seed = time(nullptr);
    PndEmcBWEndcapNoiseAdder* na = new PndEmcBWEndcapNoiseAdder(fNoiseMode, sampleRate, 
        ps_freq, ps_power_HIGH, ps_size, adc_noise_HIGH, 
        fe_noise_HIGH, ps_sigma_HIGH, ges_sigma_HIGH, seed, fVerbose);
    PndEmcBWEndcapNoiseAdder* na_lo = new PndEmcBWEndcapNoiseAdder(fNoiseMode, sampleRate, 
        ps_freq, ps_power_LOW, ps_size, adc_noise_LOW, 
        fe_noise_LOW, ps_sigma_LOW, ges_sigma_LOW, seed, fVerbose);

    if (!fSingleAPDMode) {
        fAPD_LOWHIGH = new PndEmcMultiWaveformSimulator(sampleRate, fPulseshape, 4);
        fAPD_LOWHIGH->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

        // convert waveform amplitude from energy to adc channel
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib * hglg_ratio, fAPD_LOWHIGH->GetTotalScale(0)), 0);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib, fAPD_LOWHIGH->GetTotalScale(1)), 1);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib * hglg_ratio, fAPD_LOWHIGH->GetTotalScale(2)), 2);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib, fAPD_LOWHIGH->GetTotalScale(3)), 3);

        // add noise
        fAPD_LOWHIGH->AddModifier(na, 0);
        fAPD_LOWHIGH->AddModifier(na_lo, 1);
        fAPD_LOWHIGH->AddModifier(na, 2);
        fAPD_LOWHIGH->AddModifier(na_lo, 3);

        // digitize
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(0), bits), 0);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(1), bits), 1);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(2), bits), 2);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(3), bits), 3);
    }
    else {
        fAPD_LOWHIGH = new PndEmcMultiWaveformSimulator(sampleRate, fPulseshape, 2);
        fAPD_LOWHIGH->Init(tBefore, tAfter, cutoff, activeTimeIncrement);

        // convert waveform amplitude from energy to adc channel
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib * hglg_ratio, fAPD_LOWHIGH->GetTotalScale(0)), 0);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapPulseAmplifier(
                                      1e3 / encalib, fAPD_LOWHIGH->GetTotalScale(1)), 1);

        // add noise
        fAPD_LOWHIGH->AddModifier(na, 0);
        fAPD_LOWHIGH->AddModifier(na_lo, 1);

        // digitize
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(0), bits), 0);
        fAPD_LOWHIGH->AddModifier(new PndEmcBWEndcapDigitizer(
                                      fAPD_LOWHIGH->GetTotalScale(1), bits), 1);
    }

    return kSUCCESS;
}


/**
 * @brief Runs the task.
 * 
 * Uses the waveform simulator (PndEmcAbsWaveformSimulator) to generate a waveform
 * from the hit and fill the buffer (PndEmcWaveformBuffer).
 * 
 * @param opt unused
 * @return void
 */
void PndEmcBWEndcapTimebasedWaveforms::Exec(Option_t*) 
{
    FairRootManager* ioman = FairRootManager::Instance();

    TStopwatch timer;
    if (fVerbose>0){
        timer.Start();
    }
        
    // Variable declaration
    PndEmcHit *theHit = nullptr;

    // Loop over PndEmcHits to add them to correspondent waveforms
    Int_t nHits = fHitArray->GetEntriesFast();
    if (fVerbose>2) {
        cout<< "PndEmcBWEndcapTimebasedWaveforms:: Hit array contains " << nHits << " hits" <<endl;
    }

    for (Int_t iHit=0; iHit<nHits; iHit++) {

        theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(iHit));

        if(theHit->GetModule() > 5 ) continue;    //tackles invalid PndEmcHit inforamtion (valid module number = seq 1 5

        PndEmcAbsWaveformSimulator *wfSimulator = nullptr;
        if(fExternalSimulator) {
            wfSimulator = fExternalSimulator; 
        } else {
            wfSimulator = fAPD_LOWHIGH;
        }; 

        Double_t energy = theHit->GetEnergy();
        if (fUse_photon_statistic) {
            Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
            Double_t photonStatFactor = gRandom->Gaus(1, sqrt(fExcessNoiseFactor/crystalPhotonsMeV));
            energy *= photonStatFactor;
        }    

        // construct corresponding waveform data Object
        PndEmcWaveformData wfData(theHit->GetDetectorID(),  wfSimulator);
        Double_t time_stamp_error = gRandom->Gaus(0, 0.55 + 5.5*TMath::Exp(-27.7*theHit->GetEnergy())); // ns
        wfData.SetTimeStampError(time_stamp_error); // timestamp error

        //register hit...timebased framework uses ns, whereas emc deals with seconds as time unit
        FairLink linkToHit(-1, ioman->GetEntryNr(), "EmcHit", iHit, 1.0);
        //std::cout << "-I- PndEmcBWEndcapTimebasedWaveforms::Exec eventTIme: " << ioman->GetEventTime() << " hit Time: " << theHit->GetTime()*1.0e9 << std::endl;
        if (!fTestMode)
            wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, energy);
        else
            wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, fTestEnergy);
        fWaveformBuffer->FillNewData(&wfData);    

        if (fVerbose > 1) {
            timer.Stop();
            Double_t rtime = timer.RealTime();
            Double_t ctime = timer.CpuTime();
            cout << "PndEmcBWEndcapTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
        }

    }
}

void PndEmcBWEndcapTimebasedWaveforms::SetParContainers() 
{
    // Get run and runtime database
    FairRun* run = FairRun::Instance();
    if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

    FairRuntimeDb* db = run->GetRuntimeDb();
    if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

    // Get Emc geometry parameter container
    fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

    // Get Emc digitisation parameter container
    fDigiPar = dynamic_cast<PndEmcBWEndcapDigiPar*>(db->getContainer("PndEmcBWEndcapDigiPar"));
}

ClassImp(PndEmcBWEndcapTimebasedWaveforms)
