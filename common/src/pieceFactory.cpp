#include "pieceFactory.hpp"
#include <iostream>
#include <fstream>
#include "json.hpp"
#include "graphicsFactory.hpp"
using json = nlohmann::json;
namespace fs = std::filesystem;

PieceFactory::PieceFactory(const Board& board, const fs::path& pieces_root)
    : board_(board),
      pieces_root_(pieces_root),
      graphics_factory_(),     
      physics_factory_(board)
{
    for (const auto& entry : fs::directory_iterator(pieces_root_)) {
        if (entry.is_directory()) {
            std::string piece_type = entry.path().filename().string();
            fs::path states_dir = entry.path() ; 
            std::shared_ptr<State> st = build_state_machine(states_dir);
            state_templates_[piece_type] = st;
            std::cout << "Loaded piece template: " << piece_type << std::endl;
        }
    }
}
using GlobalTrans = std::unordered_map<std::string, std::unordered_map<std::string, std::string>>;
    static GlobalTrans load_master_csv(const fs::path& states_root) {
        GlobalTrans out;
        fs::path csv_path = states_root / "transitions.csv";
        if(!fs::exists(csv_path)) return out;
        std::ifstream in(csv_path);
        if(!in) return out;
        std::string line;
        // expecting CSV header: from_state,event,to_state
        std::getline(in, line); // skip header
        while(std::getline(in,line)) {
            std::stringstream ss(line);
            std::string frm, ev, nxt;
            if(std::getline(ss, frm, ',') && std::getline(ss, ev, ',') && std::getline(ss, nxt, ',')) {
                out[frm][ev] = nxt;
            }
        }
        return out;
    }
std::shared_ptr<State> PieceFactory::build_state_machine(const fs::path& piece_dir) {
    fs::path states_root = piece_dir / "states";
    if (!fs::exists(states_root) || !fs::is_directory(states_root)) {
        throw std::runtime_error("Missing states directory: " + states_root.string());
    }
    std::cout << "board_.cell_W_pix = " << board_.cell_W_pix << ", board_.cell_H_pix = " << board_.cell_H_pix << std::endl;
    std::cout << "board_.W_cells = " << board_.W_cells << ", board_.H_cells = " << board_.H_cells << std::endl;

    GlobalTrans global_trans = load_master_csv(states_root);
    std::unordered_map<std::string, std::shared_ptr<State>> states;

    std::pair<int, int> board_size = {board_.W_cells, board_.H_cells};
    std::pair<int, int> cell_px = {board_.cell_W_pix, board_.cell_H_pix};

    for (const auto& entry : fs::directory_iterator(states_root)) {
        if (!entry.is_directory()) continue;
        std::string name = entry.path().filename().string();

        // config.json
        nlohmann::json cfg;
        fs::path cfg_path = entry.path() / "config.json";
        if (fs::exists(cfg_path)) {
            std::ifstream f(cfg_path);
            try {
                f >> cfg;
            } catch (const std::exception&) {
                std::cerr << "Failed to parse config.json for state " << name << std::endl;
            }
        }

        // Moves
        fs::path moves_path = entry.path() / "moves.txt";
        std::shared_ptr<Moves> moves_ptr;
        if (fs::exists(moves_path)) {
            moves_ptr = std::make_shared<Moves>(moves_path.string(), Cell(board_size.first, board_size.second));
        }

        // Graphics
        nlohmann::json gfx_cfg = cfg.contains("graphics") ? cfg["graphics"] : nlohmann::json{};
        auto sprites_path = entry.path() / "sprites";
        if (!fs::exists(sprites_path) || !fs::is_directory(sprites_path)) {
            std::cerr << "Missing sprites directory for state " << name << ": " << sprites_path << std::endl;
        }
        auto graphics = graphics_factory_.load(sprites_path.string(), gfx_cfg, cell_px);
        auto graphics_ptr = std::make_shared<Graphics>(graphics);

        // Physics - קריאה מה-config של המהירות, אם קיימת
        Cell default_cell(0, 0);
        float default_speed = 1.0f; // ברירת מחדל
        if (cfg.contains("physics") && cfg["physics"].contains("speed_m_per_sec")) {
            default_speed = cfg["physics"]["speed_m_per_sec"].get<float>();
            if (default_speed <= 0.0f) {
                std::cerr << "Warning: speed_m_per_sec is zero or negative for state " << name << ". Using default 1.0f." << std::endl;
                default_speed = 1.0f;
            }
        }

        std::shared_ptr<Physics> physics_ptr;

        if (name == "idle") {
            physics_ptr = std::make_shared<IdlePhysics>(default_cell, board_, default_speed);
        } else if (name == "move") {
            physics_ptr = std::make_shared<MovePhysics>(default_cell, board_, default_speed);
        } else if (name == "jump") {
            physics_ptr = std::make_shared<JumpPhysics>(default_cell, board_, default_speed);
        } else if (name == "long_rest") {
            physics_ptr = std::make_shared<LongRestPhysics>(default_cell, board_, default_speed);
        } else if (name == "short_rest") {
            physics_ptr = std::make_shared<ShortRestPhysics>(default_cell, board_, default_speed);
        } else {
            std::cerr << "Unknown state type: " << name << ", using default Physics." << std::endl;
            //physics_ptr = std::make_shared<Physics>(default_cell, board_, default_speed);
            physics_ptr = std::make_shared<IdlePhysics>(default_cell, board_, default_speed);

        }

        auto st = std::make_shared<State>(moves_ptr, graphics_ptr, physics_ptr);
        st->name = name;
        states[name] = st;
    }

    // Apply global transitions
    for (const auto& [frm, ev_map] : global_trans) {
        auto src_it = states.find(frm);
        if (src_it == states.end()) continue;
        auto src = src_it->second;
        for (const auto& [ev, nxt] : ev_map) {
            auto dst_it = states.find(nxt);
            if (dst_it == states.end()) continue;
            src->set_transition(ev, dst_it->second);
        }
    }

    auto idle_it = states.find("idle");
    if (idle_it == states.end()) {
        throw std::runtime_error("State machine missing 'idle' state in " + piece_dir.string());
    }
    return idle_it->second;
}


std::shared_ptr<Piece> PieceFactory::create_piece(const std::string& p_type, std::pair<int,int> cell) {
    auto it = state_templates_.find(p_type);
    if (it == state_templates_.end()) {
        throw std::runtime_error("Unknown piece type: " + p_type);
    }

    auto init_state = it->second->clone();

    Command init_cmd; 
    init_cmd.dst = Cell(cell.first, cell.second);

    init_state->reset(init_cmd);

    auto piece = std::make_shared<Piece>(p_type, init_state);
    return piece;
}

