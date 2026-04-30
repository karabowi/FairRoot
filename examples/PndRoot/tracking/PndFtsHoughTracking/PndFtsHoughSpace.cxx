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

#include "PndFtsHoughSpace.h"

#include <iostream>

#include "TMath.h"
#include <math.h>
#include <algorithm>

#include <set>
#include <vector>
#include <map>

// FTS
#include "PndFtsHit.h"
#include "FairHit.h"

// (Hough) tracking
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "FairTrackParP.h"
#include "PndFtsHoughTrackCand.h"

// histogramming / plotting
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"

// peak finder
#include "TSpectrum2.h"

// root IO
#include "FairRunAna.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"

#include "PndFtsHit.h"
#include "TString.h"
#include "FairHit.h"
#include "PndFtsHit.h"

#include "PndFtsTube.h"
#include "FairEventHeader.h"

ClassImp(PndFtsHoughSpace);

std::ostream &operator<<(std::ostream &os, const IdxPath &outVector)
{
  os << "[";
  Int_t lastIdx = outVector.size() - 1;
  if (lastIdx < 0) {
    os << "]";
    return os;
  }

  for (Int_t iVec = 0; iVec < lastIdx; ++iVec) {
    os << outVector[iVec] << ", ";
  }
  os << outVector[lastIdx] << "]";
  return os;
}

std::ostream &operator<<(std::ostream &os, const HitIdxPathMap &outMap)
{
  os << '\n';
  if (outMap.begin() == outMap.end()) {
    os << "{ , [] }";
    return os;
  }
  for (HitIdxPathMap::const_iterator itMap = outMap.begin(); itMap != outMap.end(); ++itMap) {
    os << "{ " << itMap->first << ", ";
    os << itMap->second;
    os << " }\n\n";
  }
  return os;
}

inline void PndFtsHoughSpace::AddHitToHS(UInt_t hitId, Double_t rho)
{
  const PndFtsHit *const hitToAdd = fTrackerTask->GetFtsHit(hitId);
  const Int_t tubeIdToAdd = hitToAdd->GetTubeID();

  if (kFALSE == IsHitFromTubeIdAlreadyAdded(tubeIdToAdd)) {
    fHitId.push_back(PndTrackCandHit(fFtsBranchId, hitId, rho));
  }
}

inline void PndFtsHoughSpace::AddHitToHS(FairLink link, Double_t rho)
{
  const PndFtsHit *const hitToAdd = fTrackerTask->GetFtsHit(link.GetIndex());
  const Int_t tubeIdToAdd = hitToAdd->GetTubeID();

  if (kFALSE == IsHitFromTubeIdAlreadyAdded(tubeIdToAdd)) {
    fHitId.push_back(PndTrackCandHit(link.GetType(), link.GetIndex(), rho));
  }
}

inline Bool_t PndFtsHoughSpace::IsHitFromTubeIdAlreadyAdded(const Int_t tubeIdToAdd)
{
  //	return kFALSE; // uncomment this line to switch off testing for duplicates
  for (size_t iTestHit = 0; iTestHit < GetNHits(); ++iTestHit) {
    const PndFtsHit *myTestHit = getHitFromHS(iTestHit);

    const Int_t tubeIdTestHit = myTestHit->GetTubeID();

    if (tubeIdToAdd == tubeIdTestHit)
      return kTRUE;

  } // for loop over all hits which have already been added to the Hough space
  return kFALSE;
};

PndFtsHoughSpace::PndFtsHoughSpace(const char *name, const Int_t refIndex,

                                   PndFtsHoughSpaceBinning binning,

                                   Double_t zRefPos, Double_t interceptZx,

                                   PndFtsHoughTrackCand *associatedTrackCand,

                                   PndFtsHoughTrackerTask *trackerTask)
  : TH2S(name, name, binning.getNBinsTheta(), binning.getThetaRadLow(), binning.getThetaRadHigh(), binning.getNBinsY(), binning.getYLow(), binning.getYHigh()),
    fTrackerTask(trackerTask), fVerbose(0), fRefIndex(refIndex), fFtsBranchId(0), fAssociatedTrackCand(associatedTrackCand), fZRefPos(zRefPos), fInterceptZx(interceptZx),
    // set from tracker task
    fField(nullptr)
{
  if (nullptr == fTrackerTask) {
    std::cerr << "PndFtsHoughSpace FATAL ERROR Tracker task pointer not set.\n";
  } else {
    fVerbose = fTrackerTask->GetVerbose();
    if (3 < fVerbose)
      std::cout << "PndFtsHoughSpace called with tracker ptr " << fTrackerTask << '\n';
    fFtsBranchId = fTrackerTask->getFtsBranchId();
    fField = fTrackerTask->getMagneticFieldPtr();

    setParametersForHsOption();
    filterInputHits();
  }
}

PndFtsHoughSpace::~PndFtsHoughSpace() {}

Bool_t PndFtsHoughSpace::setParametersForHsOption()
{
  // use option instead of GetName() or fName (even though it is the same)
  const TString option = GetName();

  fKeepBConstant = kTRUE; // kFALSE only for testing

  // set parameters according to the Hough transform I want to do
  if ("lineBeforeDipole" == option) {
    // make sure hits are not shifted for line hough transform
    if (0 != fInterceptZx) {
      std::cout << "PndFtsHoughSpace: "
                << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of stations before dipole field!\n";
      std::cout << "Will set interceptZx to 0 for " << option << '\n';
      fInterceptZx = 0.;
    }

    fUseNonSkewedStraws = kTRUE;
    fUseSkewedStraws = kFALSE;

    // Line for stations 1+2
    fOnlyUseHitsFromZ = 100.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
    fOnlyUseHitsUpToZ = 380.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value

    GetXaxis()->SetTitle("#theta [rad]");
    GetYaxis()->SetTitle("x [cm]");
  } else if ("parabola" == option) {
    fUseNonSkewedStraws = kTRUE;
    fUseSkewedStraws = kFALSE;

    // parabola for stations 3-5
    fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
    fOnlyUseHitsUpToZ = 630.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value

    GetXaxis()->SetTitle("#theta [rad]");
    GetYaxis()->SetTitle("x_{LP} [cm]");
  } else if ("parabolapz" == option) {
    fUseNonSkewedStraws = kTRUE;
    fUseSkewedStraws = kFALSE;

    // parabola for stations 3-5
    fOnlyUseHitsFromZ = 380.; // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
    fOnlyUseHitsUpToZ = 630.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value

    GetXaxis()->SetTitle("#theta [rad]");
    GetYaxis()->SetTitle("x_{LP} [cm]");
  } else if ("lineBehindDipole" == option) {
    // make sure hits are not shifted for line hough transform
    if (0 != fInterceptZx) {
      std::cout << "PndFtsHoughSpace: "
                << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of stations after dipole field!\n";
      std::cout << "Will set interceptZx to 0 for " << option << '\n';
      fInterceptZx = 0.;
    }

    fUseNonSkewedStraws = kTRUE;
    fUseSkewedStraws = kFALSE;

    // Line for stations 5+6
    fOnlyUseHitsFromZ = 550.;  // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
    fOnlyUseHitsUpToZ = 1000.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value

    GetXaxis()->SetTitle("#theta [rad]");
    GetYaxis()->SetTitle("x [cm]");
  } else if ("lineZy" == option) {
    // make sure hits are not shifted for line hough transform
    if (0 != fInterceptZx) {
      std::cout << "PndFtsHoughSpace: "
                << "fInterceptZx was set to " << fInterceptZx << " That is not correct for line HT of all stations in zy plane!\n";
      std::cout << "Will set interceptZx to 0 for " << option << '\n';
      fInterceptZx = 0.;
    }

    fUseNonSkewedStraws = kFALSE;
    fUseSkewedStraws = kTRUE;

    // Line for all FTS stations
    fOnlyUseHitsFromZ = 100.;  // Set = 100. if you want to use all FTS hits, higher if you want to exclude hits that are closer to the interaction point than the value
    fOnlyUseHitsUpToZ = 1000.; // Set = 1000. if you want to use all FTS hits, lower if you want to exclude hits that are further away from the interaction point than the value

    GetXaxis()->SetTitle("#theta [rad]");
    GetYaxis()->SetTitle("y [cm]");
  } else {
    throwError("in PndFtsHoughSpace! option " + option + " is not implemented!");
  }

  if (3 < fVerbose) {
    std::cout << "HoughSpace parameters successfully set for option " << option << '\n';
    if (10 < fVerbose) {
      std::cout << "fUseNonSkewedStraws " << fUseNonSkewedStraws << '\n';
      std::cout << "fUseSkewedStraws " << fUseSkewedStraws << '\n';
      std::cout << "fOnlyUseHitsFromZ " << fOnlyUseHitsFromZ << '\n';
      std::cout << "fOnlyUseHitsUpToZ " << fOnlyUseHitsUpToZ << '\n';

      std::cout << "fInterceptZx " << fInterceptZx << '\n';
    }
  }
  return kTRUE;
}

void PndFtsHoughSpace::filterInputHits()
{
  if (3 < fVerbose) {
    std::cout << "All FTS hits in event " << fTrackerTask->GetNFtsHits() << "\n";
  }

  for (int iHit = 0; iHit < fTrackerTask->GetNFtsHits(); iHit++) {
    const PndFtsHit *const myHit = fTrackerTask->GetFtsHit(iHit);

    // Skip hits from skewed or non-skewed straws (if I wish not to use them)
    if (0 != myHit->GetSkewed()) {
      // hit comes from skewed straw
      if (kFALSE == fUseSkewedStraws) {
        if (3 < fVerbose) {
          std::cout << "Skipping hit with index " << iHit << ", because it comes from a skewed straw! LayerID = " << myHit->GetLayerID() << '\n';
        }
        continue;
      }

    } else {
      // hit comes from non-skewed straw
      if (kFALSE == fUseNonSkewedStraws) {
        if (3 < fVerbose) {
          std::cout << "Skipping hit with index " << iHit << ", because it comes from a non-skewed straw! LayerID = " << myHit->GetLayerID() << '\n';
        }
        continue;
      }
    }

    // only hits with z component between fOnlyUseHitsFromZ and fOnlyUseHitsUpToZ will be used in the Hough transform
    const Double_t hitZLabSys = myHit->GetZ();
    if ((hitZLabSys < fOnlyUseHitsFromZ) || (hitZLabSys > fOnlyUseHitsUpToZ)) {
      if (3 < fVerbose) {
        std::cout << "Skipping hit with index " << iHit << " , because its z " << hitZLabSys << " is not in [" << fOnlyUseHitsFromZ << ", " << fOnlyUseHitsUpToZ << "]\n";
      }
      continue;
    }

    // add surviving hits to the hit vector (z coordinate as sorting parameter chosen)
    if (2 < fVerbose) {
      std::cout << "Adding hit with index " << iHit << " to the Hough space hit vector.\n";
    }
    AddHitToHS(iHit, hitZLabSys);

  } // for loop over all hits
  if (1 < fVerbose)
    std::cout << GetNHits() << " hits in Hough space.\n";
}

Bool_t PndFtsHoughSpace::FillHoles(const Int_t lastBinX, const Int_t lastBinY, const Int_t currentBinY, IdxPath *ptrThetaYIdxPathVec)
{
  // determine how many holes we have to fill
  const Int_t nHolesToFill = abs(currentBinY - lastBinY) - 1;
  for (Int_t iCorrect = 1; iCorrect <= nHolesToFill; ++iCorrect) {
    const Int_t xCorrect = round(float(iCorrect) / float(nHolesToFill)); // gives 0 or 1
    Int_t yCorrect;
    if (currentBinY > lastBinY) {
      // we go up in the second value, therefore, we need to add to the yValue
      yCorrect = iCorrect;
    } else {
      yCorrect = -iCorrect;
    }

    const Int_t interpolateBinX = lastBinX + xCorrect;
    const Int_t interpolateBinY = lastBinY + yCorrect;

    const Int_t interpolatedGlobalBin = GetBin(interpolateBinX, interpolateBinY);
    AddBinContent(interpolatedGlobalBin);

    // save interpolated globalBin into path vector
    ptrThetaYIdxPathVec->push_back(interpolatedGlobalBin);

    if (8 < fVerbose) {
      std::cout << "I am filling hole " << iCorrect << " of " << nHolesToFill << '\n';
      std::cout << "interpolatedGlobalBin = " << interpolatedGlobalBin << '\n';
      std::cout << "(lastBinX, lastBinY) = (" << lastBinX << ", " << lastBinY << ")" << '\n';
      std::cout << "(lastBinX+1, currentBinY)     = (" << lastBinX + 1 << ", " << currentBinY << ")" << '\n';
      std::cout << "xCorrect = " << xCorrect << "  yCorrect = " << yCorrect << '\n';
      std::cout << "(interpolateBinX, interpolateBinY) = (" << interpolateBinX << ", " << interpolateBinY << ")" << '\n';
    }
  } // for iCorrect
  return kTRUE;
}

void PndFtsHoughSpace::FillHoughSpace()
{
  // make sure we have hits in the Hough space
  if (0 == GetNHits()) {
    if (1 < fVerbose)
      Info("FillHoughSpace", "No hits in Hough space.");
    return;
  }

  // make Hough space according to the Hough transform I want to do
  const TString option = GetName();

  // for storing the value to be calculated in Hough transform (yValue = offset for line, yValue = Q/p_{zx} for parabola)
  Double_t yVal = 0.;

  // store bin numbers for last and current entry (for making sure that there are no holes in the histogram)
  Int_t globalBin = 0;
  Int_t currentBinX = 0, currentBinY = 0, currentBinZ = 0;
  Int_t lastBinX = 0, lastBinY = 0;

  Bool_t firstEntry = kTRUE; // is used to indicate when holes in histogram have to be filled, set this to kTRUE for the first entry FOR EACH HIT

  // This produces the Hough space for a parabola or a line (with constant B field or with B field read from field maps)
  for (size_t iHit = 0; iHit < GetNHits(); iHit++) {
    firstEntry = kTRUE;
    const PndFtsHit *myHit = getHitFromHS(iHit);

    // get hit position
    TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);
    Double_t hitXLabSys = hitPos.X();
    Double_t hitYLabSys = hitPos.Y();
    Double_t hitZLabSys = hitPos.Z();
    Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
    Double_t hitZShifted = hitZLabSys - fZRefPos;     // z coordinate in local coordinate system (for parabola and for line)

    if (1 < fVerbose) {
      std::cout << "Doing " << option << " Hough transform for hit (hitZLabSys, hitXLabSys) = (" << hitZLabSys << ", " << hitXLabSys << ") cm";
      if (kTRUE == fKeepBConstant) {
        std::cout << " ignoring B field maps\n";
      } else {
        std::cout << " reading B field maps\n";
      }
    }

    // y component of B field
    Double_t By = getByFromBField(hitXLabSys, hitYLabSys, hitZLabSys);

    //------------------
    // theta scan
    //------------------

    // get indices for first and last bins on x-axis
    Int_t iThetaFirst = fXaxis.GetFirst();
    Int_t iThetaLast = fXaxis.GetLast();

    // save path for each hit
    IdxPath globalBinPathVec;

    // calculate Hough transform for hit iHit
    // for each hit a scan in theta is done by going through the x-axis of the Hough space from lower to higher values
    for (Int_t iTheta = iThetaFirst; iTheta < iThetaLast; ++iTheta) {
      // get theta value corresponding to iTheta
      Double_t thetaRad = fXaxis.GetBinCenter(iTheta); // theta has to be stored in rad
      if (9 < fVerbose) {
        std::cout << " for thetaRad = " << thetaRad << '\n';
      }

      // calculate yVal depending on which Hough transform we need
      if ("parabola" == option) {
        // Use shifted x and shifted z for parabola
        yVal = equationParabola(thetaRad, hitZShifted, hitXShifted, By);
        if (9 < fVerbose) {
          std::cout << "Q/pzx = " << yVal;
        }
      } else if ("parabolapz" == option) {
        // Use shifted x and shifted z for parabola
        yVal = equationParabolaPz(thetaRad, hitZShifted, hitXShifted, By);
        if (9 < fVerbose) {
          std::cout << "pz/Q = " << yVal;
        }
      } else if (("lineBeforeDipole" == option) || ("lineBehindDipole" == option)) {
        // Use real x and shifted z for line
        yVal = equationLineZxOrZy(thetaRad, hitZShifted, hitXLabSys);
        if (9 < fVerbose) {
          std::cout << "xLP/PL = " << yVal;
        }
      } else if ("lineZy" == option) {
        // Use real x and shifted z for line
        yVal = equationLineZxOrZy(thetaRad, hitZShifted, hitYLabSys);
        if (9 < fVerbose) {
          std::cout << "yLine = " << yVal;
        }
      } else {
        // should never happen
        throwError("in FillHoughSpace! option " + option + " is not implemented!");
      }

      // Insert "point" into Hough space and check in which bins we filled
      globalBin = Fill(thetaRad, yVal);
      if (5 < fVerbose) {
        std::cout << "Hough point was filled into Hough space. globalbin = " << globalBin << " for option" << option << '\n';
      }
      // Find currentBinX(=iTheta) and currentBinY from globalBin
      GetBinXYZ(globalBin, currentBinX, currentBinY, currentBinZ);

      // if Fill was into a real bin (and not into over-/underflow) remove holes in Hough space (by assuming a straight line in between neighboring points)
      // for each theta 1 yVal is calculated, so holes will only appear in yVal, not in theta
      if (globalBin >= 0) // -1 would mean over- or underflow
      {
        if (5 < fVerbose) {
          std::cout << "OK! Hough point was NOT written to over- or underflow of histogram. Setting firstEntry to kFALSE now. " << option << '\n';
        }

        // TODO Remove the following check for optimization, it should always be true
        if (currentBinX != iTheta) {
          std::cerr << "\n\nError in FillHoughSpace! Hough point was filled into xBin " << currentBinX << " and not in " << iTheta << "\n";
          std::cerr << "iThetaFirst = " << iThetaLast << " iThetaLast = " << iThetaLast << "\n";
          std::cerr << "Over- or underflow on y-axis or FATAL error!\n\n\n";
        }

        // fill holes if the current Hough point is not the first entry in Hough space for the hit
        if (kFALSE == firstEntry) {
          if (5 < fVerbose) {
            std::cout << "This is not the first point of the hit in the histogram. I will fix all holes which might be between this entry and the last one in the histogram"
                      << '\n';
          }

          FillHoles(lastBinX, lastBinY, currentBinY, &globalBinPathVec);

        } // if not first entry to be written into histogram
        else {
          //		++nHitsInHoughSpace; // count hits for making of Hough space (only once per hit)
          if (5 < fVerbose) {
            std::cout << "This is the first point of the hit in the histogram. I will not try to fix any holes in the " << option << " histogram" << '\n';
          }
        }

        firstEntry = kFALSE;

        // save calculated globalBin into path vector
        globalBinPathVec.push_back(globalBin);

      } // if NOT filled into over- or underflow
      else {
        if (9 < fVerbose) {
          std::cout << "Watch out! Point was written to over- or underflow of histogram. firstEntry is set to kTRUE. " << option << '\n';
        }
        firstEntry = kTRUE; // otherwise, algorithm connects first point which does not go into over-/underflow with (0,0)
      }

      if (9 < fVerbose) {
        std::cout << "currentBinY = " << currentBinY << "  lastBinY = " << lastBinY << '\n';
      }
      lastBinX = currentBinX;
      lastBinY = currentBinY;

    } // for theta
    //		std::cout << "map before iHit " << iHit << ": " << fHitThetaYIdxPath << '\n';
    // save the final path for the hit
    if (0 < globalBinPathVec.size()) {
      HitIdxPathPair hitPathPair(iHit, globalBinPathVec);
      fHitThetaYIdxPath.insert(hitPathPair);
    }
  } // for iHit
  if (1 < fVerbose)
    std::cout << "map after all hits: " << fHitThetaYIdxPath << '\n';
  if (0 < fTrackerTask->GetSaveDebugInfo()) {
    WriteHistoOfHoughSpace();
    WriteHistoOfAllPaths();
    WriteHistoOfAllPathsForEachMcTruthTrack();
  }
}

TH2S PndFtsHoughSpace::MakeEmptyHistoOfSameDimensions(TString specifier, Int_t index) const
{
  // for getting rid of warning about potential memory leak (same name for mutliple different histos)
  static Int_t histoCounter = 0;
  TString newname = GetName();
  newname += histoCounter;
  //	std::cout<<newname << '\n';
  ++histoCounter;

  TString newTitle = GetName();
  if ("" != specifier) {
    newTitle += " ";
    newTitle += specifier;
  }
  if (-1 < index) {
    newTitle += " ";
    newTitle += index;
  }
  TH2S peaks(newname, newTitle, fXaxis.GetNbins(), fXaxis.GetXmin(), fXaxis.GetXmax(), fYaxis.GetNbins(), fYaxis.GetXmin(), fYaxis.GetXmax());
  return peaks;
}

void PndFtsHoughSpace::WriteHistoOfAllPeaks(const std::vector<PndFtsHoughSpacePeak> &peaksToPlot) const
{
  // test if we need to do anything here
  Bool_t saveEachSeparately = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kEachFoundPeakSeparately);
  Bool_t saveAllTogether = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kAllFoundPeaksTogether);
  if ((kTRUE == saveEachSeparately) && (kTRUE == saveAllTogether))
    return;

  // write out histograms containing found peaks
  // filling all peaks in separate histos and all together in one histo
  TH2S allPeaks = MakeEmptyHistoOfSameDimensions("AllPeaks");
  for (UInt_t iPeak = 0; iPeak < peaksToPlot.size(); ++iPeak) {
    TH2S onePeak = MakeEmptyHistoOfSameDimensions("Peak", iPeak);
    const Double_t currHeight = peaksToPlot[iPeak].getHeight();
    const std::set<Int_t> &binsInPeak = peaksToPlot[iPeak].getBins();
    for (std::set<Int_t>::iterator itBin = binsInPeak.begin(); itBin != binsInPeak.end(); ++itBin) {
      onePeak.SetBinContent(*itBin, currHeight);
      allPeaks.SetBinContent(*itBin, currHeight);
    }
    TString outNameOne = GetDebugOutName(onePeak.GetTitle(), currHeight);
    if (kTRUE == saveEachSeparately)
      onePeak.SaveAs(outNameOne, "LEGO2");
  }
  TString outNameAll = GetDebugOutName(allPeaks.GetTitle());
  if (kTRUE == saveAllTogether)
    allPeaks.SaveAs(outNameAll, "LEGO2");
}

void PndFtsHoughSpace::WriteHistoOfAllPaths() const
{
  // test if we need to do anything here
  Bool_t saveExclusively = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kHitCurvesExclusively);
  Bool_t saveProjected = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kHitCurvesProjected);
  if ((kTRUE == saveExclusively) && (kTRUE == saveProjected))
    return;

  // filling each path in separate histo
  for (HitIdxPathMap::const_iterator itPath = fHitThetaYIdxPath.begin(); itPath != fHitThetaYIdxPath.end(); ++itPath) {
    const Int_t currHit = itPath->first;
    TH2S onePathProjected = MakeEmptyHistoOfSameDimensions("PathProjected", currHit);
    TH2S onePathExclusively = MakeEmptyHistoOfSameDimensions("PathExclusively", currHit);
    const IdxPath &currPath = itPath->second;

    Int_t lastBinNumber = -1;
    for (size_t iGlobalBin = 0; iGlobalBin < currPath.size(); ++iGlobalBin) {
      const Int_t currBinNumber = currPath[iGlobalBin];
      // warn if we accidently saved the same bin twice in the path
      if (currBinNumber == lastBinNumber)
        std::cerr << "FATAL! Bin " << currBinNumber << " twice in a row! in hit " << currHit << '\n';
      lastBinNumber = currBinNumber;

      if (kTRUE == saveProjected) {
        const Double_t currHeight = GetBinContent(currBinNumber); // get height of Hough space
        onePathProjected.SetBinContent(currBinNumber, currHeight);
      }

      if (kTRUE == saveExclusively) {
        Int_t currBinX = 0, currBinY = 0, notUsedBinZ = 0;
        GetBinXYZ(currBinNumber, currBinX, currBinY, notUsedBinZ); // Find currBinX and currBinY from globalBin
        const Double_t currXVal = GetXaxis()->GetBinCenter(currBinX);
        const Double_t currYVal = GetYaxis()->GetBinCenter(currBinY);
        onePathExclusively.Fill(currXVal, currYVal); // exclusive fill
      }
    }
    TString outNameProj = GetDebugOutName(onePathProjected.GetTitle());
    if (kTRUE == saveProjected)
      onePathProjected.SaveAs(outNameProj, "LEGO2");
    TString outNameExcl = GetDebugOutName(onePathExclusively.GetTitle());
    if (kTRUE == saveExclusively)
      onePathExclusively.SaveAs(outNameExcl, "LEGO2");
  }
}

void PndFtsHoughSpace::WriteHistoOfAllPathsForEachMcTruthTrack() const
{
  // test if we need to do anything here
  Bool_t saveExclusively = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kMcTruthPeaksExclusively);
  Bool_t saveProjected = !(fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kMcTruthPeaksProjected);
  if ((kTRUE == saveExclusively) && (kTRUE == saveProjected))
    return;

  // create map collecting histos of all hit indices from same Mc truth track
  std::map<Int_t, std::pair<TH2S, TH2S>> mcTruthIdHistos;

  for (HitIdxPathMap::const_iterator itPath = fHitThetaYIdxPath.begin(); itPath != fHitThetaYIdxPath.end(); ++itPath) {

    // figure out to which MC Truth track hit belongs
    const Int_t currHit = itPath->first;
    const Int_t hitId = getHitIdFromHS(currHit);
    const Int_t mcTruthId = fTrackerTask->getMcTruthIdForHitId(hitId);

    // if MC truth index not yet in map add new histo
    std::map<Int_t, std::pair<TH2S, TH2S>>::iterator itFind;
    itFind = mcTruthIdHistos.find(mcTruthId);
    if (mcTruthIdHistos.end() == itFind) { // not found
      TH2S newHisto = MakeEmptyHistoOfSameDimensions("McTruthPeak projected", mcTruthId);
      TH2S newHisto2 = MakeEmptyHistoOfSameDimensions("McTruthPeak exclusive", mcTruthId);
      std::pair<TH2S, TH2S> histoPair(newHisto, newHisto2);
      std::pair<Int_t, std::pair<TH2S, TH2S>> mcTruthIdHistosPair(mcTruthId, histoPair);
      mcTruthIdHistos.insert(mcTruthIdHistosPair);
      itFind = mcTruthIdHistos.find(mcTruthId); // now histos can be found in map
    }

    // Fill current path into correct histo (first as projection, second as if only hits from same mc truth track were filled)
    const IdxPath &currPath = itPath->second;
    for (size_t iGlobalBin = 0; iGlobalBin < currPath.size(); ++iGlobalBin) {
      Int_t currBinNumber = currPath[iGlobalBin];
      std::pair<TH2S, TH2S> &histoPair = itFind->second;

      if (kTRUE == saveProjected) {
        const Double_t currHeight = GetBinContent(currBinNumber); // get height of Hough space
        histoPair.first.SetBinContent(currBinNumber, currHeight); // projection
      }

      if (kTRUE == saveExclusively) {
        Int_t currBinX = 0, currBinY = 0, notUsedBinZ = 0;
        GetBinXYZ(currBinNumber, currBinX, currBinY, notUsedBinZ); // Find currBinX and currBinY from globalBin
        const Double_t currXVal = GetXaxis()->GetBinCenter(currBinX);
        const Double_t currYVal = GetYaxis()->GetBinCenter(currBinY);
        histoPair.second.Fill(currXVal, currYVal); // exclusive fill
      }
    }
  }
  // loop over all histos that have been created
  for (std::map<Int_t, std::pair<TH2S, TH2S>>::const_iterator itHisto = mcTruthIdHistos.begin(); itHisto != mcTruthIdHistos.end(); ++itHisto) {
    const std::pair<TH2S, TH2S> &histoPair = itHisto->second;
    TString outNameProjection = GetDebugOutName(histoPair.first.GetTitle());
    TString outNameExclusive = GetDebugOutName(histoPair.second.GetTitle());
    if (kTRUE == saveProjected)
      histoPair.first.SaveAs(outNameProjection, "LEGO2");
    if (kTRUE == saveExclusively)
      histoPair.second.SaveAs(outNameExclusive, "LEGO2");
  }
}

void PndFtsHoughSpace::WriteHistoOfHoughSpace() const
{
  if (0 != fTrackerTask->GetSaveDebugInfo() % PndFtsHoughTrackerTask::kHoughSpaces)
    return;

  //	Int_t index = fHoughSpaces->GetEntriesFast();
  //	PndFtsHoughSpace* myHoughSpace = new ((*fHoughSpaces)[index])PndFtsHoughSpace(*houghSpace);
  TString title = GetTitle();
  title += " histo";
  TString outName = GetDebugOutName(title);
  SaveAs(outName, "LEGO2"); // resulting files need to have PndFtsHoughSpace replaced with TH2S
                            // sed -i 's/PndFtsHoughSpace/TH2S/g' *.rtg

  //	fOutFile = FairRootManager::Instance()->GetOutFile();
  //	if (0==fOutFile)
  //	{
  //		std::cout << "WriteHistograms: Cannot get outfile.\n";
  //	}
  //	else
  //	{
  //		//			fOutFile->cd();
  //		//			fOutFile->cd("PndFtsHoughTrackerTask");
  //		if(3<fVerbose) std::cout << "WriteHistograms: Got outfile for debugging output.\n";
  //		if (0!=houghSpace)
  //		{
  //			TString histNameOld = houghSpace->GetName();
  //			TString histNameNew = houghSpace->GetName();
  //			histNameNew+=fEventNr;
  //			houghSpace->SetName(histNameNew);
  //			houghSpace->Write();
  //			houghSpace->SetName(histNameOld);
  //		}
  //		//			fOutFile->cd();
  //	}
}

void PndFtsHoughSpace::AddHitsToTrackletByCalculating(PndFtsHoughTracklet *currentTracklet, Int_t locmax)
{
  ///////////////////////////////////////////
  // TODO this is messy, because the code is very similar to FillHoughSpace. Probably, I should find a way to merge it
  // add hits which are in the peak to the tracklet
  // 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
  // 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
  // 3 otherwise the hit is not in the peak

  // This is more complicated to check, but also true
  // A calculate the 2nd value for peak theta
  // B If hit 2nd value is within [peak 2nd value - peakSecondHw,  peak 2nd value + peakSecondHw] add the hit
  // C If hit 2nd value is < peak 2nd value - peakSecondHw, hit is in peak if the 2nd value of the next higher/lower theta bin is >= peak 2nd value
  // D If hit 2nd value is > peak 2nd value + peakSecondHw, hit is in peak if the 2nd value of the next lower/higher theta bin is <= peak 2nd value
  // E otherwise the hit is not in the peak

  const Int_t xfirst = fXaxis.GetFirst();
  const Int_t xlast = fXaxis.GetLast();
  // const Int_t yfirst  = fYaxis.GetFirst(); //[R.K. 01/2017] unused variable?
  // const Int_t ylast   = fYaxis.GetLast(); //[R.K. 01/2017] unused variable?

  // 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
  UInt_t thetaBinLo = locmax - 1;
  UInt_t thetaBinHi = locmax + 1;
  // special case if we are at the edge of the Hough space
  if ((int)thetaBinLo > xfirst) {
    thetaBinLo = xfirst;
  }
  if ((int)thetaBinHi > xlast) {
    thetaBinHi = xlast;
  }

  // get theta values
  const Double_t thetaRadLo = fXaxis.GetBinCenter(thetaBinLo);
  const Double_t thetaRadHi = fXaxis.GetBinCenter(thetaBinHi);

  // for storing the values to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
  Double_t yValLo = 0.;
  Double_t yValHi = 0.;

  for (size_t iHit = 0; iHit < GetNHits(); iHit++) {
    const PndFtsHit *myHit = getHitFromHS(iHit);

    // get hit position
    const TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);

    Double_t hitXLabSys = hitPos.X();
    Double_t hitYLabSys = hitPos.Y();
    Double_t hitZLabSys = hitPos.Z();
    Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
    Double_t hitZShifted = hitZLabSys - fZRefPos;     // z coordinate in local coordinate system (for parabola and for line)

    // y component of B field
    Double_t By = getByFromBField(hitXLabSys, hitYLabSys, hitZLabSys);

    const TString option = GetName();
    if ("parabola" == option) {
      // Use shifted x and shifted z for parabola
      yValLo = equationParabola(thetaRadLo, hitZShifted, hitXShifted, By);
      yValHi = equationParabola(thetaRadHi, hitZShifted, hitXShifted, By);
      if (9 < fVerbose) {
        std::cout << "Q/pzx = " << yValLo << '\n';
        std::cout << "Q/pzx = " << yValHi << '\n';
      }
    } else if ("parabolapz" == option) {
      yValLo = equationParabolaPz(thetaRadLo, hitZShifted, hitXShifted, By);
      yValHi = equationParabolaPz(thetaRadHi, hitZShifted, hitXShifted, By);

      if (9 < fVerbose) {
        std::cout << "pz/Q = " << yValLo << '\n';
        std::cout << "pz/Q = " << yValHi << '\n';
      }
    } else if (("lineBeforeDipole" == option) || ("lineBehindDipole" == option)) {
      // Use real x and shifted z for line

      yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitXLabSys);
      yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitXLabSys);

      if (9 < fVerbose) {
        std::cout << "xLP/PL = " << yValLo << '\n';
        std::cout << "xLP/PL = " << yValHi << '\n';
      }
    } else if ("lineZy" == option) {
      // Use real x and shifted z for line

      yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitYLabSys);
      yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitYLabSys);

      if (9 < fVerbose) {
        std::cout << "xLP = " << yValLo << '\n';
        std::cout << "xLP = " << yValHi << '\n';
      }
    } else {
      throwError("Error in FillHoughSpace! option " + option + " is not implemented!");
    }

    // 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
    const Double_t yMin = std::min(yValLo, yValHi);
    const Double_t yMax = std::max(yValLo, yValHi);
    const Double_t yMinHw = fYaxis.GetBinWidth(yMin) / 2.;
    const Double_t yMaxHw = fYaxis.GetBinWidth(yMax) / 2.;

    const Double_t peakSecondVal = currentTracklet->getSecondVal();
    if ((yMin - yMinHw <= peakSecondVal) && (yMax + yMaxHw >= peakSecondVal)) {
      currentTracklet->AddHit(fFtsBranchId, getHitIdFromHS(iHit), hitZLabSys);
    }

  } // loop over hits
}

std::vector<PndFtsHoughTracklet> PndFtsHoughSpace::FindAllPeaksBinsWoMergingWithSearchWindow(const UInt_t minHeight, const Int_t vicinityLength)
{
  std::vector<PndFtsHoughTracklet> tracklets; // for output

  // check if Hough space has at least one entry
  if (1 > GetEntries()) {
    if (1 < fVerbose)
      std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
    return tracklets;
  }

  // peak finder based on weighted means (only for 2D histograms implemented)

  // the following is an adaptation of the TH1::GetMaximumBin() code

  // define vicinityLength for search window which determines an effective height of the bin
  // set vicinityLength 0 for not using a search window
  // search window has size (2*vicinityLength+1)^2

  // move search window over histogram and select highest value
  Int_t locmax, locmay, locmaz; // location of maximum (x,y,z)

  //   -*-*-*-*-*Return location of bin with maximum value in the range*-*
  //             ======================================================
  Int_t binNumber, binx, biny;
  //, binz=0; //[R.K. 01/2017] unused variable?
  // get values for first and last bins on each axis and take into account that we might want a search window (make sure search window stays within histogram)
  const Int_t xfirst = fXaxis.GetFirst() + vicinityLength;
  const Int_t xlast = fXaxis.GetLast() - vicinityLength;
  const Int_t yfirst = fYaxis.GetFirst() + vicinityLength;
  const Int_t ylast = fYaxis.GetLast() - vicinityLength;
  //		   Int_t zfirst  = fZaxis->GetFirst()+vicinityLength;
  //		   Int_t zlast   = fZaxis->GetLast()-vicinityLength;

  // find all peaks that have a height >= minHeight
  Double_t currentHeight;
  locmax = locmay = locmaz = 0;
  //		   for (binz=zfirst;binz<=zlast;binz++) {
  for (biny = yfirst; biny <= ylast; biny++) {
    for (binx = xfirst; binx <= xlast; binx++) {
      currentHeight = 0.;
      // iterate over vicinity
      for (Int_t xVicinityBin = -vicinityLength; xVicinityBin <= vicinityLength; ++xVicinityBin) {
        for (Int_t yVicinityBin = -vicinityLength; yVicinityBin <= vicinityLength; ++yVicinityBin) {
          binNumber = GetBin(binx + xVicinityBin, biny + yVicinityBin);                                           //,binz);
          currentHeight += 1. / (1. + std::max(abs(xVicinityBin), abs(yVicinityBin))) * GetBinContent(binNumber); // linear, unendlich norm
          //							currentHeight += 1./(1.+abs(xVicinity) +abs(yVicinity))*houghspace->GetBinContent(binNumber); // linear
          // currentHeight += 1./(1.+pow(max(xVicinity,yVicinity),2))*houghspace->GetBinContent(binNumber); // quadratic, unendlich norm
        }
      }
      if (currentHeight >= minHeight) {
        locmax = binx;
        locmay = biny;
        //		               locmaz  = binz;
        // get values corresponding to the peak
        Double_t peakThetaVal = fXaxis.GetBinCenter(locmax);
        Double_t peakSecondVal = fYaxis.GetBinCenter(locmay);

        // Int_t binmaxglobal = Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram //[R.K. 01/2017] unused variable?
        Double_t peakThetaHw = fXaxis.GetBinWidth(peakThetaVal) / 2.;
        Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal) / 2.;

        // create tracklet and push it back to output
        PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
        currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHw, peakSecondHw);

        // Add hits to tracklet
        AddHitsToTrackletByCalculating(&currentTracklet, locmax);

        tracklets.push_back(currentTracklet);
      } // height is big enough
    }   // x loop
  }     // y loop
  //		   } // z loop

  if (1 < fVerbose)
    PrintFoundTracklets(tracklets);
  return tracklets;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughSpace::FindAllPeaksScanPathsMergeBins(const UInt_t minHeight)
{
  std::vector<PndFtsHoughTracklet> tracklets; // for output

  // check if Hough space has at least one entry
  if (1 > GetEntries()) {
    if (1 < fVerbose)
      std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
    return tracklets;
  }

  // this peak finder is only implemented for 2D histograms

  // for each hit scan the Hough space along the hit's path -> this reduces the 2d peak finding to nHits 1d problems plus peak merging.
  // require bin height >= minHeight
  // and that we are on a rising edge

  std::vector<PndFtsHoughSpacePeak> peaksForOneHit;        // stores (possible) peaks for one hit
  std::vector<PndFtsHoughSpacePeak> mergedPeaksForAllHits; // stores merged peaks for all hits
  mergedPeaksForAllHits.clear();

  // hit loop
  for (HitIdxPathMap::const_iterator itMap = fHitThetaYIdxPath.begin(); itMap != fHitThetaYIdxPath.end(); ++itMap) {
    const Int_t hitIdx = itMap->first;
    IdxPath path = itMap->second;
    peaksForOneHit.clear();

    // loop over bins along the path
    for (int iGlobalBin = 0; iGlobalBin < (int)path.size(); ++iGlobalBin) {
      // current bin
      const Int_t currBinNumber = path[iGlobalBin];
      const Int_t currHeight = GetBinContent(currBinNumber);

      // check if we already found a peak candidate
      const Int_t nPeaksSoFar = peaksForOneHit.size();
      if (0 < nPeaksSoFar) {                                              // if we already have a peak, we might need to continue building it
        PndFtsHoughSpacePeak &currPeak = peaksForOneHit[nPeaksSoFar - 1]; // get last peak
        if (kFALSE == currPeak.isFinished()) {                            // continue building up current peak
          if (currHeight < currPeak.getHeight())
            currPeak.setFinished(kTRUE);
          if (currHeight == currPeak.getHeight())
            currPeak.addBin(currBinNumber, hitIdx);
          if (currHeight > currPeak.getHeight())
            currPeak.replaceBins(currHeight, currBinNumber, hitIdx);
          continue; // do not create a new peak as we were building an old one
        }
      } // we have already >= 1 peaks

      if ((int)minHeight <= currHeight) { // Bin could belong to a peak

        // Make sure we are not on a falling edge by checking that the previous position was strictly lower!
        // previous bin
        const Int_t prevIdx = std::max(0, iGlobalBin - 1); // make sure we stay within bounds of vector
        const Int_t prevBinNumber = path[prevIdx];
        const Int_t prevHeight = GetBinContent(prevBinNumber);

        Bool_t rising = prevHeight < currHeight;
        if (0 == iGlobalBin)
          rising = kTRUE; // always save if we are at the beginning

        if (kTRUE == rising) { // we are on rising edge
          // Add a new peak (only if we do not continue a nonfinished existing one)
          // either we did not have any peaks or the last peak was already finished
          PndFtsHoughSpacePeak newPeak(currHeight, currBinNumber, hitIdx);
          peaksForOneHit.push_back(newPeak);
        }
      }

    } // loop over bins along the path

    // now merge peaksForOneHit with mergedPeaksForAllHits
    // loop over found peaks for latest hit and all merged hits, compare if they have overlaps, if so merge
    for (UInt_t iPeaksForOneHit = 0; iPeaksForOneHit < peaksForOneHit.size(); ++iPeaksForOneHit) {
      Bool_t merged = kFALSE;
      for (UInt_t iPeaksForAllHits = 0; iPeaksForAllHits < mergedPeaksForAllHits.size(); ++iPeaksForAllHits) {
        if (mergedPeaksForAllHits[iPeaksForAllHits].binsOverlapWith(peaksForOneHit[iPeaksForOneHit])) {
          mergedPeaksForAllHits[iPeaksForAllHits].mergeWith(peaksForOneHit[iPeaksForOneHit]);
          merged = kTRUE;
          // continue; // skip checking with all other peaks as they cannot overlap anymore (TODO Check if that is true)
        }
      }
      if (kFALSE == merged)
        mergedPeaksForAllHits.push_back(peaksForOneHit[iPeaksForOneHit]); // add peaks which could not be merged with preexisting ones
    }
  } // hit loop

  if (0 < fTrackerTask->GetSaveDebugInfo())
    WriteHistoOfAllPeaks(mergedPeaksForAllHits);

  // Build up tracklets from peaks by going through vector of merged peaks
  for (UInt_t iPeaks = 0; iPeaks < mergedPeaksForAllHits.size(); ++iPeaks) {
    const Double_t currentHeight = mergedPeaksForAllHits[iPeaks].getHeight();
    const std::set<Int_t> &binsInPeak = mergedPeaksForAllHits[iPeaks].getBins();
    // calculate center and halfwidth (HW) in theta and in secondVal for all global bins in peak
    // save min and max values
    Double_t minThetaVal = 0., maxThetaVal = 0., minSecondVal = 0., maxSecondVal = 0.;
    for (std::set<Int_t>::iterator itBin = binsInPeak.begin(); itBin != binsInPeak.end(); ++itBin) {
      Int_t currentBinX = 0, currentBinY = 0, currentBinZ = 0;
      GetBinXYZ(*itBin, currentBinX, currentBinY, currentBinZ); // get bin numbers for x, y (and z) axis
      // get values for corresponding to global bin number
      const Double_t currThetaVal = fXaxis.GetBinCenter(currentBinX);
      const Double_t currSecondVal = fYaxis.GetBinCenter(currentBinY);

      if (binsInPeak.begin() == itBin) { // initialize values if this is the first bin
        minThetaVal = currThetaVal;
        maxThetaVal = currThetaVal;
        minSecondVal = currSecondVal;
        maxSecondVal = currSecondVal;
      } else { // save min and max
        minThetaVal = std::min(minThetaVal, currThetaVal);
        maxThetaVal = std::max(maxThetaVal, currThetaVal);
        minSecondVal = std::min(minSecondVal, currSecondVal);
        maxSecondVal = std::max(maxSecondVal, currSecondVal);
      }
    }

    const Double_t peakThetaVal = (maxThetaVal + minThetaVal) / 2.;
    const Double_t peakSecondVal = (maxSecondVal + minSecondVal) / 2.;
    const Double_t peakThetaHw = (maxThetaVal - minThetaVal) / 2. + fXaxis.GetBinWidth(peakThetaVal) / 2.;
    const Double_t peakSecondHw = (maxSecondVal - minSecondVal) / 2. + fYaxis.GetBinWidth(peakSecondVal) / 2.;

    // create tracklet, add hits and push it back to output
    PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
    currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHw, peakSecondHw);
    // add hits to tracklet
    const std::set<Int_t> &hitIdsInPeak = mergedPeaksForAllHits[iPeaks].getHitIds();
    for (std::set<Int_t>::iterator itHitId = hitIdsInPeak.begin(); itHitId != hitIdsInPeak.end(); ++itHitId) {
      const Int_t hitIndex = getHitIdFromHS(*itHitId);
      const PndFtsHit *myHit = getHitFromHS(*itHitId);
      // get hit position
      const TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);
      Double_t hitZLabSys = hitPos.Z();
      currentTracklet.AddHit(fFtsBranchId, hitIndex, hitZLabSys);
    }
    tracklets.push_back(currentTracklet);
  }

  if (1 < fVerbose)
    PrintFoundTracklets(tracklets);
  return tracklets;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughSpace::FindAllPeaksScanPathsMergeBinsCalculatingPaths(const UInt_t minHeight)
{
  std::vector<PndFtsHoughTracklet> tracklets; // for output

  // check if Hough space has at least one entry
  if (1 > GetEntries()) {
    if (1 < fVerbose)
      std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
    return tracklets;
  }

  // code goes here
  throwError("This peak finder is not fully implemented!");

  // peak finder only for 2D histograms implemented

  // the following is an adaptation of the TH1::GetMaximumBin() code

  // get values for first and last bins on each axis
  Int_t xFirstBin = fXaxis.GetFirst();
  Int_t xLastBin = fXaxis.GetLast();
  Int_t yFirstBin = fYaxis.GetFirst();
  Int_t yLastBin = fYaxis.GetLast();
  //		   Int_t zfirst  = fZaxis->GetFirst();
  //		   Int_t zlast   = fZaxis->GetLast();

  // find all peaks that have a height >= minHeight
  // go from low to high in theta <- SUPER IMPORTANT
  Int_t currBinNumber;
  Double_t currHeight;
  Int_t peakBinX = 0;
  Int_t peakBinY = 0;
  // Int_t peakBinZ = 0; // location of maximum (x,y,z) //[R.K. 01/2017] unused variable?

  for (Int_t currBinX = xFirstBin; currBinX <= xLastBin; ++currBinX) {
    for (Int_t currBinY = yFirstBin; currBinY <= yLastBin; ++currBinY) {
      //		   for (binz=zfirst;binz<=zlast;binz++) {

      currBinNumber = GetBin(currBinX, currBinY); // binz
      currHeight = GetBinContent(currBinNumber);
      if (minHeight > currHeight) {
        // currBinNumber is not high enough to be considered a peak
        SetBinContent(currBinNumber, 0); // DEBUG: Uncomment this if you want to produce nice Hough space plots for demonstration
        continue;
      }
      // I found a potential peak, so I check the neighbors in next higher theta bins
      // I save the currently highest peak
      // I set the last visited peak = current peak and move to the next neighbor
      // if neighbor < highest peak and neighbor <= last peak, but above the minHeight, I set it to 0. I save its height as the last height.

      // if neighbor is higher than the last peak, I set the last visited bin to 0 and move on
      // if neighbor is the same height as last peak, I find the middle and set all others to 0 and I widen the errors of the middle bin

      Int_t iBinX = 0; // for moving to neighbors in x direction

      // save info of highest peak found while searching neighbors
      Int_t combinedPeakBinXLow = currBinX;  // saves in which x (thetaRad) bin a peak starts
      Int_t combinedPeakBinXHigh = currBinX; // saves in which x (thetaRad) bin a peak ends
      Double_t combinedPeakHeight = currHeight;

      // save info of last visited neighbor
      // Int_t lastVisitedBinX = currBinX; //[R.K. 01/2017] unused variable
      // Int_t lastVisitedBinNumber = currBinNumber; //[R.K. 01/2017] unused variable
      Double_t lastVisitedHeight = currHeight;

      // storing the current neighbor bin
      Int_t currNeighborBinX = currBinX;
      Int_t currNeighborBinNumber = currBinNumber;
      Double_t currNeighborHeight = currHeight;

      do {
        ++iBinX; // to move along the x bins
        // set current neighbor
        currNeighborBinX = currBinX + iBinX;
        if (currNeighborBinX > xLastBin) {
          break;
        }
        currNeighborBinNumber = GetBin(currNeighborBinX, currBinY); // binz
        currNeighborHeight = GetBinContent(currNeighborBinNumber);

        if (minHeight > currNeighborHeight) {
          // end of peak region in x direction found, I can get out of the loop
          SetBinContent(currNeighborBinNumber, 0); // DEBUG: Uncomment this if you want to produce nice Hough space plots for demonstration
          break;
        }

        if (currNeighborHeight > combinedPeakHeight) {
          // actually peak starts with neighbor (or even later), but current bin is not in peak
          for (Int_t xBinToDel = combinedPeakBinXLow; xBinToDel <= combinedPeakBinXHigh; ++xBinToDel) {
            Int_t BinNumberToDel = GetBin(xBinToDel, currBinY);
            SetBinContent(BinNumberToDel, 0);
          }
          combinedPeakBinXLow = currNeighborBinX;
          combinedPeakBinXHigh = currNeighborBinX;
          combinedPeakHeight = currNeighborHeight;
        } else if (currNeighborHeight < combinedPeakHeight) {
          if (currNeighborHeight <= lastVisitedHeight) {
            // peak is dropping, delete current bin (which is at the falling edge of the peak)
            SetBinContent(currNeighborBinNumber, 0);
          } else {
            // next peak starts rising, I can end the loop
            break;
          }
        } else if (currNeighborHeight == combinedPeakHeight) {
          // peak is spread over several bins
          combinedPeakBinXHigh = currNeighborBinX;
        }
        // lastVisitedBinX = currNeighborBinX; //[R.K. 01/2017] unused variable
        // lastVisitedBinNumber = currNeighborBinNumber; //[R.K. 01/2017] unused variable
        lastVisitedHeight = currNeighborHeight;

      } while (kTRUE);

      // DEBUG: uncomment this if you want to produce nice Hough space plots for demonstration
      // remove the peak which was found, otherwise it (or parts of it) might be found again
      for (Int_t xBinToDel = combinedPeakBinXLow; xBinToDel <= combinedPeakBinXHigh; ++xBinToDel) {
        Int_t BinNumberToDel = GetBin(xBinToDel, currBinY);
        SetBinContent(BinNumberToDel, 0);
      }

      // peak is in the middle
      peakBinX = (combinedPeakBinXHigh + combinedPeakBinXLow) / 2; // if sum is not an even number, I have to correct for that later
      peakBinY = currBinY;
      // locmaz  = binz;
      // get values corresponding to the peak
      Double_t peakThetaVal = fXaxis.GetBinCenter(peakBinX);
      Double_t peakSecondVal = fYaxis.GetBinCenter(peakBinY);

      // correct peak value if mean for peak cannot be divided by 2 without remainder
      if (0 != (combinedPeakBinXHigh + combinedPeakBinXLow) % 2) {
        peakThetaVal += fXaxis.GetBinWidth(peakThetaVal) / 2.;
      }

      // get full width of peak = (highest bin + halfwidth) - (lowest bin - halfwidth)
      Double_t combinedPeakThetaLowEdge = fXaxis.GetBinCenter(combinedPeakBinXLow) - fXaxis.GetBinWidth(combinedPeakBinXLow) / 2.;
      Double_t combinedPeakThetaHighEdge = fXaxis.GetBinCenter(combinedPeakBinXHigh) + fXaxis.GetBinWidth(combinedPeakBinXHigh) / 2.;

      Double_t peakThetaHw = (combinedPeakThetaHighEdge - combinedPeakThetaLowEdge) / 2.;
      Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal) / 2.;

      // create tracklet and push it back to output
      PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
      currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currHeight, peakThetaHw, peakSecondHw);

      ///////////////////////////////////////////
      // TODO this is messy, because the code is very similar to FillHoughSpace. Probably, I should find a way to merge it
      // add hits which are in the peak to the tracklet
      // 1 calculate the 2nd value for the next higher/lower theta bin of combined peak theta
      // 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
      // 3 otherwise the hit is not in the peak

      // This is more complicated to check, but also true
      // A calculate the 2nd value for peak theta
      // B If hit 2nd value is within [peak 2nd value - peakSecondHw,  peak 2nd value + peakSecondHw] add the hit
      // C If hit 2nd value is < peak 2nd value - peakSecondHw, hit is in peak if the 2nd value of the next higher/lower theta bin is >= peak 2nd value
      // D If hit 2nd value is > peak 2nd value + peakSecondHw, hit is in peak if the 2nd value of the next lower/higher theta bin is <= peak 2nd value
      // E otherwise the hit is not in the peak

      // 1 calculate the 2nd value for the next higher/lower theta bin of peak theta
      UInt_t thetaBinLo = combinedPeakBinXLow - 1;
      UInt_t thetaBinHi = combinedPeakBinXHigh + 1;
      // special case if we are at the edge of the Hough space
      if ((int)thetaBinLo > xFirstBin) {
        thetaBinLo = xFirstBin;
      }
      if ((int)thetaBinHi > xLastBin) {
        thetaBinHi = xLastBin;
      }

      // get theta values
      const Double_t thetaRadLo = fXaxis.GetBinCenter(thetaBinLo);
      const Double_t thetaRadHi = fXaxis.GetBinCenter(thetaBinHi);

      // for storing the values to be calculated in Hough transform (yValue = offset for line, yValue = Q/pzx for parabola)
      Double_t yValLo = 0.;
      Double_t yValHi = 0.;

      for (size_t iHit = 0; iHit < GetNHits(); iHit++) {
        const PndFtsHit *myHit = getHitFromHS(iHit);

        // get hit position
        const TVector3 hitPos = GetRawOrCalculatedHitPos(myHit);

        Double_t hitXLabSys = hitPos.X();
        Double_t hitYLabSys = hitPos.Y();
        Double_t hitZLabSys = hitPos.Z();
        Double_t hitXShifted = hitXLabSys - fInterceptZx; // shifts all x positions of hits so that they go through x=0 at z=zOffset (for parabola)
        Double_t hitZShifted = hitZLabSys - fZRefPos;     // z coordinate in local coordinate system (for parabola and for line)

        // y component of B field
        Double_t By = getByFromBField(hitXLabSys, hitYLabSys, hitZLabSys);

        const TString option = GetName();
        if ("parabola" == option) {
          // Use shifted x and shifted z for parabola
          yValLo = equationParabola(thetaRadLo, hitZShifted, hitXShifted, By);
          yValHi = equationParabola(thetaRadHi, hitZShifted, hitXShifted, By);
          if (9 < fVerbose) {
            std::cout << "Q/pzx = " << yValLo << '\n';
            std::cout << "Q/pzx = " << yValHi << '\n';
          }
        } else if ("parabolapz" == option) {
          yValLo = equationParabolaPz(thetaRadLo, hitZShifted, hitXShifted, By);
          yValHi = equationParabolaPz(thetaRadHi, hitZShifted, hitXShifted, By);

          if (9 < fVerbose) {
            std::cout << "pz/Q = " << yValLo << '\n';
            std::cout << "pz/Q = " << yValHi << '\n';
          }
        } else if (("lineBeforeDipole" == option) || ("lineBehindDipole" == option)) {
          // Use real x and shifted z for line

          yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitXLabSys);
          yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitXLabSys);

          if (9 < fVerbose) {
            std::cout << "xLP/PL = " << yValLo << '\n';
            std::cout << "xLP/PL = " << yValHi << '\n';
          }
        } else if ("lineZy" == option) {
          // Use real x and shifted z for line

          yValLo = equationLineZxOrZy(thetaRadLo, hitZShifted, hitYLabSys);
          yValHi = equationLineZxOrZy(thetaRadHi, hitZShifted, hitYLabSys);

          if (9 < fVerbose) {
            std::cout << "xLP = " << yValLo << '\n';
            std::cout << "xLP = " << yValHi << '\n';
          }
        } else {
          throwError("Error in FillHoughSpace! option " + option + " is not implemented!");
        }

        // 2 If peak 2nd value is within [min hit 2nd value - half width,  max hit 2nd value + half width] add the hit to the tracklet
        const Double_t yMin = std::min(yValLo, yValHi);
        const Double_t yMax = std::max(yValLo, yValHi);
        const Double_t yMinHw = fYaxis.GetBinWidth(yMin) / 2.;
        const Double_t yMaxHw = fYaxis.GetBinWidth(yMax) / 2.;

        if ((yMin - yMinHw <= peakSecondVal) && (yMax + yMaxHw >= peakSecondVal)) {
          Int_t hitIndex = fHitId.at(iHit).GetHitId();
          currentTracklet.AddHit(fFtsBranchId, hitIndex, hitZLabSys);
        }

      } // loop over hits
      /////////////////////////////////////////////////
      tracklets.push_back(currentTracklet);
      // } // z loop
    } // y loop
  }   // x loop

  if (1 < fVerbose)
    PrintFoundTracklets(tracklets);
  return tracklets;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughSpace::FindAllPeaksWithTSpectrum2(const UInt_t) // minHeight //[R.K.03/2017] unused variable(s)
{
  std::vector<PndFtsHoughTracklet> tracklets; // for output

  // check if Hough space has at least one entry
  if (1 > GetEntries()) {
    if (1 < fVerbose)
      std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
    return tracklets;
  }

  // code goes here
  throwError("This peak finder is not fully implemented!");

  // TODO: This peakfinder should be rechecked!
  // TODO: This peakfinder does not assign hits to the tracklets (so far)
  Int_t maxpeaks = 20;
  // Finding the peaks (as in example macro)
  TSpectrum2 s(maxpeaks, 3); // second argument: higher = peaks can be closer together (1 enforces 3 sigma seperation between peaks)
  s.SetAverageWindow(3);     // standard is 3 (for Markov smoothing)
  s.SetDeconIterations(100); // standard is 3, more is better
  // 2nd parameter: sigma of searched peaks = 2 standard
  // 4th parameter: threshold: (default=0.05)  peaks with amplitude less than threshold*highest_peak are discarded.  0<threshold<1
  //		Int_t nfound = s.Search(houghspace, 2,"nobackground,nomarkov",0.5); // works ok for line and for parabola, kind of...
  Int_t nfound = s.Search(this, 2, "", 0.5); // TODO: does this work for my derived class?!?
  Double_t *xpeaks = (Double_t *)s.GetPositionX();
  Double_t *ypeaks = (Double_t *)s.GetPositionY();
  s.Print();

  // for output
  for (Int_t iPeak = 0; iPeak < nfound; ++iPeak) {
    Double_t peakThetaVal = xpeaks[iPeak];
    Double_t peakSecondVal = ypeaks[iPeak];

    Int_t binmaxglobal = Fill(peakThetaVal, peakSecondVal, 0); // returns binnumber without modifying the histogram
    Double_t peakThetaHw = fXaxis.GetBinWidth(peakThetaVal) / 2.;
    Double_t peakSecondHw = fYaxis.GetBinWidth(peakSecondVal) / 2.;

    Double_t currentHeight = GetBinContent(binmaxglobal);

    // create tracklet and push it back to output
    PndFtsHoughTracklet currentTracklet(fZRefPos, fTrackerTask);
    currentTracklet.SetHoughTransformResults(peakThetaVal, peakSecondVal, currentHeight, peakThetaHw, peakSecondHw);
    tracklets.push_back(currentTracklet);
  }
  //		binmaxglobal = houghspace->Fill(peakTheta, peakSecond, 0); // returns binnumber without modifying the histogram

  if (1 < fVerbose)
    PrintFoundTracklets(tracklets);
  return tracklets;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughSpace::FindAllPeaksBlanko(const UInt_t) // minHeight //[R.K.03/2017] unused variable(s)
{
  std::vector<PndFtsHoughTracklet> tracklets; // for output

  // check if Hough space has at least one entry
  if (1 > GetEntries()) {
    if (1 < fVerbose)
      std::cout << "Hough Space is empty. No peak can be found. Return empty tracklet vector." << '\n';
    return tracklets;
  }

  // code goes here
  throwError("This peak finder is not implemented!");

  if (1 < fVerbose)
    PrintFoundTracklets(tracklets);
  return tracklets;
}

// ----------------------
// Ideas for other peak finders
// ----------------------

// 1. allPeaks>minHeight, merge peaks
// find bin >= minHeight, then merge with neighboring bins of equal height, set surrounding bins to 0 (otherwise tail of peaks will be found in addition to peak itself)
