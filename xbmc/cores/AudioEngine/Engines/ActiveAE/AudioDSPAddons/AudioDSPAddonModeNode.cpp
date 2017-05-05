#include "AudioDSPAddonModeNode.h"

using namespace DSP;
using namespace DSP::AUDIO;


namespace ActiveAE
{
CAudioDSPAddonModeNode::CAudioDSPAddonModeNode(AE_DSP_ADDON Addon, uint64_t ID, int32_t AddonModeID) :
  m_Addon(Addon),
  IADSPNode(Name, ID, ADSP_DataFormatFlagFloat)
{
  memset(&m_DllFunctions, 0, sizeof(m_DllFunctions));
}

DSPErrorCode_t CAudioDSPAddonModeNode::CreateInstance(const AEAudioFormat* InputProperties, AEAudioFormat* OutputProperties, void *Options)
{
  if (!m_Addon->GetAddonProcessingCallbacks(m_DllFunctions))
  {
    return DSP_ERR_FATAL_ERROR;
  }

  //! @todo simplify add-on mode creation API

  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPAddonModeNode::DestroyInstance()
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPAddonModeNode::ProcessInstance(float **In, float **Out)
{
  if (!m_DllFunctions.PostProcess)
  {
    return DSP_ERR_FATAL_ERROR;
  }

  m_DllFunctions.PostProcess(nullptr, 0, In, Out, 0); //! @todo change API to PostProcess(HANDLE, In, Out), size and ID is set during creation

  return DSP_ERR_NO_ERR;
}
}
