#include "GameController.h"

// Simply instantiates the game
GameController::GameController(
        const string &space_grid_file_name,
        const string &celestial_objects_file_name,
        const string &leaderboard_file_name,
        const string &player_file_name,
        const string &player_name

) {
    game = new AsteroidDash(space_grid_file_name, celestial_objects_file_name, leaderboard_file_name, player_file_name,
                            player_name);
}

// Reads commands from the given input file, executes each command in a game tick
void GameController::play(const string &commands_file) {
    ifstream file(commands_file);
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            if (line == "MOVE_UP") {
                game->player->move_up();
            } else if (line == "MOVE_DOWN") {
                game->player->move_down(game->space_grid.size());
            } else if (line == "MOVE_RIGHT") {
                game->player->move_right(game->space_grid[0].size());
            } else if (line == "MOVE_LEFT") {
                game->player->move_left();
            } else if (line == "SHOOT") {
                game->shoot();
            }else if (line == "NOP") {
                // do nothing
            }        
            game->update_space_grid();
            if(game->game_over){
                cout<<"GAME OVER!"<<endl;
                game->print_space_grid();
                game->leaderboard.print_leaderboard();
                break;
            }
            if (line == "PRINT_GRID") {
                game->print_space_grid();
            }
            game->game_time++;
        }
        if(!game->game_over){
            game->game_over = true;
            game->game_time--;
            cout<<"GAME FINISHED! No more commands!"<<endl;
            game->print_space_grid();
            game->leaderboard.print_leaderboard();
        }

        
    }
}

// Destructor to delete dynamically allocated member variables here
GameController::~GameController() {
    delete game;
}
