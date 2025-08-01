#include "graphicsFactory.hpp"

Graphics GraphicsFactory::load(const std::filesystem::path& sprites_dir,
                              const nlohmann::json& cfg,
                              std::pair<int, int> cell_size)
{
    // קראי שדות מתוך cfg ישירות
    int frames_per_sec = 6;
    bool is_loop = true;

    if (cfg.contains("frames_per_sec")) frames_per_sec = cfg["frames_per_sec"].get<int>();
    if (cfg.contains("is_loop")) is_loop = cfg["is_loop"].get<bool>();

    return Graphics(sprites_dir, cell_size, is_loop, static_cast<float>(frames_per_sec));
}

