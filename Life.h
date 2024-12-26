#pragma once

class Field
{
public:
#pragma region Constructors

    // Default field
    Field();
    // Field with size = NxM
    Field(int n, int m);
#pragma endregion

    void DefaultPreset();
    void Clear();
    void Draw();

#pragma region IndexationUtils

    // Block of utility functions for
    // array indexation

    // Normalization provides closure of field
    // e.g. (x = -1) => (x = N)
    //      (x =  N) => (x = 0)
    // Same for y

    int getN();
    int getM();
    int normalizeX(int x, bool is_y = false);
    int normalizeY(int y);
    bool getAt(int x, int y);
    void setAt(int x, int y, bool val);

#pragma endregion
};



int Play(int argc, char* argv[]);
