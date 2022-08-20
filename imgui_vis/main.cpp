
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#include "sdl_system.h"

#include <filesystem>
#include <iostream>
#include <cmath>
#include <vector>

#include "../manual/point.h"

using namespace std;
namespace fs = std::filesystem;

#define forn(i, N) for (int i = 0; i < (int)(N); i++)
#define sqr(x) (x)*(x)

int selected_id;

struct Edge {
    int from, to;
    double D;
};

int H, M, N, E;
vector<PointD> hole;
vector<Edge> edges;
vector<pair<int, int>> edgePairs;
vector<PointD> points, srcPoints;
vector<string> bonusname;
vector<PointD> bonus;
vector<vector<int>> g;

double scale = 2;
double shiftX, shiftY;
float iterStep = 0.1;

void doIter() {
    vector<PointD> forces(N);

    for (const Edge& e : edges) {
        double cd = (points[e.from] - points[e.to]).abs2();
        if (cd > e.D) {
            double f = (sqrt(cd) - sqrt(e.D)) / sqrt(e.D) * iterStep;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] + v;
            forces[e.to] = forces[e.to] - v;
        } else {
            double f = (sqrt(e.D) - sqrt(cd)) / (sqrt(cd) + 1) * iterStep;
            PointD v = (points[e.to] - points[e.from]) * f;
            forces[e.from] = forces[e.from] - v;
            forces[e.to] = forces[e.to] + v;
        }
    }

    forn(i, N) {
        double m = forces[i].abs();
        if (m > 20) forces[i] = forces[i] / m * 20;
        points[i] = points[i] + forces[i];
    }
}

void readInput(const string& fname) {
    freopen(fname.c_str(), "r", stdin);
    cin >> H;
    hole.resize(H + 1);
    forn(i, H) cin >> hole[i].x >> hole[i].y;
    hole[H] = hole[0];

    cin >> M;
    edges.resize(M);
    edgePairs.resize(M);
    forn(i, M) {
        cin >> edges[i].from >> edges[i].to;
        edgePairs[i] = {edges[i].from, edges[i].to};
    }

    cin >> N;
    points.resize(N);
    srcPoints.resize(N);
    g.clear();
    g.resize(N);
    forn(i, N) {
        cin >> srcPoints[i].x >> srcPoints[i].y;
        points[i].x = srcPoints[i].x;
        points[i].y = srcPoints[i].y;
    }

    forn(i, M) {
        edges[i].D = sqr(points[edges[i].from].y - points[edges[i].to].y) + sqr(points[edges[i].from].x - points[edges[i].to].x);
        g[edges[i].from].push_back(edges[i].to);
        g[edges[i].to].push_back(edges[i].from);
    }

    cin >> E;

    int B, bp;
    cin >> B;
    bonusname.resize(B);
    bonus.resize(B);
    cerr << "Read " << fname << endl;
    forn(ib, B) {
        cin >> bonusname[ib] >> bp >> bonus[ib].x >> bonus[ib].y;
        cerr << "contains bonus " << bonusname[ib] << " for problem " << bp << endl;
    }

    scale = 2;
    shiftX = shiftY = 0;
}

void fileWindow() {
    if(ImGui::Begin("Tests")) {
        std::string path = "../inputs_conv/";

        vector<pair<int, string>> tests;
        for (const auto & entry : fs::directory_iterator(path)) {
            string s = entry.path();
            tests.emplace_back(0, s);
            int i = 0;
            while (i < s.size() && (s[i] < '0' || s[i] > '9')) i++;
            if (i >= s.size()) continue;
            int j = i;
            while (s[j] >= '0' && s[j] <= '9') j++;
            sscanf(s.substr(i, j).c_str(), "%d", &tests.back().first);
        }

        sort(tests.begin(), tests.end());
        static int selected_id = -1;

        if (ImGui::BeginListBox("T", ImVec2(250, ImGui::GetFrameHeightWithSpacing() * 16))) {
            for (const auto& [idx, path] : tests) {
                const bool is_selected = (idx == selected_id);
                if (ImGui::Selectable(path.c_str(), is_selected)) {
                    selected_id = idx;
                    readInput(path);
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndListBox();
        }
    }

    ImGui::End();
}

void draw() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    ImU32 holeColor = IM_COL32(28, 28, 92, 255);
    int holeThickness = 3;
    auto QP = [](double x, double y) {
        return ImVec2(x * scale - shiftX, y * scale - shiftY);
    };
    forn(i, H) {
        dl->AddLine(QP(hole[i].x, hole[i].y), QP(hole[i + 1].x, hole[i + 1].y), holeColor, holeThickness);
    }

    ImU32 okEdgeColor = IM_COL32(22, 182, 22, 255);
    ImU32 badEdgeColor = IM_COL32(182, 22, 22, 255);
    ImU32 figVertexColor = IM_COL32(28, 72, 12, 255);
    int figThickness = 2;
    for (const auto& e : edges) {
        if (fabs((points[e.from] - points[e.to]).abs() - sqrt(e.D)) * 1e6 < E * sqrt(e.D))
            dl->AddLine(QP(points[e.from].x, points[e.from].y), QP(points[e.to].x, points[e.to].y), okEdgeColor, figThickness);
        else
            dl->AddLine(QP(points[e.from].x, points[e.from].y), QP(points[e.to].x, points[e.to].y), badEdgeColor, figThickness);
    }

    forn(i, N)
        dl->AddCircle(QP(points[i].x, points[i].y), 4, figVertexColor);
}

int draggedVertexInd = -1;

void processMouse() {
    auto& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    if (io.MouseWheel == 1) {
        scale = scale * 1.1;
    }
    if (io.MouseWheel == -1) {
        scale = scale / 1.1;
    }
    if (ImGui::IsMouseDown(1)) {
        shiftX -= io.MouseDelta.x;
        shiftY -= io.MouseDelta.y;
    }
    if (ImGui::IsMouseDown(0)) {
        if (draggedVertexInd != -1) {
            points[draggedVertexInd].x += io.MouseDelta.x / scale;
            points[draggedVertexInd].y += io.MouseDelta.y / scale;
        } else {
            for (int i = 0; i < N; i++) {
                double px = points[i].x * scale - shiftX;
                double py = points[i].y * scale - shiftY;
                if (sqr(px - io.MousePos.x) + sqr(py - io.MousePos.y) < 16) {
                    draggedVertexInd = i;
                }
            }
        }
    }
    if (ImGui::IsMouseReleased(0)) {
        draggedVertexInd = -1;
    }
}

void doInt() {
    forn(i, N) {
        points[i].x = round(points[i].x);
        points[i].y = round(points[i].y);
    }
}

void optsWindow() {
    if (ImGui::Begin("Solution")) {
        ImGui::SliderFloat("Iter", &iterStep, 0.01f, 0.8f, "iter step = %.3f");
        if (ImGui::Button("Do int")) doInt();

    }
    ImGui::End();
}

void inputWindow() {
    auto& io = ImGui::GetIO();
    if (ImGui::Begin("Mouse & Keyboard")) {
        if (ImGui::IsMousePosValid())
            ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
        else
            ImGui::Text("Mouse pos: <INVALID>");
        ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);

        int count = IM_ARRAYSIZE(io.MouseDown);
        ImGui::Text("Mouse down:");         for (int i = 0; i < count; i++) if (ImGui::IsMouseDown(i))      { ImGui::SameLine(); ImGui::Text("b%d (%.02f secs)", i, io.MouseDownDuration[i]); }
        ImGui::Text("Mouse clicked:");      for (int i = 0; i < count; i++) if (ImGui::IsMouseClicked(i))   { ImGui::SameLine(); ImGui::Text("b%d (%d)", i, ImGui::GetMouseClickedCount(i)); }
        ImGui::Text("Mouse released:");     for (int i = 0; i < count; i++) if (ImGui::IsMouseReleased(i))  { ImGui::SameLine(); ImGui::Text("b%d", i); }
        ImGui::Text("Mouse wheel: %.1f", io.MouseWheel);

        ImGui::Separator();

        const ImGuiKey key_first = ImGuiKey_NamedKey_BEGIN;
        ImGui::Text("Keys down:");          for (ImGuiKey key = key_first; key < ImGuiKey_COUNT; key++) { if (ImGui::IsKeyDown(key)) { ImGui::SameLine(); ImGui::Text("\"%s\" %d", ImGui::GetKeyName(key), key); } }
    }
    ImGui::End();
}

int main(int, char**)
{
    SDLWrapper sw;
    if (!sw.init()) return -1;

    while (true) {
        if (sw.checkQuit()) break;
        sw.newFrame();

        inputWindow();
        fileWindow();
        optsWindow();
        if (ImGui::IsKeyDown(537)) doIter();

        processMouse();
        draw();

        sw.finishFrame();
    }

    sw.cleanup();
    return 0;
}
