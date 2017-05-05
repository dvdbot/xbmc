#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPCopyMode.h"
#include <string.h>

using namespace DSP;
using namespace DSP::AUDIO;


namespace ActiveAE
{
CAudioDSPCopyModeCreator::CAudioDSPCopyModeCreator()
{
}

IDSPNode *CAudioDSPCopyModeCreator::InstantiateNode(uint64_t ID)
{
  CAudioDSPCopyMode *copyMode = new CAudioDSPCopyMode(ID);
  IDSPNode *node = dynamic_cast<IDSPNode*>(copyMode);

  if (!node)
  {
    delete copyMode;
  }

  return node;
}

DSPErrorCode_t CAudioDSPCopyModeCreator::DestroyNode(DSP::IDSPNode *&Node)
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
  IADSPNode("CAudioDSPCopyMode", ID, ADSP_DataFormatFlagFloat) //! @todo set format flags with |
{
}

DSPErrorCode_t CAudioDSPCopyMode::CreateInstance(const AEAudioFormat *InputProperties, AEAudioFormat *OutputProperties, void *Options)
{
  m_InputProperties = *InputProperties;
  *OutputProperties = *InputProperties;


  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::DestroyInstance()
{
  memset(&m_InputProperties, 0, sizeof(CADSPProperties));
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::ProcessInstance(float *In[AE_DSP_CH_MAX], float *Out[AE_DSP_CH_MAX])
{
  for (uint8_t ch = 0; ch < AE_DSP_CH_MAX; ch++)
  {
    for (uint32_t ii = 0; ii < m_InputProperties.m_frameSize; ii++)
    {
      Out[ch][ii] = In[ch][ii];
    }
  }

  return DSP_ERR_NO_ERR;
}
}
