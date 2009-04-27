// Temp draw camera and ray
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "LineNode.h"

#include <Math/Vector.h>
#include <Renderers/IRenderer.h>

using OpenEngine::Math::Vector;
using OpenEngine::Renderers::IRenderer;

LineNode::LineNode():
    line(Line(Vector<3,float>(0,0,0),Vector<3,float>(0,0,0))) {}

LineNode::LineNode(Line line):
    line(line) {}

void LineNode::Apply(IRenderingView* rv) {
    IRenderer* r = rv->GetRenderer();

    // draw line
    Vector<3,float> col(1.0,1.0,0.0); //yellow
    float size = 2;
    r->DrawLine(line, col, size);
}

void LineNode::SetLine(Line line) {
    this->line = line;
}
