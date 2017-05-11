set(SOURCES Models/DSPNodeModel.cpp)

set(HEADERS DSPObject.h

            Factory/Interfaces/IDSPNodeCreator.h
            Factory/Interfaces/IDSPNodeFactory.h
            Factory/Interfaces/IDSPIDFactory.h

            Models/DSPChainBasicModel.h
            Models/DSPNodeModel.h
            Models/TDSPChainNodeModel.h
            Models/Interfaces/IDSPNodeModel.h
            Models/Interfaces/IDSPNodeModelCallback.h
            Models/Interfaces/IDSPChainNodeModel.h

            Typedefs/DSPTypedefs.h)

core_add_library(dsp)
