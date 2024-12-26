#pragma once
#include <string>
#include <vector>
#include <queue>
#include <stdexcept>

std::string getword(std::string line);

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

typedef struct CellOp_s
{
    int x, y;
    bool val;
} CellOp;

/// <summary>
/// This class is used as a part of the logic
/// to read and parse files with presets
/// </summary>
class PresetParser
{
public:
    PresetParser(char* str);
    PresetParser(std::string str);
    void SetFile(char* str);
    void SetFile(std::string str);

    // Start parsing file
    // Filling queue with operations, allowing Logic class
    // to restore world from preset in file
    // Also sets preset paremeters such as name, comment and B/S
    // Note that this parameters are stored inside this class
    // and needs to be read separately from queue
    void Parse(std::queue<CellOp*>* ops_q);
    void Dump(std::queue<CellOp*>* ops_q, std::string output);

    int GetB();
    int GetS();

    std::string GetComment();
    std::string GetName();
};

/// <summary>
/// Performs all operations over cells and updates state
/// of the game by Tick()
/// </summary>
class Logic
{
public:
    // Default logic B3/S23
    Logic(Field* field);
    // Logic with Bb/Ss parameters
    // B and S are represented as integer
    // e.g. B123 => b = 123
    //      B45  => b = 45
    //      S140 => s = 140
    // WARNING: 0 in B/S parameters must NOT be first digit
    // Multiple same digits are counted as one: 1223 = 123
    Logic(Field* field, int b, int s);
    void SetField(Field* field);
    Field* GetField();
    void Tick();
    void DrawField();
    int GetFieldHeight();
    int GetFieldWidth();

    void LoadPreset(PresetParser* prepar);
    void LoadDefault();

    void FillQueueWithCurrentState(std::queue<CellOp*>* ops_q);

    void PrintMessages();

    bool GetAt(int x, int y);
};

// Struct for ModeSelector class
// Contains passed parameters from UserInterfaceWrap class
typedef struct ModeContext_s
{
    Logic** logic;
    PresetParser** prepar;
    std::string inputFile;
    std::string outputFile;
    int offline_ticks;
} ModeContext;

// Strategy abstract class for selecting different app mode
class ModeSelector
{
public:
    virtual void ConfigLogic(ModeContext context) = 0;
};

// Default mode, loading default map preset
// Ignores input file
// Sets default prepar
class DefaultMode : public ModeSelector
{
public:
    virtual void ConfigLogic(ModeContext context);
};

// Mode with loading map preset from input file
class LoadFileMode : public ModeSelector
{
public:
    virtual void ConfigLogic(ModeContext context);
};

// Offline mode. Evaluates map state and dumps result into other file
class OfflineMode : public ModeSelector
{
public:
    virtual void ConfigLogic(ModeContext context);
};

class UserInterfaceWrap
{
public:
    UserInterfaceWrap(ModeSelector* mode, 
                      std::string inputFile, 
                      std::string outputFile = std::string(""), 
                      int offlineTicks = 0);
    void Start();
    void DrawAll();
};



int Play(int argc, char* argv[]);
