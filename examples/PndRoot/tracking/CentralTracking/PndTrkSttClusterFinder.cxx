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

#include "PndTrkSttClusterFinder.h"

#include "PndSttTube.h"
#include "PndTrkVectors.h"

#include "TClonesArray.h"
#include "TVector3.h"

#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;

/** Default constructor **/
PndTrkSttClusterFinder::PndTrkSttClusterFinder(){};

/** Destructor **/
PndTrkSttClusterFinder::~PndTrkSttClusterFinder(){};

//----------begin of function PndTrkSttClusterFinder::FindTrackPattern

Short_t PndTrkSttClusterFinder::FindTrackPattern(bool *InclusionListStt,          // input; in this list the Stt multiple hits are NOT included;
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
)
{

  Short_t ContiguousTube, CurrentTube, i;

  /*
   Short_t		tCurrentListofTubes[ MAXHITSINCLUSTER ];  // current list of tubes forming the cluster;

   Vec <Short_t>
      CurrentListofTubes(tCurrentListofTubes,MAXHITSINCLUSTER , "CurrentListofTubes");

  */

  Short_t CurrentListofTubes[MAXHITSINCLUSTER]; // current list of tubes forming the cluster;

  // the following is a necessary initialization every time a new cluster is searched starting from a given seed hit;

  /*
   bool tAlreadyConsidered[ number_straws ] ;   // for each Stt Straw;
   memset ( tAlreadyConsidered,false, sizeof( tAlreadyConsidered ));
   Vec<bool> AlreadyConsidered(tAlreadyConsidered,number_straws,"AlreadyConsidered");
  */

  bool AlreadyConsidered[number_straws]; // for each Stt Straw;
  memset(AlreadyConsidered, false, sizeof(AlreadyConsidered));

  // algorithm of clusterization by proximity only;

  Short_t nHitsinCluster = 1;
  Short_t nHitsAnalyzed = 0;
  ListHitsinCluster[0] = SeedHit;
  CurrentListofTubes[0] = TubeID[SeedHit];

  // the following variable is to make sure that the number of hits in the cluster
  // will be <= MAXHITSINCLUSTER;
  bool go = true;

  // nParContiguous[i] is the number of contiguous straws to the i-th straw (therefore the dimension
  //	is nParContiguous[4542] since nParContiguous[4542-1] = contiguous of straw n. 4542;
  // ListParContiguous is the List in TubeID contiguous to a given Tube number;
  // it MUST be used as follows : ListParContiguous[ j-1 ][i] = the TubeID of the (i+1)th contiguous straw
  //	to the j-th straw (tube number); therefore the dimension is ListParContiguous[4542][6];

  while (nHitsAnalyzed < nHitsinCluster && go) { // main loop;
    CurrentTube = CurrentListofTubes[nHitsAnalyzed];
    AlreadyConsidered[CurrentTube - 1] = true;
    nHitsAnalyzed++;
    for (i = 0; i < nParContiguous[CurrentTube - 1]; i++) {
      // SttStrawOn is -1 if the Tube was not hit, it is the Stt Hit number in opposite case;
      ContiguousTube = ListParContiguous[CurrentTube - 1][i];

      if (AlreadyConsidered[ContiguousTube - 1])
        continue;

      if (SttStrawOn[ContiguousTube - 1] > -1) {
        if (!InclusionListStt[SttStrawOn[ContiguousTube - 1]])
          continue;

        //  CurrentListofHits is the List of Stt HIT numbers belonging to the present cluster;
        ListHitsinCluster[nHitsinCluster] = SttStrawOn[ContiguousTube - 1];
        CurrentListofTubes[nHitsinCluster] = ContiguousTube;
        AlreadyConsidered[ContiguousTube - 1] = true;
        nHitsinCluster++;
        if (nHitsinCluster == MAXHITSINCLUSTER) {
          go = false;
          break;
        } // end of  if( nHitsinCluster
      }
    } // end of for(i=0;i< InOut->n ...
  }   // end of while(nRemaining > 0)

  return nHitsinCluster;
}

//----------end of function PndTrkSttClusterFinder::FindTrackPattern

//----------begin function  PndTrkSttClusterFinder::GetClusters

void PndTrkSttClusterFinder::GetClusters(bool *InclusionListStt,          // input; this is the exclusion of Stt hits for Stt multiple hits;
                                         Short_t ListParContiguous[][6],  // input list (axial Stt); first dimension is NUMBER_STRAWS;
                                         Short_t *ListSttParHits,         // input
                                         Short_t *nParContiguous,         // input; number of contiguous straws (axial Stt);
                                                                          // NUMBER_STRAWS even if the numbering scheme for the Stt straws goes
                                                                          //  from 1 to NUMBER_STRAWS included;
                                         Short_t nSttParHit,              // input;
                                         const Short_t MAXFOUNDCLUSTERS,  // input;
                                         const Short_t MAXHITSINCLUSTER,  // input;
                                         const Short_t MAXSTTHITS,        // input; number of maximux allowed total Stt hits;
                                         const Short_t NUMBER_STRAWS,     // input; number of Stt Straws in total;
                                         Short_t *StrawCode,              // input;
                                         Short_t *StrawCode2,             // input;
                                         Short_t *SttStrawOn,             // input;
                                         TClonesArray * /*SttTubeArray*/, //[R.K. 9/2018] unused	// input; array of the Stt tubes;
                                         Short_t *TubeID,                 // input;

                                         Short_t *ListHitsinCluster, // output;
                                         Short_t &nFoundClusters,    // output;
                                         Short_t *nHitsinCluster     // output;
)
{

  /*
    bool tinclusion_list_stt[MAXSTTHITS];
    Vec <bool> inclusion_list_stt(tinclusion_list_stt,MAXSTTHITS,"inclusion_list_stt");
  */
  bool inclusion_list_stt[MAXSTTHITS];

  Short_t i, iParHit, icode, icode2;

  for (i = 0; i < MAXSTTHITS; i++) {
    inclusion_list_stt[i] = InclusionListStt[i];
  }

  nFoundClusters = 0;

  for (iParHit = 0; iParHit < nSttParHit; iParHit++) {

    if (!inclusion_list_stt[ListSttParHits[iParHit]])
      continue;
    icode = StrawCode[TubeID[ListSttParHits[iParHit]] - 1];
    icode2 = StrawCode2[TubeID[ListSttParHits[iParHit]] - 1];
    if (!(icode == 13 || icode == 23 || icode == 10 || icode == 20 || icode == 15 || icode == 25 || icode == 12 || icode == 22 ||

          icode2 == 13 || icode2 == 23 || icode2 == 10 || icode2 == 20 || icode2 == 15 || icode2 == 25 || icode2 == 12 || icode2 == 22))
      continue; // only seeds at the external boundary of the STT

    //---------

    // find the cluster starting from the seed Stt axial hit; the cluster will not have more than MAXHITSINCLUSTER hits;

    // in the following inclusion list the Stt multiple hits are NOT included NOR the hit already used in a previous cluster;

    nHitsinCluster[nFoundClusters] = FindTrackPattern(
      /*
          tinclusion_list_stt,		// input; here it is the exclusion of Stt hits for Stt multiple hits
                  // OR because the hit was already used in another cluster;
      */

      inclusion_list_stt, // input; here it is the exclusion of Stt hits for Stt multiple hits
                          // OR because the hit was already used in another cluster;

      &ListHitsinCluster[nFoundClusters * MAXHITSINCLUSTER], // output
      ListParContiguous,                                     // input, this is the list of contiguous TubeID ;
      MAXHITSINCLUSTER,                                      // input;
      nParContiguous,                                        // input; number of contiguous Stt axial straws to a given Stt axial straw;
      NUMBER_STRAWS,                                         // input
      ListSttParHits[iParHit],                               // input, the Seed Hit;
      SttStrawOn,                                            // input;  SttStrawOn[i] >= 0 --> it is the Stt hit
                                                             // number corresponding to Stt i-th Tube ID;
                                                             // SttStrawOn[i] == -1 --> i-th Stt straw NOT hit;
      TubeID                                                 // input
    );
    // the found hits cannot partecipate to the formation of any other connected cluster, so exclude them;
    for (i = 0; i < nHitsinCluster[nFoundClusters]; i++) {
      inclusion_list_stt[ListHitsinCluster[nFoundClusters * MAXHITSINCLUSTER + i]] = false;
    }
    nFoundClusters++;
    if (nFoundClusters == MAXFOUNDCLUSTERS)
      break;
  } // end of for(iParHit=0; iParHit<nSttParHit ; iParHit++)
}
//---------- end function  PndTrkSttClusterFinder::GetClusters

ClassImp(PndTrkSttClusterFinder)
