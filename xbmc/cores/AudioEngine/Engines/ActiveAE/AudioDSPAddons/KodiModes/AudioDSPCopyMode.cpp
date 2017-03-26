#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPCopyMode.h"

using namespace DSP;
using namespace DSP::AUDIO;


namespace ActiveAE
{
CAudioDSPCopyModeCreator::CAudioDSPCopyModeCreator(NodeID_t &ID) :
  IDSPNodeCreator(ID)
{
}

IDSPChainNode *CAudioDSPCopyModeCreator::InstantiateNode()
{
  CAudioDSPCopyMode *copyMode = new CAudioDSPCopyMode(IDSPNodeCreator::ID);
  IDSPChainNode *node = dynamic_cast<IDSPChainNode*>(copyMode);

  if (!node)
  {
    delete copyMode;
  }

  return node;
}

DSPErrorCode_t CAudioDSPCopyModeCreator::DestroyNode(DSP::IDSPChainNode *&Node)
{
  DSPErrorCode_t err = DSP_ERR_INVALID_INPUT;
  if (Node)
  {
    err = Node->Destroy();

    delete Node;
    Node = nullptr;
  }

  return err;
}


CAudioDSPCopyMode::CAudioDSPCopyMode(uint64_t ID) :
  IADSPNode("CAudioDSPCopyMode", ID, ADSP_DataFormatFlagFloat) //! @todo register internal mode and get ID from factory
{
}

DSPErrorCode_t CAudioDSPCopyMode::CreateInstance(const CADSPProperties *InputProperties, CADSPProperties *OutputProperties, void *Options)
{
  m_InputProperties = *InputProperties;
  *OutputProperties = *InputProperties;


  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::DestroyInstance()
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::ProcessInstance(float *In[AE_DSP_CH_MAX], float *Out[AE_DSP_CH_MAX])
{
  for (uint8_t ch = 0; ch < AE_DSP_CH_MAX; ch++)
  {
    for (uint32_t ii = 0; ii < m_InputProperties.frameLength; ii++)
    {
      Out[ch][ii] = In[ch][ii];
    }
  }

  return DSP_ERR_NO_ERR;
}
}
