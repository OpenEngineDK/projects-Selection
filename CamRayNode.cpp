// Temp draw camera and ray
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#include "CamRayNode.h"

#include <Math/Vector.h>
#include <Renderers/IRenderer.h>
#include <Geometry/Line.h>

using OpenEngine::Math::Vector;
using OpenEngine::Renderers::IRenderer;
using OpenEngine::Geometry::Line;

CamRayNode::CamRayNode(Camera& cam, TransformationNode& cursor):
    cam(cam)
    , cursor(cursor) {}

void CamRayNode::Apply(IRenderingView* rv) {
    IRenderer* r = rv->GetRenderer();

    // draw camera
    Vector<3,float> col(0.0,0.0,1.0); // blue
    float size = 4;
    r->DrawPoint(cam.GetPosition(), col, size);

    // draw ray
    col = Vector<3,float>(1.0,1.0,0.0); //yellow
    size = 2;
    r->DrawLine(Line(cam.GetPosition(),
                     cursor.GetPosition()),
                col, size);

    // draw sphere position
    col = Vector<3,float>(0.0,0.0,0.0); //yellow
    size = 5;
    r->DrawPoint(Vector<3,float>(0.0,0.0,0.0),col,size);

}
