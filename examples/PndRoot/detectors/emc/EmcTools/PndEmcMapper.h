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
//	EMC Mapper class. Map crystal index (fDetectorId) to two coordinate index.
// Author List:
// Dima Melnychuk
//
/////////////////////////////////////////////////////////////// 

//#pragma once
#ifndef PNDEMCMAPPER_H
#define PNDEMCMAPPER_H

#include "PndEmcTwoCoordIndex.h"
#include "TObject.h"
#include "TString.h"
#include <map>

/**
 * @brief Emc geometry mapper
 * @ingroup PndEmc
 */
class PndEmcMapper: public TObject
{

public:
	static PndEmcMapper* Instance();
	static void Init(Int_t MapVersion);
	const std::map<Int_t,PndEmcTwoCoordIndex* >& GetTciMap();
	
	/** Destructor **/
	virtual ~PndEmcMapper();    
	
	/** Accessors **/
	Int_t GetMapVersion()  const { return fMapVersion ;};
	
	void Add(PndEmcMapper*);
	
	PndEmcTwoCoordIndex* GetTCI(Int_t DetectorId);
protected:
	std::map<Int_t,PndEmcTwoCoordIndex* > fIntTwoCoordMap;
	PndEmcMapper();  
private:
	
	static PndEmcMapper* _instance;
	static Int_t fMapVersion;
	Int_t GetDetId(Int_t iTheta,Int_t iPhi);

        
	ClassDef(PndEmcMapper,1)
};

class PndEmcMapperGeo12Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo12Dat();
};

class PndEmcMapperGeo12Root: public PndEmcMapper
{
  public:
    PndEmcMapperGeo12Root();
};

class PndEmcMapperGeo3Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo3Dat();
};

class PndEmcMapperGeo3Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo3Root();
};

class PndEmcMapperGeo3RootV2: public PndEmcMapper
{
	public:
		PndEmcMapperGeo3RootV2();
};

class PndEmcMapperGeo4Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo4Dat();
};

class PndEmcMapperGeo4Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo4Root();
};

class PndEmcMapperGeo4RootV2: public PndEmcMapper
{
	public:
                PndEmcMapperGeo4RootV2();
};

class PndEmcMapperGeo5Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo5Dat();
};

class PndEmcMapperGeo5Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeo5Root();
};

class PndEmcMapperGeo6Dat: public PndEmcMapper
{
	public:
		PndEmcMapperGeo6Dat();
};

class PndEmcMapperGeoProto60Root: public PndEmcMapper
{
	public:
		PndEmcMapperGeoProto60Root();
};

#endif //PndEmcMapper_H
