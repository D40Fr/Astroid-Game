#include "Player.h"

// Constructor to initialize the player's spacecraft, position, and ammo
Player::Player(const vector<vector<bool>> &shape, int row, int col, const string &player_name, int max_ammo, int lives)
        : spacecraft_shape(shape), position_row(row), position_col(col), player_name(player_name), max_ammo(max_ammo),
          current_ammo(max_ammo), lives(lives) {
    previous_position_row = position_row;
    previous_position_col = position_col;
}

// Move player left within the grid boundaries
void Player::move_left() {
    if (position_col > 0) {
        previous_position_col = position_col;
        previous_position_row = position_row;
        position_col--;
    }
}

// Move player right within the grid boundaries
void Player::move_right(int grid_width) {
    if (position_col + spacecraft_shape[0].size() < grid_width) {
        previous_position_col = position_col;
        previous_position_row = position_row;
        position_col++;
    }
}

// Move player up within the grid boundaries
void Player::move_up() {
    if (position_row > 0) {
        previous_position_row = position_row;
        previous_position_col = position_col;
        position_row--;
    }
}

// Move player down within the grid boundaries
void Player::move_down(int grid_height) {
    if (position_row + spacecraft_shape.size() < grid_height) {
        previous_position_row = position_row;
        previous_position_col = position_col;
        position_row++;
    }
}