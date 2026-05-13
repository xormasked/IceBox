#pragma once
#include <core/drawing/menu/components/component_builder.hpp>

struct c_config {
  const char *id;
  char name[32];
  const char *author;
  const char *modified;
  bool editing = false;
};

struct multi_select_item {
  std::string label;
  bool selected = false;

  multi_select_item(std::string _label) : label{_label} {};
  multi_select_item(const char *label) : label(label) {};

  operator bool() const { return selected; }
};

class WidgetsManager {
public:
  int Config(int i, std::vector<c_config> &configs, bool loaded);
  bool Checkbox(std::string label, bool *v, int *key = nullptr,
                float *col = nullptr, std::function<void()> options = nullptr,
                bool warning = false, bool disabled = false);
  template <typename T>
  bool Slider(std::string label, T *v, T min, T max, const char *format);
  bool SliderInt(std::string label, int *v, int min, int max,
                 const char *format = "%d");
  bool SliderFloat(std::string label, float *v, float min, float max,
                   const char *format = "%.1f");
  bool ComboEx(std::string label, const char *preview_value,
               std::function<void(CompBuilder::ComboEnv env)> code);
  bool Combo(std::string label, int *v, std::vector<std::string> items);
  bool MultiCombo(std::string label, std::vector<multi_select_item> *items);
  bool Binder(std::string label, int *key);
  bool TextField(std::string label, char *buf, size_t buf_size,
                 ImVec2 size = ImVec2{0, 0}, const char *hint = 0,
                 const char *icon = 0);
  bool Button(std::string label, ImVec2 size = ImVec2{0, 0});
  bool WarningButton(std::string label, ImVec2 size = ImVec2{0, 0});
  bool ColorEdit(std::string label, float col[4]);
  bool Selectable(std::string label, bool selected, ImVec2 size = ImVec2{0, 0});
  void Separator();

  static WidgetsManager &get() {
    static WidgetsManager s{};
    return s;
  }
};