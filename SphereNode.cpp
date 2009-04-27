// Temp draw sphere
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "SphereNode.h"

#include <Math/Vector.h>
#include <Renderers/IRenderer.h>

using OpenEngine::Math::Vector;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Geometry::Line;

void SphereNode::Apply(IRenderingView* rv) {
    IRenderer* r = rv->GetRenderer();

    // draw sphere
    Vector<3,float> col(0.0,0.0,1.0); // blue
    r->DrawSphere(Vector<3,float>(0,0,0), 2.5, col);


}
