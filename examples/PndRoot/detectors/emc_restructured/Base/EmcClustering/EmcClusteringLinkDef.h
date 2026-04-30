#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class BSEmcClusteringTask + ;

// Clustering
#pragma link C++ class BSEmcRecursiveClustering + ;

#pragma link C++ class BSEmcClusteringProcess + ;
#pragma link C++ class BSEmcClusteringPar + ;
#pragma link C++ class BSEmcLocalMaxFindingProcess + ;
#pragma link C++ class BSEmcRemoveSplitOffMax + ;
#pragma link C++ class BSEmcExpClusterSplittingProcess + ;
#pragma link C++ class BSEmcAddLinksToCluster + ;
#pragma link C++ class BSEmcMCPropagationProcess + ;

#pragma link C++ class BSEmcClusterRadiusAlgo + ;
#pragma link C++ class BSEmcMakePreclusters + ;
#pragma link C++ class BSEmcMergePreclusters + ;
#pragma link C++ class BSEmcOnlineBumpSplitter + ;

#pragma link C++ class BSEmcClusteringContFact + ;

#endif
