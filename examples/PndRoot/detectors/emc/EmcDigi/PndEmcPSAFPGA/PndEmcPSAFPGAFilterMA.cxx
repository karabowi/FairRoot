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

#include "PndEmcPSAFPGAFilterMA.h"

PndEmcPSAFPGAFilterMA::PndEmcPSAFPGAFilterMA() : 
	buffer(0)
	, bpointer(0)
{
}

PndEmcPSAFPGAFilterMA::~PndEmcPSAFPGAFilterMA()
{
	buffer.resize(0);
}

void PndEmcPSAFPGAFilterMA::set(unsigned int newBufferSize)
{
	buffer.resize(newBufferSize);
	for(unsigned int i=0; i<newBufferSize; i++)
		buffer[i]=0;
	bpointer=0;
	sum = 0;
}

void PndEmcPSAFPGAFilterMA::resetToZero()
{
	for(unsigned int i=0; i<buffer.size(); i++)
		buffer[i] = 0;
	sum = 0;
	//bpointer =0;
}

float PndEmcPSAFPGAFilterMA::put(float valueToStore)
{
       
	bpointer %= buffer.size();
	sum += valueToStore - buffer[bpointer];
      //  std::cout<<"bpointer "<<bpointer<<" sum "<<sum<< " valueToStore "<< valueToStore<<" buffer[bpointer]  "<<buffer[bpointer]<<std::endl;
	buffer[bpointer] = valueToStore;
	 bpointer++;
	return sum/buffer.size();
}

