// Temp draw cursor
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _CURSOR_NODE_
#define _CURSOR_NODE_

#include <Scene/RenderNode.h>
#include <Renderers/IRenderingView.h>

using OpenEngine::Scene::RenderNode;
using OpenEngine::Renderers::IRenderingView;

class CursorNode : public RenderNode 
{
private:
public:
    void Apply(IRenderingView* r);
};

#endif 
