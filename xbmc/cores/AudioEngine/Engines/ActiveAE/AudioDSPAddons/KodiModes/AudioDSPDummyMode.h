#pragma once


#include <string>
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"
#include "cores/DSP/Factory/Interfaces/IDSPNodeCreator.h"


namespace ActiveAE
{
class CAudioDSPDummyModeCreator : public DSP::IDSPNodeCreator
{
public:
  CAudioDSPDummyModeCreator(NodeID_t &ID);

  virtual DSP::IDSPChainNode* InstantiateNode() override;
  virtual DSPErrorCode_t DestroyNode(DSP::IDSPChainNode *&Node) override;
};


class CAudioDSPDummyMode : public DSP::AUDIO::IADSPNode
{
public:
  CAudioDSPDummyMode();

  virtual DSPErrorCode_t CreateInstance(const DSP::AUDIO::CADSPProperties *InputProperties, DSP::AUDIO::CADSPProperties *OutputProperties, void *Options = nullptr) override;
  virtual DSPErrorCode_t ProcessInstance(float **In, float **Out) override;
  virtual DSPErrorCode_t DestroyInstance() override;
};
}
