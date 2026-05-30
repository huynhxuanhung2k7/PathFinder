#pragma once
#include "renderer.h"
#include "graph.h"
#include <raylib.h>
#include <vector>

class RaylibRenderer : public Renderer {
public:
    RaylibRenderer() = default;
    ~RaylibRenderer() override;

    void init() override;
    void shutdown() override;
    void render(const std::vector<AlgorithmFrame>& frames,
                const GridGraph& graph) override;
    InputAction take_input() override;
    bool should_close() const override;

private:
    bool initialized_    = false;
    bool requested_quit_ = false;

    // Layout
    static constexpr int TILE         = 11;
    static constexpr int LABEL_H      = 24;
    static constexpr int MARGIN       = 16;
    static constexpr int PANEL_GAP    = 12;
    static constexpr int PANEL_GRID_W = GridGraph::WIDTH  * TILE;
    static constexpr int PANEL_GRID_H = GridGraph::HEIGHT * TILE;
    static constexpr int PANEL_W      = PANEL_GRID_W;
    static constexpr int PANEL_H      = PANEL_GRID_H + LABEL_H;
    static constexpr int WIN_W        = 1280;
    static constexpr int WIN_H        = 800;
    static constexpr int PANEL_COUNT  = 4;

    void draw_panel(const AlgorithmFrame& f, const GridGraph& graph,
                    int origin_x, int origin_y);
    void draw_stats(const std::vector<AlgorithmFrame>& frames);
};
