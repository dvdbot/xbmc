#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonNodeCreator.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonModeNode.h"

using namespace DSP;
using namespace DSP::AUDIO;

namespace ActiveAE
{
CAudioDSPAddonNodeCreator::CAudioDSPAddonNodeCreator(const AE_DSP_ADDON &Addon) :
  m_Addon(Addon)
{
}

DSPErrorCode_t CAudioDSPAddonNodeCreator::DestroyNode(IADSPNode *&Node)
{
  if (!Node)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  DSPErrorCode_t err = Node->DestroyInstance();

  delete Node;
  Node = nullptr;

  return err;
}

IADSPNode* CAudioDSPAddonNodeCreator::InstantiateNode(uint64_t ID)
{
  IADSPNode *node = dynamic_cast<IADSPNode*>(new CAudioDSPAddonModeNode(m_Addon, ID, 0)); //! @todo use <add-on name>::<mode name> as ID identifier generation and add-on mode ID
  if (!node)
  {
    return nullptr;
  }

  return node;
}
}
