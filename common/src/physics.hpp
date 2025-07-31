// #ifndef PHYSICS_HPP
// #define PHYSICS_HPP

// #include "command.hpp"
// #include "cell.hpp"
// #include "board.hpp"
// #include <memory>

// class Physics {
// public:
//     static constexpr float DEFAULT_SPEED_M_S = 1.0f;
//     virtual std::pair<float, float> get_subpixel_pos() const {
//         return current_pos_subpix_;
//     }
//     Physics(Cell start_cell,
//             const Board& board,
//             float speed_m_s = DEFAULT_SPEED_M_S);

//     virtual ~Physics() = default;

//     virtual void reset(const Command& cmd);
//     virtual Command update(int now_ms);

//     virtual bool can_be_captured() const;
//     virtual bool can_capture() const;

//     Cell get_pos() const;
//     float get_speed_m_s() const { return speed_m_s_; }
//     int get_start_time_ms() const { return start_time_ms_; }
//     // השיטה החדשה - האם הפעולה הסתיימה?
//     virtual bool is_finished() const ;

// protected:
//     Cell start_cell_;
//     const Board& board_;
//     float speed_m_s_;

//     std::pair<float,float> current_pos_subpix_;
//     int last_update_time_ms_;
//     int start_time_ms_;
// };

// class IdlePhysics : public Physics {
// public:
//     IdlePhysics(Cell start_cell,
//                 const Board& board,
//                 float speed_m_s = DEFAULT_SPEED_M_S);

//     void reset(const Command& cmd) override;
//     bool can_be_captured() const override { return true; }
//     bool can_capture() const override { return false; }

//     Command update(int now_ms) override;

//     bool is_finished() const override; // idle תמיד "גמור"
// };

// class MovePhysics : public Physics {
// public:
//     MovePhysics(Cell start_cell,
//                 const Board& board,
//                 float speed_m_s = DEFAULT_SPEED_M_S);

//     void reset(const Command& cmd) override;
//     Command update(int now_ms) override;

//     bool is_finished() const override;

// private:
//     Cell target_cell_;
//     float total_distance_m_;
// };

// // ממשיכים גם עם התנועות הנוספות

// class JumpPhysics : public Physics {
// public:
//     JumpPhysics(Cell start_cell,
//                 const Board& board,
//                 float speed_m_s = DEFAULT_SPEED_M_S);

//     void reset(const Command& cmd) override;
//     Command update(int now_ms) override;
//     bool can_be_captured() const override { return false; }
//     bool can_capture() const override { return true; }

//     bool is_finished() const override;

// private:
//     // לדוגמה: זמן קפיצה, זמן שעבר, או מצבים פנימיים נוספים
//     int jump_duration_ms_;
//     int jump_start_time_ms_;
// };

// class LongRestPhysics : public Physics {
// public:
//     LongRestPhysics(Cell start_cell,
//                     const Board& board,
//                     float speed_m_s = DEFAULT_SPEED_M_S);

//     void reset(const Command& cmd) override;
//     Command update(int now_ms) override;
//     bool can_be_captured() const override { return true; }
//     bool can_capture() const override { return false; }

//     bool is_finished() const override;

// private:
//     int rest_duration_ms_;
//     int rest_start_time_ms_;
// };

// class ShortRestPhysics : public Physics {
// public:
//     ShortRestPhysics(Cell start_cell,
//                      const Board& board,
//                      float speed_m_s = DEFAULT_SPEED_M_S);

//     void reset(const Command& cmd) override;
//     Command update(int now_ms) override;
//     bool can_be_captured() const override { return true; }
//     bool can_capture() const override { return false; }

//     bool is_finished() const override;

// private:
//     int rest_duration_ms_;
//     int rest_start_time_ms_;
// };

// #endif // PHYSICS_HPP

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "command.hpp"
#include "cell.hpp"
#include "board.hpp"
#include <memory>

class Physics {
public:
    static constexpr float DEFAULT_SPEED_M_S = 1.0f;
    virtual std::pair<float, float> get_subpixel_pos() const {
        return current_pos_subpix_;
    }
    Physics(Cell start_cell,
            const Board& board,
            float speed_m_s = DEFAULT_SPEED_M_S);

    virtual ~Physics() = default;

    virtual void reset(const Command& cmd);
    virtual Command update(int now_ms);

    virtual bool can_be_captured() const;
    virtual bool can_capture() const;

    Cell get_pos() const;
    float get_speed_m_s() const { return speed_m_s_; }
    int get_start_time_ms() const { return start_time_ms_; }
    virtual bool is_finished() const;

    virtual std::shared_ptr<Physics> clone() const = 0; // <- הוספה

protected:
    Cell start_cell_;
    const Board& board_;
    float speed_m_s_;

    std::pair<float,float> current_pos_subpix_;
    int last_update_time_ms_;
    int start_time_ms_;
};

class IdlePhysics : public Physics {
public:
    IdlePhysics(Cell start_cell,
                const Board& board,
                float speed_m_s = DEFAULT_SPEED_M_S);

    void reset(const Command& cmd) override;
    bool can_be_captured() const override { return true; }
    bool can_capture() const override { return false; }

    Command update(int now_ms) override;

    bool is_finished() const override;

    std::shared_ptr<Physics> clone() const override {
        return std::make_shared<IdlePhysics>(*this);
    }
};

class MovePhysics : public Physics {
public:
    MovePhysics(Cell start_cell,
                const Board& board,
                float speed_m_s = DEFAULT_SPEED_M_S);
    

    void reset(const Command& cmd) override;
    Command update(int now_ms) override;

    bool is_finished() const override;

    std::shared_ptr<Physics> clone() const override {
        return std::make_shared<MovePhysics>(*this);
    }

private:
    Cell target_cell_;
    float total_distance_m_;
};

class JumpPhysics : public Physics {
public:
    JumpPhysics(Cell start_cell,
                const Board& board,
                float speed_m_s = DEFAULT_SPEED_M_S);

    void reset(const Command& cmd) override;
    Command update(int now_ms) override;
    bool can_be_captured() const override { return false; }
    bool can_capture() const override { return true; }

    bool is_finished() const override;

    std::shared_ptr<Physics> clone() const override {
        return std::make_shared<JumpPhysics>(*this);
    }

private:
    int jump_duration_ms_;
    int jump_start_time_ms_;
};

class LongRestPhysics : public Physics {
public:
    LongRestPhysics(Cell start_cell,
                    const Board& board,
                    float speed_m_s = DEFAULT_SPEED_M_S);

    void reset(const Command& cmd) override;
    Command update(int now_ms) override;
    bool can_be_captured() const override { return true; }
    bool can_capture() const override { return false; }

    bool is_finished() const override;

    std::shared_ptr<Physics> clone() const override {
        return std::make_shared<LongRestPhysics>(*this);
    }

private:
    int rest_duration_ms_;
    int rest_start_time_ms_;
};

class ShortRestPhysics : public Physics {
public:
    ShortRestPhysics(Cell start_cell,
                     const Board& board,
                     float speed_m_s = DEFAULT_SPEED_M_S);

    void reset(const Command& cmd) override;
    Command update(int now_ms) override;
    bool can_be_captured() const override { return true; }
    bool can_capture() const override { return false; }

    bool is_finished() const override;

    std::shared_ptr<Physics> clone() const override {
        return std::make_shared<ShortRestPhysics>(*this);
    }

private:
    int rest_duration_ms_;
    int rest_start_time_ms_;
};

#endif // PHYSICS_HPP
