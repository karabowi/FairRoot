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

/**
 * @class PndMultiCanvasCreator
 * @brief Creates new canvasses once the number of histograms per canvas exceeds a setable threshold
 * @detailed Automatically distributes any number of histograms to a canvas and creates a new canvas once
 * the number of pads per canvas is reached. In addition it is able to store the canvasses in various formats.
 * The default format is "png".
 *
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * @date 2021-05-21
 */

#ifndef TOOLS_PndMultiCanvasCreator_H_
#define TOOLS_PndMultiCanvasCreator_H_

#include <TString.h>
#include <TCanvas.h>
#include <vector>

class PndMultiCanvasCreator : public TObject {
 public:
  PndMultiCanvasCreator();
  virtual ~PndMultiCanvasCreator();

  void SelectPad(int padNumber);
  void SaveCanvasses();

  void SetDimensions(int x = 800, int y = 600)
  {
    fDimX = x;
    fDimY = y;
  }
  void SetPicPerCan(int val) { fPicPerCan = val; }
  void SetPrefix(TString val) { fCanPrefix = val; }
  void SetOutputDir(TString val) { fOutputDir = val; }
  void SetOutputFormat(TString val) { fOutputFormat = val; } //< defines the output format of the saved canvasses. See root's TPad::SaveAs for possible options

  /**
   * OutputFileNames are only available after SaveCanvasses() has been called
   */
  std::vector<TString> GetOutputFileNames() { return fOutputFileNames; }

 protected:
  TCanvas *createCanvas();

 private:
  std::vector<TCanvas *> fCanvasses;
  std::vector<TString> fOutputFileNames;
  TString fCanPrefix = "Can";
  TString fOutputDir = ".";
  TString fOutputFormat = "gif";
  Int_t fPicPerCan = 9;
  Int_t fDimX = 0;
  Int_t fDimY = 0;
  ClassDef(PndMultiCanvasCreator, 1);
};

#endif /* TOOLS_PndMultiCanvasCreator_H_ */
