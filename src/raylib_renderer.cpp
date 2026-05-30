#include "raylib_renderer.h"
#include <algorithm>
#include <climits>
#include <cstddef>
#include <limits>

namespace {
    // Chrome
    constexpr Color C_BG       = {244, 238, 224, 255};
    constexpr Color C_BORDER   = { 96,  84,  68, 255};
    constexpr Color C_TEXT     = { 56,  48,  40, 255};
    constexpr Color C_LABEL    = { 84,  68,  52, 255};
    constexpr Color C_BEST     = {196, 110,  44, 255};
    // Tiles
    constexpr Color C_EMPTY    = {252, 248, 238, 255};
    constexpr Color C_GRASS    = {164, 184, 124, 255};
    constexpr Color C_MUD      = {186, 144,  98, 255};
    constexpr Color C_WATER    = {120, 156, 178, 255};
    constexpr Color C_WALL     = { 64,  56,  48, 255};
    constexpr Color C_START    = { 96, 148,  92, 255};
    constexpr Color C_GOAL     = {196,  72,  56, 255};
    // Overlays
    constexpr Color C_EXPLORED = { 72, 124, 156, 180};
    constexpr Color C_PATH     = {232,  98,  60, 255};

    Color tile_color(TileType t) {
        switch (t) {
            case TileType::EMPTY: return C_EMPTY;
            case TileType::GRASS: return C_GRASS;
            case TileType::MUD:   return C_MUD;
            case TileType::WATER: return C_WATER;
            case TileType::WALL:  return C_WALL;
        }
        return C_EMPTY;
    }
}

RaylibRenderer::~RaylibRenderer() {
    if (initialized_) shutdown();
}

void RaylibRenderer::init() {
    if (initialized_) return;
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(WIN_W, WIN_H, "PathFinderXP");
    SetTargetFPS(28);
    initialized_ = true;
}

void RaylibRenderer::shutdown() {
    if (!initialized_) return;
    CloseWindow();
    initialized_ = false;
}

bool RaylibRenderer::should_close() const {
    return requested_quit_ || WindowShouldClose();
}

InputAction RaylibRenderer::take_input() {
    if (IsKeyPressed(KEY_Q) || IsKeyPressed(KEY_ESCAPE)) {
        requested_quit_ = true;
        return InputAction::QUIT;
    }
    if (IsKeyPressed(KEY_R)) return InputAction::RERUN;
    if (IsKeyPressed(KEY_G)) return InputAction::GENERATE;
    if (IsKeyPressed(KEY_C)) return InputAction::CLEAR;
    if (IsKeyPressed(KEY_F)) {
        ToggleBorderlessWindowed();
        return InputAction::FULLSCREEN;
    }
    return InputAction::NONE;
}

void RaylibRenderer::draw_panel(const AlgorithmFrame& f, const GridGraph& graph,
                                int origin_x, int origin_y) {
    DrawText(f.label.c_str(), origin_x, origin_y, 18, C_LABEL);

    const int gx = origin_x;
    const int gy = origin_y + LABEL_H;

    // Layer 1 — terrain
    for (int y = 0; y < GridGraph::HEIGHT; ++y) {
        for (int x = 0; x < GridGraph::WIDTH; ++x) {
            const Node* n = graph.get_node(x, y);
            if (!n) continue;
            DrawRectangle(gx + x * TILE, gy + y * TILE, TILE, TILE, tile_color(n->type));
        }
    }

    // Panel outline
    DrawRectangleLinesEx(
        Rectangle{ static_cast<float>(gx - 2), static_cast<float>(gy - 2),
                   static_cast<float>(PANEL_GRID_W + 4), static_cast<float>(PANEL_GRID_H + 4) },
        2.0f, C_BORDER);

    const float r = static_cast<float>(TILE);

    // Layer 2 — explored, with recency fade
    const std::size_t explore_end =
        std::min(static_cast<std::size_t>(std::max(0, f.animation_step)),
                 f.result.explored_order.size());
    constexpr std::size_t RECENCY_WINDOW = 40;
    for (std::size_t k = 0; k < explore_end; ++k) {
        const Node* n = f.result.explored_order[k];
        if (!n) continue;
        const int cx = gx + n->col * TILE + TILE / 2;
        const int cy = gy + n->row * TILE + TILE / 2;
        const std::size_t age = explore_end - 1 - k;
        const float recency = (age < RECENCY_WINDOW)
            ? 1.0f - 0.55f * (static_cast<float>(age) / RECENCY_WINDOW)
            : 0.45f;
        Color halo = C_EXPLORED; halo.a = static_cast<unsigned char>( 70.0f * recency);
        DrawCircle(cx, cy, r * 0.45f, halo);
        Color core = C_EXPLORED; core.a = static_cast<unsigned char>(180.0f * recency);
        DrawCircle(cx, cy, r * 0.28f, core);
    }

    // Layer 3 — final path
    const int exp_size = f.result.explored_size;
    const int path_visible = f.finished
                           ? static_cast<int>(f.result.path.size())
                           : std::max(0, f.animation_step - exp_size);
    for (int i = 0; i < path_visible && i < static_cast<int>(f.result.path.size()); ++i) {
        const Node* n = f.result.path[i];
        if (!n) continue;
        const int cx = gx + n->col * TILE + TILE / 2;
        const int cy = gy + n->row * TILE + TILE / 2;
        Color halo = C_PATH; halo.a = 80;
        DrawCircle(cx, cy, r * 0.55f, halo);
        DrawCircle(cx, cy, r * 0.34f, C_PATH);
    }

    // Layer 4 — start / goal markers (drawn last so nothing paints over them)
    const auto draw_marker = [&](const Node* node, Color fill, const char* letter) {
        if (!node) return;
        const int cx = gx + node->col * TILE + TILE / 2;
        const int cy = gy + node->row * TILE + TILE / 2;
        const float rad = TILE * 0.75f;
        DrawCircle(cx, cy, rad + 2, C_BG);
        DrawCircle(cx, cy, rad, fill);
        DrawCircleLines(cx, cy, rad, C_BORDER);
        DrawText(letter, cx - 3, cy - 6, 14, WHITE);
    };
    draw_marker(graph.start(), C_START, "S");
    draw_marker(graph.goal(),  C_GOAL,  "G");
}

void RaylibRenderer::draw_stats(const std::vector<AlgorithmFrame>& frames) {
    int best_visited = INT_MAX, best_hops = INT_MAX, best_cost = INT_MAX;
    double best_ms = std::numeric_limits<double>::max();
    for (const auto& f : frames) {
        if (f.result.path.empty()) continue;
        const int hops = static_cast<int>(f.result.path.size()) - 1;
        best_visited = std::min(best_visited, f.result.explored_size);
        best_hops    = std::min(best_hops,    hops);
        best_cost    = std::min(best_cost,    f.result.cost);
        best_ms      = std::min(best_ms,      f.result.elapsed.count());
    }

    const int sx = MARGIN + (PANEL_W + PANEL_GAP) * 2 + 8;
    int sy = MARGIN;
    DrawText("STATS", sx, sy, 24, C_LABEL);
    sy += 38;

    for (const auto& f : frames) {
        DrawText(f.label.c_str(), sx, sy, 18, C_LABEL);
        sy += 24;
        if (f.result.path.empty()) {
            DrawText("(no path found)", sx + 12, sy, 14, C_TEXT);
            sy += 40;
            continue;
        }
        const int hops = static_cast<int>(f.result.path.size()) - 1;
        const auto line = [&](const char* txt, bool is_best) {
            DrawText(txt, sx + 12, sy, 14, C_TEXT);
            if (is_best) DrawText("*", sx + 180, sy - 2, 18, C_BEST);
            sy += 18;
        };
        line(TextFormat("visited: %5d", f.result.explored_size), f.result.explored_size == best_visited);
        line(TextFormat("hops:    %5d", hops),                   hops == best_hops);
        line(TextFormat("cost:    %5d", f.result.cost),          f.result.cost == best_cost);
        line(TextFormat("ms:      %5.2f", f.result.elapsed.count()), f.result.elapsed.count() == best_ms);
        sy += 14;
    }

    DrawText("R: Run    G: Maze    C: Clear    F: Fullscreen    Q/ESC: Quit",
             sx, GetScreenHeight() - 36, 14, C_TEXT);
}

void RaylibRenderer::render(const std::vector<AlgorithmFrame>& frames,
                            const GridGraph& graph) {
    BeginDrawing();
    ClearBackground(C_BG);

    const int n = std::min(static_cast<int>(frames.size()), PANEL_COUNT);
    const int col0 = MARGIN,              col1 = MARGIN + PANEL_W + PANEL_GAP;
    const int row0 = MARGIN,              row1 = MARGIN + PANEL_H + PANEL_GAP;
    const int ox[4] = {col0, col1, col0, col1};
    const int oy[4] = {row0, row0, row1, row1};
    for (int i = 0; i < n; ++i) {
        draw_panel(frames[i], graph, ox[i], oy[i]);
    }
    draw_stats(frames);
    EndDrawing();
}