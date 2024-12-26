#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#ifdef _WIN32
#include <windows.h>
#define CLEAR_SCREEN system("cls");
#else
#include <unistd.h>
#define CLEAR_SCREEN system("clear");
#endif

#define ACTIVE_CELL_CHAR '#'
#define DEAD_CELL_CHAR ' '
#define END_OF_FIELD_CHAR '|'

#define SLEEP_TIME_MS 40
#define DEFAULT_FIELD_SIZE 30,60


// Cross-platform sleep function
void sleepcp(int milliseconds)
{
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 10000);
#endif
}

// Returns first word in string
std::string getword(std::string line)
{
    int pos = line.find(" ");
    if (pos >= line.size())
        return line;
    return line.substr(0, line.find(" "));
}

/// <summary>
///  Field class, contains information about cells.
///  Supports get, set by coords(x,y) and draw field in console
/// </summary>
class Field 
{
private:
    bool** _field;
    int n, m;

    // Initialize field NxM
    void createField(int _n, int _m)
    {
        this->n = _n;
        this->m = _m;
        _field = new bool* [_n];
        for (int i = 0; i < _n; i++)
            _field[i] = new bool[_m];
    }

public:
#pragma region Constructors

    // Default field
    Field()
    {
        createField(10, 10);
    }
    // Field with size = NxM
    Field(int _n, int _m)
    {
        createField(_n, _m);
    }

#pragma endregion

    void DefaultPreset()
    {
        Clear();

        setAt(3, 2, true);
        setAt(4, 3, true);
        setAt(2, 4, true);
        setAt(3, 4, true);
        setAt(4, 4, true);
    }

    void Clear()
    {
        for (int i = 0; i < this->n; i++)
            for (int j = 0; j < this->m; j++)
                _field[i][j] = false;
    }

    void Draw()
    {
        for (int i = 0; i < this->n; i++)
        {
            for (int j = 0; j < this->m; j++)
                std::cout << (getAt(i,j) ? ACTIVE_CELL_CHAR : DEAD_CELL_CHAR);
            std::cout << END_OF_FIELD_CHAR << std::endl;
        }

    }

#pragma region IndexationUtils

    // Block of utility functions for
    // array indexation

    // Normalization provides closure of field
    // e.g. (x = -1) => (x = N)
    //      (x =  N) => (x = 0)
    // Same for y

    int getN()        { return this->n; }
    int getM()        { return this->m; }
    int normalizeX(int x, bool is_y = false) 
    {
        int border = this->n;
        if (is_y) border = this->m;

        int res = x - border * (int)(x / border);
        if (res < 0) res += border;
        return res;
    }
    int normalizeY(int y) { return normalizeX(y, true); }
    bool getAt(int x, int y) { return this->_field[normalizeX(x)][normalizeY(y)]; }
    void setAt(int x, int y, bool val) { this->_field[normalizeX(x)][normalizeY(y)] = val; }
    //bool** getField() { return _field; }

#pragma endregion
};

// Struct that represents cell operation
// Means: "set <val> at <x,y>"
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

private:
    std::string _inputFile;

    std::string _presetName = std::string("Default loaded preset");
    std::string _presetComment = std::string("...");
    int b = 3, s = 23;
    bool parsed = false;

    // Choosing a parser for parameter string
    // marked with # at the beginning of the line
    // Returns true if parameter is recognized and parsed
    // false - otherwise
    bool parseParameter(std::string line)
    {
        switch (line[1])
        {
        case 'R':
            return parseR(line);
            break;
        case 'N':
            return parseN(line);
            break;
        }
        return false;
    }

    // Parser for R parameter
    bool parseR(std::string line)
    {
#ifdef _WIN32
        int ret = sscanf_s(line.c_str(), "#R B%d/S%d", &b, &s);
#else
        int ret = sscanf(line.c_str(), "#R B%d/S%d", &b, &s);
#endif
        return ret == 2;
    }
    // Parser for N parameter
    bool parseN(std::string line)
    {
        _presetComment = line.substr(3);
        return _presetComment.size() > 0;
    }
    // Parser for active cell
    bool parseCell(std::string line, std::queue<CellOp*>* ops_q)
    {
        int x, y;
        std::stringstream  linestream(line);
        linestream >> x >> y;
        if (!linestream) return false;
        CellOp* op = new CellOp;
        op->x = x;
        op->y = y;
        op->val = true;
        ops_q->push(op);
        return true;
    }

public:
    PresetParser(char* str)       { _inputFile = std::string(str); }
    PresetParser(std::string str) { _inputFile = str;              }
    void SetFile(char* str)       { _inputFile = std::string(str); }
    void SetFile(std::string str) { _inputFile = str;              }

    // Start parsing file
    // Filling queue with operations, allowing Logic class
    // to restore world from preset in file
    // Also sets preset paremeters such as name, comment and B/S
    // Note that this parameters are stored inside this class
    // and needs to be read separately from queue
    void Parse(std::queue<CellOp*>* ops_q)
    {
        std::ifstream infile(_inputFile);
        if (!infile) throw std::invalid_argument("Input file not found");
        std::string line;
        int count = 0;

        // First line is a name
        std::getline(infile, _presetName);

        while (std::getline(infile, line)) {
            count++;
            bool result;
            if (line[0] == '#') result = parseParameter(line);
            else                result = parseCell(line, ops_q);
            if (!result) std::cout << "[Line " << count << "] " 
                                   << "Failed to parse: " << line << std::endl;
        }

        parsed = true;
    }
    void Dump(std::queue<CellOp*>* ops_q, std::string output)
    {
        std::ofstream fout;
        fout.open(output);
        fout << _presetName << std::endl;
        fout << "#N " << _presetComment << std::endl;
        fout << "#R B" << b << "/S" << s << std::endl;
        for (; !ops_q->empty(); ops_q->pop())
        {
            CellOp* op = ops_q->front();
            fout << op->x << " " << op->y << std::endl;
            delete op;
        }
    }

    int GetB() { return b; }
    int GetS() { return s; }

    std::string GetComment() { return _presetComment; }
    std::string GetName()    { return _presetName;    }
};

/// <summary>
/// Performs all operations over cells and updates state
/// of the game by Tick()
/// </summary>
class Logic 
{
private:
    Field* _field_ptr;
    std::queue<CellOp*> ops_q;
    int b, s;
    std::vector<std::string> load_messages;

    // Sum of active neighbours for cell at (x,y)
    int activeCellSum(int x, int y)
    {
        int sum = 0;
        for (int i = x - 1; i <= x + 1; i++)
        {
            for (int j = y - 1; j <= y + 1; j++)
            {
                if (i == x && j == y) continue;
                sum += (_field_ptr->getAt(i, j)) ? 1 : 0;
            }
        }
        return sum;
    }
    // Check sum of neighbour active cells for birth
    // If s == true, it checks S (survival) instead
    // For S better use checkS(sum)
    bool checkB(int sum, bool check_s = false)
    {
        int _b = b;
        if (check_s) _b = s;

        while (_b > 0)
        {
            if (_b % 10 == sum)
                return true;
            _b /= 10;
        }

        return false;
    }
    // Check sum of neighbour active cells for survival
    bool checkS(int sum)
    {
        return checkB(sum, true);
    }
    // Push new cell operation into queue
    void pushOp(int x, int y, bool val)
    {
        CellOp* op = new CellOp;
        op->x = x;
        op->y = y;
        op->val = val;
        ops_q.push(op);
    }
    // Scans entire field and build operation queue
    // that can transform field to next iteration
    void scanField()
    {
        for (int i = 0; i < _field_ptr->getN(); i++)
        {
            for (int j = 0; j < _field_ptr->getM(); j++)
            {
                int cellSum  = activeCellSum(i,j);
                bool alive   = _field_ptr->getAt(i, j);
                bool birth   = checkB(cellSum);
                bool survive = checkS(cellSum);

                bool newState = (alive && survive) || birth;
                if (newState != alive) pushOp(i, j, newState);
            }
        }
    }
    // Applies cell operations listed in ops_q
    // and updates field
    void applyCellOpsQueue()
    {
        for (; !ops_q.empty() > 0; ops_q.pop())
        {
            CellOp* op = ops_q.front();
            if (_field_ptr->getAt(op->x, op->y) == op->val)
            {
                std::ostringstream oss;
                oss << "[Note] Overlaping coordinats on input: (" << op->x << ", " << op->y << ")";
                load_messages.push_back(oss.str());
            }
            _field_ptr->setAt(op->x, op->y, op->val);
            delete op;
        }
    }

    // For debug. Prints map of active neighbours
    void printCellSums()
    {
        for (int i = 0; i < _field_ptr->getN(); i++)
        {
            for (int j = 0; j < _field_ptr->getM(); j++)
                std::cout << activeCellSum(i, j);
            std::cout << std::endl;
        }
    }

public:
    // Default logic B3/S23
    Logic(Field* field)
    {
        b = 3;
        s = 23;
        _field_ptr = field;
    }
    // Logic with Bb/Ss parameters
    // B and S are represented as integer
    // e.g. B123 => b = 123
    //      B45  => b = 45
    //      S140 => s = 140
    // WARNING: 0 in B/S parameters must NOT be first digit
    // Multiple same digits are counted as one: 1223 = 123
    Logic(Field* field, int b, int s)
    {
        this->b = b;
        this->s = s;
        _field_ptr = field;
    }
    void SetField(Field* field)
    {
        _field_ptr = field;
    }
    Field* GetField()
    {
        return _field_ptr;
    }
    void Tick()
    {
        scanField();
        applyCellOpsQueue();
    }
    void DrawField() { _field_ptr->Draw(); }
    int GetFieldHeight() { return _field_ptr->getN(); }
    int GetFieldWidth() { return _field_ptr->getM(); }

    void LoadPreset(PresetParser* prepar)
    {
        _field_ptr->Clear();
        prepar->Parse(&ops_q);
        this->b = prepar->GetB();
        this->s = prepar->GetS();
        applyCellOpsQueue();
    }
    void LoadDefault()
    {
        _field_ptr->Clear();
        _field_ptr->DefaultPreset();
    }

    void FillQueueWithCurrentState(std::queue<CellOp*>* ops_q)
    {
        for (int i = 0; i < _field_ptr->getN(); i++)
        {
            for (int j = 0; j < _field_ptr->getM(); j++)
            {
                if (_field_ptr->getAt(i, j))
                    ops_q->push(new CellOp{ i, j, true });
            }
        }
    }

    void PrintMessages()
    {
        for (auto iter = load_messages.begin(); iter != load_messages.end(); iter++)
        {
            std::cout << *iter << std::endl;
        }

        load_messages.clear();
    }
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
private:

public:
    virtual void ConfigLogic(ModeContext context)
    {
        Field* f = new Field(DEFAULT_FIELD_SIZE);
        Logic* l = new Logic(f, 3, 23);
        l->LoadDefault();
        *(context.logic) = l;
        *(context.prepar) = new PresetParser("");
    }
};

// Mode with loading map preset from input file
class LoadFileMode : public ModeSelector
{
private:

public:
    virtual void ConfigLogic(ModeContext context)
    {
        Field* f = new Field(DEFAULT_FIELD_SIZE);
        Logic* l = new Logic(f, 3, 23);
        PresetParser* p = new PresetParser(context.inputFile);
        l->LoadPreset(p);
        *(context.logic) = l;
        *(context.prepar) = p;
    }
};

// Offline mode. Evaluates map state and dumps result into other file
class OfflineMode : public ModeSelector
{
public:
    virtual void ConfigLogic(ModeContext context)
    {
        std::cout << "Evaluating state..." << std::endl;


        Field* f = new Field(DEFAULT_FIELD_SIZE);
        Logic* l = new Logic(f, 3, 23);
        PresetParser* p = new PresetParser(context.inputFile);
        l->LoadPreset(p);

        for (int i = 0; i < context.offline_ticks; i++)
            l->Tick();
        std::queue<CellOp*> q;
        l->FillQueueWithCurrentState(&q);
        p->Dump(&q, context.outputFile);

        std::cout << "Simulation completed" << std::endl;
        std::cout << "Created file: " << context.outputFile << std::endl;
        exit(0);
    }
};

class UserInterfaceWrap
{
private:
    Logic* _logic;
    PresetParser* _prepar;
    int _ticks = 0;
    bool _help = false;
    std::string _dump_file = std::string("");

    // Error indicator for commands
    // 0 - no errors
    // 1 - tick error
    // 2 - dump error
    // 3 - help error
    // 4 - exit error
    int _errNo = 0;
    std::string _error_msg = std::string("");


    // Set game mode using ModeSelector and loading file by name
    void setMode(ModeSelector* mode, std::string inputFile, std::string outputFile = std::string(""), int offline_ticks = 0)
    {
        mode->ConfigLogic(ModeContext{ &_logic, &_prepar, inputFile, outputFile, offline_ticks});
    }
    // Draw map
    void drawField()
    {
        _logic->DrawField();
        for (int i = 0; i < _logic->GetFieldWidth(); i++)
            std::cout << "=";
        std::cout << END_OF_FIELD_CHAR << std::endl;
    }
    // Draw info box
    void drawInfo()
    {
        std::cout << "[INFO]----------------------------" << std::endl;
        if (_prepar == nullptr)
        {
            std::cout << "Default loaded preset" << std::endl;
        }
        else
        {
            std::cout << "Name: " << _prepar->GetName() << std::endl;
        }
        if (_errNo != 0)
        {
            std::cout << "[Error No. " << _errNo << "]:" << _error_msg << std::endl;
            _errNo = 0;
        }
    }
    // Draw box with user input and help
    void drawUserInput(bool help = false)
    {
        std::cout << "[INPUT]---------------------------" << std::endl;
        if (help)
        {
            std::cout << "Type \"tick\" <n> to advance game on n ticks." << std::endl;
            std::cout << "Type \"dump\" <file> to save state in file." << std::endl;
            std::cout << "Type \"exit\" to end game." << std::endl;
        }
        else std::cout << "Type \"help\" to view commands." << std::endl;
        std::cout << "Input command: ";
    }

    void dumpFile()
    {
        std::queue<CellOp*> ops_q;
        _logic->FillQueueWithCurrentState(&ops_q);
        _prepar->Dump(&ops_q, _dump_file);
        std::cout << "Dump file created: " << _dump_file << std::endl;
        _dump_file = std::string("");
    }

    void ticks()
    {
        while (_ticks > 0)
        {
            _logic->Tick();
            CLEAR_SCREEN
            drawField();
            std::cout << "Remained ticks = " << --_ticks << std::endl;
            sleepcp(SLEEP_TIME_MS);
        }
    }

    void parseUInput(std::string line)
    {
        std::string word = getword(line);
        if (word == std::string("dump"))
        {
            if (line.size() > 5)
            {
                std::string arg = line.substr(5);
                _dump_file = arg;
            }
            else
            {
                _errNo = 2;
                _error_msg = std::string("Command \"dump\" requires argument");
            }
                
        }
        else if (word == std::string("tick"))
        {
            if (line.size() > 4)
            {
                std::string arg = line.substr(5);
                try
                {
                    _ticks = std::stoi(arg);
                }
                catch (const std::exception&)
                {
                    _errNo = 1;
                    _error_msg = std::string("Invalid argument for \"tick\": ") + std::string(arg);
                    _ticks = 0;
                }
            }
            else
            {
                _ticks = 1;
            }
        }
        else if (word == std::string("exit"))
        {
            if (line.size() > 4)
            {
                _errNo = 4;
                _error_msg = std::string("Command \"exit\" takes no arguments");
            }
            else
            {
                std::cout << "Closing game." << std::endl;
                exit(0);
            }
        }
        else if (word == std::string("help"))
        {
            _help = true;
            if (line.size() > 4)
            {
                _errNo = 3;
                _error_msg = std::string("Command \"help\" takes no arguments");
            }
        }
    }

public:
    UserInterfaceWrap(ModeSelector* mode, std::string inputFile, std::string outputFile = std::string(""), int offlineTicks = 0)
    {
        std::cout << "Loading..." << std::endl;
        setMode(mode, inputFile, outputFile, offlineTicks);
        std::cout << "Complete." << std::endl;
    }

    void Start()
    {
        while (true)
        {
            std::string input_line;

            ticks();
            CLEAR_SCREEN
            drawField();
            drawInfo();
            if (!_dump_file.empty())
                dumpFile();
            _logic->PrintMessages();
            drawUserInput(_help);
            _help = false;

            std::getline(std::cin, input_line);
            parseUInput(input_line);
        }
        
    }

    void DrawAll()
    {
        drawField();
        drawInfo();
        drawUserInput();
    }

};

#pragma region StolenParser

char* getCmdOption(char** begin, char** end, const std::string& option)
{
    char** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

#pragma endregion

int Play(int argc, char* argv[])
{
    ModeSelector* mode = NULL;
    std::string file = std::string("");
    std::string out_file = std::string("");
    int offl_iterations = 0;
    char* res;
    if (argc == 1)
        mode = new DefaultMode();
    else if (argc == 2)
    {
        mode = new LoadFileMode();
        file = std::string(argv[1]);
    }
    else
    {
        if (cmdOptionExists(argv, argv + argc, "-o")
            && cmdOptionExists(argv, argv + argc, "-i"))
        {
            mode = new OfflineMode();
            file = std::string(argv[1]);

            res = getCmdOption(argv, argv + argc, "-o");
           
            if (res == NULL || std::string("-i") == std::string(res))
            {
                std::cout << "Incorrect usage." << std::endl;
                std::cout << "Specify output file (-o <filename>)" << std::endl;
                exit(1);
            }
            out_file = std::string(res);

            {
                res = getCmdOption(argv, argv + argc, "-i");
                if (res == NULL)
                {
                    std::cout << "Incorrect usage." << std::endl;
                    std::cout << "Specify iterations number (-i <n>)" << std::endl;
                    exit(1);
                }
            }
            std::string iters = std::string(res);

            offl_iterations = std::stoi(iters);
        }
        else
        {
            std::cout << "Incorrect usage." << std::endl;
            std::cout << "Default mode: no arguments" << std::endl;
            std::cout << "Load file mode: <filename>" << std::endl;
            std::cout << "Offline mode: <filename> -o <outputfile> -i <number>" << std::endl;
            return 1;
        }
    }
    if (mode == NULL) mode = new DefaultMode();
    UserInterfaceWrap* UI = new UserInterfaceWrap(mode, file, out_file, offl_iterations);
    UI->Start();
    return 0;
}
