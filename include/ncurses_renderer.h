#pragma once
#include "renderer.h"
#include "graph.h"

class NcursesRenderer: public Renderer {
    public:
    NcursesRenderer() = default;
    ~NcursesRenderer() override;

    void init() override;
    void shutdown() override;

    void render(const std::vector<AlgorithmFrame>& frames,
                const GridGraph& graph) override;
    InputAction take_input()    override;
    bool should_close()   const override;

    private:
    bool initialized_ = false;
    bool requested_quit_ = false;

    //each cell is 2 chars wide (terminal cells are ~2:1 tall:wide → 2x1 reads square)
    static constexpr int CELL_W      = 2;

    static constexpr int PANEL_W     = GridGraph::WIDTH * CELL_W + 2;  // +2 border
    static constexpr int PANEL_H     = GridGraph::HEIGHT + 1;          // top border only — demov2 trim
    static constexpr int PANEL_COUNT = 4;

    static constexpr int STATS_GAP   = 2;
    static constexpr int STATS_X     = 2 * PANEL_W + STATS_GAP;        // sidebar starts here
    static constexpr int STATS_COLS  = 28;

    //ncurses use short IDs
    enum class Color: short {
        WALL = 1, EMPTY, GRASS, MUD, WATER,
        EXPLORED, PATH, START, GOAL, BEST
    };

    //helper functions
    void init_colors();
    void draw_panel(const AlgorithmFrame& f, const GridGraph& graph, int origin_row, int origin_col);
    void draw_cell(int row, int col, Color c, const char* glyph);                       // attron/mvprintw/attroff
    void draw_cell_bold(int row, int col, Color c, const char* glyph);                  // same, with A_BOLD for S/G/water/path
    void draw_stats(const std::vector<AlgorithmFrame>& frames);                         // sidebar layout, best-value '*' markers
    Color tile_to_color(TileType t) const;
    const char* terrain_glyph(TileType t) const;                                        // tile -> visible char
};