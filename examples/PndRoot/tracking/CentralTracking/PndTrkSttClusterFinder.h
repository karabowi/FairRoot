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

#ifndef PndTrkSttClusterFinder_H
#define PndTrkSttClusterFinder_H 1

#include "TClonesArray.h"

class PndTrkSttClusterFinder {

 public:
  /** Default constructor **/
  PndTrkSttClusterFinder();

  /** Destructor **/
  virtual ~PndTrkSttClusterFinder();

  Short_t FindTrackPattern(bool *InclusionListStt,          // input; here it is the exclusion of Stt hits for Stt multiple hits
                                                            // OR because the hit was already used in another cluster;
                           Short_t *ListHitsinCluster,      // output
                           Short_t (*ListParContiguous)[6], // input, this is the list of contiguous TubeID ;
                           Short_t MAXHITSINCLUSTER,        // input;
                           Short_t *nParContiguous,         // input; number of contiguous Stt axial straws to a given Stt axial straw;
                           Short_t number_straws,           // input
                           Short_t SeedHit,                 // input
                           Short_t *SttStrawOn,             // input;  SttStrawOn[i] >= 0 --> it is the Stt hit
                                                            // number corresponding to Stt i-th Tube ID;
                                                            // SttStrawOn[i] == -1 --> i-th Stt straw NOT hit;
                           Short_t *TubeID                  // input

  );

  void GetClusters(bool *InclusionListStt,         // input; this is the exclusion of Stt hits for Stt multiple hits
                                                   // OR because the hit was already used in another cluster;
                   Short_t ListParContiguous[][6], // input list (axial Stt); first dimension is NUMBER_STRAWS;
                   Short_t *ListSttParHits,        // input
                   Short_t *nParContiguous,        // input; number of contiguous straws (axial Stt);
                                                   // NUMBER_STRAWS even if the numbering scheme for the Stt straws goes
                                                   //  from 1 to NUMBER_STRAWS included;
                   Short_t nSttParHit,             // input;
                   const Short_t MAXFOUNDCLUSTERS, // input;
                   const Short_t MAXHITSINCLUSTER, // input;
                   const Short_t MAXSTTHITS,       // input; number of maximux allowed total Stt hits;
                   const Short_t NUMBER_STRAWS,    // input; number of Stt Straws in total;
                   Short_t *StrawCode,             // input;
                   Short_t *StrawCode2,            // input;
                   Short_t *SttStrawOn,            // input;
                   TClonesArray *SttTubeArray,     // input; array of the Stt tubes;
                   Short_t *TubeID,                // input;

                   Short_t *ListHitsinCluster, // output;
                   Short_t &nFoundClusters,    // output;
                   Short_t *nHitsinCluster     // output;
  );

  ClassDef(PndTrkSttClusterFinder, 1);
};

#endif
