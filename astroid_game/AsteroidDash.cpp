#include "AsteroidDash.h"
#include <fstream>
#include <sstream>

// Constructor to initialize AsteroidDash with the given parameters
AsteroidDash::AsteroidDash(const string &space_grid_file_name,
                           const string &celestial_objects_file_name,
                           const string &leaderboard_file_name,
                           const string &player_file_name,
                           const string &player_name)

        : leaderboard_file_name(leaderboard_file_name), leaderboard(Leaderboard()) {

    read_player(player_file_name, player_name);  // Initialize player using the player.dat file
    read_space_grid(space_grid_file_name);  // Initialize the grid after the player is loaded
    read_celestial_objects(celestial_objects_file_name);  // Load celestial objects
    leaderboard.read_from_file(leaderboard_file_name);
}

// Function to read the space grid from a file
void AsteroidDash::read_space_grid(const string &input_file) {
    ifstream f(input_file);

    if (!f.is_open()) {
        cerr << "Error: Could not open grid file " << input_file << endl;
        return;
    }
    
    string line;
    while (getline(f, line)) {
        vector<int> row;
        for (char c : line) {
            if (c == '0') {
                row.push_back(0);
            } else if (c == '1') {
                row.push_back(1);
            }
        }
        if (!row.empty()) {
            space_grid.push_back(row);
        }
    }
    f.close();
}

// Function to read the player from a file
void AsteroidDash::read_player(const string &player_file_name, const string &player_name) {
    ifstream f(player_file_name);
    
    if (!f.is_open()) {
        cerr << "Error: Could not open player file" << player_file_name << endl;
        return;
    }

    bool line1 = true;
    int start_row = 0, start_col = 0;
    vector<vector<bool>> shape;
    string line;

    while (getline(f, line)) {
        istringstream line_stream(line);
        
        if (line1) {
            line_stream >> start_row >> start_col;
            line1 = false;
            continue;
        }
        
        vector<bool> row;
        char c;
        while (line_stream >> c) {
            row.push_back(c != '0');
        }
        shape.push_back(row);
    }
    f.close();
    
    player = new Player(shape, start_row, start_col, player_name);
}

void create_rotations(CelestialObject* object) {
    object->right_rotation = object;
    object->left_rotation = object;

    vector<vector<bool>> curr_shape = object->shape;
    vector<vector<vector<bool>>> different_rotations;
    different_rotations.push_back(curr_shape);

    CelestialObject* last_rotation = object;  // Keep track of the last unique rotation

    for (int i = 0; i < 3; i++) {
        bool different = true;
        vector<vector<bool>> rotated_shape(curr_shape[0].size(), vector<bool>(curr_shape.size()));
        
        for (int i = 0; i < curr_shape.size(); i++) {
            for (int j = 0; j < curr_shape[0].size(); j++) {
                rotated_shape[j][curr_shape.size()-1-i] = curr_shape[i][j]; // rotate
            }
        }
        
        // Check if this rotation is unique
        for (int k = 0; k < different_rotations.size(); k++) {
            if (rotated_shape == different_rotations[k]) {
            different = false;
            break;
            }
        }

        if (different) {
            CelestialObject* new_rotation = new CelestialObject(object);
            new_rotation->shape = rotated_shape;
            different_rotations.push_back(rotated_shape);

            // Link the new rotation into the circular list
            new_rotation->left_rotation = last_rotation;
            new_rotation->right_rotation = object;
            object->left_rotation = new_rotation;
            last_rotation->right_rotation = new_rotation;
            
            last_rotation = new_rotation;
        }
        curr_shape = rotated_shape;
    }
}

void link_objects(CelestialObject*& curr, CelestialObject*& celestial_objects_list_head, const vector<vector<bool>>& shape, ObjectType type, int start_row, int time_of_appearance) {
    CelestialObject* new_object = new CelestialObject(shape, type, start_row, time_of_appearance);

    if (curr == nullptr) {
        celestial_objects_list_head = new_object;
    } else {
        curr->next_celestial_object = new_object;
        create_rotations(curr);
    }
    curr = new_object;
}

// Function to read celestial objects from a file
void AsteroidDash::read_celestial_objects(const string &input_file) {
    ifstream f(input_file);
    if (!f.is_open()) {
        cerr << "Error: Cant not open calestial_object file" << endl;
        return;
    }

    string line;
    CelestialObject* curr_object = nullptr;
    vector<vector<bool>> shape = {};
    int start_row = 0;
    int time_of_appearance = 0;
    ObjectType type = ASTEROID;
    
    while (getline(f, line)) {
        if (!line.empty()){
            if (line[0] == '[' || line[0] == '{' || line[0] == '1' || line[0] == '0') {
                // Start of new object
                if (line[0] == '[') {
                    type = ASTEROID;
                }
                else if (line[0] == '{') {
                    type = LIFE_UP; // or ammo not decided yet
                }

                // Reading shape matrix
                vector<bool> row;
                for (char c : line) {
                    if (c == '0' || c == '1') {
                        if (c == '1') {
                            row.push_back(true);
                        }
                        else {
                            row.push_back(false);
                        }
                    }
                    else if (c == ']' || c == '}') {
                        break;
                    }
                }
                if (!row.empty()) {
                    shape.push_back(row);
                }
                continue;
            }

            if (line[0] == 's') {
                string number_part;
                for (int i = 2; i < line.size(); i++) {
                    number_part += line[i];
                }
                start_row = stoi(number_part);
            }
            else if (line[0] == 't') {
                string number_part;
                for (int i = 2; i < line.size(); i++) {
                    number_part += line[i];
                }
                time_of_appearance = stoi(number_part);

                // Check if the next line is not 'e'
                if (f.peek() != 'e') {
                    link_objects(curr_object, celestial_objects_list_head, shape, type, start_row, time_of_appearance);
                    shape.clear();
                }
            }
            else if (line[0] == 'e') {
                string number_part;
                for (int i = 2; i < line.size(); i++) {
                    number_part += line[i];
                }
                string effect = number_part;
                type = (effect == "life") ? LIFE_UP : AMMO;
                link_objects(curr_object, celestial_objects_list_head, shape, type, start_row, time_of_appearance);
                shape.clear();
            }
        }
    }
    
    if (curr_object != nullptr) {
        create_rotations(curr_object);
    }
    f.close();
}



// Print the entire space grid
void AsteroidDash::print_space_grid() const {
    cout<<"Tick: "<<game_time<<endl;
    cout<<"Lives: "<<player->lives<<endl;
    cout<<"Ammo: "<<player->current_ammo<<endl;
    cout<<"Score: "<<current_score<<endl;
    if (leaderboard.highest_score == 0 || current_score > leaderboard.highest_score) {
        cout<<"High Score: "<<current_score<<endl;
    } else {
        cout<<"High Score: "<<leaderboard.highest_score<<endl;
    }
    if (game_over) {
        cout<<"Player: "<<player->player_name<<endl;
    }
    for(int i = 0; i < space_grid.size(); i++) {
        for(int j = 0; j < space_grid[i].size(); j++) {
            if (space_grid[i][j] == 0) {
                cout << unoccupiedCellChar;
            } else {
                cout << occupiedCellChar;
            }
        }
        cout << endl;
    }
    cout << endl;
}

void AsteroidDash::projectile_update() {
    for (int i = 0; i < projectiles.size();) {
        // set all projectiles in grid 0
        space_grid[projectiles[i].first][projectiles[i].second] = 0;
        
        // if projectiles out of grid remove it from vector
        if (projectiles[i].second >= space_grid[0].size()-1) {
            projectiles.erase(projectiles.begin() + i);
        } else { // shift right
            space_grid[projectiles[i].first][++projectiles[i].second] = 1;
            i++;
        }
    }
}

void AsteroidDash::player_update() {
    // Clear previous position
    int prev_row = player->previous_position_row;
    int prev_col = player->previous_position_col;

    for (int i = 0; i < player->spacecraft_shape.size(); i++) {
        for (int j = 0; j < player->spacecraft_shape[i].size(); j++) {
            if (player->spacecraft_shape[i][j]) {
                space_grid[prev_row + i][prev_col + j] = 0;
            }
        }
    }
    // Set new position
    for (int i = 0; i < player->spacecraft_shape.size(); i++) {
        for (int j = 0; j < player->spacecraft_shape[i].size(); j++) {
            if (player->spacecraft_shape[i][j] && !game_over) {
                space_grid[player->position_row + i][player->position_col + j] = 1;
            }
        }
    }
}

void AsteroidDash::celestial_object_update() {
    CelestialObject* prev = nullptr;
    CelestialObject* curr = celestial_objects_list_head;

    while (curr != nullptr) {

        if (game_time >= curr->time_of_appearance) {
            
            // Clear last position
            if (curr->current_col >= 0 || (curr->current_col < 0 && curr->current_col > 0 - curr->shape[0].size())) {
                grid_clear_or_set(curr, 0);
            }

            // Enter the grid or shigft left
            if (curr->current_col == -999) {
                curr->current_col = space_grid[0].size() - 1;
            } else {
                curr->current_col--;
            }

            // Collusion check
            bool collision = check_collision(curr, player);
            if (collision) {
                if (curr->object_type == AMMO) {
                    player->current_ammo = player->max_ammo;
                } else if (curr->object_type == LIFE_UP) {
                    player->lives++;
                } else {
                    player->lives--;
                    if (player->lives == 0) {
                        game_over = true;
                        LeaderboardEntry* new_entry = new LeaderboardEntry(current_score, time(nullptr), player->player_name);
                        leaderboard.insert(new_entry);
                        leaderboard.write_to_file(leaderboard_file_name);
                    }
                    current_score--;
                }
                // Remove collided object
                if (prev == nullptr) {
                    celestial_objects_list_head = curr->next_celestial_object;
                } else {
                    prev->next_celestial_object = curr->next_celestial_object;
                }
                CelestialObject* temp = curr;
                curr = curr->next_celestial_object;
                CelestialObject::delete_rotations(temp);
                delete temp;
                continue;
            }
            current_score++;
            

            // Draw new grid
            if (curr->current_col >= 0 || (curr->current_col < 0 && curr->current_col > 0 - curr->shape[0].size())) {
                grid_clear_or_set(curr, 1);
            } else if (curr->current_col == 0 - curr->shape[0].size()) { // Object leaves the grid completely i have to remove it from list
                if (prev == nullptr) {
                    celestial_objects_list_head = curr->next_celestial_object;
                } else {
                    prev->next_celestial_object = curr->next_celestial_object;
                }
                CelestialObject* temp = curr;
                curr = curr->next_celestial_object;
                CelestialObject::delete_rotations(temp);
                delete temp;
                continue;
            }
        }
        prev = curr;
        curr = curr->next_celestial_object;
    }
}

void AsteroidDash::grid_clear_or_set(const CelestialObject* curr, int number) {
    for (int i = 0; i < curr->shape.size(); i++) {
        for (int j = 0; j < curr->shape[i].size(); j++) {
            if (curr->shape[i][j] == 1) {
                int row = curr->starting_row + i;
                int col = curr->current_col + j;
                if (row >= 0 && row < space_grid.size() && col >= 0 && col < space_grid[0].size()) {
                    space_grid[row][col] = number;
                }
            }
        }
    }
}
// Function to update the space grid with player, celestial objects, and any other changes
// It is called in every game tick before moving on to the next tick.
void AsteroidDash::update_space_grid() {
    projectile_update();
    celestial_object_update();
    player_update();
}

bool AsteroidDash::check_collision(CelestialObject* celestial_object, Player* player) {
    // Iterate through each point in the celestial object's shape
    for (int i = 0; i < celestial_object->shape.size(); i++) {
        for (int j = 0; j < celestial_object->shape[i].size(); j++) {
            if (celestial_object->shape[i][j]) {
                int grid_row = celestial_object->starting_row + i;
                int grid_col = celestial_object->current_col + j;

                // Check if this part of the celestial object overlaps with the player
                if (grid_row >= player->position_row && 
                    grid_row < player->position_row + player->spacecraft_shape.size() &&
                    grid_col >= player->position_col && 
                    grid_col < player->position_col + player->spacecraft_shape[0].size()) {

                    // Check the specific point in the player's shape for collision
                    if (player->spacecraft_shape[grid_row - player->position_row][grid_col - player->position_col]) {
                        return true;  // Collision detected
                    }
                }
            }
        }
    }
    return false;  // No collision detected
}

// Corresponds to the SHOOT command.
// It should shoot if the player has enough ammo.
// It should decrease the player's ammo
void AsteroidDash::shoot() {
    if (player->current_ammo > 0) {
        int center_row = player->position_row + player->spacecraft_shape.size() / 2;
        int bullet_col = player->position_col + player->spacecraft_shape[0].size() - 1;
        
        if (bullet_col < space_grid[0].size()) {
            projectiles.push_back({center_row, bullet_col});    
        }
        player->current_ammo--;
    }
}

// Destructor. Remove dynamically allocated member variables here.
AsteroidDash::~AsteroidDash() {

    delete player;

    while (celestial_objects_list_head != nullptr) {
        CelestialObject* temp = celestial_objects_list_head;
        celestial_objects_list_head = celestial_objects_list_head->next_celestial_object;
        CelestialObject::delete_rotations(temp);
        delete temp;
    }
}