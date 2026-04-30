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
 @class PndFtsHoughSpace

 @author Martin J. Galuska <martin [dot] j [dot] galuska (at) physik [dot] uni (minus) giessen [dot] de>

 @brief Class for Hough space based on TH2S (for the moment). Saves the hits which enter this Hough space and finds peaks.

Note //! after a member means for root that it should not write it to disk when the object is saved

TODO Save path through Hough spaces with global indices!!!
TODO: Separate peak finder from Hough space

 The angle (theta in rad) is always on x-axis, the value on the y-axis depends on the kind of Hough transform:
  HT type  | yValue
  --------:|:-------
  parabola | Q/p_{zx}
  line     | intercept (Achsenabschnitt) (in z-x- or z-y-plane)

 All FTS hits are scanned within the constructor by filterInputHits().
 The hits which are relevant for this Hough space are saved in fHitId and can be accessed with getHit(UInt_t index).

 Created: 21.02.2014
 */

#ifndef PndFtsHoughSpace_H
#define PndFtsHoughSpace_H

#include "PndFtsHoughTrackerTask.h"
#include "PndFtsHoughSpacePeak.h"
#include "PndFtsHoughSpaceBinning.h"

#include "TH2.h"
#include <cmath>
#include <vector>
#include "Rtypes.h"     // for Double_t, Int_t, etc
#include "FairLogger.h" // for FairLogger, MESSAGE_ORIGIN

#include "PndTrackCandHit.h"
#include "TClonesArray.h"

// magnetic field
#include "FairField.h"
#include "TVector3.h"

#include "PndFtsHoughTracklet.h"
#include "PndFtsHoughTrackCand.h"

// For error throwing
#include "TString.h"
#include <stdexcept>

// for saving the path through the Hough space
typedef std::vector<Int_t> IdxPath;               // helper -- path for one hit
typedef std::map<Int_t, IdxPath> HitIdxPathMap;   // that is the one I need -- map of hit indices to path for the corresponding hits
typedef std::pair<Int_t, IdxPath> HitIdxPathPair; // helper for inserting one pair of hit index and path in map

// cout for the above types
std::ostream &operator<<(std::ostream &os, const IdxPath &outVector);
std::ostream &operator<<(std::ostream &os, const HitIdxPathMap &outMap);

class PndFtsHoughSpace : public TH2S {
 public:
  // Constructors/Destructors ---------
  PndFtsHoughSpace(const char *name = nullptr, const Int_t refIndex = -1,

                   PndFtsHoughSpaceBinning binning = PndFtsHoughSpaceBinning(),

                   Double_t zRefPos = 0., Double_t interceptZx = 0.,

                   PndFtsHoughTrackCand *associatedTrackCand = nullptr,

                   PndFtsHoughTrackerTask *trackerTask = nullptr);
  ~PndFtsHoughSpace();

  //	TH2S ExportTH2S();

  // General info for peak finders
  //	The peaks contain the following information
  //			peakTheta = theta for peak
  //			peakSecond = Q/pzx for peak (parabola HT)
  //			peakSecond = intercept for peak (line HT) (in z-x- or z-y-plane)
  //
  //			peakThetaHw = half width of peak in theta
  //			peakSecondHw = half width of peak in second value (see above for what it stands for)
  //
  //			actualHeight height of the peak in the histogram (in counts)

  /**@brief Finds all peaks that satisfy the minimum height requirement minHeight.
   *
   * @param minHeight only bins of at least this height are regarded as possibly belonging to a peak.
   * @return tracklets containing all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
   */
  std::vector<PndFtsHoughTracklet> FindAllPeaksScanPathsMergeBins(const UInt_t minHeight);

  /**@brief Finds all peaks that satisfy the minimum height requirement minHeight.

   * @param minHeight only bins of at least this height are regarded as possibly belonging to a peak.
   * @return tracklets containing all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
   */
  std::vector<PndFtsHoughTracklet> FindAllPeaksScanPathsMergeBinsCalculatingPaths(const UInt_t minHeight);

  /**@brief Finds all peaks that satisfy the minimum height requirement minHeight.

   * @param minHeight only bins of at least this height are regarded as possibly belonging to a peak.
   * @return tracklets containing all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
   */
  std::vector<PndFtsHoughTracklet> FindAllPeaksWithTSpectrum2(const UInt_t minHeight);

  /**@brief Finds all peaks that satisfy the minimum height requirement minHeight.

   * @param minHeight only bins of at least this height are regarded as possibly belonging to a peak.
   * @return tracklets containing all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
   */
  std::vector<PndFtsHoughTracklet> FindAllPeaksBinsWoMergingWithSearchWindow(const UInt_t minHeight, const Int_t vicinityLength = 0);

  /**@brief Finds all peaks that satisfy the minimum height requirement minHeight.

   * @param minHeight only bins of at least this height are regarded as possibly belonging to a peak.
   * @return tracklets containing all values found for the peaks in the Hough space. They will contain the hitIds of all hits that contribute to the peaks.
   */
  std::vector<PndFtsHoughTracklet> FindAllPeaksBlanko(const UInt_t minHeight);

  /**@brief Fills the Hough space using the equation which corresponds to the name of the Hough space.

   If something goes wrong the function throws a runtime_error (probably Hough space name is set incorrectly).

   !!! WARNING The theta values (in rad) are NOT the same as in the interaction point.
   They are always calculated relative to a shifted coordinate system and only 2-dimensional (either in z-x- or z-y-plane) !!!

   The angle (theta in rad) to the z-axis in the z-x- or z-y-plane at a z reference position will be scanned
   from theta corresponding to lowest bin to theta corresponding to highest bin of x-axis.

   TODO For each hit the "path" through the 2d histogram is saved for peak finding.

   y component of B field will be read from field maps if fKeepBConstant is kFALSE.

   @param[in] Running index, only needed for Debugging output of Hough spaces
   */
  void FillHoughSpace();

  // operators
  // PndFtsHoughSpace are the same if they contain the same hits, that means if the PndTrackCand are the same, therefore no need to implement that operator here

  // Accessors -----------------------
  inline void Print() const;

  void setVerbose(Int_t verbose) { fVerbose = verbose; };

  Double_t getInterceptZx() const { return fInterceptZx; };

  Double_t getZRefPos() const { return fZRefPos; };

  inline UInt_t GetNHits() const { return fHitId.size(); };

 private:
  // for PandaRoot input/output
  PndFtsHoughTrackerTask *fTrackerTask; //!

  /** @brief For error reporting */
  void throwError(const TString s) const { throw std::runtime_error(s.Data()); };

  Bool_t setParametersForHsOption(); // set parameters according to the kind of Hough transform I want to do
  void filterInputHits(); // copies input hits (based on z coordinate and skewed/non-skewed) from fFtsHitArray (all FTS hits) to fHitId (only the hits that qualify for the specific
                          // Hough transform)
  inline void AddHitToHS(UInt_t hitId, Double_t rho);
  inline void AddHitToHS(FairLink link, Double_t rho);
  inline Bool_t IsHitFromTubeIdAlreadyAdded(const Int_t tubeIdToAdd);
  inline const PndFtsHit *getHitFromHS(UInt_t index) const;                                // gets the FTS hit corresponding to index
  inline Int_t getHitIdFromHS(UInt_t index) const { return fHitId.at(index).GetHitId(); }; // gets the FTS hit Id corresponding to index
  inline const TVector3 CalculateHitPosFromIntersectionsWithZxTrackModel(const PndFtsHit *const myHit) const;
  inline const TVector3 GetRawOrCalculatedHitPos(const PndFtsHit *const myHit) const;

  void AddHitsToTrackletByCalculating(PndFtsHoughTracklet *currentTracklet, Int_t locmax);

  /**For each hit index the "path through the Hough space" [ordered globalbins which were filled during the thetaRad scan] is saved
   * This is useful for peak finding.
   * Map index: hit index, use as argument of getHit.
   * The map gives the vector of globalbins which were filled during the theta scan for the hit.
   */
  HitIdxPathMap fHitThetaYIdxPath; //!

  // Private Data Members ------------
  Int_t fVerbose;        //!
  const Int_t fRefIndex; ///< for debugging output (to which index in some loop does this Hough space and its output belong to)

  /// @ brief FTS Hits
  Int_t fFtsBranchId;
  std::vector<PndTrackCandHit> fHitId; //! ///< @brief hits relevant for this Hough space
  ///< first index is detId, second index is hit Id

  PndFtsHoughTrackCand *fAssociatedTrackCand; ///< @brief For skewed hits a track candidate is needed

  // only hits with a z value (in the laboratory system, zreal) between onlyusehitsfromz and onlyusehitsuptoz will be used for building the houghspace
  Double_t fOnlyUseHitsFromZ;
  Double_t fOnlyUseHitsUpToZ;
  Bool_t fUseNonSkewedStraws; // if kTRUE, then hits from non-skewed straws are used for Hough transform
  Bool_t fUseSkewedStraws;    // if kTRUE, then hits from skewed straws are used for Hough transform

  Bool_t fKeepBConstant; // set only to kFALSE for testing
  // If kTRUE the y-component of the B-field is not used in the parabola hough transform
  // if kFALSE the parabola's shape will be adjusted based on the magnetic field maps

  // at which z value the values are calculated
  Double_t fZRefPos; // is used to redefine an origin for the coordinate system (so that the angle definition gives meaningful theta values)
  //			Double_t interceptZx; // cannot be constant, because might need to be reset if set incorrectly (has to be 0 for line HT)
  // is used to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
  // (zreal=zOffset, xreal=interceptZx) = (zshifted = 0, xshifted = 0)
  // zshifted = zreal - zRefPos
  // xshifted = xreal - interceptZx
  // zreal = zshifted + zRefPos
  // xreal = xshifted + interceptZx
  // For the z-x-plane parabola, a shift in x (hitshiftinx) needs to be set (which should be the result of the straight line hough transform before the dipole field)
  // For the straight line (stations before dipole field) hitshiftinx HAS TO BE ZERO

  // is used only for parabola in zx plane to shift the true x values of hits so that they hit the point (zOffset|0) in z-x-plane (value is determined by line fit on chambers1+2)
  Double_t fInterceptZx;

  // for B field access
  FairField *fField; //!

  // for HoughTransform
  /////////////////////
  // if kTRUE will correct the pzx prediction according to values which should be obtained from a line fit mc truth momentum VS. reco momentum with high statistics
  static const Bool_t fCorrectPzx = kFALSE;

  /**@brief Makes sure there are no holes in the Hough space by filling them with a line.

   Fills holes between (lastBinX, lastBinY) and (currentBinX, currentBinY) = (lastBinX+1, currentBinY) with a line.

   Holes can only appear in the yDirection and cannot appear in theta direction:
     We go from one bin to the next.
     From lower theta values to the next higher value.
   * @param lastBinX
   * @param lastBinY
   * @param currentBinY
   * @param ptrThetaYIdxPathVec Pointer to the vector which contains the path through the Hough space. If any holes are filled, the interpolated values will be written into this
   vector.
   * @return
   */
  inline Bool_t FillHoles(const Int_t lastBinX, const Int_t lastBinY, const Int_t currentBinY, IdxPath *ptrThetaYIdxPathVec);

  inline Double_t equationParabola(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
  {
    // for parabola equation
    const Double_t n = 1.;
    const Double_t e = 1.;
    const Double_t c = n * e / 2.;

    Double_t yVal = 1. / c / By * (-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad)) / pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2);

    // next line is with rotation as in paper (I believe it is incorrect)
    //					value = 1. / c / By 	* (hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta))/ pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2);
    return yVal;
  };

  inline Double_t equationParabolaPz(Double_t thetaRad, Double_t hitZShifted, Double_t hitXShifted, Double_t By)
  {
    // for parabola equation
    const Double_t n = 1.;
    const Double_t e = 1.;
    const Double_t c = n * e / 2.;

    // Use shifted x and shifted z for parabola
    Double_t yVal = c * By * pow((hitZShifted * cos(thetaRad) + hitXShifted * sin(thetaRad)), 2) / (-hitZShifted * sin(thetaRad) + hitXShifted * cos(thetaRad));

    // next line is with rotation as in paper (I believe it is incorrect)
    //					value = c*By*pow((hitZshifted * cos(realtheta) + hitXshifted * sin(realtheta)), 2)/(hitZshifted * sin(realtheta) - hitXshifted * cos(realtheta));

    return yVal;
  };

  inline Double_t equationLineZxOrZy(Double_t thetaRad, Double_t hitZShifted, Double_t hitXLabSys)
  { // TODO: Check if that also works in zy plane
    // calculate b which is the distance of point on line at z = zOffset from z axis
    Double_t yVal = -tan(thetaRad) * hitZShifted + hitXLabSys;
    return yVal;
  }

  //------
  // DEBUG
  //-----
  /** @brief For writing out debugging histograms.
   */
  inline TString GetDebugOutName(TString title = "", Int_t param = -1) const;
  /** @brief For writing out Hough spaces as histograms (for debugging purposes).
   */
  void WriteHistoOfHoughSpace() const;
  /** @brief Makes a new empty histogram with the same binning and limits as this.
   */
  TH2S MakeEmptyHistoOfSameDimensions(TString specifier = "", Int_t index = -1) const;
  /** @brief For writing out peaks in Hough spaces as histograms (for debugging purposes).
   */
  void WriteHistoOfAllPeaks(const std::vector<PndFtsHoughSpacePeak> &peaksToPlot) const;
  /** @brief For writing out paths (how the Hough space was filled seen from one hit) as histograms (for debugging purposes).
   */
  void WriteHistoOfAllPaths() const;
  /** @brief For writing out paths from hits which stem from the same MC truth tracks (how the Hough space would be filled if only hits from one track were present) as histograms
   * (for debugging and parameter optimization purposes).
   */
  void WriteHistoOfAllPathsForEachMcTruthTrack() const;
  inline void PrintFoundTracklets(const std::vector<PndFtsHoughTracklet> &tracklets) const;
  inline Double_t getByFromBField(Double_t hitXLabSys, Double_t hitYLabSys, Double_t hitZLabSys);

 public:
  ClassDef(PndFtsHoughSpace, 1);
};

// inline functions
void PndFtsHoughSpace::Print() const
{
  std::cout << "=========== PndFtsHoughSpace::Print() ==========" << '\n';
  std::cout << "fZRefPos = " << fZRefPos << '\n';
  std::cout << "fInterceptZx = " << fInterceptZx << 'n';
  TH2S::Print();
}

void PndFtsHoughSpace::PrintFoundTracklets(const std::vector<PndFtsHoughTracklet> &tracklets) const
{
  std::cout << tracklets.size() << " peaks found for " << GetName() << '\n';
  if (10 < fVerbose) {
    for (UInt_t i = 0; i < tracklets.size(); ++i) {
      tracklets[i].Print();
    }
  }
}

const PndFtsHit *PndFtsHoughSpace::getHitFromHS(UInt_t index) const
{
  if (GetNHits() < index)
    throwError("index too high");

  Int_t hitIndex = fHitId.at(index).GetHitId();
  const PndFtsHit *const myHit = (PndFtsHit *)fTrackerTask->GetFtsHit(hitIndex);
  return myHit;
}

const TVector3 PndFtsHoughSpace::GetRawOrCalculatedHitPos(const PndFtsHit *const myHit) const
{
  // get hit position
  TVector3 hitPos;
  if (kFALSE == myHit->GetSkewed()) {
    myHit->Position(hitPos);
  } else {
    hitPos = CalculateHitPosFromIntersectionsWithZxTrackModel(myHit);
  }
  return hitPos;
}

const TVector3 PndFtsHoughSpace::CalculateHitPosFromIntersectionsWithZxTrackModel(const PndFtsHit *const myHit) const
{
  if (0 == fAssociatedTrackCand)
    throwError("No track cand associated with Hough space. Cannot calculate possible hit pos. from track model.");

  const PndFtsTube *ftsTube = fTrackerTask->GetFtsTube(myHit);
  const TVector3 wireDirection = ftsTube->GetWireDirection();
  const TVector3 wireCenter = ftsTube->GetPosition();

  const Double_t hitZLabSys = wireCenter.Z();

  // calculate xTM according to track model at hitZLabSys
  const Double_t xTM = fAssociatedTrackCand->getXLabSys(hitZLabSys);
  // calculate param which is needed for xStraw = xTM
  // xTM = wireCenter.X() + param*wireDirection.X()
  const Double_t param = (xTM - wireCenter.X()) / wireDirection.X();
  // calculate corresponding yStraw
  const Double_t yStraw = wireCenter.Y() + param * wireDirection.Y();

  TVector3 crossingPosition(xTM, yStraw, hitZLabSys);
  return crossingPosition;
}

TString PndFtsHoughSpace::GetDebugOutName(TString title, Int_t param) const
{
  TString debugOut = "/home/plots/";
  debugOut += fTrackerTask->GetEventNr();
  debugOut += title;
  if (-1 != fRefIndex) {
    debugOut += "-rId";
    debugOut += fRefIndex;
  }
  if (-1 != param) {
    debugOut += param;
  }
  debugOut += ".rtg"; // root textual graphics ;) -- actually just a macro // png does not work in this way
  return debugOut;
}

Double_t PndFtsHoughSpace::getByFromBField(Double_t hitXLabSys, Double_t hitYLabSys, Double_t hitZLabSys)
{
  Double_t By = 0.;
  // set y component of B field constant
  if (kTRUE == fKeepBConstant) {
    // do not take B field into account
    By = 1.;
  } else {
    // Use B field information
    Double_t po[3], BB[3];
    po[0] = hitXLabSys; // Use magnetic field at real (not shifted) x position
    po[1] = hitYLabSys;
    po[2] = hitZLabSys;
    fField->GetFieldValue(po, BB); // return value in KG (G3)
    By = BB[1] / 10.;              // By is y-component of magnetic field in Tesla
  }
  return By;
}

#endif
