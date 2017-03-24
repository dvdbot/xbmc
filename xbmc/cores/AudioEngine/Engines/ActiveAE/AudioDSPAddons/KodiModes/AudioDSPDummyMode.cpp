#include "cores/AudioEngine/Engines/ActiveAE/AudioDSPAddons/KodiModes/AudioDSPDummyMode.h"

using namespace DSP;
using namespace DSP::AUDIO;


namespace ActiveAE
{
CAudioDSPDummyModeCreator::CAudioDSPDummyModeCreator(NodeID_t &ID) :
  IDSPNodeCreator(ID)
{
}

IDSPChainNode *CAudioDSPDummyModeCreator::InstantiateNode()
{
  return nullptr;
}

DSPErrorCode_t CAudioDSPDummyModeCreator::DestroyNode(DSP::IDSPChainNode *&Node)
{
  return DSPErrorCode_t();
}


CAudioDSPDummyMode::CAudioDSPDummyMode() :
  IADSPNode("CAudioDSPDummyMode", 0/*ID*/, ADSP_DataFormatFlagFloat) //! @todo register internal mode and get ID from factory
{
}

DSPErrorCode_t CAudioDSPDummyMode::CreateInstance(const CADSPProperties *InputProperties, CADSPProperties *OutputProperties, void *Options)
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPDummyMode::DestroyInstance()
{
  return DSP_ERR_NO_ERR;
}

DSPErrorCode_t CAudioDSPDummyMode::ProcessInstance(float **In, float **Out)
{
  return DSP_ERR_NO_ERR;
}
}
