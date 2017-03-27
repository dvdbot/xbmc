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

#include <list>

#include "cores/DSP/Factory/Interfaces/IDSPNodeCreator.h"
#include "cores/DSP/Factory/Interfaces/IDSPNodeFactory.h"


namespace DSP
{
class CDSPNodeFactory : public IDSPNodeFactory
{
  typedef std::list<IDSPNodeCreator*> DSPNodeCreators_t;
public:
  virtual ~CDSPNodeFactory()
  {
    for (std::list<IDSPNodeCreator*>::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
    {
      if (*iter)
      {
        delete *iter;
      }
    }

    m_DSPNodeCreators.clear();
  }

  virtual DSPErrorCode_t RegisterCreator(IDSPNodeCreator *Creator)
  {
    if (!Creator)
    {
      return DSP_ERR_INVALID_INPUT;
    }

    DSPNodeCreators_t::iterator iter = getCreator(Creator);
    if (iter == m_DSPNodeCreators.end())
    {
      m_DSPNodeCreators.push_back(Creator);
      return DSP_ERR_NO_ERR;
    }

    return DSP_ERR_ALREADY_REGISTERED;
  }

  virtual DSPErrorCode_t DeregisterCreator(uint64_t ID)
  {
    DSPNodeCreators_t::iterator iter = getCreator(ID);
    if (iter == m_DSPNodeCreators.end())
    {
      return DSP_ERR_CREATOR_ID_NOT_FOUND;
    }
    
    m_DSPNodeCreators.erase(iter);

    return DSP_ERR_NO_ERR;
  }

  virtual IDSPNode* InstantiateNode(uint64_t ID)
  {
    DSPNodeCreators_t::iterator iter = getCreator(ID);
    if (iter == m_DSPNodeCreators.end())
    {
      return nullptr;
    }

    return (*iter)->InstantiateNode();
  }

  virtual DSPErrorCode_t DestroyNode(IDSPNode *&Node)
  {
    DSPNodeCreators_t::iterator iter = getCreator(Node->ID);
    if (iter == m_DSPNodeCreators.end())
    {
      return DSP_ERR_CREATOR_ID_NOT_FOUND;
    }

    return (*iter)->DestroyNode(Node);
  }

private:
  DSPNodeCreators_t::iterator getCreator(uint64_t ID)
  {
    for (DSPNodeCreators_t::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
    {
      if ((*iter)->GetID() == ID)
      {
        return iter;
      }
    }

    return m_DSPNodeCreators.end();
  }

  DSPNodeCreators_t::iterator getCreator(IDSPNodeCreator *Creator)
  {
    for (DSPNodeCreators_t::iterator iter = m_DSPNodeCreators.begin(); iter != m_DSPNodeCreators.end(); ++iter)
    {
      if (*iter == Creator)
      {
        return iter;
      }
    }

    return m_DSPNodeCreators.end();
  }

  std::vector<IDSPNodeCreator*> m_DSPNodeCreators;
};
}
