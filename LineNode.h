// Temp draw line
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _LINE_NODE_
#define _LINE_NODE_

#include <Scene/RenderNode.h>
#include <Renderers/IRenderingView.h>
#include <Geometry/Line.h>

using OpenEngine::Scene::RenderNode;
using OpenEngine::Renderers::IRenderingView;
using OpenEngine::Geometry::Line;

class LineNode : public RenderNode 
{
private:
    Line line;
public:
    LineNode(Line line);
    LineNode();
    void Apply(IRenderingView* r);
    void SetLine(Line line);
};

#endif 
