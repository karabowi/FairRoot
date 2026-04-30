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

#include "PndFtsHoughTrackFinder.h"
#include "PndFtsHoughSpaceBinning.h"

ClassImp(PndFtsHoughTrackFinder);

const Double_t PndFtsHoughTrackFinder::fThetaRadLineBehindDipoleMatchesToParabolaIfBelow = 5 * TMath::DegToRad();

PndFtsHoughTrackFinder::PndFtsHoughTrackFinder(PndFtsHoughTrackerTask *trackerTask)
  : fTrackerTask(trackerTask), fParams(),

    // min peak heights
    fMinPeakHeightZxLineBeforeDipole(6), fMinPeakHeightZxParabola(8), fMinPeakHeightZxLineBehindDipole(6), fMinPeakHeightZyLine(8),

    // event statistics
    fNLinesBeforeDipoleFound(0), fNLinesBehindDipoleFound(0), fNParabolasFound(0), fNTracksFound(0)
{
  if (nullptr == fTrackerTask) {
    std::cout << "PndFtsHoughTrackFinder FATAL ERROR Tracker task not set.\n";
  } else {
    if (3 < fTrackerTask->GetVerbose())
      std::cout << "PndFtsHoughTrackFinder called with tracker ptr " << fTrackerTask << '\n';
  }
}

PndFtsHoughTrackFinder::~PndFtsHoughTrackFinder()
{
  if (3 < fTrackerTask->GetVerbose())
    fTrackerTask->fLogger->Info(MESSAGE_ORIGIN, "Destructor of PndFtsHoughTrackFinder");
}

std::vector<PndFtsHoughTracklet> PndFtsHoughTrackFinder::FindLinesBehindDipoleZx()
{
  // zx plane: Straight line Hough transform behind dipole

  // the following parameters are valid at z position fZParabolaLine

  // min. and max. theta angles (in zx plane) for lines
  static const Int_t thetaDegLowHigh = 80; //  - for low, + for high (in degree)

  // min. and max. distance from z axis in x direction
  static const Double_t xLowHigh = 200; //  - for low, + for high (in cm)

  static const Int_t stepsPerThetaDeg = 8; // greater number means finer scanning in theta
  static const Int_t nBinsInSecVal = 400;  // greater number means more bins for second value of Hough space

  PndFtsHoughSpaceBinning binningZxLineBehindDipole(stepsPerThetaDeg, thetaDegLowHigh, nBinsInSecVal, xLowHigh);
  PndFtsHoughSpace houghSpaceZxLineBehindDipole("lineBehindDipole", -1, binningZxLineBehindDipole, fParams.getZParabolaLine(), 0., 0, fTrackerTask);

  // Do straight line Hough transform on non-skewed hits from stations 1+2
  try {
    houghSpaceZxLineBehindDipole.FillHoughSpace();
  } catch (std::runtime_error &e) {
    std::cerr << "Hough Space for zx line behind dipole could not be created! \n";
    std::cerr << "runtime_error: " << e.what() << '\n';
  }
  // find peaks for line hough space and store in vector
  std::vector<PndFtsHoughTracklet> trackletsLineBehindDipole = houghSpaceZxLineBehindDipole.FindAllPeaksScanPathsMergeBins(fMinPeakHeightZxLineBehindDipole);
  return trackletsLineBehindDipole;
}

std::vector<PndFtsHoughTracklet> PndFtsHoughTrackFinder::FindLinesBeforeDipoleZx()
{
  //----------------------------------
  // zx plane: Straight line Hough transform before dipole

  // parameters: for hits with MC truth coordinates with 6 min peak height
  // stepsPerThetaDeg = 16
  // binsInSecVal = 1000;

  // parameters: for hits with center of straws coordinates with 6 min peak height
  // stepsPerThetaDeg = 16
  // binsInSecVal = 1000;

  // the following parameters are valid at z position fZLineParabola

  // min. and max. theta angles (in zx plane) for lines
  static const Int_t thetaDegLowHigh = 25; // - for low, + for high (in degree)

  // min. and max. distance from z axis in x direction
  static const Double_t xLowHigh = 80; // - for low, + for high (in cm)

  static const Int_t stepsPerThetaDeg = 8; // greater number means finer scanning in theta
  static const Int_t nBinsInSecVal = 160;  // greater number means more bins for second value of Hough space

  PndFtsHoughSpaceBinning binningZxLineBeforeDipole(stepsPerThetaDeg, thetaDegLowHigh, nBinsInSecVal, xLowHigh);
  PndFtsHoughSpace houghSpaceZxLineBeforeDipole("lineBeforeDipole", -1, binningZxLineBeforeDipole, fParams.getZLineParabola(), 0., 0, fTrackerTask);

  // Do straight line Hough transform on non-skewed hits from stations 1+2
  try {
    houghSpaceZxLineBeforeDipole.FillHoughSpace();
  } catch (std::runtime_error &e) {
    std::cerr << "Hough Space could not be created! \n";
    std::cerr << "runtime_error: " << e.what() << '\n';
  }
  // find peaks for line Hough space and store in vector
  //	std::vector<PndFtsHoughTracklet> trackletsLineBeforeDipole = houghSpaceZxLineBeforeDipole.FindAllPeaksBinsWoMergingWithSearchWindow(fMinPeakHeightZxLineBeforeDipole);
  std::vector<PndFtsHoughTracklet> trackletsLineBeforeDipole = houghSpaceZxLineBeforeDipole.FindAllPeaksScanPathsMergeBins(fMinPeakHeightZxLineBeforeDipole);
  return trackletsLineBeforeDipole;
}

void PndFtsHoughTrackFinder::FindMatchingParabolaToLineBeforeDipoleZxAndAddLineBehindDipole(const std::vector<PndFtsHoughTracklet> &linesBeforeDipole,
                                                                                            const std::vector<PndFtsHoughTracklet> &linesBehindDipole)
{
  // loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
  // After that try to match a line after dipole to that track candidate.
  // If that is not possible, accept short track candidate if particle is likely to have left detector acceptance, otherwise discard.
  // LB4D = line before dipole

  // loop over lines before dipole
  for (UInt_t iLB4D = 0; iLB4D < linesBeforeDipole.size(); ++iLB4D) {
    const Double_t peakThetaRadLB4D = linesBeforeDipole[iLB4D].getThetaRadVal();
    const Double_t peakInterceptLB4D = linesBeforeDipole[iLB4D].getSecondVal();
    const Double_t peakThetaRadHwLB4D = linesBeforeDipole[iLB4D].getThetaRadHw();

    // determine where to look for parabola
    const Double_t parabolaStepsPerThetaDeg = fParams.getParabolaStepsPerThetaDeg();
    const Double_t parabolaHwScan = fParams.getParabolaHwScan();

    const Double_t parabolaThetaRadLow = peakThetaRadLB4D - parabolaHwScan * peakThetaRadHwLB4D;  // in rad
    const Double_t parabolaThetaRadHigh = peakThetaRadLB4D + parabolaHwScan * peakThetaRadHwLB4D; // in rad
    if (parabolaThetaRadHigh == parabolaThetaRadLow)
      std::cout << "ERROR: low and high are the same for parabola!\n";
    const Double_t parabolaThetaDegLow = parabolaThetaRadLow * TMath::RadToDeg();   // in deg
    const Double_t parabolaThetaDegHigh = parabolaThetaRadHigh * TMath::RadToDeg(); // in deg

    UInt_t thetaBins = ceil(parabolaStepsPerThetaDeg * (parabolaThetaDegHigh - parabolaThetaDegLow));

    if (1 < fTrackerTask->GetVerbose())
      std::cout << "event: " << fTrackerTask->GetEventNr() << " Line " << iLB4D << "\n thetaRadLowParabola=" << parabolaThetaRadLow
                << " thetaRadHighParabola=" << parabolaThetaRadHigh << " thetaBins=" << thetaBins << "  peakThetaRadLB4D=" << peakThetaRadLB4D
                << " peakThetaRadHwLB4D=" << peakThetaRadHwLB4D << " peakInterceptLB4D=" << peakInterceptLB4D << " peakThetaRadHwLB4D=" << peakThetaRadHwLB4D << '\n';

    const Int_t parabolaNBinsPzxInv = fParams.getParabolaNBinsPzxInv();
    const Double_t parabolaQDivPzxArgMax = fParams.getParabolaQDivPzxArgMax();

    PndFtsHoughSpaceBinning binningZxParabola(thetaBins, parabolaThetaRadLow, parabolaThetaRadHigh, parabolaNBinsPzxInv, -1 * parabolaQDivPzxArgMax, parabolaQDivPzxArgMax);
    PndFtsHoughSpace houghspaceZxParabola("parabola", iLB4D, binningZxParabola, fParams.getZLineParabola(), peakInterceptLB4D, 0, fTrackerTask);

    // Do parabola Hough transform for current line before dipole (shifts FTS hits by hitshiftinx) for non-skewed hits in stations 3+4+5
    try {
      houghspaceZxParabola.FillHoughSpace();
    } catch (std::runtime_error &e) {
      std::cerr << "Hough Space for zx parabola could not be created! \n";
      std::cerr << "runtime_error: " << e.what() << '\n';
    }

    // find peaks for zx parabola Hough space (for current line before dipole)
    //		std::vector<PndFtsHoughTracklet> zxParabolaTracklets = houghspaceZxParabola.FindAllPeaksScanPathsMergeBins(fMinPeakHeightZxParabola);
    std::vector<PndFtsHoughTracklet> zxParabolaTracklets = houghspaceZxParabola.FindAllPeaksBinsWoMergingWithSearchWindow(fMinPeakHeightZxParabola);

    // construct newHoughTrackCand from line+parabola+line (in zx)
    if (1 < fTrackerTask->GetVerbose()) {
      std::cout << "Create track candidates from line in zx before dipole " << iLB4D << " and all parabolas within dipole.\n"
                << "And all lines behind dipole which match line+parabola (if exist)\n";
    }
    // loop over all parabolas which were found for one line before the dipole
    for (UInt_t iParabola = 0; iParabola < zxParabolaTracklets.size(); ++iParabola) {
      // Try to find a matching line behind the dipole

      PndFtsHoughTrackCand lineParabola(fTrackerTask);
      lineParabola.SetZxLineBeforeDipole(linesBeforeDipole[iLB4D]);
      lineParabola.SetZxParabola(zxParabolaTracklets[iParabola]);

      // loop over all lines behind dipole
      // add track cands for each matching line behind dipole (LBhD)
      Bool_t foundLineBehindDipole = kFALSE;
      for (UInt_t iLBhD = 0; iLBhD < linesBehindDipole.size(); ++iLBhD) {
        if (kTRUE == LineBehindDipoleMatchesToLinePlusParabola(lineParabola, linesBehindDipole[iLBhD])) {
          PndFtsHoughTrackCand lineParabolaLine(lineParabola);              // make a copy
          lineParabolaLine.SetZxLineBehindDipole(linesBehindDipole[iLBhD]); // add line behind dipole
          fHoughTrackCandsZxPlaneOnly.push_back(lineParabolaLine);          // add as track candidate
          foundLineBehindDipole = kTRUE;
        }
      }

      // iif no lines behind dipole matched, add track cand without any line behind dipole
      if (kFALSE == foundLineBehindDipole)
        fHoughTrackCandsZxPlaneOnly.push_back(lineParabola);
    }
  } // loop over all line tracklets which were found by line HT before dipole field
}

void PndFtsHoughTrackFinder::FindZyLineMatchingToLineParabolaLineInZx()
{
  // loop over line+parabola+line track candidates in zx plane, pass track cand. to Hough space (one Hough transform needed for each line+parabola+line track cand.)
  // Check all skewed hits whether they might belong to any of the track candidates
  // Calculate (x,z) coordinate hypotheses for skewed hits which might belong to track candidates and run line Hough transform on these hypotheses
  //----------------------------------
  // zy plane: Straight line Hough transform
  // loop over line+parabola+line (LPL) from zx plane
  for (UInt_t iLPL = 0; iLPL < fHoughTrackCandsZxPlaneOnly.size(); ++iLPL) {
    // determine where to look for line in zy plane
    static const Int_t stepsPerThetaDegZyLine = 4; // greater number means finer scanning in theta
    static const Int_t thetaDegLowZyLine = -18;    // in degree
    static const Int_t thetaDegHighZyLine = 18;    // in degree
    // create Hough space
    PndFtsHoughSpaceBinning binningZyLine(stepsPerThetaDegZyLine * (thetaDegHighZyLine - thetaDegLowZyLine),
                                          thetaDegLowZyLine * TMath::DegToRad(),  // in rad
                                          thetaDegHighZyLine * TMath::DegToRad(), // in rad
                                          stepsPerThetaDegZyLine * 16,            // TODO: Check values
                                          -80.,                                   // in cm // TODO: Check values
                                          80.                                     // in cm
    );
    PndFtsHoughSpace houghspaceZyLine("lineZy", iLPL, binningZyLine, fParams.getZLineParabola(), 0., &(fHoughTrackCandsZxPlaneOnly[iLPL]), fTrackerTask);
    // Do line Hough transform for current line+parabola+line for skewed hits in all stations
    try {
      houghspaceZyLine.FillHoughSpace();
    } catch (std::runtime_error &e) {
      std::cerr << "Hough Space for zy line before dipole could not be created! \n";
      std::cerr << "runtime_error: " << e.what() << '\n';
    }
    // find peaks for line Hough space and store in vector
    std::vector<PndFtsHoughTracklet> trackletsZyLine = houghspaceZyLine.FindAllPeaksScanPathsMergeBins(fMinPeakHeightZyLine);
    // construct newHoughTrackCand by adding zy line to line+parabola+line (in zx)
    if (1 < fTrackerTask->GetVerbose()) {
      std::cout << "Create track candidates by adding line in zy to line+parabola+line in zx " << iLPL << "\n"
                << "If no zy line can be found, delete track candidate.\n";
    }
    // loop over all zy lines which were found for line+parabola+line
    for (UInt_t iZyLine = 0; iZyLine < trackletsZyLine.size(); ++iZyLine) {
      PndFtsHoughTrackCand fullTrackCand(fHoughTrackCandsZxPlaneOnly[iLPL]); // make copy
      fullTrackCand.SetZyLine(trackletsZyLine[iZyLine]);                     // add zy line
      fHoughTrackCandsComplete.push_back(fullTrackCand);                     // add as track candidate
    }
  } // loop over all line+parabola+line from zx plane
}

// void PndFtsHoughTrackFinder::CreatePndTrackCands() {
//
//}

void PndFtsHoughTrackFinder::FindTracks()
{

  if (1 < fTrackerTask->GetVerbose())
    std::cout << "PndFtsHoughTrackFinder::FindTracks()\n";

  // reset
  fHoughTrackCandsZxPlaneOnly.clear();
  fHoughTrackCandsComplete.clear();

  //--------------------------
  // Do we have enough hits in the FTS?
  Int_t minFtsHits = fMinPeakHeightZxParabola + fMinPeakHeightZxLineBeforeDipole;
  if (minFtsHits > fTrackerTask->GetNFtsHits()) {
    if (0 < fTrackerTask->GetVerbose())
      std::cout << "Skip event " << fTrackerTask->GetEventNr() << " since we have too few hits in FTS. (" << fTrackerTask->GetNFtsHits() << " present, " << minFtsHits
                << " required)\n";
    return;
  }

  // zx plane: Straight line Hough transform behind dipole
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "Lines behind dipole:\n";
  std::vector<PndFtsHoughTracklet> linesBehindDipole = FindLinesBehindDipoleZx();
  fNLinesBehindDipoleFound = linesBehindDipole.size();
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "event " << fTrackerTask->GetEventNr() << ": " << fNLinesBehindDipoleFound << " lines behind dipole found.\n";

  // zx plane: Straight line Hough transform before dipole
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "Lines before dipole:\n";
  std::vector<PndFtsHoughTracklet> linesBeforeDipole = FindLinesBeforeDipoleZx();
  fNLinesBeforeDipoleFound = linesBeforeDipole.size();
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "event " << fTrackerTask->GetEventNr() << ": " << fNLinesBeforeDipoleFound << " lines before dipole found.\n";

  // loop over all line tracklets which were found by line HT before dipole field and find a matching parabola
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "Matching parabolas to lines before dipole:\n";
  FindMatchingParabolaToLineBeforeDipoleZxAndAddLineBehindDipole(linesBeforeDipole, linesBehindDipole);
  fNParabolasFound = fHoughTrackCandsZxPlaneOnly.size();
  if (0 < fTrackerTask->GetVerbose())
    std::cout << "event " << fTrackerTask->GetEventNr() << ": " << fNParabolasFound << " parabolas matched!\n";

  // loop over line+parabola+line (LPL) from zx plane and do straight line Hough transform in zy plane
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "Lines in zy plane:\n";
  FindZyLineMatchingToLineParabolaLineInZx();
  fNTracksFound = fHoughTrackCandsComplete.size();
  if (1 < fTrackerTask->GetVerbose())
    std::cout << "event " << fTrackerTask->GetEventNr() << ": " << fNTracksFound << " tracks found.\n";

  // HIER GEHT ES WEITER!

  // TODO: Convert momenta to physical units
  //	pzinvpeak = 1./pzinvpeak*0.00299792458;
  //	pzinvpeakWithBField = pzinvpeak/BMeanForParabola;

  //	if(1<fTrackerTask->GetVerbose()) {
  //
  //		// TODO Add missing output
  //
  //
  //
  //
  //		//		if (kTRUE == correctpz)
  //		//		{
  //		//			std::cout << "Watch out! p_z value in plot is already corrected!!!"  << std::endl;
  //		//		}
  //	}
}

Bool_t PndFtsHoughTrackFinder::FilterTrackletsBasedOnSharedHits(UInt_t maxAcceptableSharedHits, std::vector<PndFtsHoughTracklet> &tracklets)
{
  // to store indices that I want to "delete"
  std::set<UInt_t> indicesToDelete;
  std::set<UInt_t>::iterator findIndex;

  // for all tracklets that share more than maxAcceptableSharedHits hits keep only the heighest peak
  // compare all tracklets with each other, keep track of which tracklets should be "deleted"
  for (UInt_t iTrackletLeft = 0; iTrackletLeft < tracklets.size(); ++iTrackletLeft) {
    for (UInt_t iTrackletRight = iTrackletLeft + 1; iTrackletRight < tracklets.size(); ++iTrackletRight) {
      // check if two tracks share more than maxAcceptableSharedHits hits
      PndFtsHoughTracklet trackletLeft = tracklets[iTrackletLeft];
      PndFtsHoughTracklet trackletRight = tracklets[iTrackletRight];
      const UInt_t nSharedHits = trackletLeft.getNSharedHits(trackletRight);
      if (nSharedHits > maxAcceptableSharedHits) {
        // mark the peak with the lower "height" for deletion
        const Double_t heightLeft = trackletLeft.getPeakHeightFromPeakFinder();
        const Double_t heightRight = trackletRight.getPeakHeightFromPeakFinder();
        if (heightLeft > heightRight) {
          indicesToDelete.insert(iTrackletRight);
        } else if (heightLeft < heightRight) {
          indicesToDelete.insert(iTrackletLeft);
        } else if (heightLeft == heightRight) {
          std::cout << "WARNING: Found two peaks of the same height that share " << nSharedHits << " hits.\n";
          std::cout << "The max. number of shared hits was set to " << maxAcceptableSharedHits << " hits.\n";
          std::cout << "Both peaks / tracklets will be kept. \n";
        }
      }
    }
  } // end for loop: compare all tracklets with each other

  // if we have no tracklets to "delete", we don't need to do anything
  if (0 == indicesToDelete.size()) {
    if (1 < fTrackerTask->GetVerbose()) {
      std::cout << "FilterTrackletsBasedOnSharedHits: No tracklets are marked for deletion.\n";
    }
    return kTRUE;
  }

  // create new tracklets vector and copy all entries from input vector to it which are not marked for deletion
  std::vector<PndFtsHoughTracklet> filteredTracklets;
  for (UInt_t iTracklet = 0; iTracklet < tracklets.size(); ++iTracklet) {
    // check if the index is marked for "deletion"
    findIndex = indicesToDelete.find(iTracklet);
    if (findIndex == indicesToDelete.end()) {
      // index was not found -> entry is not marked for deletion -> I should copy it
      filteredTracklets.push_back(tracklets[iTracklet]);
    }
  }
  // replace input tracklets with filtered ones
  tracklets = filteredTracklets;
  return kTRUE;
}
