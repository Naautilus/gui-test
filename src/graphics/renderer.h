#pragma once
#include "implot.h"
#include "image/image.h"
#include "../data/data_history.h"

namespace graphics {

class renderer {
    const double WINDOW_SIZE_BUFFER = 25;
    const double GRAPH_BORDER_WIDTH = 5;
    double content_scale;
    int x_size;
    int y_size;
    void image_window(std::string name, image& image, ImVec2 pos, ImVec2 size);
    void graph_window(std::string name, data_history data_history_, double min, double max, ImPlotColormap colormap, ImVec2 pos, ImVec2 size);
    void control_window();
    void automation_window();
    void communications_window();
    public:
    void add_text_to_communications_window(bool write, std::string text); // bool write = true if writing to teensy, false if reading from teensy
    renderer();
};

void start_renderer();

}