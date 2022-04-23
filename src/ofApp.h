#pragma once

#include "ofMain.h"
#include "ofxImGui.h"
#include "RandomTheme.h"
#include "cNode.hpp"
#include "cNodeInstances.hpp"
#include "cUI.hpp"
#include "cOSCNode.hpp"
#include "cOSCManager.h"


void FxTestBed(bool*); // Forward declared

// It is easy to extend ImGui making simple functions that use the ImGui API.
// If you need access to inner ImGui variables (eg: more complex tweaks), it's better to write an ImGui plugin and load that. (see imdrawlist_party.cpp)
// Code from ImGuiDemo.cpp
static void HelpMarker(const char* desc){
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

class ofApp : public ofBaseApp{

	public:
		ofApp() : v(0) {}

        void setup() {
            // Enable viewports, enable state saving
            gui.setup(nullptr, false, ImGuiConfigFlags_ViewportsEnable, true);
            //ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

//            inconsolataFont = gui.addFont("../../../data/AvenirNextCondensed-Regular.ttf", 24);
/*            ImGuiIO& io = ImGui::GetIO();
            auto normalCharRanges = io.Fonts->GetGlyphRangesDefault();

            inconsolataFont = gui.addFont("../../../data/Inconsolata-g.otf", 14, nullptr, normalCharRanges );*/
//            show_app_console = true;
 //           ImGui::show_app_log = true;

            gLog = &log;
            ImGuiContext& g = *GImGui;
            g.IO.ConfigWindowsMoveFromTitleBarOnly = true;
            nodeManager = new cNodeManager();
/*            nodeManager->CreateNewContext();
            nodeManager->CreateNewContext();
            nodeManager->CreateNewContext();
            nodeManager->CreateNewContext();
            nodeManager->CreateNewContext();*/
            nodeManager->SetContext(0);
            variableManager = new cVariableManager();
            uiManager = new cUIManager();
            oscManager = new cOSCManager();

            variableManager -> AddVariable("frame", frameNumber);
            uiManager -> CreateNamedUI("min_max");
            
            // The backgroundColor is stored as an ImVec4 type but can handle ofColor
            backgroundColor = ofColor(114, 144, 154);

            show_test_window = true;
            show_another_window = true;
		}
    
        void update()
        {
            nodeManager -> Update();
        }
    
		void draw() {
            // Precalcs
            #define plotSize 200
            static float realFPS[plotSize]={0};
            static float avgFPS=0;
            avgFPS=0;
            
            frameNumber ++;
            
            for(int i=0; i<plotSize-1; ++i){
                avgFPS+=realFPS[i];
                realFPS[i]=realFPS[i+1];
            }
            realFPS[plotSize-1]=ofGetFrameRate();
            avgFPS=(avgFPS+realFPS[plotSize-1])/plotSize;

            //backgroundColor is stored as an ImVec4 type but is converted to ofColor automatically
            ofSetBackgroundColor(backgroundColor);

            // Start drawing to ImGui (newFrame)
			gui.begin();

//            ImGui::PushFont(inconsolataFont);
            
            // Draw a menu bar
            ImGui::BeginMainMenuBar();

            if(ImGui::BeginMenu( "File"))
            {
                if(ImGui::MenuItem( "New Node Editor" ))
                    nodeManager -> CreateNewContext();
                if(ImGui::MenuItem( "Save Nodes" ))
                    nodeManager -> Save();
                if(ImGui::MenuItem( "Load Nodes" ))
                    nodeManager -> Load();
                if(ImGui::MenuItem( "Save UI" ))
                    uiManager -> Save();
                if(ImGui::MenuItem( "Load UI" ))
                    uiManager -> Load();
                if(ImGui::MenuItem("Show Log", nullptr, showLog))
                    showLog = !showLog;
                if(ImGui::MenuItem("Debug Mode", nullptr, debug))
                    debug = !debug;
                ImGui::EndMenu();
            }
            
            if(ImGui::BeginMenu( "Options"))
            {
                ImGui::Checkbox("Show ImGui Metrics", &showMetrics);
                ImGui::SameLine(); HelpMarker("The ImGui metrics is a gui window allowing you to inspect what is going on and how imgui is intagrated and interfaced with oF.");
                ImGui::Checkbox("Show ImGui Demo", &showDemo);
                ImGui::SameLine(); HelpMarker("The ImGui Demo Window demonstrates many gui possibilities and serves as a reference for available gui widgets, together with its source code.");

                // Exit
                ImGui::Separator();
                if(ImGui::MenuItem( "Quit" )){
                    ofExit();
                }

                ImGui::EndMenu();
            }


            ImGui::EndMainMenuBar();
            
            nodeManager -> DrawAll(windowToBeFocussed);
            windowToBeFocussed = nullptr;

            if(showSampleWindow)
            {
                if(ImGui::Begin("Sample Window", &showSampleWindow)){
                    if (ImGui::Button("Another Window"))
                    {
                        //bitwise OR
                        show_another_window ^= 1;
                    }

                    ImGui::Dummy(ImVec2(10,10));
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

                    ImGui::ShowStyleEditor();
                    ImGui::ShowFontSelector("label4FontSelector");

                    // Texture loading
                    ImGui::Dummy(ImVec2(10,10));
                    if(!fileNames.empty()){
                        //ofxImGui::VectorListBox allows for the use of a vector<string> as a data source
                        static int currentListBoxIndex = 0;
                        if(ofxImGui::VectorListBox("VectorListBox", &currentListBoxIndex, fileNames)){
                            ofLog() << " VectorListBox FILE PATH: "  << files[currentListBoxIndex].getAbsolutePath();
                            imageButtonID = fileTextures[currentListBoxIndex];
                        }

                        //ofxImGui::VectorCombo allows for the use of a vector<string> as a data source
                        static int currentFileIndex = 0;
                        if(ofxImGui::VectorCombo("VectorCombo", &currentFileIndex, fileNames)){
                            ofLog() << "VectorCombo FILE PATH: "  << files[currentFileIndex].getAbsolutePath();
                            pixelsButtonID = fileTextures[currentFileIndex];
                        }
                    }

                    //GetImTextureID is a static function define in Helpers.h that accepts ofTexture, ofImage, or GLuint
                    ImGui::Dummy(ImVec2(10,10));
                    if(ImGui::ImageButton(GetImTextureID(imageButtonID), ImVec2(200, 200))){
                           ofLog() << "PRESSED";
                    }
                    //or do it manually
                    ImGui::SameLine();
                    ImGui::Image(GetImTextureID(pixelsButtonID), ImVec2(200, 200));

                    ImGui::Image((ImTextureID)(uintptr_t)textureSourceID, ImVec2(200, 200));
                }
                ImGui::End();
            }

            if (show_another_window)
            {
                //note: ofVec2f and ImVec2f are interchangeable
                ImGui::SetNextWindowSize(ofVec2f(200,100), ImGuiCond_FirstUseEver);
                ImGui::Begin("Another Window", &show_another_window);
                ImGuiWindow *w = ImGui::GetCurrentWindow();
                uiManager -> Draw();
                ImGui::End();
            }
            if(showMetrics){
                ImGui::SetNextWindowPos(ImVec2(ofGetWindowPositionX()+ofGetWidth()-350,ofGetWindowPositionY()+30), ImGuiCond_FirstUseEver );
                ImGui::ShowMetricsWindow( &showMetrics );
            }

            if(showDemo) ImGui::ShowDemoWindow(&showDemo);

            if(showLog)
                log.Draw("Log", &showLog);

//            ImGui::PopFont();
//            ImDrawData* drawData = ImGui::GetDrawData();

            gui.end();
            
            //must set context depending on active window
            nodeManager -> HandleKeys();
            
            ImGui::GetIO().DisplayFramebufferScale = ImVec2(1.0, 1.0);

            gui.draw();

            ImGuiContext& g = *GImGui;
            int maxStamp = 0;
            int maxStamped = 0;
            for(int i = 0; i < g.Viewports.size(); i ++)
            {
                if(g.Viewports[i] -> LastFrontMostStampCount > maxStamp)
                {
                    maxStamp = g.Viewports[i] -> LastFrontMostStampCount;
                    maxStamped = i;
                }
            }
            ImGuiViewportP *frontPort = g.Viewports[maxStamped];
            ImGuiWindow *frontWindow = frontPort -> Window;
            if(frontWindow != nullptr && g.WindowsFocusOrder.back() != frontWindow)
            {
                if(ImGui::IsAnyMouseDown() == false)
                    windowToBeFocussed = frontWindow;
            }

        }
    



        //--------------------------------------------------------------
        void keyPressed(int key){

            ofLogVerbose(__FUNCTION__) << key;
            switch (key) {

                case 's':
                {
                    break;
                }
            }
        }

        //--------------------------------------------------------------
        void keyReleased(int key){
            ofLogVerbose(__FUNCTION__) << key;
        }

        void mouseScrolled(int x, int y, float scrollX, float scrollY){
            ofLogVerbose(__FUNCTION__) << "x: " << x << " y: " << y;
        }
        //--------------------------------------------------------------
        void mouseMoved(int, int){}

        //--------------------------------------------------------------
        void mouseDragged(int , int , int){}

        //--------------------------------------------------------------
        void mousePressed(int, int, int){}

        //--------------------------------------------------------------
        void mouseReleased(int, int, int){}

        //--------------------------------------------------------------
        void windowResized(int, int){}

        //--------------------------------------------------------------
        void gotMessage(ofMessage){}

        //--------------------------------------------------------------
        void dragEvent(ofDragInfo){}

	private:
        ofxImGui::Gui gui;

        // Variables exposed to ImGui
        bool showMetrics      = false;
        bool showDemo         = false;
        bool showFX           = false;
        bool showSampleWindow = false;
        bool showLog = false;

        float v = 0;

        int   linesPerSide  = 15;
        float linesSpacing  = 0.02f;
        float lineThickness = 0.8f;
        bool  drawLines     = false;

        ImVec4 backgroundColor;

        bool   show_test_window;
        bool   show_another_window;

        ofImage   imageButtonSource;
        GLuint    imageButtonID;
        ofPixels  pixelsButtonSource;
        GLuint    pixelsButtonID;
        ofTexture textureSource;
        GLuint    textureSourceID;
    
        std::vector<std::string> fileNames;
        std::vector<ofFile> files;
        std::vector<GLuint> fileTextures;

//        std::vector<std::pair<int, int>> links;
//        ImNodesContext* nodeContext = nullptr;
        cNodeManager *nodeManager = nullptr;
        cUIManager *uiManager = nullptr;
        cVariableManager *variableManager = nullptr;
        ImFont* inconsolataFont = nullptr;
        cOSCManager *oscManager = nullptr;
        int frameNumber = 0;
        cLog log;
        ofMatrix4x4 view;
        ofMatrix4x4 projection;
        ofMatrix4x4 gizmoMatrix;
        ImGuiWindow *windowToBeFocussed = nullptr;
};

