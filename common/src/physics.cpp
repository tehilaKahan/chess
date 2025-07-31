#include "physics.hpp"
#include <cmath>
#include <iostream> // אם רוצים debug

// ---------- Physics ----------

Physics::Physics(Cell start_cell,
                 const Board& board,
                 float speed_m_s)
    : start_cell_(start_cell),
      board_(board),
      speed_m_s_(speed_m_s),
      last_update_time_ms_(0),
      start_time_ms_(0),
      current_pos_subpix_(board.cell_to_pixels(start_cell))
{}

void Physics::reset(const Command& cmd) {
    // אתחל את מיקום ההתחלה למיקום היעד בפקודה
    start_cell_ = cmd.dst;

    // אתחל את מיקום התת-פיקסלים למרכז התא
    current_pos_subpix_ = board_.cell_to_pixels(start_cell_);

    // איפוס זמני התחלה ועדכון
    start_time_ms_ = 0;
    last_update_time_ms_ = 0;

    // מהירות יכולה להתעדכן או להשאר כפי שהיא
    // speed_m_s_ נשאר כפי שהוגדר או אפשר לעדכן כאן אם רוצים
}

Command Physics::update(int now_ms) {
    last_update_time_ms_ = now_ms;
    return Command(); // לא זז
}

bool Physics::can_be_captured() const {
    return true;
}

bool Physics::can_capture() const {
    return true;
}

Cell Physics::get_pos() const {
    return Cell{
        static_cast<int>(current_pos_subpix_.first),
        static_cast<int>(current_pos_subpix_.second)
    };
}

bool Physics::is_finished() const {
    return false; // כברירת מחדל, לא סיימנו כלום
}

// ---------- IdlePhysics ----------

IdlePhysics::IdlePhysics(Cell start_cell,
                         const Board& board,
                         float speed_m_s)
    : Physics(start_cell, board, speed_m_s)
{}

void IdlePhysics::reset(const Command& cmd) {
    Physics::reset(cmd);
}

Command IdlePhysics::update(int now_ms) {
    last_update_time_ms_ = now_ms;
    return Command();
}

bool IdlePhysics::is_finished() const {
    // idle תמיד פעיל, לא מסתיים מעצמו
    return false;
}

// ---------- MovePhysics ----------

// MovePhysics::MovePhysics(Cell start_cell,
//                          const Board& board,
//                          float speed_m_s)
//     : Physics(start_cell, board, speed_m_s),
//       target_cell_(start_cell),
//       total_distance_m_(0.0f)
// {}
MovePhysics::MovePhysics(Cell start_cell, const Board& board, float speed_m_s)
    : Physics(start_cell, board, speed_m_s),
      target_cell_({0, 0}),  // אתחול ברור
      total_distance_m_(0.0f)
{}


// void MovePhysics::reset(const Command& cmd) {
//     start_cell_ = cmd.src;
//     target_cell_ = cmd.dst;
//     current_pos_subpix_ = board_.cell_to_pixels(start_cell_);
    
//     int now_ms = cmd.timestamp;  // או קח את הזמן ממקום אחר אם אין בפקודה

//     last_update_time_ms_ = now_ms;
//     start_time_ms_ = now_ms;

//     auto start_pix = current_pos_subpix_;
//     auto target_pix = board_.cell_to_pixels(target_cell_);
//     float dx = target_pix.first - start_pix.first;
//     float dy = target_pix.second - start_pix.second;
//     total_distance_m_ = std::sqrt(dx*dx + dy*dy);
// }
// void MovePhysics::reset(const Command& cmd) {
//     std::cout << "[MovePhysics] reset called with cmd: src=(" << cmd.src.row << "," << cmd.src.col 
//               << ") dst=(" << cmd.dst.row << "," << cmd.dst.col << ")" << std::endl;
    
//     start_cell_ = cmd.src;
//     target_cell_ = cmd.dst;
    
//     std::cout << "[MovePhysics] target_cell_ set to: (" << target_cell_.row << "," << target_cell_.col << ")" << std::endl;
    
//     current_pos_subpix_ = board_.cell_to_pixels(start_cell_);
    
//     std::cout << "[MovePhysics] start position: (" << current_pos_subpix_.first << "," << current_pos_subpix_.second << ")" << std::endl;

//     int now_ms = cmd.timestamp;
//     last_update_time_ms_ = now_ms;
//     start_time_ms_ = now_ms;

//     auto start_pix = current_pos_subpix_;
//     auto target_pix = board_.cell_to_pixels(target_cell_);
    
//     std::cout << "[MovePhysics] target position: (" << target_pix.first << "," << target_pix.second << ")" << std::endl;
    
//     float dx = target_pix.first - start_pix.first;
//     float dy = target_pix.second - start_pix.second;
//     total_distance_m_ = std::sqrt(dx*dx + dy*dy);
    
//     std::cout << "[MovePhysics] total_distance_m_: " << total_distance_m_ << std::endl;
// }

// ב-MovePhysics::reset()
void MovePhysics::reset(const Command& cmd) {
    start_cell_ = cmd.src;
    target_cell_ = cmd.dst;
    
    // חישוב פשוט במקום board_.cell_to_pixels()
    current_pos_subpix_.first = 370 + cmd.src.col * 87;  // 370 = offset_x משוער
    current_pos_subpix_.second = 55 + cmd.src.row * 87;   // 55 = offset_y משוער
    
    int now_ms = cmd.timestamp;
    last_update_time_ms_ = now_ms;
    start_time_ms_ = now_ms;
    
    float target_x = 370 + cmd.dst.col * 87;
    float target_y = 55 + cmd.dst.row * 87;
    
    float dx = target_x - current_pos_subpix_.first;
    float dy = target_y - current_pos_subpix_.second;
    total_distance_m_ = std::sqrt(dx*dx + dy*dy);
}
Command MovePhysics::update(int now_ms) {
    if (last_update_time_ms_ == 0) {
        last_update_time_ms_ = now_ms;
        start_time_ms_ = now_ms;
    }

    int delta_ms = now_ms - last_update_time_ms_;
    float delta_sec = delta_ms / 1000.0f;
    last_update_time_ms_ = now_ms;

    // חישוב פשוט במקום board_.cell_to_pixels()
    float target_x = 370 + target_cell_.col * 87;
    float target_y = 55 + target_cell_.row * 87;

    float dx = target_x - current_pos_subpix_.first;
    float dy = target_y - current_pos_subpix_.second;
    float distance_to_target = std::sqrt(dx*dx + dy*dy);

    float speed_pixels_per_sec = 150; // 1.5 * 100
    float traveled_pixels = delta_sec * speed_pixels_per_sec;

    if (traveled_pixels >= distance_to_target) {
        current_pos_subpix_.first = target_x;
        current_pos_subpix_.second = target_y;
        return Command(now_ms, "", "done", target_cell_, target_cell_);
    } else {
        float ratio = traveled_pixels / distance_to_target;
        current_pos_subpix_.first += dx * ratio;
        current_pos_subpix_.second += dy * ratio;
        return Command();
    }
}

bool MovePhysics::is_finished() const {
    float target_x = 370 + target_cell_.col * 87;
    float target_y = 55 + target_cell_.row * 87;
    
    return (std::abs(current_pos_subpix_.first - target_x) < 0.01f) &&
           (std::abs(current_pos_subpix_.second - target_y) < 0.01f);
}



// Command MovePhysics::update(int now_ms) {
//     std::cout << "[MovePhysics] update called\n";
//     std::cout << "[MovePhysics] now_ms: " << now_ms << "\n";
//     std::cout << "[MovePhysics] last_update_time_ms_: " << last_update_time_ms_ << "\n";

//     if (last_update_time_ms_ == 0) {
//         last_update_time_ms_ = now_ms;
//         std::cout << "[MovePhysics] Starting move at time: " << now_ms << " ms\n";
//         start_time_ms_ = now_ms;
//     }

//     int delta_ms = now_ms - last_update_time_ms_;
//     std::cout << "[MovePhysics] delta_ms (now - last): " << delta_ms << " ms\n";

//     float delta_sec = delta_ms / 1000.0f;
//     std::cout << "[MovePhysics] delta_sec: " << delta_sec << " seconds\n";

//     last_update_time_ms_ = now_ms;

//     auto target_pix = board_.cell_to_pixels(target_cell_);
//     std::cout << "[MovePhysics] target_pix: (" << target_pix.first << ", " << target_pix.second << ")\n";

//     float dx = target_pix.first - current_pos_subpix_.first;
//     float dy = target_pix.second - current_pos_subpix_.second;

//     float distance_to_target = std::sqrt(dx*dx + dy*dy);
//     std::cout << "[MovePhysics] Distance to target: " << distance_to_target << " pixels\n";

//     float speed_pixels_per_sec = speed_m_s_ * board_.get_pixels_per_meter();
//     std::cout << "[MovePhysics] Speed: " << speed_m_s_ << " m/s, Pixels per meter: " << board_.get_pixels_per_meter() << "\n";
//     std::cout << "[MovePhysics] Speed in pixels/sec: " << speed_pixels_per_sec << "\n";

//     float traveled_pixels = delta_sec * speed_pixels_per_sec;
//     std::cout << "[MovePhysics] Traveled pixels this update: " << traveled_pixels << "\n";

//     if (traveled_pixels >= distance_to_target) {
//         current_pos_subpix_ = target_pix;
//         std::cout << "[MovePhysics] Movement done. Position set to target (" 
//                   << current_pos_subpix_.first << ", " << current_pos_subpix_.second << ")\n";
//         return Command(now_ms, "", "done", target_cell_, target_cell_);
//     } else {
//         float ratio = traveled_pixels / distance_to_target;
//         current_pos_subpix_.first += dx * ratio;
//         current_pos_subpix_.second += dy * ratio;
//         std::cout << "[MovePhysics] Moving. Current position: (" 
//                   << current_pos_subpix_.first << ", " << current_pos_subpix_.second << ")\n";
//         return Command();
//     }
// }


// bool MovePhysics::is_finished() const {
//     auto target_pix = board_.cell_to_pixels(target_cell_);
//     return (std::abs(current_pos_subpix_.first - target_pix.first) < 0.01f) &&
//            (std::abs(current_pos_subpix_.second - target_pix.second) < 0.01f);
// }
// bool MovePhysics::is_finished() const {
//     auto target_pix = board_.cell_to_pixels(target_cell_);
//     bool finished = (std::abs(current_pos_subpix_.first - target_pix.first) < 0.01f) &&
//                    (std::abs(current_pos_subpix_.second - target_pix.second) < 0.01f);
    
//     std::cout << "[MovePhysics] is_finished check:" << std::endl;
//     std::cout << "  current_pos: (" << current_pos_subpix_.first << ", " << current_pos_subpix_.second << ")" << std::endl;
//     std::cout << "  target_pix: (" << target_pix.first << ", " << target_pix.second << ")" << std::endl;
//     std::cout << "  diff_x: " << std::abs(current_pos_subpix_.first - target_pix.first) << std::endl;
//     std::cout << "  diff_y: " << std::abs(current_pos_subpix_.second - target_pix.second) << std::endl;
//     std::cout << "  finished: " << (finished ? "true" : "false") << std::endl;
    
//     return finished;
// }


// ---------- JumpPhysics ----------

JumpPhysics::JumpPhysics(Cell start_cell,
                         const Board& board,
                         float speed_m_s)
    : Physics(start_cell, board, speed_m_s)
{}

void JumpPhysics::reset(const Command& cmd) {
    Physics::reset(cmd);
    // כאן אפשר לאתחל מידע מיוחד לקפיצה, למשל שינוי גובה או מצבי ביניים
    std::cout << "JumpPhysics reset called\n";
}

Command JumpPhysics::update(int now_ms) {
    if (last_update_time_ms_ == 0)
        start_time_ms_ = now_ms;
    last_update_time_ms_ = now_ms;

    // TODO: הוסף כאן לוגיקה מתקדמת של קפיצה

    // כרגע פשוט מחזיר פקודה ריקה (אין שינוי)
    return Command();
}

bool JumpPhysics::is_finished() const {
    int elapsed = last_update_time_ms_ - start_time_ms_;
    return elapsed >= 1000; // לדוגמה: קפיצה שנמשכת שנייה אחת
}

// ---------- LongRestPhysics ----------

LongRestPhysics::LongRestPhysics(Cell start_cell,
                                 const Board& board,
                                 float speed_m_s)
    : Physics(start_cell, board, speed_m_s)
{}

void LongRestPhysics::reset(const Command& cmd) {
    Physics::reset(cmd);
    std::cout << "LongRestPhysics reset called\n";
}

Command LongRestPhysics::update(int now_ms) {
    if (last_update_time_ms_ == 0)
        start_time_ms_ = now_ms;

    last_update_time_ms_ = now_ms;

    int elapsed = now_ms - start_time_ms_;
    int duration_ms = 5000;  // מנוחה ארוכה של 5 שניות

    if (elapsed >= duration_ms) {
        return Command(now_ms, "", "done", start_cell_, start_cell_);
    }

    return Command();
}


bool LongRestPhysics::is_finished() const {
    int elapsed = last_update_time_ms_ - start_time_ms_;
    return elapsed >= 3000; // לדוגמה: מנוחה ארוכה של 3 שניות
}

// ---------- ShortRestPhysics ----------

ShortRestPhysics::ShortRestPhysics(Cell start_cell,
                                   const Board& board,
                                   float speed_m_s)
    : Physics(start_cell, board, speed_m_s)
{}

void ShortRestPhysics::reset(const Command& cmd) {
    Physics::reset(cmd);
    std::cout << "ShortRestPhysics reset called\n";
}

Command ShortRestPhysics::update(int now_ms) {
    if (last_update_time_ms_ == 0)
        start_time_ms_ = now_ms;

    last_update_time_ms_ = now_ms;

    int elapsed = now_ms - start_time_ms_;
    int duration_ms = 3000;  // למשל, מנוחה קצרה של שנייה אחת

    if (elapsed >= duration_ms) {
        // סיימנו את המנוחה, מחזירים פקודת done
        return Command(now_ms, "", "done", start_cell_, start_cell_);
    }

    // עוד לא סיימנו, מחזירים פקודה ריקה
    return Command();
}


bool ShortRestPhysics::is_finished() const {
    int elapsed = last_update_time_ms_ - start_time_ms_;
    return elapsed >= 1000; // לדוגמה: מנוחה קצרה של שנייה אחת
}
