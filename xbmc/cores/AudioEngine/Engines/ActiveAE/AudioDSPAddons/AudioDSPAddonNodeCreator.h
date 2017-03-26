#pragma once


#include "cores/DSP/Factory/Interfaces/IDSPNodeCreator.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/ActiveAEDSPAddon.h"

namespace ActiveAE
{
class CAudioDSPAddonNodeCreator : public DSP::IDSPNodeCreator
{
public:
  CAudioDSPAddonNodeCreator(NodeID_t &ID, const AE_DSP_ADDON &Addon);

  virtual DSP::IDSPNode* InstantiateNode();
  virtual DSPErrorCode_t DestroyNode(DSP::IDSPNode *&Node);

private:
  const AE_DSP_ADDON m_Addon;
};
}
