#ifndef GRAPHICS_FACTORY_H
#define GRAPHICS_FACTORY_H
#include "json.hpp"
#include <filesystem>
#include <string>
#include <map>
#include <utility>
#include "graphics.hpp"
#include <memory>

class GraphicsFactory {
public:
Graphics load(const std::filesystem::path& sprites_dir,
                              const nlohmann::json& cfg,
                              std::pair<int, int> cell_size);

};

#endif // GRAPHICS_FACTORY_H
