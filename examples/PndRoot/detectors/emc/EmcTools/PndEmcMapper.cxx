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

/////////////////////////////////////////////////////////////
// Description:
//	EMC Mapper class. Map crystal index (fDetectorId) to two coordinate index.
//
// Author List:
// Dima Melnychuk
// 27/04/2007 Stefano Spataro (encoding for the Fwd Endcup)
//
/////////////////////////////////////////////////////////////// 

#include "PndEmcMapper.h"
#include "PndEmcTwoCoordIndex.h"
		
#include <iostream>
#include <fstream>		

using namespace std;

PndEmcMapper *PndEmcMapper::_instance = nullptr;
Int_t PndEmcMapper::fMapVersion = 0;
   
PndEmcMapper::PndEmcMapper(): fIntTwoCoordMap()
{
} 
void PndEmcMapper::Init(Int_t mapVersion)
{
	if (mapVersion==0)
	{
		cout<<"Emc mapper version 0 does not exist"<<endl;
	}
	else if (mapVersion!=fMapVersion)
	{	
		fMapVersion=mapVersion;
		switch (mapVersion) {
			case 1:
				_instance = new PndEmcMapperGeo12Root();
				_instance->Add(new PndEmcMapperGeo3RootV2());
				_instance->Add(new PndEmcMapperGeo4RootV2());
				_instance->Add(new PndEmcMapperGeo5Root());
				break;
			case 2:
				_instance = new PndEmcMapperGeo12Dat();
				_instance->Add(new PndEmcMapperGeo3Dat());
				_instance->Add(new PndEmcMapperGeo4Dat());
				_instance->Add(new PndEmcMapperGeo5Dat());
				break;
			case 3:
				_instance = new PndEmcMapperGeo12Dat();
				_instance->Add(new PndEmcMapperGeo3Root());
				_instance->Add(new PndEmcMapperGeo4RootV2());
				_instance->Add(new PndEmcMapperGeo5Root());
				break;
			case 4:
				_instance = new PndEmcMapperGeo3RootV2();
				break;
			case 5:
				_instance = new PndEmcMapperGeo5Root();
				break;
			case 6:
				_instance = new PndEmcMapperGeo6Dat();
				break;
			case 7:
				_instance = new PndEmcMapperGeoProto60Root();
				break;
			case 8: // old backward emc mappping (geom. version 2008)
				_instance = new PndEmcMapperGeo4Root();
				break;
			case 9: // old barrel emc mapping
				_instance = new PndEmcMapperGeo12Dat();
				_instance->Add(new PndEmcMapperGeo3RootV2());
				_instance->Add(new PndEmcMapperGeo4RootV2());
				_instance->Add(new PndEmcMapperGeo5Root());
				break;
			default :
				cout<<"Emc Mapper version "<<mapVersion<<" is not defined"<<endl;
		} 
	}
}

PndEmcMapper* PndEmcMapper::Instance ()
{
  if (_instance == nullptr) {
    cout<<"Emc mapper should be initialised first with PndEmcMapper::Init()"<<endl;
    return nullptr;
  }
  return _instance;
}

void PndEmcMapper::Add(PndEmcMapper *newMapper)
{
	std::map<Int_t,PndEmcTwoCoordIndex* > newMapp = newMapper->GetTciMap();
	
	fIntTwoCoordMap.insert(newMapp.begin(),newMapp.end());
}

// -----   Destructor   -----------------------------------
PndEmcMapper::~PndEmcMapper()
{
	delete _instance;
	for(std::map<Int_t,PndEmcTwoCoordIndex* >::iterator iter = fIntTwoCoordMap.begin();
	    iter != fIntTwoCoordMap.end(); ++iter){
		delete (*iter).second;
	}
	fIntTwoCoordMap.clear();
}
// --------------------------------------------------------

PndEmcMapperGeo12Dat::PndEmcMapperGeo12Dat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	// Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-2
	// 72(iTheta)x160(iPhi) for barrel part
	// index iTheta from 1 (backward part) till 72 (forward)
	for (Int_t module=1; module<=2; module++)
	{
		if (module ==2)
		{
		for (Int_t row=1; row<=29;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					if ((copy==1 || copy==9) && (crystal>=4 && crystal<=6) && (row<=3)) continue;
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=-row+30;
				
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}	
		}
		else if (module ==1)
		{
		for (Int_t row=1; row<=43;row++)
			for (Int_t crystal=1; crystal<=10;crystal++)
				for (Int_t copy=1; copy<=16;copy++)
				{
					iPhi=(11-crystal)+(copy-1)*10;
					iTheta=row+29;
					
					detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
					_tci=new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
					fIntTwoCoordMap[detId]=_tci;
				}
		}
	}
}

PndEmcMapperGeo12Root::PndEmcMapperGeo12Root()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
        // Crys 1-5000; copyNo 1-20; nRow 1-100, nMod 1-2
	// 72(iTheta)x160(iPhi) for barrel part
	// index iTheta from 1 (backward part) till 72 (forward)
	for (Int_t module = 1; module <= 2; module++) {
		if (module == 2) {
			for (Int_t row = 1; row <= 28; row++) {
				for (Int_t crystal = 1; crystal <= 10; crystal++) {
					for (Int_t copy = 1; copy <= 16; copy++) {
						if ((copy==1 || copy==9) && (crystal>=3 && crystal<=8) && (row<=3)) continue;
						iPhi = (11-crystal)+(copy-1)*10;
						iTheta = -row + 29;

						detId = module*100000000 + row*1000000 + copy*10000 + crystal;
						_tci = new PndEmcTwoCoordIndex(iTheta, iPhi, detId);
						fIntTwoCoordMap[detId] = _tci;
					}
				}
			}
		}
		if (module == 1) {
			for (Int_t row = 1; row <= 43; row++) {
				for (Int_t crystal = 1; crystal <= 10; crystal++) {
					for (Int_t copy=1; copy<=16;copy++) {
          				if ((copy==1 || copy==9) && (crystal>=3 && crystal<=8) && (row<=3)) continue;
						iPhi = (11-crystal)+(copy-1)*10;
						iTheta = row + 28;

						detId = module*100000000 + row*1000000 + copy*10000 + crystal;
						_tci = new PndEmcTwoCoordIndex(iTheta,iPhi,detId);
						fIntTwoCoordMap[detId] = _tci;
					}
				}
			}
		} // if module
	}
}

PndEmcMapperGeo3Dat::PndEmcMapperGeo3Dat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t   detId, detId_tmp, iX, iY;  //iTheta,iPhi, //[R.K. 01/2017] unused variable
	detId = detId_tmp =iX = iY = 0;  //iTheta = iPhi =  //[R.K. 01/2017] unused variable
	
	Int_t module =3;
	for (Int_t row=1; row<=40;row++)
		for (Int_t crystal=1; crystal<=40;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
			{
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				
				_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
				fIntTwoCoordMap[detId]=_tci;
			}	
}

PndEmcMapperGeo3Root::PndEmcMapperGeo3Root()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	Int_t module =3;
	for (Int_t row=1; row<=36;row++)
		for (Int_t crystal=1; crystal<=36;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
			{
				if (copy==1)   {	iX = -row+1; iY =  crystal;   }
				if (copy==2)   {	iX = -row+1; iY = -crystal+1; }
				if (copy==3)   {	iX =  row;   iY = -crystal+1; }
				if (copy==4)   {     iX =  row;   iY =  crystal;   }			   
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
				
				_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);
				fIntTwoCoordMap[detId]=_tci;
			}
}

PndEmcMapperGeo3RootV2::PndEmcMapperGeo3RootV2()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	Int_t module =3;
	for (Int_t row = -37; row <= 37; row++)
		for (Int_t col = -36; col <= 36; col++)
			if (row != 0 && col != 0)//in copy numbering of the crystals in the geometry root file, there's no CrystalCol=0 or CrystalRow=0
			{
			detId =  module*100000000 + (row+37)*1000000 + (col+36);
		
			iX = -col; //the minus sign before 'iX' is introduced, since the geometry of FwEndCap gets rotated by 180 deg around the y-axis in PndEmc.cxx
			iY = row;
			if (iX >0) // these two if conditions are introduced here to make the mapping down here (_tci) a continuous pattern
				iX -= 1;
			if (iY >0)
				iY -= 1;
			_tci=new PndEmcTwoCoordIndex(iX+250,iY+250,detId);

			fIntTwoCoordMap[detId]=_tci;
			}
}

PndEmcMapperGeo4Dat::PndEmcMapperGeo4Dat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	Int_t module =4;
	for (Int_t row=1; row<=18;row++)
		for (Int_t crystal=1; crystal<=18;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
			{
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
			
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				
				_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
				fIntTwoCoordMap[detId]=_tci;
			}	
}

PndEmcMapperGeo4Root::PndEmcMapperGeo4Root()
{
   // *** for testing of BwEndCap ONLY --> from "emc_module4_*.root" file ***
   // 26.02.2009
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	
	Int_t module=4;
	for (Int_t row=1; row<=16;row++)
		for (Int_t crystal=1; crystal<=16;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
	      {
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
				
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				_tci=new PndEmcTwoCoordIndex(iX+350,iY+350,detId);
				fIntTwoCoordMap[detId]=_tci;
	      }
}

PndEmcMapperGeo4RootV2::PndEmcMapperGeo4RootV2()
{
   // *** 2017 version of the BwEndCap geometry ***
  	PndEmcTwoCoordIndex *_tci;
	Int_t  detId, iX, iY, iXrel, iYrel, rowc;
	detId = iX = iY = iXrel = iYrel = rowc = 0;

	Int_t module=4;

	for (Int_t row=0; row<10;row++){ //-- loop over submodules in one quarter (called row to keep
                                         //-- nomenclature of PndEmcStructure)
	  for (Int_t crystal=0; crystal<16;crystal++){ //-- loop over crystals in one submodule

	    if( row < 6 ){ // side submodules

	      iXrel =  4*(row%3) + (crystal%4) -  6;
	      iYrel = -4*(row/3) - (crystal/4) + 14;

	    } else { // corner submodules
	      rowc = row - 6;

	      iXrel =  4*(rowc%2) + (crystal%4) +  6;
	      iYrel = -4*(rowc/2) - (crystal/4) + 13;

	    }

	    for (Int_t copy=0; copy<4;copy++){ //-- loop over quarters

	      if (copy==0) {  iX =  iXrel  ; iY =  iYrel  ; }
	      if (copy==1) {  iX = -iYrel-1; iY =  iXrel  ; }
	      if (copy==2) {  iX = -iXrel-1; iY = -iYrel-1; }
	      if (copy==3) {  iX =  iYrel  ; iY = -iXrel-1; }

	      detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
	      _tci=new PndEmcTwoCoordIndex(iX+351,iY+351,detId);
	      fIntTwoCoordMap[detId]=_tci;
	    }
	  }
	}
}



PndEmcMapperGeo5Dat::PndEmcMapperGeo5Dat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	Int_t module =5;
	
	for (Int_t row=1; row<=14;row++)
		for (Int_t crystal=1; crystal<=7;crystal++)
			for (Int_t copy=1; copy<=4;copy++)
			{
				if (copy==1) {  iX = -row+1; iY =  crystal;   }
				if (copy==2) {  iX = -row+1; iY = -crystal+1; }
				if (copy==3) {  iX =  row;   iY = -crystal+1; }
				if (copy==4) {  iX =  row;   iY =  crystal;   }
			
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
				
				_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
				fIntTwoCoordMap[detId]=_tci;
			}	
}

PndEmcMapperGeo5Root::PndEmcMapperGeo5Root()
{
   // *** for testing of Fsc ONLY --> from "emc_module5_*.root" file ***
   // 20.04.2010
  	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;

	Int_t module=5;
	for (Int_t row=1; row<=28;row++)
		for (Int_t crystal=1; crystal<=54;crystal++)
	      {
				Int_t copy = 1;
				iX = row;
				iY =  crystal;
				detId =  module*100000000 + row*1000000 + copy*10000 + crystal;
				_tci=new PndEmcTwoCoordIndex(iX+450,iY+450,detId);
				fIntTwoCoordMap[detId]=_tci;
	      }
}

PndEmcMapperGeo6Dat::PndEmcMapperGeo6Dat()
{
	PndEmcTwoCoordIndex *_tci;
	Int_t iTheta, iPhi, detId, detId_tmp, iX, iY;
	iTheta = iPhi = detId = detId_tmp =iX = iY = 0;
	Int_t module =6;
	for (Int_t row=1; row<=5;row++)
	for (Int_t crystal=1; crystal<=5;crystal++)
	{
		Int_t copy = 1;
		iX = row; 
		iY =  crystal; 
		
		detId =  module*100000000 + row*1000000 + copy*10000 + crystal;	
		
		_tci=new PndEmcTwoCoordIndex(iX+550,iY+550,detId);
		fIntTwoCoordMap[detId]=_tci;
	}	

}

PndEmcMapperGeoProto60Root::PndEmcMapperGeoProto60Root(){
	//Proto60
	//18.01.10
	PndEmcTwoCoordIndex *_tci;
	Int_t detId = 0;
	Int_t module = 7;
	Int_t copy = 1;
	for(Int_t row=1; row<=6; row ++)
		for(Int_t crystal=1;crystal<=10;crystal++)
		{
			detId = module*100000000 + row*1000000 + copy*10000 + crystal;
			_tci = new PndEmcTwoCoordIndex(row+1000,crystal+1000,detId);
			fIntTwoCoordMap[detId]=_tci;
		}
}

PndEmcTwoCoordIndex* PndEmcMapper::GetTCI(Int_t DetectorId)
{
	PndEmcTwoCoordIndex* tci= fIntTwoCoordMap[DetectorId];
	return tci;
}



const std::map<Int_t,PndEmcTwoCoordIndex* >& PndEmcMapper::GetTciMap()
{
	return fIntTwoCoordMap;
}

Int_t PndEmcMapper::GetDetId(Int_t iTheta,Int_t iPhi)
{
  Int_t module,row,copy,crystal,detId;
  module = row = copy = crystal = detId = 0;
	if (iTheta<=29) 
	{
		module=2;
		row=30-iTheta;
		copy=(iPhi-1)/10+1;
		crystal=10 - (iPhi-1)%10;
	}
	else if ((iTheta>29)&&(iTheta<73))
	{
		module=1;
		row=iTheta-29;
		copy=(iPhi-1)/10+1;
		crystal=10 - (iPhi-1)%10;
	}
	
	////////////////////////////////////////////////// mapping for FwEndCap geometry, 2011
	if (iTheta>200 && iTheta<300)
	  {
	    module=3; 
	    copy = 0;
	    if (iTheta<250 && iPhi>=250) {
	      row = (iPhi-250)+1+37;
	      crystal = -(iTheta-250)+36;
	    }
	    else if (iTheta<250 && iPhi<250) {  
	      row = (iPhi-250)+37;
	      crystal = -(iTheta-250)+36;
	    }
	    else if (iTheta>=250 && iPhi>=250) {
	      row = (iPhi-250)+1+37;
	      crystal = -(iTheta-250+1)+36;
	    }
	    else if (iTheta>=250 && iPhi<250) {
	      row = (iPhi-250)+37;
	      crystal = -(iTheta-250+1)+36;
	    }
	  }
         //////////////////////////////////////////////////

	if ((iTheta>300)&&(iTheta<400))
	  {
	    module=4; 
	    if ((iTheta<=350)&& (iPhi>350))
	      {
		copy = 1;  row = 1-(iTheta-350);  crystal = iPhi-350;
	      }
	    if ((iTheta<=350)&& (iPhi<=350))
	      {
		copy = 2;  row = 1-(iTheta-350);    crystal = 1-(iPhi-350);
	      }
	    if ((iTheta>350)&& (iPhi<=350))
	      {
		copy = 3;  row = (iTheta-350);    crystal = 1-(iPhi-350);
	      }
	    if ((iTheta>350)&& (iPhi>350))
	      {
		copy = 4;  row = (iTheta-350);        crystal = iPhi-350;
	      }
	  }
	if ((iTheta>400)&&(iTheta<500))
	  {
	    module=5; 
	    if ((iTheta<=450)&& (iPhi>450))
	      {
		copy = 1;  row = 1-(iTheta-450);  crystal = iPhi-450;
	      }
	    if ((iTheta<=450)&& (iPhi<=450))
	      {
		copy = 2;  row = 1-(iTheta-450);    crystal = 1-(iPhi-450);
	      }
	    if ((iTheta>450)&& (iPhi<=450))
	      {
		copy = 3;  row = (iTheta-450);    crystal = 1-(iPhi-450);
	      }
	    if ((iTheta>450)&& (iPhi>450))
	      {
		copy = 4;  row = (iTheta-450);        crystal = iPhi-450;
	      }
	  }
	if ((iTheta>500)&&(iTheta<600))
	  {
	    module=6; 
	    copy = 1;  
	    row = iTheta-550;
	    crystal = iPhi-550;
	  }
	
	detId =  module*100000000 + row*1000000 + copy*10000 + crystal;

   return detId;
}

ClassImp(PndEmcMapper)
