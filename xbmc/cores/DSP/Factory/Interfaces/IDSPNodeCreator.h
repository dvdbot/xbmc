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


#include "cores/DSP/Nodes/Interfaces/IDSPNode.h"

namespace DSP
{
class IDSPNodeModel;
}

namespace DSP
{
class IDSPNodeCreator
{
  friend class IDSPNodeModel;

public:
  IDSPNodeCreator() : ID(0) {}

  virtual IDSPNode* InstantiateNode() = 0;
  virtual DSPErrorCode_t DestroyNode(IDSPNode *&Node) = 0;

  const uint64_t GetID() { return ID; }

private:
  uint64_t ID;
};
}
