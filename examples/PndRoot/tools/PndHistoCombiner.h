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
 * @class PndHistoCombiner
 * @brief Combines histograms with the same name from different files in one overlay histogram
 * @details The class runs through all histograms or branches of the first file given and checks if histograms/branches with the same name
 * exist in the other files. If this is the case they are plotted into a common histogram via the option "same".
 * The legend of the histogram is filled with the short name given by the user. The histograms are saved in a canvas as jpg files.
 * The number of histograms per canvas can be set.
 * In addition it is possible to perform a Kolmogorov, Chisquare or Anderson test between the first and all other histograms in one stack.
 * If the test fail the word "FAILED" will be added to the title of the histogram. Furthermore, a cdash output can be created which
 * uploads the probability of the test to the cdash server.
 *
 * @author Tobias Stockmanns <t.stockmanns@fz-juelich.de>
 * @date 2021-06-01
 */

#ifndef TOOLS_PNDHISTOCOMBINER_H_
#define TOOLS_PNDHISTOCOMBINER_H_

#include <TObject.h>
#include <TString.h>

#include <vector>
#include <map>

#include "PndMultiCanvasCreator.h"

#include "FairLogger.h"

class TFile;
class TH1;

class PndHistoCombiner : public TObject {
 public:
  struct fileData {
    fileData(){};
    fileData(TString fileName, TString shortName) : fFileName(fileName), fShortName(shortName){};
    TString fFileName;
    TString fShortName;
    TFile *fFile = nullptr;
//    Double_t fP = -1.;
  };

  struct histoComparisonValues{
	  Double_t fChi2 = -1.; //< result/threshold for Chi2
	  Double_t fKol = -1.;  //< result/threshold for Kolmogorov-Smirnov test
	  Double_t fAD = -1.;   //< result/threshold for Anderson-Darling test
  };

  PndHistoCombiner();
  virtual ~PndHistoCombiner();

  /**
   * @brief Adds the file which contain the histos to be plotted together. First file is the one comparisons are made to.
   * @param fileName full path to file
   * @param shortName name displayed in histogram legend
   */

  void AddFileNamesAndShortName(TString fileName, TString shortName)
  {
    fileData myData(fileName, shortName);
    fFileData.push_back(myData);
  }

  /**
   * @brief Main method to generate histograms
   */
  int GenerateHistos();

  /**
   * @brief Main method to generate histograms from branches
   */
  int GenerateBranches();

  /**
   * @brief Only those histograms are combined with part of their matching the histoName
   * @param histoName part of the histo name to be compared
   */
  void AddHistoSelector(TString histoName) { fHistoSelector.push_back(histoName); }

  /**
   * @brief Only those branches are combined with part of their matching the branchName
   * @param branchName part of the histo name to be compared
   */
  void AddBranchSelector(TString branchName) { fBranchSelector.push_back(branchName); }

  /**
   *
   * @brief pictures per canvas. They will be organized in a quadratic way
   * @param val number of pictures/pads
   */
  void SetPicPerCan(int val) { fCanCreator.SetPicPerCan(val); }
  void SetCanvasPrefix(TString val) { fCanCreator.SetPrefix(val); }
  void SetCanvasOutputDir(TString val) { fCanCreator.SetOutputDir(val); }
  void SetCanvasOutputFormat(TString val) { fCanCreator.SetOutputFormat(val); }

  void SetPulls(bool dopulls = true)
  {
    fDoPulls = dopulls;
//    SetDimensions(400, 400);
  }

  void SetDimensions(int x = 400, int y = 300) { fCanCreator.SetDimensions(x, y); }

  /**
   * @brief perform Kolmogorov or chi2 test between histograms of first file and all others.
   * @param val test type. 0: none, 1: Kolmogorov test, 2: chisquare test
   * @param threshold for the probability of the test. Every p larger than threshold will be counted as passed
   */
  void SetGlobalThresholds(double thresholdKol = 0.03, double thresholdChi2 = -1.0, double thresholdAD = -1.0)
  {
	  fThresholds.clear();
	  AddHistoThresholds("", thresholdKol, thresholdChi2, thresholdAD);
  }

  void AddHistoThresholds(TString histoName, double thresholdKol = 0.03, double thresholdChi2 = -1.0, double thresholdAD = -1.0){
	  histoComparisonValues thresholds;
	  thresholds.fKol = thresholdKol;
	  thresholds.fChi2 = thresholdChi2;
	  thresholds.fAD = thresholdAD;
	  fThresholds.push_back(std::make_pair(histoName, thresholds));
  }

  void AddHistoThresholds(TString histoName, std::vector<double> thresholds){
	  if (thresholds.size() == 3)
		  AddHistoThresholds(histoName, thresholds[0], thresholds[1], thresholds[2]);
	  else {
		  LOG(warning) << "Wrong number of threshold values: " << histoName << " : " << thresholds.size();
	  }
  }

  /**
   * @brief Select to generate output string automatically to upload data to CDASH server
   */
  void SetCDashOutput(bool val = true) { fCreateCDashOutput = val; }
  /**
   * @brief Generate output string to upload data to CDASH server
   *        This can be called automatically or by hand
   */
  void CreateCDashOutput();

 protected:
  void InitFiles();
  bool CheckHistoName(TString histoName);
  bool CheckBranchName(TString branchName);
  bool CheckBranchType(TString branchType);
  void DrawAndTestHistoStack(std::vector<TH1 *> &histos);
  void DrawAndTest2DHistoStack(std::vector<TH1 *> &histos);
  std::pair<TPad *, TPad *> SplitPadForPulls();
  void DrawPulls(std::vector<TH1 *> &histos);
  void CreateLegend(std::vector<TH1 *> &histos);
  histoComparisonValues PerformTest(TH1 *h1, TH1 *h2);
  bool TestPassed(TString& histoTitle, histoComparisonValues results, histoComparisonValues thresholds)
  {
	bool result = true;
	if (thresholds.fKol > 0){
		if (results.fKol < thresholds.fKol)
			result = false;
		histoTitle += " P(K)= ";
		histoTitle += (TString::Format("%.2f", results.fKol));
	}
	if (thresholds.fChi2 > 0){
		if (results.fChi2 < thresholds.fChi2)
			result = false;
		histoTitle += " /P(#Chi^{2})= ";
		histoTitle += (TString::Format("%.2f", results.fChi2));
	}
	if (thresholds.fAD > 0){
		if (results.fAD < thresholds.fAD)
			result = false;
		histoTitle += " /P(AD)= ";
		histoTitle += (TString::Format("%.2f", results.fAD));
	}
	return result;
  }

  /**
   * @brief Add to output string to upload data to CDASH server
   */
  void AddCDashOutput(TString prefix, TString histoName, histoComparisonValues value);

  TString RemoveSpecialCharacters(TString input);

 private:
  std::vector<std::pair<TString, histoComparisonValues>> fThresholds;  //< (part of) histogram name, thresholds for comaprison
  std::vector<fileData> fFileData;
  std::vector<TString> fHistoSelector;
  std::vector<TString> fBranchSelector;
  std::vector<TString> fCDashOutput;
  PndMultiCanvasCreator fCanCreator;
  bool fCreateCDashOutput = false;
  int fFailCount = 0;
  int fPadNumber = 0;
  bool fDoPulls = false;

  ClassDef(PndHistoCombiner, 1);
};

#endif /* TOOLS_PNDHISTOCOMBINER_H_ */
