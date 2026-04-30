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

#include "PndEmcPSAFPGAFilterDelay.h"
#include <iostream>
PndEmcPSAFPGAFilterDelay::PndEmcPSAFPGAFilterDelay() : 
	buffer(0)
	, bpointer(0)
{
}

PndEmcPSAFPGAFilterDelay::~PndEmcPSAFPGAFilterDelay() {
	buffer.resize(0);
}

void PndEmcPSAFPGAFilterDelay::set(unsigned int newBufferSize) {
	buffer.resize(newBufferSize);
       //  std::cout<<" buffer_size " <<newBufferSize<<std::endl;
	for(unsigned int i=0; i<newBufferSize; i++)
		buffer[i]=0;
	bpointer=0;
}

float PndEmcPSAFPGAFilterDelay::put(float valueToStore) {
  
	bpointer %= buffer.size();
              //std::cout<<" bpointer "<<bpointer<<" buffer_size " <<buffer.size()<<std::endl;
	float out = buffer[bpointer];
	buffer[bpointer] = valueToStore;
	bpointer++;
	return out;
}
/*void PndEmcPSAFPGAFilterDelay::out() {
	for(unsigned int i=0; i<newBufferSize; i++)
		buffer[i]=0;
	bpointer=0;
}*/
void PndEmcPSAFPGAFilterDelay::resetToZero() {
	for(unsigned int i=0; i<buffer.size(); i++)
		buffer[i] = 0;
	bpointer = 0;
}
