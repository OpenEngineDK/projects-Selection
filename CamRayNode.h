// Temp draw camera and ray
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _CAM_RAY_NODE_
#define _CAM_RAY_NODE_

#include <Scene/RenderNode.h>
#include <Renderers/IRenderingView.h>
#include <Display/Camera.h>
#include <Scene/TransformationNode.h>

using OpenEngine::Scene::RenderNode;
using OpenEngine::Renderers::IRenderingView;
using OpenEngine::Display::Camera;
using OpenEngine::Scene::TransformationNode;

class CamRayNode : public RenderNode 
{
private:
    Camera& cam;
    TransformationNode& cursor;
public:
    CamRayNode(Camera& cam, TransformationNode& cursor);
    void Apply(IRenderingView* r);
};

#endif 
