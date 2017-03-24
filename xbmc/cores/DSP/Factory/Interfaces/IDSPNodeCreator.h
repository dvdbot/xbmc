#pragma once
/*
 *      Copyright (C) 2005-2017 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */


#include "cores/DSP/Nodes/Interfaces/IDSPChainNode.h"
#include "cores/DSP/Typedefs/DSPTypedefs.h"

namespace DSP
{
class IDSPNodeCreator
{
public:
  IDSPNodeCreator(uint64_t ID) : ID(ID) {}

  virtual IDSPChainNode* InstantiateNode() = 0;
  virtual DSPErrorCode_t DestroyNode(IDSPChainNode *&Node) = 0;

  const uint64_t ID;
};

template<class T>
class TDSPNodeCreator : public IDSPNodeCreator
{
public:
  TDSPNodeCreator(uint64_t ID) : IDSPNodeCreator(ID) {}
  TDSPNodeCreator(uint64_t ID, std::string Name) : IDSPNodeCreator(ID), m_Name(Name) {}

  virtual IDSPChainNode* InstantiateNode()
  {
    IDSPChainNode *node = nullptr;
    node = dynamic_cast<IDSPChainNode*>(new T(ID, m_Name));

    if (!node)
    {
      return nullptr;
    }

    //! @todo implement initialize interface for reseting state

    return node;
  }

  virtual DSPErrorCode_t DestroyNode(IDSPChainNode *&Node)
  {
    if(!Node)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    DSPErrorCode_t err = Node->Destroy();

    delete Node;
    Node = nullptr;

    return err;
  }

private:
  std::string m_Name;
};

template<class T, class TConstructor>
class TDSPNodeCreatorConstructor : public IDSPNodeCreator
{
public:
  TDSPNodeCreatorConstructor(uint64_t ID, TConstructor Constructor) : IDSPNodeCreator(ID), m_Constructor(Constructor) {}

  virtual IDSPChainNode* InstantiateNode()
  {
    IDSPChainNode *node = nullptr;
    node = dynamic_cast<IDSPChainNode*>(new T(m_Constructor));

    if (!node)
    {
      return nullptr;
    }

    //! @todo implement initialize interface for reseting state

    return node;
  }

  virtual DSPErrorCode_t DestroyNode(IDSPChainNode *&Node)
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

private:
  TConstructor m_Constructor;
};
}
