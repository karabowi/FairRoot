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


#include "BSEmcPlotWaveforms.h"

#include <string>
#include <vector>

#include "PndContainerI.h"

#include "BSEmcHistogrammer.h"
#include "BSEmcMultiWaveform.h"

void BSEmcPlotWaveforms::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  t_register->Request({"", "BSEmcMultiWaveform", kFALSE, kFALSE});
}

void BSEmcPlotWaveforms::GetDataContainer(PndContainerRegister *t_register)
{
  fWaveformArray = t_register->GetDefaultBranch<BSEmcMultiWaveform>();
}

void BSEmcPlotWaveforms::InitHistos(const TString &t_folderprefix)
{
  SetFolderPrefix(t_folderprefix);
  TString foldername = t_folderprefix + "/ControlHistos/Waveforms";
  fHistogrammer->Create1DHist("WaveformNumber", foldername, {1000, -0.5, 999.5, "Number of waveforms per Event"});
  fHistogrammer->Create1DHist("WaveformTypes", foldername, {1000, -0.5, 999.5, "Number of waveforms per Hit"});
  fHistogrammer->Create2DHist("HighGainWaveforms", foldername, {512, -0.5, 511.5, "Sample"}, {100, 0, 1, "Normalized pulse height"});
}

void BSEmcPlotWaveforms::Process()
{
  fHistogrammer->Fill("WaveformNumber", fWaveformArray->GetSize(), 1);

  fHistogrammer->Fill("WaveformTypes", fWaveformArray->GetSize(), 1);
  TH2 *hist = fHistogrammer->Get2DHist("HighGainWaveforms");
  for (const BSEmcMultiWaveform *wf : fWaveformArray->GetVectorOfPtrToConst()) {
    Int_t bincounter = 0;
    const auto &samples = wf->GetSignal(0);
    const Double_t max = *std::max_element(samples.begin(), samples.end());
    for (Double_t signal : samples) {
      hist->Fill(bincounter, signal / max);
      ++bincounter;
    }
  }
  ++fEvent;
}
