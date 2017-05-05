#pragma once


#include <string>
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"
#include "cores/DSP/Factory/Interfaces/IDSPNodeCreator.h"
#include "addons/kodi-addon-dev-kit/include/kodi/kodi_adsp_types.h"


namespace ActiveAE
{
class CAudioDSPCopyModeCreator : public DSP::TDSPNodeCreator<CAudioDSPCopyModeCreator>
{
public:
  CAudioDSPCopyModeCreator();

  virtual DSP::IDSPNode* InstantiateNode(uint64_t ID) override;
  virtual DSPErrorCode_t DestroyNode(DSP::IDSPNode *&Node) override;
};


class CAudioDSPCopyMode : public DSP::AUDIO::IADSPNode
{
  CAudioDSPCopyMode() : IADSPNode("CAudioDSPCopyMode", 0, DSP::AUDIO::ADSP_DataFormatFlagINVALID) {} // hide default constructor to prevent creation without a valid ID
public:
  CAudioDSPCopyMode(uint64_t ID);

  virtual DSPErrorCode_t CreateInstance(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options = nullptr) override;
  virtual DSPErrorCode_t ProcessInstance(float *In[AE_DSP_CH_MAX], float *Out[AE_DSP_CH_MAX]) override;
  virtual DSPErrorCode_t DestroyInstance() override;

private:
  AEAudioFormat m_InputProperties;
};
}
