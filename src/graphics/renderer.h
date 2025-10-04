#pragma once
#include "implot.h"
#include "image/image.h"
#include "../data/data_history.h"

namespace renderer {

constexpr double WINDOW_SIZE_BUFFER = 25;
constexpr double GRAPH_BORDER_WIDTH = 5;
extern double content_scale;
extern int x_size;
extern int y_size;
void image_window(std::string name, image& image, ImVec2 pos, ImVec2 size);
void graph_window(std::string name, data_history data_history_, double min, double max, ImPlotColormap colormap, ImVec2 pos, ImVec2 size);
void control_window();
void automation_window();
void communications_window();

void start_renderer();

}