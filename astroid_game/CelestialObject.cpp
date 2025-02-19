#include "CelestialObject.h"
#include <iostream>
using namespace std;
// Constructor to initialize CelestialObject with essential properties
CelestialObject::CelestialObject(const vector<vector<bool>> &shape, ObjectType type, int start_row,
                                 int time_of_appearance)
        : shape(shape), object_type(type), starting_row(start_row), time_of_appearance(time_of_appearance) {

}


// Copy constructor for CelestialObject
CelestialObject::CelestialObject(const CelestialObject *other)
        : shape(other->shape),  // Copy the 2D vector shape
          object_type(other->object_type),  // Copy the object type
          starting_row(other->starting_row),  // Copy the starting row
          time_of_appearance(other->time_of_appearance)  // Copy the time of appearance
{
    right_rotation = this;
    left_rotation = this;
    next_celestial_object = other->next_celestial_object;
}

// Function to delete rotations of a given celestial object. It should free the dynamically allocated
// memory for each rotation.
void CelestialObject::delete_rotations(CelestialObject *target) {
    if (target == nullptr) {
        return;
    }

    CelestialObject* curr = target->right_rotation;
    while (curr != nullptr && curr != target) {
        CelestialObject* temp = curr;
        curr = curr->right_rotation;
        delete temp;
    }
    // Since no rotation left they will point target
    target->right_rotation = target;
    target->left_rotation = target;
}
