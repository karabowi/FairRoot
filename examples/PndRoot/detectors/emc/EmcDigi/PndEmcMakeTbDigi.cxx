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
//      Class PndEmcMakeTbDigi. This class takes array of EmcHit's and produce
// an array of EmcDigis.
// It is convenient to study reconstruction algoritms without
// disturbance from digitization
//----------------------------------------------------------------------

#include "PndEmcMakeTbDigi.h"

#include "PndEmcDigi.h"
#include "PndEmcGeoPar.h"
#include "PndEmcDigiPar.h"
#include "PndEmcRecoPar.h"
#include "PndEmcMapper.h"
#include "PndEmcStructure.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TRandom.h"
#include "TROOT.h"

#include <iostream>
//#include <map>

using std::cout;
using std::endl;
using std::fstream;

PndEmcMakeTbDigi::PndEmcMakeTbDigi(Bool_t storedigis)
  : fHitArray(nullptr), fDigiArray(nullptr), fThreshold(0), fDigiPosMethod(""), fEmcDigiRescaleFactor(0), fEmcDigiPositionDepthPWO(0), fEmcDigiPositionDepthShashlyk(0),
    fUseDigiEffectiveSmearing(0), fDetectedPhotonsPerMeV(0), fNPhotoElectronsPerMeVAPDBarrel(0), fNPhotoElectronsPerMeVAPDBWD(0), fNPhotoElectronsPerMeVVPT(0),
    fSensitiveAreaAPD(0), fSensitiveAreaVPT(0), fQuantumEfficiencyAPD(0), fQuantumEfficiencyVPT(0), fExcessNoiseFactorAPD(0), fExcessNoiseFactorVPT(0),
    fIncoherent_elec_noise_width_GeV_APD(0), fIncoherent_elec_noise_width_GeV_VPT(0), fMapVersion(0), fGeoPar(new PndEmcGeoPar()), fDigiPar(new PndEmcDigiPar()),
    fRecoPar(new PndEmcRecoPar()), fStoreDigis(storedigis), fNrOfEvents(0), evtCounter(0), nDigiProg(0)
{
	fDigiPosMethod="depth";// "surface" or "depth"
	fEmcDigiRescaleFactor=1.08;
}

//--------------
// Destructor --
//--------------
PndEmcMakeTbDigi::~PndEmcMakeTbDigi()
{
}


/**
 * @brief Init Task
 * 
 * Prepares the TClonesArray of PndEmcHit for reading and PndEmcDigi for writing.
 * Also reads the paramters for the EMC.
 * 
 * @return InitStatus
 * @retval kSUCCESS success
 */
InitStatus PndEmcMakeTbDigi::Init()
{
	// Get RootManager
	FairRootManager* ioman = FairRootManager::Instance();
	if ( ! ioman )
	{
		cout << "-E- PndEmcMakeTbDigi::Init: "
		<< "RootManager not instantiated!" << endl;
		return kFATAL;
	}

	// Get nr of events	(divide by 100 for progress counter)
	fNrOfEvents = ((ioman->GetInTree())->GetEntriesFast())/100;
	
	// Get input array
	fHitArray =dynamic_cast<TClonesArray *> (ioman->GetObject("EmcHit"));
	if ( ! fHitArray ) {
		cout << "-W- PndEmcMakeTbDigi::Init: "
		<< "No EmcHit array!" << endl;
		return kERROR;
	}
	
	// Create and register output array
/*	fDigiArray = new TClonesArray("PndEmcDigi");
	ioman->Register("EmcDigi","Emc",fDigiArray,fStoreDigis);*/
	fDigiArray = ioman->Register("EmcDigi", "PndEmcDigi", "Emc", fStoreDigis);
	
	fEmcDigiPositionDepthPWO=fRecoPar->GetEmcDigiPositionDepthPWO();
	fEmcDigiPositionDepthShashlyk=fRecoPar->GetEmcDigiPositionDepthShashlyk();
	if (!fDigiPosMethod.compare("surface"))
	{
		PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::surface, 1., 0. );
	}
	else if (!fDigiPosMethod.compare("depth"))
	{
	   PndEmcDigi::selectDigiPositionMethod( PndEmcDigi::depth, 
			 fEmcDigiPositionDepthPWO, fEmcDigiPositionDepthShashlyk, fEmcDigiRescaleFactor);
	}
	else 
	{
		cout << "-W- PndEmcMakeTbDigi::Init: "
		<< "Unknown digi position method!" << endl;
		return kERROR;
	}
	
	// The following parameters define if energy of hit will be copied to digi or it will be smeared
	fUseDigiEffectiveSmearing=fDigiPar->GetUseDigiEffectiveSmearing();
	fDetectedPhotonsPerMeV=fDigiPar->GetDetectedPhotonsPerMeV();
	fSensitiveAreaAPD=fDigiPar->GetSensitiveAreaAPD();
	fSensitiveAreaVPT=fDigiPar->GetSensitiveAreaVPT();
	fQuantumEfficiencyAPD=fDigiPar->GetQuantumEfficiencyAPD();
	fQuantumEfficiencyVPT=fDigiPar->GetQuantumEfficiencyVPT();
	fExcessNoiseFactorAPD=fDigiPar->GetExcessNoiseFactorAPD();
	fExcessNoiseFactorVPT=fDigiPar->GetExcessNoiseFactorVPT();
	fIncoherent_elec_noise_width_GeV_APD=fDigiPar->GetIncoherent_elec_noise_width_GeV_APD(); //GeV
	fIncoherent_elec_noise_width_GeV_VPT=fDigiPar->GetIncoherent_elec_noise_width_GeV_VPT(); //GeV
	
	fThreshold=fDigiPar->GetEnergyDigiThreshold();
	fGeoPar->InitEmcMapper();  
	PndEmcStructure::Instance();


	// Calculate number of photoelectrons for APD and VPT
	// The number fDetectedPhotonsPerMeV is the measured number of photoelectrons with PM covering the whole rear surface divided by quantum efficiency of PM (18%)
	// To estimate Number of photoelectrons in barrel the rare surface is taken equal for all the crystals 745 mm^2, which is average surface, hovewer it varies depending on the type of the crystal
	// For forward and backward endcap rear surface is equal 26x26=676 mm^2
	// Therefore the different number of photoelectrons are used with APD for barrel and backward endcap
	fNPhotoElectronsPerMeVAPDBarrel=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/745.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVAPDBWD=fDetectedPhotonsPerMeV*fSensitiveAreaAPD/676.*fQuantumEfficiencyAPD;
	fNPhotoElectronsPerMeVVPT=fDetectedPhotonsPerMeV*fSensitiveAreaVPT/676.*fQuantumEfficiencyVPT;

  LOG(info) << " PndEmcMakeTbDigi: Intialization successfull";

  return kSUCCESS;
}

/**
 * @brief Runs the task
 * 
 * Creates digis from the hits and adds noise.
 * 
 * @param opt unused
 * @return void
 */
void PndEmcMakeTbDigi::Exec(Option_t*)
{

	if (evtCounter-1==nDigiProg*fNrOfEvents) {
		cout << "\r[INFO\t] PndEmcMakeTbDigi: " << nDigiProg << "\% completed." << std::flush;	
		nDigiProg += 1;
	}
	evtCounter++;

	// Reset output array
	if ( ! fDigiArray ) Fatal("Exec", "No Digi Array");
	fDigiArray->Delete();
  
	// Variable declaration
  PndEmcHit *theHit = nullptr;

  // Loop over PndEmcHits to add them to correspondent wavefoorms
	Int_t nHits = fHitArray->GetEntriesFast();
	
	//cout<<"Hit array contains "<<nHits<< " hits"<<endl;
	for (Int_t iHit=0; iHit<nHits; iHit++) {
		theHit = (PndEmcHit*) fHitArray->At(iHit);
		int detId=theHit->GetDetectorID();
		Double_t energy=theHit->GetEnergy();
		
			Int_t trackId=theHit->GetRefIndex();
			Double_t time=theHit->GetTime();
			
			// Smear hit energy as sigma/E=sqrt((a/sqrt(E))^2+(E_noise/E)^2)
			// i.e stochastic and noise term of energy resolution are taken into account
			if (fUseDigiEffectiveSmearing==1){
				int module = theHit->GetModule();
				Double_t a,sigma_E;
				switch (module){
					case 1: // Barrel 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 2: // Barrel 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 7: // Proto60
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBarrel*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));

						break;
					case 3: // FWD endcap 
						a=sqrt(fExcessNoiseFactorVPT/(fNPhotoElectronsPerMeVVPT*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_VPT/energy,2));
						break;
					case 4: // BWD endcap 
						a=sqrt(fExcessNoiseFactorAPD/(fNPhotoElectronsPerMeVAPDBWD*1e3)); // 1e3 is conversion from MeV to GeV
						sigma_E=sqrt(pow(a/sqrt(energy),2)+pow(fIncoherent_elec_noise_width_GeV_APD/energy,2));
						break;
					case 5: // shashlyk
						// sigma/E=5.6/E+2.4/sqrt(E)+1.3 (%)
						sigma_E=sqrt(pow(0.056/energy,2)+pow(0.024/sqrt(energy),2)+0.013);
						break;
					default:
						std::cout<<"PndEmcMakeTbDigi::Exec - Unknown module number in EMC digitization"<<std::endl;
						abort();
				}
				
				energy= gRandom->Gaus(energy,sigma_E*energy);
			}
			if(fUseDigiEffectiveSmearing ==2){
				Double_t nPhotons=(energy*1e3*fDetectedPhotonsPerMeV);
				if(theHit->GetModule()==7&&theHit->GetRow()==4&&theHit->GetCrystal()==5){
					energy= gRandom->Gaus(0,fIncoherent_elec_noise_width_GeV_VPT)+(gRandom->PoissonD(nPhotons)/1.0e3)/fDetectedPhotonsPerMeV;
				}else{					
					energy= gRandom->Gaus(0,fIncoherent_elec_noise_width_GeV_APD)+(gRandom->PoissonD(nPhotons)/1.0e3)/fDetectedPhotonsPerMeV;
				}
			}

		if (energy>fThreshold&& detId>0)
		{
			AddDigi(trackId,detId, energy, time,iHit); 
		}
	}

}

/**
 * @brief Adds a PndEmcDigi to to fDigiArray and returns it.
 * 
 * @param trackID ID of track which created the digi
 * @param detID Detector ID for digi (c.f. PndEmcMapper)
 * @param energy Deposited energy (including noise)
 * @param time Timestamp of the hit
 * @param hitIndex Index of the hit in the TClonesArray
 * @return PndEmcDigi*
 */
PndEmcDigi* PndEmcMakeTbDigi::AddDigi(Int_t trackID, Int_t detID, Double_t energy, Double_t time, Int_t hitIndex)
{
  TClonesArray& clref = *fDigiArray;
  Int_t size = clref.GetEntriesFast();
  PndEmcDigi* newDigi = new(clref[size]) PndEmcDigi(trackID, detID, energy, FairRootManager::Instance()->GetEventTime() + time*1.e9, hitIndex);
  newDigi->SetTimeStamp(gRandom->Gaus(FairRootManager::Instance()->GetEventTime() + time * 1.0e9, GetEnergyDepTimeResolution(energy)));
  newDigi->SetEnergy(gRandom->Gaus(energy, CorrectForEnergyDepResolution(energy)));
  return newDigi;
}

Double_t PndEmcMakeTbDigi::GetEnergyDepTimeResolution(Double_t E)
{	
	// calculate energy-dependent time resolution. Formula taken from http://dx.doi.org/10.1016/j.nima.2011.06.044
	return 0.55+5.5*TMath::Exp(-(TMath::Log(2)/0.0250)*E);
}

Double_t PndEmcMakeTbDigi::CorrectForEnergyDepResolution(Double_t E)
{	
	// correct energy with energy-dependent resolution. Formula taken from http://dx.doi.org/10.1016/j.nima.2011.06.044
	return (0.0102+0.0167/TMath::Sqrt(E))*E;
}

void PndEmcMakeTbDigi::SetParContainers() 
{
  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) LOG(fatal) << "SetParContainers: No runtime database";

  // Get Emc geometry parameter container
  fGeoPar = (PndEmcGeoPar*) db->getContainer("PndEmcGeoPar");
  // Get Emc digitisation parameter container
  fDigiPar = (PndEmcDigiPar*) db->getContainer("PndEmcDigiPar");
  // Get Emc reconstruction parameter container
  fRecoPar = (PndEmcRecoPar*) db->getContainer("PndEmcRecoPar");
 
}

void PndEmcMakeTbDigi::SetStorageOfData(Bool_t val)
{
  fStoreDigis=val;
  return;
}
  
ClassImp(PndEmcMakeTbDigi)
