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
#include "PndEmcShashlykTimebasedWaveforms.h"

#include "FairRun.h"
#include "PndEmcWaveform.h"
#include "PndEmcWaveformData.h"
#include "PndEmcWaveformBuffer.h"
#include "PndEmcAbsWaveformModifier.h"
#include "PndEmcFullStackedWaveformSimulator.h"
#include "PndEmcMultiWaveformSimulator.h"
#include "PndEmcBWEndcapPulseAmplifier.h" // same to the bwec
#include "PndEmcShashlykNoiseAdder.h"
#include "PndEmcBWEndcapDigitizer.h" // same to the bwec
#include "PndEmcHit.h"
#include "PndEmcShashlykPulseshape.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"
#include "PndEmcShashlykDigiPar.h"
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

PndEmcShashlykTimebasedWaveforms::PndEmcShashlykTimebasedWaveforms(Int_t verbose, Bool_t storewaves) :
    PndPersistencyTask("PndEmcShashlykTimebasedWaveforms", verbose),
    fHitArray(NULL), fWaveformBuffer(NULL), fStoreDataClass(kFALSE), fActivateBuffering(kFALSE), fDigiPar(NULL), fGeoPar(NULL), fUse_photon_statistic(kFALSE), fNPhotoElectronsPerMeV(0), fExcessNoiseFactor(0.), fExternalSimulator(NULL), fSimulator(NULL)
{
    SetPersistency(storewaves);
    fTestMode = kFALSE;
    fTestEnergy = -1.;
}

//--------------
// Destructor --
//--------------
PndEmcShashlykTimebasedWaveforms::~PndEmcShashlykTimebasedWaveforms() 
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
InitStatus PndEmcShashlykTimebasedWaveforms::Init()
{
    // Get RootManager
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman) {
        cout << "-E- PndEmcShashlykTimebasedWaveforms::Init: "
        << "RootManager not instantiated!" << endl;
        return kFATAL;
    }
    
    // Get input array
    fHitArray = (TClonesArray*) ioman->GetObject("EmcHit");
    if (!fHitArray) {
        cout << "-W- PndEmcShashlykTimebasedWaveforms::Init: "
        << "No EmcHit array!" << endl;
        return kERROR;
    }

    // Create and activiate output Buffer....choose between PndEmcWaveform and PndEmcMultiWaveform
    fWaveformBuffer = new PndEmcWaveformBuffer("EmcWaveform", "PndEmcWaveform", "Emc", GetPersistency());

    fWaveformBuffer = (PndEmcWaveformBuffer*) ioman ->RegisterWriteoutBuffer("EmcTimebasedWaveform", fWaveformBuffer);
    fWaveformBuffer->ActivateBuffering(fActivateBuffering);

    if (fStoreDataClass) {
        fWaveformBuffer->StoreWaveformData("EmcWaveformData", "Emc", GetPersistency());
    }

    if(!fDigiPar) {
        cout << "-E- PndEmcShashlykTimebasedWaveforms::Init: "
            << "no DigiPar containter found" << endl;
        return kERROR;
    }

    fUse_photon_statistic = fDigiPar->GetUse_photon_statistic();

    if (fUse_photon_statistic) {
        fNPhotoElectronsPerMeV = fDigiPar->GetDetectedPhotonsPerMeV() * fDigiPar->GetQuantumEfficiency();
        fExcessNoiseFactor = fDigiPar->GetExcessNoiseFactor();
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
    const Double_t mu = fDigiPar->GetPulseshapeMu()-TMath::Log(sampleRate);  //?    //1.4752 convert from samples to ns
    const Double_t sigma = fDigiPar->GetPulseshapeSigma();     //?    //0.6109
    const Double_t noise_width = fDigiPar->GetNoiseWidth();    // ADC      // 1.0
    const Double_t encalib = fDigiPar->GetEnCalib();           // MeV/ADC  // 0.77
    //fSamplingFactor = fDigiPar->GetSamplingFactor();           //1    //2.524

    PndEmcAbsPulseshape* fPulseshape = new PndEmcShashlykPulseshape(mu, sigma);
    //cout << "ideal wf: " << endl;
    //for (int i = 0; i < 100; i++) {
    //    cout << (*fPulseshape)(i/10., 100, 1.) << endl;
    //}

    fSimulator = new PndEmcFullStackedWaveformSimulator(sampleRate, fPulseshape);
    fSimulator->Init(tBefore, tAfter, cutoff, activeTimeIncrement);
    // convert waveform amplitude from energy to adc channel
    fSimulator->AddModifier(new PndEmcBWEndcapPulseAmplifier(1./encalib*1e3, 1.));

    // add noise
    fSimulator->AddModifier(new PndEmcShashlykNoiseAdder(noise_width, time(NULL)));

    // digitize
    fSimulator->AddModifier(new PndEmcBWEndcapDigitizer(1., bits));

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
void PndEmcShashlykTimebasedWaveforms::Exec(Option_t*) 
{
    FairRootManager* ioman = FairRootManager::Instance();

    TStopwatch timer;
    if (fVerbose>0){
        timer.Start();
    }
        
    // Variable declaration
    PndEmcHit* theHit = NULL;
    
    // Loop over PndEmcHits to add them to correspondent waveforms
    Int_t nHits = fHitArray->GetEntriesFast();
    if (fVerbose>2) {
        cout<< "PndEmcShashlykTimebasedWaveforms:: Hit array contains " << nHits << " hits" <<endl;
    }

    for (Int_t iHit=0; iHit<nHits; iHit++) {

        theHit = dynamic_cast<PndEmcHit*>(fHitArray->At(iHit));

        if(theHit->GetModule() > 5 ) continue;    //tackles invalid PndEmcHit inforamtion (valid module number = seq 1 5
        
        PndEmcAbsWaveformSimulator* simulator = NULL;
        if(fExternalSimulator) {
            simulator = fExternalSimulator; 
        } else {
            simulator = fSimulator;
        }; 

        //Double_t energy = theHit->GetEnergy() * fSamplingFactor;
        Double_t energy = theHit->GetEnergy();
        if (fUse_photon_statistic) {
            Double_t crystalPhotonsMeV = 1.0e3 * energy * fNPhotoElectronsPerMeV;
            Double_t photonStatFactor = gRandom->Gaus(1, sqrt(fExcessNoiseFactor/crystalPhotonsMeV));
            energy *= photonStatFactor;
        }    

        // construct corresponding waveform data Object
        PndEmcWaveformData wfData(theHit->GetDetectorID(),  simulator);
        Double_t time_stamp_error = gRandom->Gaus(0, 0.55 + 5.5*TMath::Exp(-27.7*theHit->GetEnergy())); // ns
        wfData.SetTimeStampError(time_stamp_error); // timestamp error

        //register hit...timebased framework uses ns, whereas emc deals with seconds as time unit
        FairLink linkToHit(-1, ioman->GetEntryNr(), "EmcHit", iHit, 1.0);
        //std::cout << "-I- PndEmcShashlykTimebasedWaveforms::Exec eventTIme: " << ioman->GetEventTime() << " hit Time: " << theHit->GetTime()*1.0e9 << std::endl;
        if (!fTestMode)
            wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, energy);
        else
            wfData.AddHit(linkToHit, ioman->GetEventTime() + theHit->GetTime()*1.0e9, fTestEnergy);
        fWaveformBuffer->FillNewData(&wfData);    

        if (fVerbose > 1) {
            timer.Stop();
            Double_t rtime = timer.RealTime();
            Double_t ctime = timer.CpuTime();
            cout << "PndEmcShashlykTimebasedWaveforms, Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
        }

    }
}

void PndEmcShashlykTimebasedWaveforms::SetParContainers() 
{
    // Get run and runtime database
    FairRun* run = FairRun::Instance();
    if ( ! run ) Fatal("SetParContainers", "No analysis run");

    FairRuntimeDb* db = run->GetRuntimeDb();
    if ( ! db ) Fatal("SetParContainers", "No runtime database");

    // Get Emc geometry parameter container
    fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");

    // Get Emc digitisation parameter container
    fDigiPar = dynamic_cast<PndEmcShashlykDigiPar*>(db->getContainer("PndEmcShashlykDigiPar"));
}

ClassImp(PndEmcShashlykTimebasedWaveforms)
