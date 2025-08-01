#include "gameFactory.hpp"
#include "img.hpp"  // תקן את הנתיב
#include "pieceFactory.hpp"
#include "../../server/src/game.hpp"  // הוסף את זה
#include <filesystem>
#include <memory>
#include <iostream>

Game create_game(const std::string& board_img_path, const std::string& pieces_folder_path) {
    try {
        // נתיב יחסי במקום קשיח
        std::string background_path = pieces_folder_path + "/../background.jpg";
        
        auto background_img = std::make_shared<Img>();
        if (std::filesystem::exists(background_path)) {
            background_img->read(background_path, {1440, 810}, true);
        } else {
            // צור תמונת רקע ברירת מחדל
            background_img->create_blank(1440, 810, true);
        }

        auto board_img = std::make_shared<Img>();
        board_img->read(board_img_path);

        int orig_board_width = board_img->width();
        int orig_board_height = board_img->height();
        int screen_width = background_img->width();
        int screen_height = background_img->height();

        // חשב קנה מידה
        double scale_factor = std::min(
            static_cast<double>(screen_width) / orig_board_width,
            static_cast<double>(screen_height) / orig_board_height
        );

        int new_board_width = static_cast<int>(orig_board_width * scale_factor);
        int new_board_height = static_cast<int>(orig_board_height * scale_factor);

        // טען מחדש בגודל חדש
        board_img->read(board_img_path, {new_board_width, new_board_height}, true);

        // חשב אופסטים
        int offset_x = (screen_width - new_board_width) / 2;
        int offset_y = (screen_height - new_board_height) / 2;

        // צייר לוח על רקע
        board_img->draw_on(*background_img, offset_x, offset_y);

        // צור לוח
        Board board(87, 87, 8, 8, background_img);
        board.offset_x = offset_x;
        board.offset_y = offset_y;

        PieceFactory factory(board, pieces_folder_path);

        // בדוק אם קובץ pieces.json קיים
        std::string pieces_file = pieces_folder_path + "/pieces.json";
        std::vector<std::shared_ptr<Piece>> pieces;
        
        if (std::filesystem::exists(pieces_file)) {
            Game temp_game({}, board);
            pieces = temp_game.load_pieces_from_file(pieces_file, factory);
        }

        return Game(pieces, board);
        
    } catch (const std::exception& e) {
        std::cerr << "Error creating game: " << e.what() << std::endl;
        
        // משחק ברירת מחדל
        auto default_img = std::make_shared<Img>();
        default_img->create_blank(800, 800, true);
        Board default_board(87, 87, 8, 8, default_img);
        return Game({}, default_board);
    }
}


// Game create_game(const std::string& board_img_path, const std::string& pieces_folder_path) {
//     using namespace std;
//     using namespace std::filesystem;

//     // טען את תמונת הרקע מהנתיב הקבוע
//     auto background_img = std::make_shared<Img>();
//     background_img->read("C:\\CTD25\\cpp\\background.jpg", {1200, 800}, true); // שומר פרופורציה
//     std::cout << "Background image loaded from: C:\\CTD25\\cpp\\background.jpg" << std::endl;
//     // קריאת תמונת הלוח
//     auto board_img = std::make_shared<Img>();
//     board_img->read(board_img_path, {800, 800}, true);  // שומר פרופורציה ושקיפות
//     std::cout << "Board image loaded from: " << board_img_path << std::endl;
//     // חישוב האופסטים למרכז הלוח על הרקע
//     // int offset_x = (background_img->width() - board_img->width()) / 2;
//     // int offset_y = (board_img->height() - background_img->height()) / 2;
//     int offset_x = std::max(0, (background_img->width() - board_img->width()) / 2);
// int offset_y = std::max(0, (background_img->height() - board_img->height()) / 2);

// std::cout << "Background size: " << background_img->width() << "x" << background_img->height() << std::endl;
// std::cout << "Board size: " << board_img->width() << "x" << board_img->height() << std::endl;

//     std::cout << "Offset for board image: (" << offset_x << ", " << offset_y << ")" << std::endl;
//     // ציור הלוח על גבי הרקע
//     board_img->draw_on(*background_img, offset_x, offset_y);
//     std::cout << "Board image drawn on background." << std::endl;
//     // יוצרים לוח עם התמונה החדשה (הרקע עם הלוח במרכז)
//     Board board(100, 100, 8, 8, background_img);
//     std::cout << "Board created with dimensions: " << board.img->width() << "x" << board.img->height() << std::endl;
//     // מפעל חתיכות
//     PieceFactory factory(board, pieces_folder_path);
//     std::cout << "Piece factory created with pieces folder: " << pieces_folder_path << std::endl;
//     // יוצרים את המשחק
//     Game game({}, board);
//     std::cout << "Game created with empty pieces." << std::endl;
//     // טוענים את החיילים
//     auto pieces = game.load_pieces_from_file(pieces_folder_path + "/pieces.json", factory);
//     std::cout << "Pieces loaded from: " << pieces_folder_path << "/pieces.json" << std::endl;
//     for (const auto& piece : pieces) {
//         board.place_piece(piece);
//     }
//     std::cout << "Pieces placed on the board." << std::endl;
//     return Game(pieces, board);
// }


// Game create_game(const std::string& board_img_path, const std::string& pieces_folder_path) {
//     using namespace std;
//     using namespace std::filesystem;

//     // יצירת תמונת לוח
//     auto board_img = std::make_shared<Img>();
//     board_img->read(board_img_path, {800, 800});

//     // יצירת אובייקט Board
//     Board board(100, 100, 8, 8, board_img);

//     // יצירת מפעל חתיכות
//     PieceFactory factory(board, pieces_folder_path);

//     // יצירת המשחק עם לוח ריק
//     Game game({}, board);

//     // טעינת החיילים מקובץ JSON
//     std::vector<std::shared_ptr<Piece>> pieces = game.load_pieces_from_file(pieces_folder_path + "/pieces.json", factory);

//     for (const auto& piece : pieces) {
//         board.place_piece(piece);
//     }

//     return Game(pieces, board);
// }
