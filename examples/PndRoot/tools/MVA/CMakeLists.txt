# Create a library called "libMva".

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
   ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/tools/MVA
  ${CMAKE_SOURCE_DIR}/tools/MVA/Clusters
  ${CMAKE_SOURCE_DIR}/tools/MVA/KNN
  ${CMAKE_SOURCE_DIR}/tools/MVA/LVQ
  ${CMAKE_SOURCE_DIR}/tools/MVA/TMVAkd_KNN
  ${CMAKE_SOURCE_DIR}/tools/MVA/TMVA_MCL
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${SIMPATH}/lib
)
 
link_directories( ${LINK_DIRECTORIES})
# ###################################
# Set HEADER files
SET (HEADERS
PndMvaClassifier.h
PndMvaDataSet.h
PndMvaTrainer.h
PndMvaUtil.h
PndMvaVariable.h
PndMvaClass.h
PndMvaVarPCATransform.h
Clusters/PndMvaCluster.h
Clusters/PndSomNode.h
Clusters/PndMvaSOMTrainer.h
KNN/PndStdKnnClassify.h
KNN/PndPrzWindowClassify.h
LVQ/PndLVQClassify.h
LVQ/PndLVQTrain.h
TMVAkd_KNN/PndKnnClassify.h
TMVAkd_KNN/PndKnnTrain.h
# TMVAkd_KNN/PndProjectedKNN.h
)
# MCL TMVA Headers.
SET (TMVA_ALG_HEADR
TMVA_MCL/PndMultiClassMlpTrain.h
TMVA_MCL/PndMultiClassMlpClassify.h
TMVA_MCL/PndMultiClassBdtTrain.h
TMVA_MCL/PndMultiClassBdtClassify.h
)

# Set SOURCE files
SET (SRCS
PndMvaClassifier.cpp
PndMvaDataSet.cpp
PndMvaTrainer.cpp
PndMvaUtil.cpp
PndMvaVarPCATransform.cpp
Clusters/PndMvaCluster.cpp
Clusters/PndSomNode.cpp
Clusters/PndMvaSOMTrainer.cpp
KNN/PndStdKnnClassify.cpp
KNN/PndPrzWindowClassify.cpp
LVQ/PndLVQClassify.cpp
LVQ/PndLVQTrain.cpp
TMVAkd_KNN/PndKnnClassify.cpp
TMVAkd_KNN/PndKnnTrain.cpp
# TMVAkd_KNN/PndProjectedKNN.cpp
)

# MCL TMVA Sources.
SET (TMVA_ALG_SOURCE
#TMVA_MCL/PndMultiClassMlpTrain.cpp
TMVA_MCL/PndMultiClassMlpClassify.cpp
#TMVA_MCL/PndMultiClassBdtTrain.cpp
TMVA_MCL/PndMultiClassBdtClassify.cpp
)

MESSAGE("-- <MVA_INFO> Building MVA with multiClass.")
SET(HEADERS ${HEADERS} ${TMVA_ALG_HEADR})
SET(SRCS ${SRCS} ${TMVA_ALG_SOURCE})

set(LINKDEF MVALinkDef.h)
set(LIBRARY_NAME Mva)

set(DEPENDENCIES Base ParBase PndData generalTools Stt Fts TMVA Tree)

PANDA_GENERATE_LIBRARY()

Install( FILES
PndMvaClass.h
PndMvaClassifier.h
PndMvaDataSet.h
PndMvaTools.h
PndMvaTrainer.h
PndMvaUtil.h
PndMvaVarPCATransform.h
PndMvaVariable.h
TMVA_MCL/PndMultiClassBdtClassify.h
TMVA_MCL/PndMultiClassBdtTrain.h
TMVA_MCL/PndMultiClassMlpClassify.h
TMVA_MCL/PndMultiClassMlpTrain.h
KNN/PndPrzWindowClassify.h
KNN/PndStdKnnClassify.h
TMVA_PDE_FOAM/PndPDEFoamClassify.h
TMVA_PDE_FOAM/PndPDEFoamTrain.h
Clusters/PndMvaCluster.h
Clusters/PndMvaSOMTrainer.h
Clusters/PndSomNode.h
TMVAkd_KNN/PndKnnClassify.h
TMVAkd_KNN/PndKnnTrain.h
TMVAkd_KNN/PndProjectedKNN.h
LVQ/PndLVQClassify.h
LVQ/PndLVQTrain.h
DESTINATION ${PANDA_INCLUDE}
)
