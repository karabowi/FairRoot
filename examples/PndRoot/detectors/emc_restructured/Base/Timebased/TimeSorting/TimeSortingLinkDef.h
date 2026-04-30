#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Timebased
#pragma link C++ class BSEmcDigiTimeBuncherProcess + ;

// Timesorting
#pragma link C++ class BSEmcRingSorter < BSEmcDigi > +;
#pragma link C++ class BSEmcRingSorter < BSEmcPrecluster > +;

#pragma link C++ class BSEmcSorterTask < BSEmcDigi > +;
#pragma link C++ class BSEmcSorterTask < BSEmcPrecluster > +;
#pragma link C++ class BSEmcFetchDigisFromPreclusterLinks + ;
#pragma link C++ class BSEmcDigiTimeBuncherProcess + ;
#pragma link C++ class BSEmcPreclusterTimeBuncherProcess + ;

#endif
