#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPCopyMode.h"
#include <string.h>

using namespace DSP;
using namespace DSP::AUDIO;


namespace ActiveAE
{
CAudioDSPCopyModeCreator::CAudioDSPCopyModeCreator()
{
}

IADSPNode *CAudioDSPCopyModeCreator::InstantiateNode(uint64_t ID)
{
  CAudioDSPCopyMode *copyMode = new CAudioDSPCopyMode(ID);
  IADSPNode *node = dynamic_cast<IADSPNode*>(copyMode);

  if (!node)
  {
    delete copyMode;
  }

  return node;
}

DSPErrorCode_t CAudioDSPCopyModeCreator::DestroyNode(IADSPNode *&Node)
{
  DSPErrorCode_t err = DSP_ERR_INVALID_INPUT;
  if (Node)
  {
    err = Node->DestroyInstance();

    delete Node;
    Node = nullptr;
  }

  return err;
}


CAudioDSPCopyMode::CAudioDSPCopyMode(uint64_t ID) :
  IADSPNode("CAudioDSPCopyMode", ID, ADSP_DataFormatFlagFloat) //! @todo set format flags with |
{
}

DSPErrorCode_t CAudioDSPCopyMode::CreateInstance(AEAudioFormat &InputProperties, AEAudioFormat &OutputProperties, void *Options/* = nullptr*/)
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::DestroyInstance()
{
  m_InputFormat.m_channelLayout.Reset();
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPCopyMode::ProcessInstance(void *In, void *Out)
{
  //for (uint8_t ch = 0; ch < AE_DSP_CH_MAX; ch++)
  //{
  //  for (uint32_t ii = 0; ii < m_InputProperties.m_frameSize; ii++)
  //  {
  //    Out[ch][ii] = In[ch][ii];
  //  }
  //}

  return DSP_ERR_NO_ERR;
}
}
