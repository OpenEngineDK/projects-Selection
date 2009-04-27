// Temp draw cursor
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "AxisNode.h"

#include <Math/Vector.h>
#include <Renderers/IRenderer.h>
#include <Geometry/Line.h>

using OpenEngine::Math::Vector;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Geometry::Line;

void AxisNode::Apply(IRenderingView* rv) {
    Vector<3,float> col(1.0,0.0,0.0); // red
    float size = 3;

    IRenderer* r = rv->GetRenderer();
    r->DrawLine(Line(Vector<3,float>(0.0,0.0,0.0),
                     Vector<3,float>(10.0,0.0,0.0)), 
                col, size);
    
    col = Vector<3,float>(0.0,1.0,0.0); //green
    r->DrawLine(Line(Vector<3,float>(0.0,0.0,0.0),
                     Vector<3,float>(0.0,10.0,0.0)), 
                col, size);

    col = Vector<3,float>(0.0,0.0,1.0); //blue
    r->DrawLine(Line(Vector<3,float>(0.0,0.0,0.0),
                     Vector<3,float>(0.0,0.0,10.0)), 
                col, size);

}
