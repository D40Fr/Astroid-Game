#include "Leaderboard.h"
#include <fstream>
#include <sstream>
#include <iostream>


// Read the stored leaderboard status from the given file such that the "head_leaderboard_entry" member
// variable will point to the highest all-times score, and all other scores will be reachable from it
// via the "next_leaderboard_entry" member variable pointer.
void Leaderboard::read_from_file(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()){
        //cerr << "There is not leaderboard file"<< endl;
        // No leader board file provided
        return;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()){
            istringstream line_stram(line);
            unsigned long score;
            time_t last_played;
            string player_name;
            line_stram >> score >> last_played >> player_name;
            LeaderboardEntry* new_entry = new LeaderboardEntry(score, last_played, player_name);
            insert(new_entry);
        }
    }
    
    file.close();
}

// Write the latest leaderboard status to the given file in the format specified in the PA instructions
void Leaderboard::write_to_file(const string &filename) {
    ofstream file(filename);
    if (!file.is_open()){
        cerr << "Error: Could not open file line40 leaderboard to write" << endl;
        return;
    }
    LeaderboardEntry* curr = head_leaderboard_entry;
    while (curr != nullptr) {
        file << curr->score << " "<< curr->last_played << " "<< curr->player_name << "\n";
        curr = curr->next;
    }
    file.close();
}

// Print the current leaderboard status to the standard output in the format specified in the PA instructions
void Leaderboard::print_leaderboard() {
    int position = 1;
    LeaderboardEntry* curr = head_leaderboard_entry;

    cout << "Leaderboard\n-----------\n";
    while (curr != nullptr && position <= MAX_LEADERBOARD_SIZE) {
        char timestamp[55];
        strftime(timestamp, sizeof(timestamp), "%H:%M:%S/%d.%m.%Y", localtime(&curr->last_played));
        
        cout << position++ <<". "<< curr->player_name <<" "<< curr->score <<" "<< timestamp <<"\n";
        curr = curr->next;
    }
}

//  Insert a new LeaderboardEntry instance into the leaderboard, such that the order of the high-scores
//  is maintained, and the leaderboard size does not exceed 10 entries at any given time (only the
//  top 10 all-time high-scores should be kept in descending order by the score).
void Leaderboard::insert(LeaderboardEntry *new_entry) {
    if (new_entry->score > highest_score) {
        highest_score = new_entry->score;
    }
    // Insert at beginnig
    if (head_leaderboard_entry == nullptr || new_entry->score > head_leaderboard_entry->score) {
        // Insert at beginning
        new_entry->next = head_leaderboard_entry;
        head_leaderboard_entry = new_entry;
    } else { // İnsert at middle of list
        LeaderboardEntry* curr = head_leaderboard_entry;
        while (curr->next != nullptr && curr->next->score >= new_entry->score) {
            curr = curr->next;
        }
        new_entry->next = curr->next;
        curr->next = new_entry;
    }
    
    // List size max 10
    int size = 1;
    LeaderboardEntry* curr = head_leaderboard_entry;
    while (curr != nullptr && curr->next != nullptr) {
        if (size == MAX_LEADERBOARD_SIZE) {
            LeaderboardEntry* extra = curr->next; // 10th node
            curr->next = nullptr;
            
            // Delete after 10th node
            while (extra != nullptr) {
                LeaderboardEntry* temp = extra;
                extra = extra->next;
                delete temp;
            }
            break;
        }
        size++;
        curr = curr->next;
    }
}

// Free dynamically allocated memory used for storing leaderboard entries
Leaderboard::~Leaderboard() {
    LeaderboardEntry *curr = head_leaderboard_entry;
    while (curr != nullptr) {
        LeaderboardEntry* temp = curr;
        curr = curr->next;
        delete temp; 
    }
}
