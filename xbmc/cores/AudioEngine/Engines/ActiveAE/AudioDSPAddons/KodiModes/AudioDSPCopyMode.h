#pragma once


#include <string>
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"
#include "cores/DSP/Factory/Interfaces/IDSPNodeCreator.h"
#include "c:\Users\aturan\Projekte\privat\kodi_adsp_v2\kodi_17.1_adsp_v2\xbmc\addons\kodi-addon-dev-kit\include\kodi\kodi_adsp_types.h"


namespace ActiveAE
{
class CAudioDSPCopyModeCreator : public DSP::IDSPNodeCreator
{
public:
  CAudioDSPCopyModeCreator(NodeID_t &ID);

  virtual DSP::IDSPChainNode* InstantiateNode() override;
  virtual DSPErrorCode_t DestroyNode(DSP::IDSPChainNode *&Node) override;
};


class CAudioDSPCopyMode : public DSP::AUDIO::IADSPNode
{
  CAudioDSPCopyMode() : IADSPNode("CAudioDSPCopyMode", 0, ADSP_DataFormatFlagINVALID) {} // hide default constructor to prevent creation without a valid ID
public:
  CAudioDSPCopyMode(uint64_t ID);

  virtual DSPErrorCode_t CreateInstance(const DSP::AUDIO::CADSPProperties *InputProperties, DSP::AUDIO::CADSPProperties *OutputProperties, void *Options = nullptr) override;
  virtual DSPErrorCode_t ProcessInstance(float *In[AE_DSP_CH_MAX], float *Out[AE_DSP_CH_MAX]) override;
  virtual DSPErrorCode_t DestroyInstance() override;

private:
  DSP::AUDIO::CADSPProperties m_InputProperties;
};
}
