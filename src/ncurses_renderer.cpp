#include "ncurses_renderer.h"
#include "graph.h"
 
#include <ncurses.h>
#include <algorithm>
#include <climits>      // INT_MAX
#include <limits>       // std::numeric_limits
#include <csignal>      // std::signal
#include <cstdlib>      // std::_Exit
#include <cstdio>       // std::fprintf

namespace {
// [HD-MODERN] SIGINT handler — Ctrl-C without endwin() leaves the terminal
// in raw mode and the user has to type `reset` blindly to recover. Demov2-style
// guard: restore the terminal, then _Exit (signal-safe — skips destructors that
// could deadlock during signal delivery).
void handle_sigint(int) {
    endwin();
    std::_Exit(0);
}
}  // namespace

NcursesRenderer::~NcursesRenderer() {
    shutdown();
}
 
void NcursesRenderer::init() {
    if (initialized_) return;

    initscr();

    // [HD-MODERN] Color support guard — fail fast on monochrome terminals
    // instead of silently rendering everything in the default attribute.
    if (!has_colors()) {
        endwin();
        std::fprintf(stderr, "Terminal does not support colors.\n");
        std::_Exit(1);
    }

    cbreak();               // disable line buffering
    noecho();               // typed chars don't echo
    curs_set(0);            // hide blinking cursor
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);

    start_color();
    use_default_colors();   // lets us pass -1 as a "use terminal default" bg
    init_colors();

    // [HD-MODERN] Terminal-size guard — needs at least the full 2x2 panel grid
    // plus the stats sidebar plus the status bar. Anything smaller would clip
    // panels and look broken; better to refuse with a clear error.
    const int needed_cols = STATS_X + STATS_COLS;
    const int needed_rows = 2 * PANEL_H + 1;        // +1 for status bar row
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    if (rows < needed_rows || cols < needed_cols) {
        endwin();
        std::fprintf(stderr,
            "Terminal too small for 2x2 layout.\n"
            "  Current:  %d cols x %d rows\n"
            "  Required: %d cols x %d rows (or larger)\n"
            "Maximise the window or shrink the terminal font.\n",
            cols, rows, needed_cols, needed_rows);
        std::_Exit(1);
    }

    std::signal(SIGINT, handle_sigint);

    initialized_ = true;
}
 
void NcursesRenderer::shutdown() {
    if (!initialized_) return;
 
    endwin();
    initialized_ = false;
}
 
void NcursesRenderer::init_colors() {
    // start_color() and use_default_colors() already called in init().
    // bg = -1 means terminal's default background
    // The only solid block is WALL (white-on-white)
    init_pair(static_cast<short>(Color::WALL),     COLOR_WHITE,  COLOR_WHITE);
    init_pair(static_cast<short>(Color::EMPTY),    -1,           -1);
    init_pair(static_cast<short>(Color::GRASS),    COLOR_GREEN,  -1);
    init_pair(static_cast<short>(Color::MUD),      COLOR_YELLOW, -1);
    init_pair(static_cast<short>(Color::WATER),    COLOR_BLUE,   -1);
    init_pair(static_cast<short>(Color::EXPLORED), COLOR_CYAN,   -1);
    init_pair(static_cast<short>(Color::PATH),     COLOR_RED,    -1);
    init_pair(static_cast<short>(Color::START),    COLOR_GREEN,  -1);
    init_pair(static_cast<short>(Color::GOAL),     COLOR_RED,    -1);
    init_pair(static_cast<short>(Color::BEST),     COLOR_YELLOW, -1);  // best-value '*' markers
}
 
void NcursesRenderer::render(const std::vector<AlgorithmFrame>& frames,
                             const GridGraph& graph) {
    erase();
    
    const int n = std::min(static_cast<int>(frames.size()), PANEL_COUNT);
    for (int i = 0; i < n; i++) {
        const int origin_row = (i / 2) * PANEL_H;
        const int origin_col = (i % 2) * PANEL_W;
        draw_panel(frames[i], graph, origin_row, origin_col);
    }
 
    // sidebar stats + status bar — once per frame, not once per panel
    draw_stats(frames);
    mvprintw(LINES - 1, STATS_X, "R:Run  G:Maze  C:Clear  Q:Quit");
 
    refresh();
}
 
void NcursesRenderer::draw_panel(const AlgorithmFrame& f,
                                 const GridGraph& graph,
                                 int origin_row,
                                 int origin_col) {
    // Top horizontal rule with the label overlaid on it.
    mvhline(origin_row, origin_col, ACS_HLINE, PANEL_W);
    attron(A_BOLD);
    mvprintw(origin_row, origin_col + 2, " %s ", f.label.c_str());
    attroff(A_BOLD);
 
    const int grid_top  = origin_row + 1;
    const int grid_left = origin_col + 1;
 
    //layer 1 - terrain
    for (int y = 0; y < GridGraph::HEIGHT; y++) {
        for (int x = 0; x < GridGraph::WIDTH; x++) {
            const Node* node = graph.get_node(x, y);
            if (!node || node->type == TileType::EMPTY) continue;       // defensive null guard
            const int r = grid_top + y;
            const int c = grid_left + x * CELL_W;
            // WATER is bold (deepest blue) — differentiates from MUD which shares the '~' glyph.
            if (node->type == TileType::WATER) {
                draw_cell_bold(r, c, tile_to_color(node->type), terrain_glyph(node->type));
            } else {
                draw_cell(r, c, tile_to_color(node->type), terrain_glyph(node->type));
            }
        }
    }
 
    //layer 2 - explored trail
    const int steps = std::min(f.animation_step, static_cast<int>(f.result.explored_order.size()));
    for (int i = 0; i < steps; i++) {
        const Node* node = f.result.explored_order[i];
        if (!node) continue;                                            // defensive null guard
        draw_cell(grid_top + node->row,
                  grid_left + node->col * CELL_W,
                  Color::EXPLORED,
                  ". ");
    }

    //layer 3 - final path
    if (f.finished) {
        for (const Node* node: f.result.path) {
            if (!node) continue;                                        // defensive null guard
            draw_cell_bold(grid_top + node->row,
                           grid_left + node->col * CELL_W,
                           Color::PATH,
                           "**");
        }
    }
 
    //layer 4 - start + goal markers 
    const Node* s = graph.start();
    const Node* g = graph.goal();
    if (s) draw_cell_bold(grid_top + s->row, grid_left + s->col * CELL_W, Color::START, "S ");
    if (g) draw_cell_bold(grid_top + g->row, grid_left + g->col * CELL_W, Color::GOAL,  "G ");
}
 
void NcursesRenderer::draw_cell(int row, int col, Color c, const char* glyph) {
    const short pair_id = static_cast<short>(c);
    attron(COLOR_PAIR(pair_id));
    mvprintw(row, col, "%s", glyph);
    attroff(COLOR_PAIR(pair_id));
}
 
void NcursesRenderer::draw_cell_bold(int row, int col, Color c, const char* glyph) {
    const short pair_id = static_cast<short>(c);
    attron(COLOR_PAIR(pair_id) | A_BOLD);
    mvprintw(row, col, "%s", glyph);
    attroff(COLOR_PAIR(pair_id) | A_BOLD);
}
 
void NcursesRenderer::draw_stats(const std::vector<AlgorithmFrame>& frames) {
    int    best_visited = INT_MAX;
    int    best_hops    = INT_MAX;
    int    best_cost    = INT_MAX;
    double best_ms      = std::numeric_limits<double>::max();
    for (const auto& f : frames) {
        if (f.result.path.empty()) continue;
        const int hops = static_cast<int>(f.result.path.size()) - 1;
        best_visited = std::min(best_visited, f.result.explored_size);
        best_hops    = std::min(best_hops,    hops);
        best_cost    = std::min(best_cost,    f.result.cost);
        best_ms      = std::min(best_ms,      f.result.elapsed.count());
    }
 
    
    const auto star = [&](int row, int col_offset, bool is_best) {
        if (!is_best) return;
        const short pid = static_cast<short>(Color::BEST);
        attron(COLOR_PAIR(pid) | A_BOLD);
        mvaddch(row, STATS_X + col_offset, '*');
        attroff(COLOR_PAIR(pid) | A_BOLD);
    };
 
    int y = 0;
    attron(A_BOLD);
    mvprintw(y++, STATS_X, "STATS");
    attroff(A_BOLD);
    ++y;
 
    for (const auto& f : frames) {
        attron(A_BOLD);
        mvprintw(y++, STATS_X, "%s", f.label.c_str());
        attroff(A_BOLD);
 
        if (f.result.path.empty()) {
            mvprintw(y++, STATS_X, "  (no path found)");
            ++y;
            continue;
        }
 
        const int hops = static_cast<int>(f.result.path.size()) - 1;
 
        mvprintw(y, STATS_X, "  visited: %5d", f.result.explored_size);
        star(y, 20, f.result.explored_size == best_visited);
        ++y;
 
        mvprintw(y, STATS_X, "  hops:    %5d", hops);
        star(y, 20, hops == best_hops);
        ++y;
 
        mvprintw(y, STATS_X, "  cost:    %5d", f.result.cost);
        star(y, 20, f.result.cost == best_cost);
        ++y;
 
        mvprintw(y, STATS_X, "  ms:      %5.3f", f.result.elapsed.count());
        star(y, 20, f.result.elapsed.count() == best_ms);
        ++y;
 
        ++y;  
    }
}
 
NcursesRenderer::Color NcursesRenderer::tile_to_color(TileType t) const {
    switch (t) {
        case TileType::WALL:  return Color::WALL;
        case TileType::EMPTY: return Color::EMPTY;
        case TileType::GRASS: return Color::GRASS;
        case TileType::MUD:   return Color::MUD;
        case TileType::WATER: return Color::WATER;
    }
    return Color::EMPTY;                 // unreachable with enum class
}
 
const char* NcursesRenderer::terrain_glyph(TileType t) const {
  
    switch (t) {
        case TileType::WALL:  return "  ";  
        case TileType::GRASS: return "..";  
        case TileType::MUD:   return "~~";   
        case TileType::WATER: return "~~";   
        case TileType::EMPTY: return "  ";  
    }
    return "  ";
}
 
InputAction NcursesRenderer::take_input() {
    const int ch = getch();              
    switch (ch) {
        case 'q': case 'Q': case 27:     
            requested_quit_ = true;
            return InputAction::QUIT;
        case 'r': case 'R':
            return InputAction::RERUN;
        case 'f': case 'F':
            return InputAction::FULLSCREEN;   // ncurses ignores; raylib will honor
        default:
            return InputAction::NONE;         
    }
}
 
bool NcursesRenderer::should_close() const {
    return requested_quit_;
}