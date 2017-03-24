#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonNodeCreator.h"
#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/AudioDSPAddonModeNode.h"

using namespace DSP;


namespace ActiveAE
{
CAudioDSPAddonNodeCreator::CAudioDSPAddonNodeCreator(NodeID_t &ID, const AE_DSP_ADDON &Addon) :
  IDSPNodeCreator(ID), //! @todo assign mode ID
  m_Addon(Addon)
{
}

DSPErrorCode_t CAudioDSPAddonNodeCreator::DestroyNode(IDSPChainNode *&Node)
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

IDSPChainNode* CAudioDSPAddonNodeCreator::InstantiateNode()
{
  IDSPChainNode *node = dynamic_cast<IDSPChainNode*>(new CAudioDSPAddonModeNode(m_Addon, IDSPNodeCreator::ID, 0)); //! @todo use <add-on name>::<mode name> as ID identifier generation and add-on mode ID
  if (!node)
  {
    return nullptr;
  }

  return node;
}
}
