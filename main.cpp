// Main setup for the OpenEngine DragonPanic project.
// -------------------------------------------------------------------
// Copyright (C) 2008 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

// OpenEngine stuff
#include <Meta/Config.h>

// Core structures
#include <Core/Engine.h>

// Display structures
#include <Display/IFrame.h>
#include <Display/FollowCamera.h>
#include <Display/Frustum.h>
#include <Display/InterpolatedViewingVolume.h>
#include <Display/ViewingVolume.h>
// SDL implementation
#include <Display/SDLFrame.h>
#include <Devices/SDLInput.h>

// Rendering structures
#include <Scene/RenderNode.h>
// OpenGL rendering implementation
#include <Renderers/OpenGL/LightRenderer.h>
//#include <Renderers/OpenGL/BufferedRenderer.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/TextureLoader.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/File.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>

// OBJ and TGA plugins
#include <Resources/ITextureResource.h>
#include <Resources/SDLImage.h>
#include <Resources/OBJResource.h>
//#include <Resources/SDLFont.h>
#include <Resources/CairoFont.h>

// Scene structures
#include <Scene/ISceneNode.h>
#include <Scene/SceneNode.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/VertexArrayTransformer.h>
#include <Scene/DisplayListTransformer.h>
#include <Scene/PointLightNode.h>
#include <Scene/RenderStateNode.h>

// Utilities and logger
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
//#include <Utils/EventProfiler.h>
//#include <Utils/FPSSurface.h>

// OERacer utility files
#include <Utils/QuitHandler.h>


// project stuff
//#include <Utils/MouseSelector.h>
#include <Utils/MouseSelection.h>
#include <Utils/SelectionSet.h>
//#include "CursorNode.h"
#include "AxisNode.h"
//#include "CamRayNode.h"
//#include "LineNode.h"
//#include "SphereNode.h"
#include <Utils/SelectionTool.h>
#include <Utils/TransformationTool.h>
#include <Utils/CameraTool.h>
#include <Utils/ToolChain.h>

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Display;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&                engine;
    IFrame*                 frame;
    Viewport*               viewport;
    IViewingVolume*         viewingvolume;
    Camera*                 camera;
    Frustum*                frustum;
    IRenderer*              renderer;
    IMouse*                 mouse;
    IKeyboard*              keyboard;
    ISceneNode*             scene;
    MouseSelection*          ms;
    //    TransformationSelector* ts;
    OpenEngine::Renderers::TextureLoader* textureLoader;

    Config(IEngine& engine)
        : engine(engine)
        , frame(NULL)
        , viewport(NULL)
        , viewingvolume(NULL)
        , camera(NULL)
        , frustum(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)        
        , ms(NULL)
          //        , ts(NULL)
        , textureLoader(NULL)
    {}
};

// Forward declaration of the setup methods
void SetupResources(Config&);
void SetupDevices(Config&);
void SetupDisplay(Config&);
void SetupRendering(Config&);
void SetupScene(Config&);
void SetupDebugging(Config&);

int main(int argc, char** argv) {
    // Setup logging facilities.
    Logger::AddLogger(new StreamLogger(&std::cout));

    // Print usage info.
    logger.info << "========= ";
    logger.info << "Running The OpenEngine Selector Test Project";
    logger.info << " =========" << logger.end;

    logger.info << "This project is a simple testing project for OpenEngine.";
    logger.info << logger.end;

    logger.info << "  for control information see: ";
    logger.info << "KeyboardLayout.txt in the project repository" << logger.end;
    logger.info << logger.end;

    // Create an engine and config object
    Engine* engine = new Engine();
    Config config(*engine);

    // Setup the engine
    SetupResources(config);
    SetupDisplay(config);
    SetupDevices(config);
    SetupRendering(config);
    SetupScene(config);
    
    // Possibly add some debugging stuff
    SetupDebugging(config);

    // Start up the engine.
    engine->Start();

    // release event system
    // post condition: scene and modules are not processed
    delete engine;

    delete config.scene;

    // Return when the engine stops.
    return EXIT_SUCCESS;
}

void SetupResources(Config& config) {
    // set the resources directory
    // @todo we should check that this path exists
    // set the resources directory
    string resources = "projects/Selection/data/";
    DirectoryManager::AppendPath(resources);

    // load resource plug-ins
    ResourceManager<IModelResource>::AddPlugin(new OBJPlugin());
    ResourceManager<ITextureResource>::AddPlugin(new SDLImagePlugin());
    ResourceManager<IFontResource>::AddPlugin(new CairoFontPlugin());
}

void SetupDisplay(Config& config) {
    if (config.frame         != NULL ||
        config.viewingvolume != NULL ||
        config.camera        != NULL ||
        config.frustum       != NULL ||
        config.viewport      != NULL)
        throw Exception("Setup display dependencies are not satisfied.");

    config.frame         = new SDLFrame(800, 600, 32);
    config.viewingvolume = new ViewingVolume();//new InterpolatedViewingVolume(*(new ViewingVolume()));
    config.camera        = new Camera( *config.viewingvolume );
    config.camera->SetPosition(Vector<3,float>(100,100,100));
    config.camera->LookAt(Vector<3,float>(0,0,0));
    //config.frustum       = new Frustum(*config.camera, 20, 3000);
    config.viewport      = new Viewport(0,0,399,299);
    config.viewport->SetViewingVolume(config.camera);

    config.engine.InitializeEvent().Attach(*config.frame);
    config.engine.ProcessEvent().Attach(*config.frame);
    config.engine.DeinitializeEvent().Attach(*config.frame);
}

void SetupDevices(Config& config) {
    if (config.keyboard != NULL ||
        config.mouse    != NULL)
        throw Exception("Setup devices dependencies are not satisfied.");
    // Create the mouse and keyboard input modules
    SDLInput* input = new SDLInput();
    config.keyboard = input;
    config.mouse = input;

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);

    // Bind to the engine for processing time
    config.engine.InitializeEvent().Attach(*input);
    config.engine.ProcessEvent().Attach(*input);
    config.engine.DeinitializeEvent().Attach(*input);
}

void SetupRendering(Config& config) {
    if (config.viewport == NULL ||
        config.renderer != NULL ||
        config.camera == NULL)
        throw Exception("Setup renderer dependencies are not satisfied.");

    // Create a renderer
    config.renderer = new OpenGL::Renderer(config.viewport);
    //config.renderer = new BufferedRenderer(config.viewport);

    // Setup a rendering view
    IRenderingView* rv = new OpenGL::RenderingView(*config.viewport);
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    config.textureLoader = new OpenEngine::Renderers::TextureLoader(*config.renderer);
    config.renderer->PreProcessEvent().Attach(*config.textureLoader);

    //    DisplayListTransformer* dlt = new DisplayListTransformer(rv);
    //    config.renderer->InitializeEvent().Attach(*dlt);

    //    config.renderer->PreProcessEvent()
    //    .Attach( *(new OpenEngine::Renderers::OpenGL::LightRenderer(*config.camera)) );

    config.engine.InitializeEvent().Attach(*config.renderer);
    config.engine.ProcessEvent().Attach(*config.renderer);
    config.engine.DeinitializeEvent().Attach(*config.renderer);

    // mouse selector stuff
    SelectionSet<ISceneNode>* ss = new SelectionSet<ISceneNode>();
    config.ms = new MouseSelection(*config.frame, *config.mouse, NULL);

    TransformationTool* tt = new TransformationTool(*config.textureLoader);
    ss->ChangedEvent().Attach(*tt);
    CameraTool* ct   = new CameraTool();
    ToolChain* tc    = new ToolChain();
    SelectionTool* st = new SelectionTool(*ss);
    tc->PushBackTool(ct);
    tc->PushBackTool(tt);
    tc->PushBackTool(st);

    config.renderer->PostProcessEvent().Attach(*config.ms);
    config.mouse->MouseMovedEvent().Attach(*config.ms);
    config.mouse->MouseButtonEvent().Attach(*config.ms);
    config.keyboard->KeyEvent().Attach(*config.ms);

    //add frustrum cameras
    int width = 800;
    int height = 600;
    float dist = 100;

    // bottom right
    Camera* cam_br = new Camera(*(new ViewingVolume()));
    cam_br->SetPosition(Vector<3,float>(0,0,dist));
    cam_br->LookAt(0,0,0);
    Viewport* vp_br = new Viewport(width/2, 0, width,height/2);
    vp_br->SetViewingVolume(cam_br);
    OpenGL::RenderingView* rv_br = new OpenGL::RenderingView(*vp_br);
    config.renderer->ProcessEvent().Attach(*rv_br);
    // top right
    Camera* cam_tr = new Camera(*(new ViewingVolume()));
    cam_tr->SetPosition(Vector<3,float>(0,dist,0));
    cam_tr->LookAt(0,0,0);
    Viewport* vp_tr = new Viewport(width/2,height/2, width,height);
    vp_tr->SetViewingVolume(cam_tr);
    OpenGL::RenderingView* rv_tr = new OpenGL::RenderingView(*vp_tr);
    config.renderer->ProcessEvent().Attach(*rv_tr);

    // top left
    Camera* cam_tl = new Camera(*(new ViewingVolume()));
    cam_tl->SetPosition(Vector<3,float>(dist,0,0));
    cam_tl->LookAt(0,0,0);
    Viewport* vp_tl = new Viewport(0,height/2, width/2,height);
    vp_tl->SetViewingVolume(cam_tl);
    OpenGL::RenderingView* rv_tl = new OpenGL::RenderingView(*vp_tl);
    config.renderer->ProcessEvent().Attach(*rv_tl);

    config.ms->BindTool(config.viewport, tc);
    config.ms->BindTool(vp_br, tc);
    config.ms->BindTool(vp_tl, tc);
    config.ms->BindTool(vp_tr, tc);

}

void SetupScene(Config& config) {
    if (config.scene  != NULL ||
        config.mouse  == NULL ||
        config.keyboard == NULL)
        throw Exception("Setup scene dependencies are not satisfied.");

    // Create a root scene node
    config.scene = new SceneNode();
    
//     TransformationNode* curTrans = config.ms->GetCursorNode();
//     CursorNode* curNode          = new CursorNode();
//     curTrans->AddNode(curNode);
//     config.scene->AddNode(curTrans);

    // AxisNode* axisNode = new AxisNode();
    // config.scene->AddNode(axisNode);

//     CamRayNode* camRayNode = new CamRayNode(*config.camera, *config.ms->GetCursorNode());
//     config.scene->AddNode(camRayNode);

//     TransformationNode* sphereTrans = new TransformationNode();
//     SphereNode* sphereNode          = new SphereNode();
//     sphereTrans->AddNode(sphereNode);
//     config.scene->AddNode(sphereTrans);
    

    // Load the model
    TransformationNode* t1 = new TransformationNode();
    TransformationNode* t2 = new TransformationNode();
    //t2->Scale(0.5,0.5,0.5);

    IModelResourcePtr mod_res = ResourceManager<IModelResource>::Create("Dragon/DragonHead.obj");
    mod_res->Load();
    t1->AddNode(mod_res->GetSceneNode());
    mod_res->Unload();
    mod_res = ResourceManager<IModelResource>::Create("Dragon/DragonJaw.obj");
    mod_res->Load();
    t2->AddNode(mod_res->GetSceneNode());

    config.scene->AddNode(t1);
    config.scene->AddNode(t2);

    config.ms->SetScene(config.scene);


    config.textureLoader->Load(*config.scene);
    // Transform the scene to use vertex arrays
    //VertexArrayTransformer vaT;
    //vaT.Transform(*config.scene);

    // Supply the scene to the renderer
    config.renderer->SetSceneRoot(config.scene);

    //config.textureLoader->SetDefaultReloadPolicy(OpenEngine::Renderers::TextureLoader::RELOAD_NEVER);
}

void SetupDebugging(Config& config) {
    // main engine events
    // Visualization of the frustum
    if (config.frustum != NULL) {
        config.frustum->VisualizeClipping(true);
        config.scene->AddNode(config.frustum->GetFrustumNode());
    }
}

