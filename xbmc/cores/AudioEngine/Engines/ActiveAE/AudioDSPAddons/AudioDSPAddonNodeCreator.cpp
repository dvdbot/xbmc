#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonNodeCreator.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonModeNode.h"

using namespace DSP;


namespace ActiveAE
{
CAudioDSPAddonNodeCreator::CAudioDSPAddonNodeCreator(const AE_DSP_ADDON &Addon) :
  m_Addon(Addon)
{
}

DSPErrorCode_t CAudioDSPAddonNodeCreator::DestroyNode(IDSPNode *&Node)
{
  if (!Node)
  {
    return DSP_ERR_INVALID_INPUT;
  }

  DSPErrorCode_t err = Node->Destroy();

  delete Node;
  Node = nullptr;

  return err;
}

IDSPNode* CAudioDSPAddonNodeCreator::InstantiateNode(uint64_t ID)
{
  IDSPNode *node = dynamic_cast<IDSPNode*>(new CAudioDSPAddonModeNode(m_Addon, ID, 0)); //! @todo use <add-on name>::<mode name> as ID identifier generation and add-on mode ID
  if (!node)
  {
    return nullptr;
  }

  return node;
}
}
