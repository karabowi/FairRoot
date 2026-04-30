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
//	Class PndEmcErrorMatrix
//      Calculate Error Matrix for the given EmcCluster
//      with parametrization defined by the given parameter PndEmcErrorMatrixPar
//
//------------------------------------------------------------------------

#include "PndEmcErrorMatrix.h"
#include "PndEmcErrorMatrixPar.h"
#include "TSystem.h"
#include <cmath>
#include "FairLogger.h"

PndEmcErrorMatrix::PndEmcErrorMatrix():
fErrorMatrixParObject(new PndEmcErrorMatrixParObject())
{
}

void PndEmcErrorMatrix::Init(PndEmcErrorMatrixParObject *par)
{
	fErrorMatrixParObject=par;
}

void PndEmcErrorMatrix::InitFromFile(Int_t geomVersion)
{
	TString fFileName;
	switch (geomVersion){
		case 1:
		case 17:
		case 19:
			// correspond to geometries "emc_module12.dat","emc_module3new.root","emc_module4_StraightGeo24.4.root","emc_module5_fsc.root"
			fFileName="emc_error_matrix_1.root";
			break;
		default:
			std::cout<<"Error matrix for EMC geometry: "<<geomVersion<<" does not exist. Default is used."<<std::endl;
			fFileName="emc_error_matrix_default.root";
	}

	TString emc_error_file_name = gSystem->Getenv("VMCWORKDIR");
	emc_error_file_name+="/input/";
	emc_error_file_name+=fFileName;
	
	std::cout<<"EMC error matrix is read from: "<<emc_error_file_name<<std::endl;
	
	TFile *errorMatrixfile = new TFile(emc_error_file_name.Data());
  if (errorMatrixfile == nullptr) {
    LOG(error) << " PndEmcErrorMatrix: Could not open file " << emc_error_file_name << " for Emc error matrix parameters";
  } else {
    errorMatrixfile->GetObject("PndEmcErrorMatrixParObject",fErrorMatrixParObject);
    if (fErrorMatrixParObject == nullptr) {
      LOG(error) << " PndEmcErrorMatrix: Could not get Emc error matrix information from file " << emc_error_file_name;
    }
  }
}

PndEmcErrorMatrix::~PndEmcErrorMatrix(){}

PndEmcErrorMatrixParObject* PndEmcErrorMatrix::GetParObject()
{
	return fErrorMatrixParObject;
}

	

TMatrixD PndEmcErrorMatrix::GetErrorMatrix(const PndEmcCluster &cluster) const {
	Double_t clEnergy=cluster.GetEnergy();
	Double_t clTheta=cluster.theta();
	Double_t clPhi=cluster.phi();
	
	// In which part of EMC the cluster is located
	// 1,2 - barrel, 3 - fwd endcap, 4- backward endcap, 5 - shashlyk 
	Int_t module=cluster.GetModule();
	
	enum {barrel, fwcap, bwcap, fsc};
	Int_t clusterInComponent=-1; //in which detector component is the cluster
	
	if( (module==1)||(module==2) ){
		clusterInComponent = barrel;
	}else if( module==3 ){
		clusterInComponent = fwcap;
	}else if( module==4 ){
		clusterInComponent = bwcap;
	}else if( module==5 ){
		clusterInComponent = fsc;	
	}else{
		std::cout<<"Incorrect EMC module in PndEmcErrorMatrix: Module = "<<module<<std::endl;
		abort();
	}
	
	//functions used for parametrization
	//Energy: Delta(E)/E = (a^2/E^power) + const^2 + (quadr/E)^2
	//position: Delta(x)=(a*a/E^power) + const^2
	Double_t pars[10];
	fErrorMatrixParObject->GetErrorMatrix(clusterInComponent, pars);
	Double_t engParA, engPower, engConst, engQuadr, pos1ParA, pos1Power, pos1Const, pos2ParA, pos2Power, pos2Const;
	engParA=pars[0];
	engPower=pars[1];
	engConst=pars[2];
	engQuadr=pars[3];
	pos1ParA=pars[4];
	pos1Power=pars[5];
	pos1Const=pars[6];
	pos2ParA=pars[7];
	pos2Power=pars[8];
	pos2Const=pars[9];
	
	//errors on position are estimated w/ respect to 
	//nominal z=100. (for fwcap, bwcap, fsc) ---> dx(z) and dy(z) 
	//and 
	//nominal R=100. (for barrel)   ---> dz(r) and dphi(r)
	//calculated errors are scaled with the following factors
	//
	Double_t barrelRadius=54.;
	Double_t bwCapPosZ = 56.;
	Double_t fwCapPosZ = 208.2;
	Double_t fscPosZ =   780.;

	Double_t scaleFactor[4];
	scaleFactor[bwcap]=bwCapPosZ/100.;
	scaleFactor[barrel]=barrelRadius/100.;
	scaleFactor[fwcap]=fwCapPosZ/100.;
	scaleFactor[fsc]=fscPosZ/100.;

	Double_t energyMinCutOff[4]={ 0.1, 0.1, 0.1, 0.5 };
	Double_t energyMaxCutOff[4]={ 1.5, 5., 7., 7. };
	Double_t eng=clEnergy;
	if(clEnergy>energyMaxCutOff[clusterInComponent]) eng=energyMaxCutOff[clusterInComponent];
	if(clEnergy<energyMinCutOff[clusterInComponent]) eng=energyMinCutOff[clusterInComponent];

	//std::cout << "+++++++++ Cluster in " << i << std::endl;
	//enrgy error
	Double_t errEnergy = engParA*engParA/pow(eng,engPower) 
			+ pow(engConst, 2.0) 
			+ pow(engQuadr/eng,2.0) ;
	//std::cout << "Error on energy for " << clEnergy << " GeV is " << errEnergy << std::endl;
	errEnergy*=clEnergy;
	
	//position coordinate errors
	Double_t pos1Err= pow(pos1ParA,2.)/pow(eng,pos1Power)+pow(pos1Const,2.);
	//std::cout << "Error on pos1 for " << clEnergy << " GeV is " << pos1Err << std::endl;
	pos1Err*=scaleFactor[clusterInComponent];
	
	Double_t pos2Err= pow(pos2ParA,2.)/pow(eng,pos2Power)+pow(pos2Const,2.);
	//std::cout << "Error on pos2 for " << clEnergy << " GeV is " << pos2Err << std::endl;
	if(clusterInComponent!=barrel)
		pos2Err*=scaleFactor[clusterInComponent];
	
	
	//error matrix for E, pos1, pos2 and arbitrary pos3
	TMatrixD theError(3,3);
	theError(0,0)=errEnergy*errEnergy;
	theError(1,1)=pos1Err*pos1Err;
	theError(2,2)=pos2Err*pos2Err;
	
	//std::cout << "theError before transformation: " << theError << std::endl;
	
	Double_t sin_theta=sin(clTheta);
	Double_t sin_phi  =sin(clPhi);
	Double_t cos_theta=cos(clTheta);
	Double_t cos_phi  =cos(clPhi);
	
	
	TMatrixD trans(4,3);
	if(clusterInComponent==barrel){
		barrelRadius=scaleFactor[barrel]*100.;
		
		trans(0,0)=1.;                       //dE/dE
		trans(0,1)=0.;                       //dE/dz
		trans(0,2)=0.;                       //dE/dphi
		
		trans(1,0)=0.;                       //dTheta/dE
		trans(1,1)=-sin_theta*sin_theta/barrelRadius;  //dTheta/dz
		trans(1,2)=0.;                       //dTheta/dphi
		
		trans(2,0)=0.;                       //dPhi/dE
		trans(2,1)=0.;                       //dPhi/dz
		trans(2,2)=1.;                       //dPhi/dphi
		
		trans(3,0)=0.;                       //dR/dE
		trans(3,1)=0.;                       //dR/dz
		trans(3,2)=0.;                       //dR/dphi
		
	}else{
		Double_t R=fabs(scaleFactor[clusterInComponent]*100./cos_theta); //a bit ugly, but theta is never 90 deg for these components 
		//trans(row, col)
		trans(0,0)=1.;                       //dE/dE
		trans(0,1)=0.;                       //dE/dx
		trans(0,2)=0.;                       //dE/dy
		
		trans(1,0)=0.;                       //dTheta/dE
		trans(1,1)= cos_theta * cos_phi /R;  //dTheta/dx
		trans(1,2)= cos_theta * sin_phi /R;  //dTheta/dy
		
		trans(2,0)=0.;//dPhi/dE
		trans(2,1)= -sin_phi /(R*sin_theta); //dPhi/dx
		trans(2,2)= cos_phi/(R*sin_theta);   //dPhi/dy
		
		trans(3,0)=0.;                       //dR/dE
		//trans(4,2)= sin_theta * cos_phi /R;  //dR/dx
		//trans(4,3)= sin_theta * sin_phi /R;  //dR/dy
		trans(3,1)= 0.0;  //dR/dx
		trans(3,2)= 0.0;  //dR/dy
	
	}
	
	// Error matrix in (E, theta, phi, R)
	TMatrixD errorMatrix=similarityWith(theError,trans);
	
	return errorMatrix;
	
}

TMatrixD PndEmcErrorMatrix::Get4MomentumErrorMatrix(const PndEmcCluster &cluster) const {
	// Conversion from (E, theta, phi, r) to ( px, py, pz, E )
	
	double z_cluster = cluster.where().Z();
	double perp = cluster.where().Perp();
	double mag = cluster.where().Mag();
	double cos_theta = z_cluster / mag;
	double sin_theta = perp / mag;
	double sin_phi = cluster.where().Y() / perp;
	double cos_phi = cluster.where().X() / perp;
	double e=cluster.GetEnergy();
	double p = e;

	// Create a matrix to transform the error matrix
	TMatrixD toComp( 4, 4 );
	toComp(0,0) = sin_theta * cos_phi * e/p;
	toComp(0,1) = p * cos_theta * cos_phi;
	toComp(0,2) = -p * sin_theta * sin_phi;
	toComp(0,3) = 0;
	toComp(1,0) = sin_theta * sin_phi * e/p;
	toComp(1,1) = p * cos_theta * sin_phi;
	toComp(1,2) = p * sin_theta * cos_phi;
	toComp(1,3) = 0;
	toComp(2,0) = cos_theta * e/p;
	toComp(2,1) = -p * sin_theta;
	toComp(2,2) = 0;
	toComp(2,3) = 0;
	toComp(3,0) = 1;
	toComp(3,1) = 0;
	toComp(3,2) = 0;
	toComp(3,3) = 0;

	TMatrixD errorMatrix=similarityWith(GetErrorMatrix(cluster),toComp);
	
	return errorMatrix;
}


TMatrixD PndEmcErrorMatrix::GetErrorP7(const PndEmcCluster &cluster) const {
	// Conversion from (E, theta, phi, r) to ( x, y, z, px, py, pz, E )
	
	double z_cluster = cluster.where().Z();
	double perp = cluster.where().Perp();
	double mag = cluster.where().Mag();
	double cos_theta = z_cluster / mag;
	double sin_theta = perp / mag;
	double sin_phi = cluster.where().Y() / perp;
	double cos_phi = cluster.where().X() / perp;
	double e=cluster.GetEnergy();
	double p = e;

	// Create a matrix to transform the error matrix
	TMatrixD toComp( 7, 4 );
	toComp(0,0) = 0;
	toComp(0,1) = mag * cos_theta * cos_phi;
	toComp(0,2) = -mag* sin_theta * sin_phi;
	toComp(0,3) = sin_theta * cos_phi;
	toComp(1,0) = 0;
	toComp(1,1) = mag * cos_theta * sin_phi;
	toComp(1,2) = mag * sin_theta * cos_phi;
	toComp(1,3) = sin_theta * sin_phi;
	toComp(2,0) = 0;
	toComp(2,1) = -mag * sin_theta;
	toComp(2,2) = 0;
	toComp(2,3) = cos_theta;
	toComp(3,0) = sin_theta * cos_phi;
	toComp(3,1) = p * cos_theta * cos_phi;
	toComp(3,2) = -p * sin_theta * sin_phi;
	toComp(3,3) = 0;
	toComp(4,0) = sin_theta * sin_phi;
	toComp(4,1) = p * cos_theta * sin_phi;
	toComp(4,2) = p * sin_theta * cos_phi;
	toComp(4,3) = 0;
	toComp(5,0) = cos_theta;
	toComp(5,1) = -p * sin_theta;
	toComp(5,2) = 0;
	toComp(5,3) = 0;
	toComp(6,0) = 1;
	toComp(6,1) = 0;
	toComp(6,2) = 0;
	toComp(6,3) = 0;

	TMatrixD errorMatrix=similarityWith(GetErrorMatrix(cluster),toComp);
	
	return errorMatrix;
}

// Function is copied from BbrGeom/BbrError.cc
// It does the same as m1*mat*m1^T
// but with assumption that mat, and output matrix are symmetric

TMatrixD similarityWith(const TMatrixD& mat, const TMatrixD& m1)
{
	TMatrixD result(m1.GetNrows(),m1.GetNrows());
	
	TMatrixD temp = m1*mat;
	double tmp;
	
	for (int r = 0; r < m1.GetNrows(); r++) {
		for (int c = 0; c <= r; c++) {
			tmp = 0.;
			for (int k = 0; k < m1.GetNcols(); k++) {
				tmp += temp(r,k)*m1(c,k);
			}
			result(r,c) = tmp;
			// Modification from original code, to make output matrix explicitly symmetric
			// In original babar code symmetric matrix were stored as lower triangular.
			if (r!=c)
				result(c,r) = tmp;
		}
	}
	return result;
}
