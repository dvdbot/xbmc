#pragma once


#include <string>
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/Interfaces/IADSPNode.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ActiveAEDSPAddon.h"


namespace ActiveAE
{
class CAudioDSPAddonModeNode : public DSP::AUDIO::IADSPNode
{
public:
  CAudioDSPAddonModeNode(AE_DSP_ADDON Addon, uint64_t ID, int32_t AddonModeID);

  virtual DSPErrorCode_t CreateInstance(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options = nullptr) override;
  virtual DSPErrorCode_t ProcessInstance(float **In, float **Out) override;
  virtual DSPErrorCode_t DestroyInstance() override;

private:
  AE_DSP_ADDON m_Addon; //! @todo m_Addon is easier, but call history is bigger
  AudioDSP m_DllFunctions; //! @todo m_DllFunctions is more complex, but call history is smaller
};
}
