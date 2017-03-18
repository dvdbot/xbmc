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

#include "DSPChain/Nodes/Interfaces/IADSPChainNode.h"
#include "DSPChain/Factory/Interfaces/IDSPNodeCreator.h"


namespace DSPChain
{
class IDSPNodeFactory
{
public:
  virtual DSPErrorCode_t RegisterCreator(IDSPNodeCreator *Creator) = 0;
  virtual DSPErrorCode_t DeregisterCreator(uint64_t ID) = 0;

  virtual IDSPChainNode* InstantiateNode(uint64_t ID) = 0;
  virtual DSPErrorCode_t DestroyNode(IDSPChainNode *&Node) = 0;
};
}
