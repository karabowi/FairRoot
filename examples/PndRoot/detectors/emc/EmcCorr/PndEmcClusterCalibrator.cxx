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

//--------------------------------------------------------------------------
// Description:
//	Class PndEmcClusterCalibrator
//      Do an energy and position corrections for EMC cluster
//      PndEmcClusterCalibrator is a factory which call specific method depending on the input parameter "method"
//      method=1 - PndEmcClusterHistCalibrator is called, which perform correction from the interpolation on 2-dimensional histogram on (Energy, theta)
//      method=2 - PndEmcClusterSimpleCalibrator is called, where energy correction is parametrised as a function of (Energy, theta)
//      PndEmcAbsClusterCalibrator - abstract interface class
//
// Versions of the correction:
// 1 - "emc_module12.dat","emc_module3_2011_new.root","emc_module4_StraightGeo24.4.root","emc_module5_fsc.root" (+ full PANDA geometry) TGeant3
//
// Author List:
//      D.Melnychuk
//      PndEmcClusterHistCalibrator class is based on code PndEmcMakeCorr.cxx
//      (A. Biegun, M. Babai)
//      PndEmcClusterSimpleCalibrator class is based on EmcPhotonSimpleCalib class in Babar framework (Jan Zhong)
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "PndEmcClusterCalibrator.h"

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "PndEmcStructure.h"
#include "PndEmcDataTypes.h"

#include "PndEmcMapper.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"
#include "PndEmcBump.h"
#include "PndEmcClusterCalibrationPar.h"
#include "FairLogger.h"

#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;

//----------------
// Constructors --
//----------------
PndEmcClusterCalibrator::PndEmcClusterCalibrator() {}

//--------------
// Destructor --
//--------------
PndEmcClusterCalibrator::~PndEmcClusterCalibrator() {}

PndEmcAbsClusterCalibrator *PndEmcClusterCalibrator::MakeEmcClusterCalibrator(Int_t method, Int_t version)
{
  switch (method) {
  case 1: return new PndEmcClusterHistCalibrator(version); break;
  case 2: return new PndEmcClusterSimpleCalibrator(version); break;
  default:
    cout << "PndEmcClusterCalibrator:: Method " << method << " is not defined. Default PndEmcClusterHistCalibrator is used" << endl;
    return new PndEmcClusterHistCalibrator(version);
  }
}

/////////////////////////////////////////////////////////////////
//////////// PndEmcAbsClusterCalibrator /////////////////////////
/////////////////////////////////////////////////////////////////
PndEmcAbsClusterCalibrator::PndEmcAbsClusterCalibrator(Int_t version) : fVersion(version), fPath("") {}

PndEmcAbsClusterCalibrator::~PndEmcAbsClusterCalibrator() {}

/////////////////////////////////////////////////////////////////
//////////// PndEmcClusterHistCalibrator ////////////////////////
/////////////////////////////////////////////////////////////////

PndEmcClusterHistCalibrator::PndEmcClusterHistCalibrator(Int_t version)
  : fPhoton(new TFile()), fElectron(new TFile()), fPion(new TFile()), fHEnergyRatioBarrelPhoton(0), fHEnergyRatioFwdPhoton(0), fHEnergyRatioBwdPhoton(0),
    fHEnergyRatioShashlykPhoton(0), fHThetaDiffBarrelPhoton(0), fHThetaDiffFwdPhoton(0), fHThetaDiffBwdPhoton(0), fHThetaDiffShashlykPhoton(0)
{

  fPath = getenv("VMCWORKDIR");
  fPath += "/macro/params/";

  TString fileNamePhoton;
  fileNamePhoton.Form("emc_correction_hist_gamma_%i.root", version);
  fileNamePhoton = fPath + fileNamePhoton;

  fPhoton = new TFile(fileNamePhoton, "READ");
  if (fPhoton->IsZombie()) {
    std::cout << "EMC cluster correction file " << fileNamePhoton << " for photons does not exist for given version" << std::endl;
  } else {
    // Photon
    fHEnergyRatioBarrelPhoton = (TH2F *)fPhoton->Get("hisEnergyRatioBarrel");
    fHThetaDiffBarrelPhoton = (TH2F *)fPhoton->Get("hisThetaDiffBarrel");
    fHEnergyRatioFwdPhoton = (TH2F *)fPhoton->Get("hisEnergyRatioFwd");
    fHThetaDiffFwdPhoton = (TH2F *)fPhoton->Get("hisThetaDiffFwd");
    fHEnergyRatioBwdPhoton = (TH2F *)fPhoton->Get("hisEnergyRatioBwd");
    fHThetaDiffBwdPhoton = (TH2F *)fPhoton->Get("hisThetaDiffBwd");
    fHEnergyRatioShashlykPhoton = (TH2F *)fPhoton->Get("hisEnergyRatioShashlyk");
    fHThetaDiffShashlykPhoton = (TH2F *)fPhoton->Get("hisThetaDiffShashlyk");
  }

  // 	TString fileNameElectron;
  // 	TString fileNamePion;
  // 	fileNameElectron.Form("emc_correction_hist_electron_%i.root",version);
  // 	fileNamePion.Form("emc_correction_hist_pion_%i.root",version);
  // 	fileNameElectron=fPath+fileNameElectron;
  // 	fileNamePion=fPath+fileNamePion;
  // 	fHEnergyRatioBarrelElectron=0; fHEnergyRatioFwdElectron=0; fHEnergyRatioBwdElectron=0; fHEnergyRatioShashlykElectron=0;
  // 	fHThetaDiffBarrelElectron=0; fHThetaDiffFwdElectron=0; fHThetaDiffBwdElectron=0; fHThetaDiffShashlykElectron=0;
  // 	fHEnergyRatioBarrelPion=0; fHEnergyRatioFwdPion=0; fHEnergyRatioBwdPion=0; fHEnergyRatioShashlykPion=0;
  // 	fHThetaDiffBarrelPion=0; fHThetaDiffFwdPion=0; fHThetaDiffBwdPion=0; fHThetaDiffShashlykPion=0;
  //
  //
  // 	fElectron = new TFile(fileNameElectron,"READ");
  // 	if (fElectron->IsZombie())
  // 	{
  // 		std::cout<<"EMC cluster correction file "<<fileNameElectron<<" for electrons does not exist for given version"<<std::endl;
  // 	} else
  // 	{
  // 		// Electron
  // 		fHEnergyRatioBarrelElectron = (TH2F*) fElectron->Get("hisEnergyRatioBarrel");
  // 		fHThetaDiffBarrelElectron = (TH2F*) fElectron->Get("hisThetaDiffBarrel");
  // 		fHEnergyRatioFwdElectron = (TH2F*) fElectron->Get("hisEnergyRatioFwd");
  // 		fHThetaDiffFwdElectron = (TH2F*) fElectron->Get("hisThetaDiffFwd");
  // 		fHEnergyRatioBwdElectron = (TH2F*) fElectron->Get("hisEnergyRatioBwd");
  // 		fHThetaDiffBwdElectron = (TH2F*) fElectron->Get("hisThetaDiffBwd");
  // 		fHEnergyRatioShashlykElectron = (TH2F*) fElectron->Get("hisEnergyRatioShashlyk");
  // 		fHThetaDiffShashlykElectron = (TH2F*) fElectron->Get("hisThetaDiffShashlyk");
  // 	}
  //
  // 	fPion = new TFile(fileNamePion,"READ");
  // 	if (fPion->IsZombie())
  // 	{
  // 		std::cout<<"EMC cluster correction file "<<fileNamePion<<" for pions does not exist for given version"<<std::endl;
  // 	} else
  // 	{
  // 		// Pion
  // 		fHEnergyRatioBarrelPion = (TH2F*) fPion->Get("hisEnergyRatioBarrel");
  // 		fHThetaDiffBarrelPion = (TH2F*) fPion->Get("hisThetaDiffBarrel");
  // 		fHEnergyRatioFwdPion = (TH2F*) fPion->Get("hisEnergyRatioFwd");
  // 		fHThetaDiffFwdPion = (TH2F*) fPion->Get("hisThetaDiffFwd");
  // 		fHEnergyRatioBwdPion = (TH2F*) fPion->Get("hisEnergyRatioBwd");
  // 		fHThetaDiffBwdPion = (TH2F*) fPion->Get("hisThetaDiffBwd");
  // 		fHEnergyRatioShashlykPion = (TH2F*) fPion->Get("hisEnergyRatioShashlyk");
  // 		fHThetaDiffShashlykPion = (TH2F*) fPion->Get("hisThetaDiffShashlyk");
  // 	}
}

PndEmcClusterHistCalibrator::~PndEmcClusterHistCalibrator()
{
  fPhoton->Close();
  delete fPhoton;
  // 	fElectron->Close();
  // 	fPion->Close();
  // 	delete fElectron;
  // 	delete fPion;
}

Double_t PndEmcClusterHistCalibrator::Energy(PndEmcCluster *theCluster, Int_t pid)
{
  Double_t valzEn = 0.; // FIXME is that a good initialisation value?
  Bool_t use_interpolation = kTRUE;

  Double_t energy = theCluster->GetEnergy();
  TVector3 position = theCluster->where();
  Double_t theta = position.Theta();
  Double_t thetaRad = theta * (180. / TMath::Pi());

  Int_t module = theCluster->GetModule();

  // TH2F *fHEnergyRatioBarrel, *fHThetaDiffBarrel, *fHEnergyRatioFwd, *fHThetaDiffFwd,
  //*fHEnergyRatioBwd, *fHThetaDiffBwd, *fHEnergyRatioShashlyk, *fHThetaDiffShashlyk; //[R.K. 01/2017] unused variable?

  switch (pid) {
  case 22: // photon
    switch (module) {
    case 1:
    case 2: valzEn = GetValueInZ(fHEnergyRatioBarrelPhoton, energy, thetaRad, use_interpolation); break;
    case 3: valzEn = GetValueInZ(fHEnergyRatioFwdPhoton, energy, thetaRad, use_interpolation); break;
    case 4: valzEn = GetValueInZ(fHEnergyRatioBwdPhoton, energy, thetaRad, use_interpolation); break;
    case 5: valzEn = GetValueInZ(fHEnergyRatioShashlykPhoton, energy, thetaRad, use_interpolation); break;
    default: std::cout << "Wrong EMC module in PndEmcClusterHistCalibrator" << std::endl;
    }
    break;
    // 		case 11: // electron
    // 		case -11: // positron
    // 			switch (module)
    // 			{
    // 				case 1:
    // 				case 2:
    // 					valzEn = GetValueInZ( fHEnergyRatioBarrelElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 3:
    // 					valzEn = GetValueInZ( fHEnergyRatioFwdElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 4:
    // 					valzEn = GetValueInZ( fHEnergyRatioBwdElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 5:
    // 					valzEn = GetValueInZ( fHEnergyRatioShashlykElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				default:
    // 					std::cout<<"Wrong EMC module in PndEmcClusterHistCalibrator"<<std::endl;
    // 			}
    // 			break;
    // 		case 211:
    // 		case -211:
    // 			switch (module)
    // 			{
    // 				case 1:
    // 				case 2:
    // 					valzEn = GetValueInZ( fHEnergyRatioBarrelPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 3:
    // 					valzEn = GetValueInZ( fHEnergyRatioFwdPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 4:
    // 					valzEn = GetValueInZ( fHEnergyRatioBwdPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 5:
    // 					valzEn = GetValueInZ( fHEnergyRatioShashlykPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				default:
    // 					std::cout<<"Wrong EMC module in PndEmcClusterHistCalibrator"<<std::endl;
    // 			}
    // 			break;
  default:
    std::cout << "Emc cluster correction for pid= " << pid << " is not defined. Photon values is used." << std::endl;
    switch (module) {
    case 1:
    case 2: valzEn = GetValueInZ(fHEnergyRatioBarrelPhoton, energy, thetaRad, use_interpolation); break;
    case 3: valzEn = GetValueInZ(fHEnergyRatioFwdPhoton, energy, thetaRad, use_interpolation); break;
    case 4: valzEn = GetValueInZ(fHEnergyRatioBwdPhoton, energy, thetaRad, use_interpolation); break;
    case 5: valzEn = GetValueInZ(fHEnergyRatioShashlykPhoton, energy, thetaRad, use_interpolation); break;
    default: std::cout << "Wrong EMC module in PndEmcClusterHistCalibrator" << std::endl;
    }
  }

  Double_t energyCorrected = energy / valzEn;

  return energyCorrected;
}

TVector3 PndEmcClusterHistCalibrator::Where(PndEmcCluster *theCluster, Int_t pid)
{
  Double_t valzTh = 0.; // FIXME is that a good initialisation value?
  Bool_t use_interpolation = kTRUE;

  Double_t energy = theCluster->GetEnergy();
  TVector3 position = theCluster->where();
  Double_t thetaRad = position.Theta() * (180. / TMath::Pi());
  Double_t phiRad = position.Phi();
  Double_t mag = position.Mag();

  Int_t module = theCluster->GetModule();

  switch (pid) {
  case 22: // photon
    switch (module) {
    case 1:
    case 2: valzTh = GetValueInZ(fHThetaDiffBarrelPhoton, energy, thetaRad, use_interpolation); break;
    case 3: valzTh = GetValueInZ(fHThetaDiffFwdPhoton, energy, thetaRad, use_interpolation); break;
    case 4: valzTh = GetValueInZ(fHThetaDiffBwdPhoton, energy, thetaRad, use_interpolation); break;
    case 5: valzTh = GetValueInZ(fHThetaDiffShashlykPhoton, energy, thetaRad, use_interpolation); break;
    default: std::cout << "Wrong EMC module in PndEmcClusterHistCalibrator" << std::endl;
    }
    break;
    // 		case 11: // electron
    // 		case -11: // positron
    // 			switch (module)
    // 			{
    // 				case 1:
    // 				case 2:
    // 					valzTh = GetValueInZ( fHThetaDiffBarrelElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 3:
    // 					valzTh = GetValueInZ( fHThetaDiffFwdElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 4:
    // 					valzTh = GetValueInZ( fHThetaDiffBwdElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 5:
    // 					valzTh = GetValueInZ( fHThetaDiffShashlykElectron, energy, thetaRad, use_interpolation);
    // 					break;
    // 				default:
    // 					std::cout<<"Wrong EMC module in PndEmcClusterHistCalibrator"<<std::endl;
    // 			}
    // 			break;
    // 		case 211:
    // 		case -211:
    // 			switch (module)
    // 			{
    // 				case 1:
    // 				case 2:
    // 					valzTh = GetValueInZ( fHThetaDiffBarrelPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 3:
    // 					valzTh = GetValueInZ( fHThetaDiffFwdPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 4:
    // 					valzTh = GetValueInZ( fHThetaDiffBwdPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				case 5:
    // 					valzTh = GetValueInZ( fHThetaDiffShashlykPion, energy, thetaRad, use_interpolation);
    // 					break;
    // 				default:
    // 					std::cout<<"Wrong EMC module in PndEmcClusterHistCalibrator"<<std::endl;
    // 			}
    // 			break;
  default:
    std::cout << "Emc cluster correction for pid= " << pid << " is not defined. Photon values is used." << std::endl;
    switch (module) {
    case 1:
    case 2: valzTh = GetValueInZ(fHThetaDiffBarrelPhoton, energy, thetaRad, use_interpolation); break;
    case 3: valzTh = GetValueInZ(fHThetaDiffFwdPhoton, energy, thetaRad, use_interpolation); break;
    case 4: valzTh = GetValueInZ(fHThetaDiffBwdPhoton, energy, thetaRad, use_interpolation); break;
    case 5: valzTh = GetValueInZ(fHThetaDiffShashlykPhoton, energy, thetaRad, use_interpolation); break;
    default: std::cout << "Wrong EMC module in PndEmcClusterHistCalibrator" << std::endl;
    }
  }

  Double_t thetaCorrected = valzTh + thetaRad;
  Double_t thetaCorrectedRad = thetaCorrected * (TMath::Pi() / 180.);

  TVector3 correctedPos;
  correctedPos.SetMagThetaPhi(mag, thetaCorrectedRad, phiRad);

  return correctedPos;
}

//-------------
// Methods   --
//-------------
Int_t PndEmcClusterHistCalibrator::FindTheBin(TH2 *lookup_table, Float_t value_x, Float_t value_y, Int_t &bin_x, Int_t &bin_y)
{
  bin_x = lookup_table->GetXaxis()->FindBin(value_x);
  bin_y = lookup_table->GetYaxis()->FindBin(value_y);

  if ((bin_x < 1) || (bin_x > lookup_table->GetXaxis()->GetNbins())) {
    bin_x = -1;
    bin_y = -1;
    return -1;
  }

  if ((bin_y < 1) || (bin_y > lookup_table->GetYaxis()->GetNbins())) {
    bin_x = -1;
    bin_y = -1;
    return -2;
  }

  return 0; // Success
}

Double_t PndEmcClusterHistCalibrator::GetValueInZ(TH2 *lookup_table, Float_t value_x, Float_t value_y, Bool_t use_interpolation)
{
  // We own the EmcLocMaxInfo objects.  Delete from last time.
  // Clean-up res, We need an empty set to store the results.

  if (use_interpolation) {
    // cout<<"use_interpolation = kTRUE "<<endl;
    //
    // Use the interpolarion routine of ROOT:
    // Interpolate approximates the value via bilinear
    // interpolation based on the four nearest bin centers
    // see Wikipedia, Bilinear Interpolation
    // Andy Mastbaum 10/8/2008
    // vaguely based on R.Raja 6-Sep-2008
    //
    // cout<<"value_x = "<< value_x <<", value_y = "<<value_y <<endl;

    // If values are outside histogram range return 1
    Int_t binx, biny, retval;
    retval = FindTheBin(lookup_table, value_x, value_y, binx, biny);
    if (retval) {
      // std::cout<<"Energy = "<<value_x<<" theta="<<value_y<<std::endl;
      return 1.;
    }
    return (lookup_table->Interpolate(value_x, value_y));
  } else {
    // cout<<"use_interpolation = kFALSE "<<endl;
    Int_t binx, biny, retval;
    retval = FindTheBin(lookup_table, value_x, value_y, binx, biny);
    if (retval) {
      cout << "<E> Error in FindTheBin, check your table and input values!!!!: " << retval << endl;
      return 0;
    }

    return (lookup_table->GetBinContent(binx, biny));
  }
  return 0;
}

/////////////////////////////////////////////////////////////////
//////////// PndEmcClusterSimpleCalibrator //////////////////////
/////////////////////////////////////////////////////////////////
PndEmcClusterSimpleCalibrator::PndEmcClusterSimpleCalibrator(Int_t version) : fParObject(new PndEmcClusterCalibrationParObject())
{
  fPath = getenv("VMCWORKDIR");
  fPath += "/macro/params/";

  TString fileNamePhoton;
  fileNamePhoton.Form("emc_correction_par_gamma_%i.root", version);
  fileNamePhoton = fPath + fileNamePhoton;
  TFile *fPhoton;

  // 	TString fileNameElectron;
  // 	TString fileNamePion;
  // 	fileNameElectron.Form("emc_correction_par_electron_%i.root",version);
  // 	fileNamePion.Form("emc_correction_par_pion_%i.root",version);
  // 	fileNameElectron=fPath+fileNameElectron;
  // 	fileNamePion=fPath+fileNamePion;
  // 	TFile *fElectron, *fPion;

  fPhoton = new TFile(fileNamePhoton, "READ");
  if (fPhoton->IsZombie()) {
    std::cout << "EMC cluster correction file " << fileNamePhoton << " for photons does not exist for given version" << std::endl;
  } else {
    // Photon
    fPhoton->GetObject("PndEmcClusterCalibrationParObject", fParObject);
    if (fParObject == nullptr) {
      LOG(error) << " PndEmcClusterSimpleCalibrator: Could not get Emc cluster calibration information from file " << fPhoton;
    } else {
      std::cout << "EMC photon calibration parameters are read succesfully" << std::endl;
    }
  }
  // 	fElectron = new TFile(fileNameElectron,"READ");
  // 	if (fElectron->IsZombie())
  // 	{
  // 		std::cout<<"EMC cluster correction file "<<fileNameElectron<<" for electrons does not exist for given version"<<std::endl;
  // 	} else
  // 	{
  // 		// Photon
  // 		fElectron->GetObject("PndEmcClusterCalibrationParObject",fParObject);
  // 		if(fParObject == nullptr){
  // 			std::cout << "-E- PndEmcClusterSimpleCalibrator: Could not get Emc cluster calibration information from file " << fElectron<< std::endl;
  // 		}
  // 	}
  //
  // 	fPion = new TFile(fileNamePion,"READ");
  // 	if (fPion->IsZombie())
  // 	{
  // 		std::cout<<"EMC cluster correction file "<<fileNamePion<<" for pions does not exist for given version"<<std::endl;
  // 	} else
  // 	{
  // 		// Photon
  // 		fPion->GetObject("PndEmcClusterCalibrationParObject",fParObject);
  // 		if(fParObject == nullptr){
  // 			std::cout << "-E- PndEmcClusterSimpleCalibrator: Could not get Emc cluster calibration information from file " << fPion << std::endl;
  // 		}
  // 	}
}

TVector3 PndEmcClusterSimpleCalibrator::Where(PndEmcCluster *clust, Int_t) // pid //[R.K.03/2017] unused variable(s)
{
  return clust->where();
}

Double_t PndEmcClusterSimpleCalibrator::Energy(PndEmcCluster *clust, Int_t pid)
{
  if (pid != 22) {
    std::cout << "PndEmcClusterSimpleCalibrator:: EMC Cluster energy correction is not defined for pid=" << pid << ". Photon correction is used" << std::endl;
  }

  Double_t e = clust->GetEnergy();
  TVector3 clusterPosition = clust->where();
  Double_t theta_cluster = clusterPosition.Theta();

  Double_t e1 = e;
  Double_t theta1 = theta_cluster;

  if ((clusterPosition.Z() < 180.0) && (theta_cluster < 141. * TMath::Pi() / 180.)) {
    if (e < 0.03)
      e1 = 0.03;
    if (e > 8.0)
      e1 = 8.0;
  }

  if ((clusterPosition.Z() < 180.0) && (theta_cluster > 141. * TMath::Pi() / 180.)) {
    if (e < 0.03)
      e1 = 0.03;
    if (e > 2.0)
      e1 = 2.0;
  }

  if (clusterPosition.Z() > 180.0) {
    if (e < 0.01)
      e1 = 0.01;
    if (e > 16.0)
      e1 = 16.0;
  }

  Int_t iParSet; // EMC component and energy range (1 - barrel (below 1 GeV, 2 -barrel (above 1 geV, 3 -forward endcap, 4 -backward endcap, 5 -shashlyk) )

  if (clusterPosition.Z() > 500.0) {
    iParSet = 5;
  } else if ((clusterPosition.Z() < 180.0) && (theta_cluster > 141. * TMath::Pi() / 180.)) {
    iParSet = 4;
  } else if ((clusterPosition.Z() < 180.0) && (theta_cluster < 141. * TMath::Pi() / 180.)) {
    if (e1 < 1.0)
      iParSet = 1;
    else
      iParSet = 2;
  } else {
    iParSet = 3;
  }

  Double_t pars[10];
  fParObject->GetCalibrationPar(iParSet, pars);

  double p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;
  double eout;

  if (iParSet == 5) {
    p0 = pars[0];
    p1 = pars[1];
    p2 = pars[2];
    p3 = pars[3];
    p4 = pars[4];
    eout = (p0 - p1 / sqrt(e1) + p2 / e1 + p3 / (e1 * e1) - p4 / (e1 * sqrt(e1))) * e;
  } else {
    p0 = pars[0];
    p1 = pars[1];
    p2 = pars[2];
    p3 = pars[3];
    p4 = pars[4];
    p5 = pars[5];
    p6 = pars[6];
    p7 = pars[7];
    p8 = pars[8];
    p9 = pars[9];

    double factor1 = p0 + p1 * log(e1) + p2 * log(e1) * log(e1) + p3 * log(e1) * log(e1) * log(e1) + p4 * cos(theta1) + p5 * cos(theta1) * cos(theta1) +
                     p6 * cos(theta1) * cos(theta1) * cos(theta1) + p7 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) +
                     p8 * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) * cos(theta1) + p9 * log(e1) * cos(theta1);
    eout = e * exp(factor1);
  }

  return eout;
}

PndEmcClusterSimpleCalibrator::~PndEmcClusterSimpleCalibrator() {}
