#pragma once
#include "pathfinder_base.h"
#include <string>
#include <vector>

//Rendering state
struct AlgorithmFrame {
    std::string label;
    SearchResult result;
    int animation_step = 0;
    int frames_held = 0;
    bool finished = false;
};


//UI
enum class InputAction {
    NONE,       
    RERUN,      //R
    QUIT,       //Q
    FULLSCREEN,  //F - GUI only
    GENERATE,    //G
    CLEAR
};

//Backend abstract class
class Renderer {
    public:
    virtual ~Renderer() = default;

    virtual void init()     = 0;
    virtual void shutdown() = 0;

    virtual void render(const std::vector<AlgorithmFrame>& frames,
                        const GridGraph& graph) = 0;
                        
    virtual InputAction take_input()    = 0;
    virtual bool should_close() const   = 0;
    
    protected:
    Renderer() = default;
};