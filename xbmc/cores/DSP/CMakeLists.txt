set(HEADERS DSPObject.h

            Factory/DSPNodeFactory.h
            Factory/Interfaces/IDSPNodeCreator.h
            Factory/Interfaces/IDSPNodeFactory.h
            Factory/Interfaces/IDSPIDFactory.h
            
            Models/DSPChainBasicModel.h
            Models/DSPChainModel.h
            Models/TDSPChainNodeModel.h
            Models/Interfaces/IDSPChainModel.h
            Models/Interfaces/IDSPChainNodeModel.h

            Nodes/Interfaces/IDSPNode.h

            Processors/Interfaces/IDSPProcessor.h
            
            Typedefs/DSPTypedefs.h)

core_add_library(dsp)