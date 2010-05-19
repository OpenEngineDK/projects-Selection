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
// #include <Display/SDLFrame.h>
// #include <Devices/SDLInput.h>

// Rendering structures
#include <Scene/RenderNode.h>
// OpenGL rendering implementation
#include <Renderers/OpenGL/LightRenderer.h>
#include <Renderers/OpenGL/Renderer.h>
#include <Renderers/OpenGL/RenderingView.h>
#include <Renderers/TextureLoader.h>

// Resources
#include <Resources/IModelResource.h>
#include <Resources/File.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ResourceManager.h>

// OBJ and TGA plugins
#include <Resources/ITexture2D.h>
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
#include <Utils/SelectionTool.h>
//#include <Utils/TransformationTool.h>
#include <Utils/CameraTool.h>
#include <Utils/ToolChain.h>

#include <Display/IRenderCanvas.h>
#include <Display/OpenGL/RenderCanvas.h>
#include <Display/OpenGL/ColorStereoCanvas.h>
#include <Display/OpenGL/SplitScreenCanvas.h>
#include <Display/IEnvironment.h>
#include <Display/SDLEnvironment.h>
//#include <Display/GLUTEnvironment.h>
//#include <Core/GLUTEngine.h>

// Additional namespaces
using namespace OpenEngine::Core;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Devices;
using namespace OpenEngine::Display;
using namespace OpenEngine::Display::OpenGL;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Utils;

// Configuration structure to pass around to the setup methods
struct Config {
    IEngine&                engine;
    IEnvironment*           env;
    IFrame&                 frame;
    ICanvas*                canvas;
    IRenderer*              renderer;
    IMouse*                 mouse;
    IKeyboard*              keyboard;
    ISceneNode*             scene;
    MouseSelection*         ms;
    //    TransformationSelector* ts;
    OpenEngine::Renderers::TextureLoader* textureLoader;

    Config(IEngine& engine, IEnvironment* env, IFrame& frame)
        : engine(engine)
        , env(env)
        , frame(frame)
        , canvas(NULL)
        , renderer(NULL)
        , mouse(NULL)
        , keyboard(NULL)
        , scene(NULL)        
        // , ms(NULL)
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
    IEngine* engine = new Engine();
    IEnvironment* env = new SDLEnvironment(800, 600, 16);//, FRAME_FULLSCREEN);
    Config config(*engine, env, env->CreateFrame());

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
    if (config.canvas        != NULL)
        throw Exception("Setup display dependencies are not satisfied.");
    
    config.engine.InitializeEvent().Attach(*config.env);
    config.engine.ProcessEvent().Attach(*config.env);
    config.engine.DeinitializeEvent().Attach(*config.env);
}

void SetupDevices(Config& config) {
    if (config.keyboard != NULL ||
        config.mouse    != NULL)
        throw Exception("Setup devices dependencies are not satisfied.");
    // Create the mouse and keyboard input modules
    config.keyboard = config.env->GetKeyboard();
    config.mouse = config.env->GetMouse();

    // Bind the quit handler
    QuitHandler* quit_h = new QuitHandler(config.engine);
    config.keyboard->KeyEvent().Attach(*quit_h);
}

void SetupRendering(Config& config) {
    if (config.renderer != NULL)
        throw Exception("Setup renderer dependencies are not satisfied.");
    
    // Create a root scene node
    RenderStateNode* rsn = new RenderStateNode();
    config.scene = rsn;
    rsn->EnableOption(RenderStateNode::LIGHTING);
    
    // Create a renderer
    config.renderer = new Renderer();
    LightRenderer* lr = new LightRenderer();
    config.renderer->PreProcessEvent().Attach(*lr);

    // Setup a rendering view
    IRenderingView* rv = new RenderingView();
    config.renderer->ProcessEvent().Attach(*rv);

    // Add rendering initialization tasks
    config.textureLoader = new OpenEngine::Renderers::TextureLoader(*config.renderer);
    config.textureLoader->SetDefaultReloadPolicy(OpenEngine::Renderers::TextureLoader::RELOAD_NEVER);

    // mouse selector stuff
    SelectionSet<ISceneNode>* ss = new SelectionSet<ISceneNode>();
    config.ms = new MouseSelection(/*config.frame,*/ *config.mouse, *config.keyboard);
    config.renderer->PostProcessEvent().Attach(*config.ms);
    // TransformationTool* tt = new TransformationTool(*config.textureLoader);
    // ss->ChangedEvent().Attach(*tt);
    CameraTool* ct   = new CameraTool();
    ToolChain* tc    = new ToolChain();
    SelectionTool* st = new SelectionTool(*ss);
    tc->PushBackTool(ct);
    // tc->PushBackTool(tt);
    tc->PushBackTool(st);

    config.renderer->PostProcessEvent().Attach(*st);

    float dist = 100;
    IRenderCanvas* c1 = new ColorStereoCanvas();
    Camera* cam = new Camera( *(new ViewingVolume()) );
    cam->SetPosition(Vector<3,float>(100,100,100));
    cam->LookAt(Vector<3,float>(0,0,0));
    c1->SetViewingVolume(cam);
    c1->SetRenderer(config.renderer);
    c1->SetScene(config.scene);

    IRenderCanvas* c2 = new RenderCanvas();
    cam = new Camera( *new ViewingVolume() );
    cam->SetPosition(Vector<3,float>(0,0,dist));
    cam->LookAt(0,0,0);
    c2->SetViewingVolume(cam);
    c2->SetRenderer(config.renderer);
    c2->SetScene(config.scene);

    IRenderCanvas* c3 = new RenderCanvas();
    cam = new Camera( *new ViewingVolume() );
    cam->SetPosition(Vector<3,float>(0,dist,0));
    cam->LookAt(0,0,0);
    c3->SetViewingVolume(cam);
    c3->SetRenderer(config.renderer);
    c3->SetScene(config.scene);

    IRenderCanvas* c4 = new ColorStereoCanvas();
    cam = new Camera( *new ViewingVolume() );
    cam->SetPosition(Vector<3,float>(dist,0,0));
    cam->LookAt(0,0,0);
    c4->SetViewingVolume(cam);
    c4->SetRenderer(config.renderer);
    c4->SetScene(config.scene);

    ICanvas* left  = new SplitScreenCanvas(*c1, *c2, SplitScreenCanvas::HORIZONTAL);
    ICanvas* right = new SplitScreenCanvas(*c3, *c4, SplitScreenCanvas::HORIZONTAL);
    config.canvas  = new SplitScreenCanvas(*left, *right);
   
    //config.frame.SetCanvas(config.canvas);
    //config.frame.SetCanvas(left);
    //config.frame.SetCanvas(right);
    config.frame.SetCanvas(c1);
    
    config.ms->BindTool(c1, tc);
    config.ms->BindTool(c2, tc);
    config.ms->BindTool(c3, tc);
    config.ms->BindTool(c4, tc);
    // config.ms->BindTool(vp_br, tc);
    // config.ms->BindTool(vp_tl, tc);
    // config.ms->BindTool(vp_tr, tc);

}

void SetupScene(Config& config) {
    if (config.scene  == NULL ||
        config.mouse  == NULL ||
        config.keyboard == NULL)
        throw Exception("Setup scene dependencies are not satisfied.");

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

    PointLightNode* light = new PointLightNode();
    TransformationNode* lightTrans = new TransformationNode();
    lightTrans->Move(0,1000,0);
    lightTrans->AddNode(light);
    config.scene->AddNode(lightTrans);
        
    // config.ms->SetScene(config.scene);
    // config.renderer->InitializeEvent().Attach(*config.textureLoader);
    config.textureLoader->Load(*config.scene);
}

void SetupDebugging(Config& config) {
    // main engine events
}

