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

#include "PndEmcShapingNoiseAdder.h"
#include "PndEmcAbsWaveformSimulator.h"

#include "PndEmcWaveform.h"
#include "TRandom.h"
#include "TMath.h"

PndEmcShapingNoiseAdder::PndEmcShapingNoiseAdder() : 
	fShapingTime(0),
	fNoiseWidth(0.),
	fInputScale(0.0),
	fSingleNoiseWidth(0.0),
	fMA_buffer(0) {
}

PndEmcShapingNoiseAdder::PndEmcShapingNoiseAdder(Double_t shapingTime, Double_t sampleRate, Double_t noiseWidth, Double_t inputScale) :
	fShapingTime(shapingTime),
	fNoiseWidth(noiseWidth),
	fInputScale(inputScale),
	//MA buffer size as long as no of samples forming rising edge of wf

	fMA_buffer((Int_t) (shapingTime*sampleRate)) {
                   //std::cout << " shapingTime " <<shapingTime<<" sampleRate "<<sampleRate<<"noiseWidth "<<noiseWidth<<  std::endl;
		Int_t bufferSize = fMA_buffer.GetSize();
                //std::cout << "-E bufferSize" <<bufferSize<< std::endl;
		if(!bufferSize) {
			std::cout << "-E PndEmcShapedNoiseWaveformSimulator: pulse raises in less than one sample. Noise generation will fail" << std::endl;
		}

		//noise of individual sample is averaged over bufferSize samples --> fNoiseWidth² = bufferSize * fSingleNoiseWidth²
		fSingleNoiseWidth = fNoiseWidth / TMath::Sqrt(bufferSize);			//still in GeV

		//Init buffer
		while(bufferSize--) { 								//init buffer
			fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale));
                    //    std::cout << " fMA_buffer " <<fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale))<<" fInputScale "<<fInputScale<<" fSingleNoiseWidth "<<fSingleNoiseWidth<< std::endl;
		}
	}


void PndEmcShapingNoiseAdder::Modify(PndEmcWaveform* wf) {

	for(std::vector<Double_t>::iterator it=GetWaveformReference(wf).begin(); it!=GetWaveformReference(wf).end(); ++it) { 
	
               if(fNoiseWidth>0.01){(*it) += fMA_buffer(gRandom->Gaus(0.0005, fSingleNoiseWidth*fInputScale));}
                  else{(*it) += fMA_buffer(gRandom->Gaus(0.0005/16, fSingleNoiseWidth*fInputScale));} //smear with shaping noise
                  //  (*it) += fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale));
                 //(*it) +=0;
                   // Int_t bufferSize = fMA_buffer.GetSize();
                   //std::cout << " fMA_buffer.GetSize() "<<fMA_buffer.GetSize()<<std::endl;
                   //std::cout << " fMA_buffer " <<fMA_buffer(gRandom->Gaus(0, fSingleNoiseWidth*fInputScale))<<" fInputScale "<<fInputScale<<" fSingleNoiseWidth "<<fSingleNoiseWidth<<  std::endl;
                   
	}
}
