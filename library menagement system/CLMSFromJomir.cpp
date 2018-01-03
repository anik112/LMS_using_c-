
/****************************************************************|
 *                                                              *
 *      @file       :   CLMS                                    *
 *      @author     :   Jomir Uddin                             *
 *      @version    :   3.0                                     *
 *      @language   :   C++                                     *
 *      @compiler   :   GNU GCC                                 *
 *                                                              *
 ****************************************************************
 *                                                              *
 *      @project    :   College Library Management System       *
 *      @institute  :   Generic Polytechnic Institute           *
 *      @department :   Computer Technology                     *
 *      @semester   :   Seven                                   *
 *      @members    :   Null                                    *
 *                                                              *
|****************************************************************/

//------------------------------- Preprocessor Directive -------------------------------//

// Header Files
#include <iostream>     // standard I/O stream
#include <fstream>      // file based I/O stream
#include <cstdio>       // standard I/O of C
#include <cstring>      // memory and string functions
#include <cstdlib>      // common types, variables, functions
#include <vector>       // c++ vector
#include <stdexcept>    // standard exception classes
#include <conio.h>      // console level I/O
#include <io.h>         // system level I/O
#include <windows.h>    // Win32 API

// Macro to define File Stream flag
#define Beg ios::beg
#define Cur ios::cur
#define End ios::end
#define In  ios::in  | ios::binary
#define Out ios::out | ios::binary
#define App ios::app | ios::binary
#define Ate ios::ate | ios::binary
#define Bit ios::binary
#define Rnd In|Out|Ate

// Macro to define data file name
#define AppDataFile     "Database\\AppData.db"
#define BookDataFile    "Database\\BookData.db"
#define IssueDataFile   "Database\\IssueData.db"

// Macro to define data file type
#define apData          AppData()
#define bkData          BookData()
#define stData          StudentData()

// Macro to draw main border
#define BORDER      Draw(4,105,5,2,_H);  \
                    Draw(5,105,5,4,_H);  \
                    Draw(2,105,5,20,_H); \
                    Draw(2,17,5,3,_V);   \
                    Draw(2,17,109,3,_V);

// Macro to flush the Stream Buffer (if any error occurs)
#define FlushBuffer     if(cin.fail()) { cin.clear(); cin.ignore(32767,'\n'); }

// Macro to prompt user for an action
#define Prompt(_x,_y,_str)  {                                                       \
                        setpos(_x,_y); cout<<_str;                                  \
                        setpos(_x+((strlen(_str)-8)/2)+1,_y+2); cout<<"\xB2 1. Yes";\
                        setpos(_x+((strlen(_str)-8)/2)+1,_y+4); cout<<"\xB2 2. No"; \
                    }

#define PromptEX(_x,_y,_str,_yes,_no)   {                   \
                                Prompt(_x,_y,_str);         \
                                while(true){                \
                                    switch(getch()-'0'){    \
                                        case 1: return _yes;\
                                        case 2: return _no; \
                                    }                       \
                                }                           \
                            }

// Object to manipulate console buffer
int setpos_gap = 1;
COORD _coord = {0, 0};
HANDLE _hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

// Macro to set cursor position
#define initpos(x,y,next)   { _coord.X = x, _coord.Y = y, setpos_gap = next; }
#define pos                 { SetConsoleCursorPosition(_hConsoleOutput, _coord); _coord.Y += setpos_gap; }
#define setpos(x,y)         { _coord.X = x, _coord.Y = y; SetConsoleCursorPosition(_hConsoleOutput, _coord); }

// Macro : Others
#define CLMS        main
#define ENTER       13
#define BACKSPACE   8
#define PassMax     8
#define ShowCursor  Cursor(1);  // show the cursor
#define HideCursor  Cursor(0);  // hide the cursor
#define Repeat(_n)  for(int _i=0; _i<_n; _i++)  // simplify the 'for statement'

//------------------------------- Global Variable Section -------------------------------//

// namespace directive
using namespace std;

// Anonymous Enumeration to define arguments of Draw Function
enum { _H, _V };  // H & V indicates horizontal and vertical respectively

// Anonymous Enumeration to define Return Value of Primary Functions
enum { _Menu, _Add, _Browse, _Search, _Edit, _Delete, _Issue, _Intro = -1 };

// Anonymous Enumeration to define Return Value of Sub Functions
enum { _IssueMenu, _CreateIssue, _BrowseIssue, _SearchIssue, _DeleteIssue };

// structure to select book category
struct cat{ int dept, sem; };

const string _str_dept[7] = {"CMT","ENT","ET","CT","MT","AIDT","TXT"};
const vector<string> _dept(&_str_dept[0],&_str_dept[7]);

const string _str_sem[7]  = {"1st","2nd","3rd","4th","5th","6th","7th"};
const vector<string> _sem(&_str_sem[0],&_str_sem[7]);

//--------------------------------- Forward Declaration ---------------------------------//

// Function Prototype
int  Admin();
int  Add();
int  Browse();
int  Search();
int  Edit();
int  Delete();
int  Query();
int  Book_Issue();
int  Create_Issue();
int  Browse_Issue();
int  Search_Issue();
int  Delete_Issue();
void Intro();
void Menu();
void Close();
bool Login();
void Password(char*);
bool CheckBook(int, int);
bool CheckIssue(int, int);
bool CheckCapacity(int, int);
bool CheckAvailability(int, int, int);
void Draw(int, int, int = 0, int = 0, int = -1, int = 1, int = 1);
void UpdateEncryption(const char*, const char*);
void Cursor(bool);
void Input(char*, size_t);
template <typename T>
void Input(T&);
bool strccmp(const char*, const char*);
bool strfind(const char*, const char*);
cat  Category();

// ----------------------------------- Class Section -----------------------------------//

// Class to hold Application Data
class AppData{
protected:
    int  rackMax;
    char pass[10];
    char config[10];
    static char encMask[20];
    // Function to access it's current object
    AppData& Self()
    {
        return *this;   // return reference of current object
    }
};
char AppData::encMask[];   // definition of static member

// Class to hold Book Data
class BookData{
protected:
    int dept, sem, code, quantity, rack;
    char name[50], author[30];
    // Function to access it's current object
    BookData& Self()
    {
        return *this;   // return reference of current object
    }
};

// Class to hold Student Data
class StudentData: public BookData{
protected:
    int stid;
    char stname[50];
    // Function to access it's current object
    StudentData& Self()
    {
        return *this;   // return reference of current object
    }
};

// Class to handle file operation
template <typename FType>
class FileManager:public AppData, public StudentData{
private:
    FType file;
    char fileName[50];
    ios_base::openmode mode;
public:
    // Constructor to open FILE
    FileManager(const char* _fileName, ios_base::openmode _mode = App)
    {
        strcpy(fileName,_fileName);     // set file name
        mode = _mode;                   // set file mode
        file.open(fileName,mode);       // open file
    }

    // Overloaded Parenthesis Operator to open FILE
    void operator()(const char* _fileName, ios_base::openmode _mode = App)
    {
        strcpy(fileName,_fileName);     // set new file name
        mode = _mode;                   // set new file mode
        file.open(fileName,mode);       // open file
    }

    // Overloaded Parenthesis Operator to use File Stream functions
    FType& operator()()
    {
        return file;    // return reference of File Stream
    }

    // Function to write data into Disk
    template <typename DType>
    FType& write(DType)
    {
        // copy the whole object into one string
        char *data = new char[sizeof(DType)];
        memcpy(static_cast<void*>(data),reinterpret_cast<void*>(&DType::Self()),sizeof(DType));

        // Encrypt every bit of data
        Encryption(data,sizeof(DType));

        // write encrypted data into disk
        file.write(data,sizeof(DType));
        file.flush();
        delete[] data;
        return file;
    }

    // Function to read data from Disk
    template <typename DType>
    FType& read(DType)
    {
        // get encrypted data from disk
        char *data = new char[sizeof(DType)];
        file.read(data, sizeof(DType));

        // Decrypt every bit of data
        Encryption(data,sizeof(DType));

        // copy decrypted data into object
        memcpy(reinterpret_cast<void*>(&DType::Self()),static_cast<void*>(data),sizeof(DType));
        delete[] data;
        return file;
    }

    // Function to Encrypt/Decrypt Data
    void Encryption(char *data, int length)
    {
        for(int i=0; i<length; i++){
            for(int j=0; encMask[j]; j++){
                data[i] ^= encMask[j]+j;
                data[i] ^= i;
                data[i] ^= j;
                data[i] ^= i*j+1;
                data[i] ^= 5284249;
            }
            data[i] ^= 4891684;
            data[i] ^= 1145326;
            data[i] ^= 4006243;
        }
    }

    // Function to set the mask value of Encryption
    void setMask(const char* _mask)
    {
        char temp[20];
        strcpy(temp,_mask);
        strcat(temp,"x9y0stl");
        strcpy(encMask,temp);
    }

    // Function to set physical 'End Of File' (EOF)
    bool setEOF(int position)
    {
        file.close();   // close the current file from FileStream
        // Open the same file with WIN32_API
        HANDLE hfile = CreateFile(TEXT(fileName),
                                 GENERIC_WRITE,
                                 FILE_SHARE_WRITE,
                                 NULL,
                                 OPEN_EXISTING,
                                 FILE_ATTRIBUTE_NORMAL,
                                 NULL);
        // Handle Error
        DWORD dwErr = GetLastError();
        if(dwErr > 0) {
            return false;
        }
        SetFilePointer(hfile, position, NULL, FILE_BEGIN);  // set File Pointer at 'position'
        SetEndOfFile(hfile);    // set EOF at current position
        CloseHandle(hfile);     // close the file from WIN32_API
        file.open(fileName, mode);  // now open the file with FileStream again for next operation
        return true;
    }

    // Restore Access
    #define A using AppData
    A::pass; A::config; A::rackMax;
    #define B using BookData
    B::dept; B::sem; B::code; B::quantity; B::rack;
    B::name; B::author;
    #define S using StudentData
    S::stid; S::stname;

    // Destructor to Close FILE
    ~FileManager()
    {
        file.close();
    }
};

//-------------------------------- Function Definition ---------------------------------//

// Main Function
int CLMS()
{
    Intro();
    return 0;
}

// Function to draw 'Login window'
void Intro()
{
    HideCursor;
    system("cls");
    system("color 3f");
    SetConsoleTitle(TEXT("College Library Management System"));    // set console title
    BORDER; Draw(1,34,6,3,_H); cout<<" College Library Management System "; Draw(1,34);
    setpos(32,5); Repeat(51) cout<<static_cast<char>(203);
    setpos(32,9); Repeat(51) cout<<static_cast<char>(202);
    Draw(2,3,32,6,_V); Draw(1,3,33,6,_V);
    Draw(1,3,81,6,_V); Draw(2,3,82,6,_V);
    setpos(43,7); cout<<"Generic Polytechnic Institute";
    Draw(2,10,32,9,_V);  Draw(5,51,32,19,_H); Draw(2,10,82,9,_V);
    Draw(8,47,34,10,_H); Draw(8,47,34,18,_H);
    Draw(3,7,34,11,_V);  setpos(34,10); cout<<"\xC2"; setpos(34,18); cout<<"\xC1";
    Draw(3,7,80,11,_V);  setpos(80,10); cout<<"\xC2"; setpos(80,18); cout<<"\xC1";
    Draw(8,45,35,12,_H);
    setpos(52,11); cout<<"Login Panel";
    SetFileAttributes(TEXT(AppDataFile),FILE_ATTRIBUTE_NORMAL);
    SetFileAttributes(TEXT(BookDataFile),FILE_ATTRIBUTE_NORMAL);
    SetFileAttributes(TEXT(IssueDataFile),FILE_ATTRIBUTE_NORMAL);
    Login();    // call Login() function
}

// Function to handle Login operation
bool Login()
{
    FileManager<fstream> login(AppDataFile,Rnd);
    // Create a new file (if not exists)
    if(!login().is_open()){
        mkdir("Database");
        login(AppDataFile,Out);
        login().close();
        login(AppDataFile,Rnd);
    }
    login().seekg(0,Beg);
    if(login.read(apData).eof()){  // set up password
        setpos(40,14);  cout<<"You Haven't Set Up a Password Yet!";
        setpos(43,16);  cout<<"[Press AnyKey to Continue...]";
        getch();
        while(true){
            Draw(9,44,36,13,_H,5);
            setpos(42,17);  cout<<"[Valid: a-z, A-Z, 0-9 | Max: 8]";
            setpos(36,14);  cout<<"Enter a New Password: ";
            char pass[PassMax+1];
            Password(pass);
            setpos(36,15);  cout<<"Enter Again to Confirm: ";
            char checkpass[PassMax+1];
            Password(checkpass);
            Draw(9,44,36,13,_H,5);
            if(!strcmp(pass,checkpass)){
                strcpy(login.pass,pass);
                strcpy(login.config,"color 3f");
                login.rackMax = 0;
                login.setMask(login.pass);
                login().clear();
                login().seekp(0,Beg);
                login.write(apData);
                login().close();
                UpdateEncryption("-",pass);
                setpos(38,14);  cout<<"\xDB\xB2\xB1\xB0 Password Successfully Created \xB0\xB1\xB2\xDB";
                setpos(45,17);  cout<<"[Press AnyKey to Continue]";
                getch();
                Menu();
            }
            setpos(41,14);  cout<<"\xDB\xB2\xB1\xB0 Passwords Do Not Match! \xB0\xB1\xB2\xDB";
            setpos(44,17);  cout<<"[Press AnyKey to Try Again]";
            getch();
        }
    } else {    // login
        setpos(36,15); cout<<"Enter Password to Continue: ";
        char pass[PassMax+1];
        while(true){
            Password(pass);
            login.setMask(pass);
            login().seekg(0,Beg);
            login.read(apData);
            Draw(9,44,36,13,_H,5);
            if(!strcmp(login.pass,pass)){
                setpos(52,15); cout<<"Loading"; Repeat(3){ cout<<'.'; Sleep(100); }
                system(login.config);
                login().close();
                Menu();
            } else {
                Sleep(100);
                setpos(48,14); cout<<"Password Incorrect!";
                setpos(43,15); cout<<"Please Try Again: ";
            }
        }
    }
}

// Function to configure the program
int Admin()
{
    Repeat:
    system("cls");
    BORDER; Draw(1,41,6,3,_H); cout<<" Administrator Panel "; Draw(1,41);

    //--------------------- Design: Menu Items ---------------------//
    Draw(1,15,57,5,_V);
    initpos(8,6,2);
	pos cout<<"\xDB\xDB\xB2\xB1 1. Change Password";
	pos cout<<"\xDB\xDB\xB2\xB1 2. Change Theme";
	pos cout<<"\xDB\xDB\xB2\xB1 3. Rack Capacity";
	pos cout<<"\xDB\xDB\xB2\xB1 4. Reset All";
	pos cout<<"\xDB\xDB\xB2\xB1 5. Log Out";
	pos cout<<"\xDB\xDB\xB2\xB1 6. Shut Down";
	pos cout<<"\xDB\xDB\xB2\xB1 0. Main Menu";
    setpos(72,11);  cout<<"Press Your Desired Key: ";
    //----------------- End of: Menu Items Design -----------------//

    FileManager<fstream> admin(AppDataFile,Rnd);
    admin().seekg(0,Beg);
    admin.read(apData);
    while(true){
        switch(getch()-'0'){
            case 1: {
                Draw(9,30,8,6,_H,7,2);
                Draw(9,40,64,11,_H,2);
                setpos(8,11);   cout<<"\xDB\xDB\xB2\xB1 Enter Old Password: ";
                char oldpass[PassMax+1];
                Password(oldpass);
                Draw(9,35,8,11,_H);
                if(strcmp(admin.pass,oldpass)){
                    setpos(19,11);  cout<<"\xDB\xB2\xB1\xB0 Wrong Password! \xB0\xB1\xB2\xDB";
                    setpos(76,11);  cout<<"[Press Any Key]";
                    getch();
                    admin().clear();
                    admin().close();
                    goto Repeat;
                }
                while(true){
                    setpos(16,18);  cout<<"[Valid: a-z, A-Z, 0-9 | Max: 8]";
                    setpos(8,10);   cout<<"\xDB\xDB\xB2\xB1 Enter a New Password: ";
                    char pass[PassMax+1];
                    Password(pass);
                    strcpy(admin.pass,pass);
                    setpos(8,13);   cout<<"\xDB\xDB\xB2\xB1 Enter Again to Confirm: ";
                    char checkpass[PassMax+1];
                    Password(checkpass);
                    Draw(9,40,8,10,_H,2,3);
                    if(!strcmp(admin.pass,checkpass)){
                        admin().clear();
                        admin.setMask(admin.pass);
                        admin().seekp(0,Beg);
                        admin.write(apData).close();
                        UpdateEncryption(oldpass,admin.pass);
                        setpos(11,11);  cout<<"\xDB\xB2\xB1\xB0 Password Successfully Changed \xB0\xB1\xB2\xDB";
                        setpos(70,11);  cout<<"[Press AnyKey to Continue]";
                        getch();
                        admin().clear();
                        admin().close();
                        admin.setMask(admin.pass);
                        goto Repeat;
                    }
                    setpos(15,11);  cout<<"\xDB\xB2\xB1\xB0 Passwords Do Not Match! \xB0\xB1\xB2\xDB";
                    setpos(71,11);  cout<<"[Press AnyKey to Try Again]";
                    getch();
                    Draw(9,40,14,11,_H); Draw(9,28,70,11,_H);
                }
            }
            case 2: {
                Draw(9,30,8,6,_H,7,2);
                Draw(9,40,64,11,_H,2);
                initpos(8,6,2);
                pos cout<<"\xB2\xB2\xB0 1. Theme 1";
                pos cout<<"\xB2\xB2\xB0 2. Theme 2";
                pos cout<<"\xB2\xB2\xB0 3. Theme 3";
                pos cout<<"\xB2\xB2\xB0 4. Theme 4";
                pos cout<<"\xB2\xB2\xB0 5. Theme 5";
                pos cout<<"\xB2\xB2\xB0 6. Default";
                pos cout<<"\xB2\xB2\xB0 0. Main Menu";
                setpos(72,11);  cout<<"Press Your Desired Key: ";
                const char theme[][10] = {"color 0b","color 1b","color 0a","color 1a","color 5f","color 3f"};
                while(true){
                    int select = getch()-'0';
                    if(!select){
                        break;
                    }
                    else if(select>=1 && select<=6){
                        if(!strcmp(admin.config,theme[select-1])){
                            setpos(27,12);   cout<<"\xDB\xB2\xB1\xB0 Already in Use! \xB0\xB1\xB2\xDB";
                            Draw(9,24,72,11,_H);
                            setpos(76,11);  cout<<"[Press Any Key]";
                            getch();
                            break;
                        }
                        system(theme[select-1]);
                        strcpy(admin.config,theme[select-1]);
                        admin().seekg(0,Beg);
                        admin.write(apData);
                        break;
                    }
                }
                admin().clear();
                admin().close();
                goto Repeat;
            }
            case 3: {
                Draw(9,30,8,6,_H,7,2);
                Draw(9,40,64,11,_H,2);
                if(admin.rackMax){
                    setpos(8,11);   cout<<"\xDB\xDB\xB2\xB1 Rack Capacity [for each]: ";
                    cout<<admin.rackMax;
                    setpos(76,11);  cout<<"[Press Any Key]";
                    getch();
                } else {
                    setpos(8,11);   cout<<"\xDB\xDB\xB2\xB1 Set Rack Capacity [for each]: ";
                    Input(admin.rackMax);
                    admin().seekg(0,Beg);
                    admin.write(apData);
                    Draw(9,48,8,11,_H);
                    setpos(24,11);  cout<<"\xDB\xB2\xB1\xB0 Done \xB0\xB1\xB2\xDB";
                    setpos(76,11);  cout<<"[Press Any Key]";
                    getch();
                }
                admin().clear();
                admin().close();
                goto Repeat;
            }
            case 4: {
                Draw(9,30,8,6,_H,7,2);
                Draw(9,40,64,11,_H,2);
                setpos(8,9);    cout<<"\xDB\xDB\xB2\xB1 1. Reset All (App. Data) ";
                setpos(8,11);   cout<<"\xDB\xDB\xB2\xB1 2. Reset All + Format Database";
                setpos(8,13);   cout<<"\xDB\xDB\xB2\xB1 0. Go Back";
                setpos(72,11);  cout<<"Press Your Desired Key: ";
                int opt;
                while(true){
                    opt = getch()-'0';
                    if(!opt){
                        admin().clear();
                        admin().close();
                        goto Repeat;
                    }
                    else if(opt==1 || opt==2){
                        break;
                    }
                }
                Draw(9,48,8,9,_H,3,2);
                Draw(9,24,72,11,_H);
                setpos(8,11);   cout<<"\xDB\xDB\xB2\xB1 Enter Your Password: ";
                char pass[PassMax+1];
                Password(pass);
                Draw(9,35,8,11,_H);
                if(strcmp(admin.pass,pass)){
                    setpos(19,11);  cout<<"\xDB\xB2\xB1\xB0 Wrong Password! \xB0\xB1\xB2\xDB";
                    setpos(76,11);  cout<<"[Press Any Key]";
                    getch();
                    admin().clear();
                    admin().close();
                    goto Repeat;
                }
                Prompt(72,10,"Are You Sure to Reset?");
                while(true){
                    switch(getch()-48){
                        case 1:
                            admin().close();
                            admin(AppDataFile,Out);
                            admin().close();
                            if(opt==1){
                                UpdateEncryption(pass,"-");
                            } else
                            if(opt==2){
                                admin(BookDataFile,Out);
                                admin().close();
                                admin(IssueDataFile,Out);
                                admin().close();
                            }
                            Draw(9,25,72,10,_H,3,2);
                            setpos(19,11);  cout<<"\xDB\xB2\xB1\xB0 Reset Successful \xB0\xB1\xB2\xDB";
                            setpos(76,11);  cout<<"[Press Any Key]";
                            getch();
                            return _Intro;
                        case 2:
                            Draw(9,25,72,10,_H,3,2);
                            setpos(19,11);  cout<<"\xDB\xB2\xB1\xB0 Process Canceled \xB0\xB1\xB2\xDB";
                            setpos(76,11);  cout<<"[Press Any Key]";
                            getch();
                            admin().clear();
                            admin().close();
                            goto Repeat;
                    }
                }
            }
            case 5: {
                return _Intro;
            }
            case 6: {
                system("cls");
                BORDER; Draw(1,34,6,3,_H); cout<<" College Library Management System "; Draw(1,34);
                Draw(4,21,47,9,_H); Draw(5,21,47,13,_H);
                Draw(4,3,47,10,_V);  Draw(5,3,67,10,_V);
                setpos(50,11); cout<<"Shutting Down";
                Repeat(3){ Sleep(300); cout<<"."; } Sleep(300);
                for(int i=6, j=108; i<58; i++, j--){
                    Draw(1,15,i,5,_V);
                    Draw(1,15,j,5,_V);
                    Sleep(30);
                }
                system("cls");
                admin.~FileManager();
                SetFileAttributes(TEXT(AppDataFile),FILE_ATTRIBUTE_READONLY);
                SetFileAttributes(TEXT(BookDataFile),FILE_ATTRIBUTE_READONLY);
                SetFileAttributes(TEXT(IssueDataFile),FILE_ATTRIBUTE_READONLY);
                exit(0);
            }
            case 0: {
                return _Menu;
            }
            default: {
                Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
                setpos(79,12);  cout<<"\aWarning!!! ";
                Sleep(700);     Draw(9,10,65,10,_H);
                setpos(68,11);  cout<<"You Have Pressed an Invalid Key.";
                setpos(65,12);  cout<<"Please Press a Valid Key (Hints: 0~6): ";
                break;
            }
        }
    }
}

// Function to represent Main Menu
void Menu()
{
    Repeat:
    system("cls");
    BORDER; Draw(1,46,6,3,_H); cout<<" MAIN MENU "; Draw(1,46);

    //--------------------- Design: Menu Items ---------------------//
    Draw(1,15,57,5,_V);
    initpos(8,6,2);
	pos cout<<"\xDB\xDB\xB2\xB1 1. Add New Record";
	pos cout<<"\xDB\xDB\xB2\xB1 2. Browse Book List";
	pos cout<<"\xDB\xDB\xB2\xB1 3. Search Book Record";
	pos cout<<"\xDB\xDB\xB2\xB1 4. Edit Book Record";
	pos cout<<"\xDB\xDB\xB2\xB1 5. Delete Book Record";
	pos cout<<"\xDB\xDB\xB2\xB1 6. Book Issue Menu";
	pos cout<<"\xDB\xDB\xB2\xB1 0. Administrator Panel";
    setpos(72,11);  cout<<"Press Your Desired Key: ";
    //----------------- End of: Menu Items Design -----------------//

    int select = getch()-48;
    while(true){
        switch(select){
            case 1:
                select = Add();
                break;
            case 2:
                select = Browse();
                break;
            case 3:
                select = Search();
                break;
            case 4:
                select = Edit();
                break;
            case 5:
                select = Delete();
                break;
            case 6:
                select = Book_Issue();
                break;
            case 0:
                select = Admin();
                break;
            default: {
                Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
                setpos(79,12);  cout<<"\aWarning!!! ";
                Sleep(700);     Draw(9,10,65,10,_H);
                setpos(68,11);  cout<<"You Have Pressed an Invalid Key.";
                setpos(65,12);  cout<<"Please Press a Valid Key (Hints: 0~6): ";
                select = getch()-48;
                break;
            }
        }
        if(select==_Intro) Intro();
        if(select==_Menu) goto Repeat;
    }
}

// Function to add a new book
int Add()
{
    system("cls");
	BORDER; Draw(1,43,6,3,_H); cout<<" ADD BOOK RECORD ";  Draw(1,43);
	cat select = Category();
	if(select.dept == _Menu){
        return _Menu;
	}
	else if(select.dept==8 || select.sem==8){
        setpos(21,11);  cout<<"\xDB\xB2\xB1\xB0 Sorry! You Must Have to Select Specific Department and Semester. \xB0\xB1\xB2\xDB";
        getch();
        return _Menu;
	}

    //--------------------- Design: Input Field ---------------------//
    Draw(1,15,72,5,_V);
    initpos(8,6,1);
    pos cout<<"\xDB\xDB\xB2\xB1 "<<_dept[select.dept-1]<<"-"<<select.sem<<":";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB1 Subject Code   :";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB1 Subject Name   :";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB1 Author Name    :";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB1 Book Quantity  :";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB1 Rack Number    :";
    pos cout<<"\xDB\xB1";
    pos cout<<"\xDB\xB2\xB1\xB1\xB1\xB1\xB0\xB0\xB0\xB0";
    setpos(75,11);  cout<<"Console is now in scanning mode.";
    setpos(79,12);  cout<<"Please Enter Your Data...";
    //----------------- End of: Input Field Design -----------------//

    FileManager<ofstream> book(BookDataFile);            // object to manipulate File
    book.dept = select.dept, book.sem = select.sem;

    /* Get Input from User */
    initpos(29,8,2);
    pos Input(book.code);
    pos Input(book.name,sizeof(book.name));
    pos Input(book.author,sizeof(book.author));
    pos Input(book.quantity);
    pos Input(book.rack);

    /* Prompt user for confirmation */
    Draw(9,32,75,11,_H,2);
    setpos(78,7);   cout<<"\xB2\xB1\xB0 Data Scan Finished \xB0\xB1\xB2";
    Sleep(100);
    Prompt(81,12,"Do You Want to Save?");
    while(true){
        int opt = getch()-48;
        if(opt==1){
            Draw(9,18,82,7,_H);
            Draw(9,20,81,12,_H,3,2);
            setpos(90,12);  cout<<"   ";
            setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
            if(CheckBook(book.code,select.dept)){     // check whether the data already exists or not
                Draw(9,32,75,11,_H,2);
                setpos(78,7);   cout<<"\xB2\xB1\xB0 Data Already Exists \xB0\xB1\xB2";
            }
            else if(CheckCapacity(book.rack,book.quantity)){    // check capacity
                Draw(9,32,75,11,_H,2);
                setpos(76,7);   cout<<"\xB2\xB1\xB0 Rack Capacity Exceeded \xB0\xB1\xB2";
            }
            else {
                book.write(bkData);     // write into Disk
                setpos(81,7);   cout<<" Successfully Saved ";
            }
            break;
        }
        else if(opt==2){
            Draw(9,18,82,7,_H);
            Draw(9,20,81,12,_H,3,2);
            setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
            setpos(90,12);  cout<<"   ";
            setpos(81,7);   cout<<"  Process Canceled  ";
            break;
        }
    }
    //-------------------- End of Functionality ---------------------//

    PromptEX(74,12,"Do You Want to Add Another Record?",_Add,_Menu);    // prompt user for next action
}

// Function to browse book record
int Browse()
{
    system("cls");
	BORDER; Draw(1,42,6,3,_H); cout<<" BROWSE BOOK RECORD ";  Draw(1,41);
	cat select = Category();
	if(select.dept == _Menu){
        return _Menu;
	}

    //--------------------- Design: Table ---------------------//
    setpos(6,5); cout<<" DEPT   SEM   CODE                    BOOK NAME                        AUTHOR NAME         QTY    RACK";
    Draw(8,103,6,6,_H);
    Draw(3,12,12,5,_V); setpos(12,6); cout<<"\xC5";
    Draw(3,12,18,5,_V); setpos(18,6); cout<<"\xC5";
    Draw(3,12,25,5,_V); setpos(25,6); cout<<"\xC5";
    Draw(3,12,71,5,_V); setpos(71,6); cout<<"\xC5";
    Draw(3,12,94,5,_V); setpos(94,6); cout<<"\xC5";
    Draw(3,12,102,5,_V);setpos(102,6); cout<<"\xC5";
    Draw(9,105,5,20,_H);
    //----------------- End of : Table Design------------------//

    FileManager<ifstream> book(BookDataFile), issue(IssueDataFile);   // object to manipulate File
    int line = 7, tBook=0, tIssue=0;
    while(!book.read(bkData).eof()) {        // read data from Disk until EOF
        if(select.dept==8){
            if(select.sem==8){
                // Print Everything
            } else {
                if(select.sem!=book.sem)
                    continue;
            }
        }
        else {
            if(select.sem==8){
                if(select.dept!=book.dept)
                    continue;
            } else {
                if(select.dept!=book.dept || select.sem!=book.sem)
                    continue;
            }
        }
        // Handle Error
        try {
            _dept.at(book.dept-1);
            _sem.at(book.sem-1);
        }
        catch (const std::out_of_range&) {
            Draw(9,103,6,5,_H,15);
            Draw(2,105,5,20,_H);
            setpos(40,12);  cout<<"\xDB\xB2\xB1\xB0 Something Went Wrong! \xB0\xB1\xB2\xDB";
            getch();
            return _Menu;
        }
        // Print Book Data
        setpos(7,line);     cout<<_dept[book.dept-1];
        setpos(14,line);    cout<<_sem[book.sem-1];
        setpos(20,line);    cout<<book.code;
        setpos(27,line);    cout<<book.name;
        setpos(73,line);    cout<<book.author;
        setpos(96,line);    cout<<book.quantity;
        setpos(104,line);   cout<<book.rack;
        tBook+=book.quantity; line++;
        // check issue
        issue().seekg(0,Beg);
        while(!issue.read(stData).eof()){
            if(issue.dept==book.dept && issue.code==book.code){
                tIssue++;
            }
        } issue().clear();
    }
    if(line<17){
        line = 17;
    }
    // Extended Design : Dynamic
    Draw(8,103,6,line,_H);
    Draw(3,line-17,12,17,_V);  setpos(12,line); cout<<"\xC1";
    Draw(3,line-17,18,17,_V);  setpos(18,line); cout<<"\xC1";
    Draw(3,line-17,25,17,_V);  setpos(25,line); cout<<"\xC1";
    Draw(3,line-17,71,17,_V);  setpos(71,line); cout<<"\xC1";
    Draw(3,line-17,94,17,_V);  setpos(94,line); cout<<"\xC1";
    Draw(3,line-17,102,17,_V); setpos(102,line);cout<<"\xC1";
    Draw(2,line-17,5,20,_V);
    Draw(2,line-17,109,20,_V);
    Draw(2,105,5,line+3,_H);
    // summery
    setpos(15,line+1); cout<<"Total Books: "<<tBook;
    setpos(45,line+1); cout<<"Available Books: "<<tBook-tIssue;
    setpos(80,line+1); cout<<"Press AnyKey : [Go Back] ";
    getch();
    return _Menu;
}



// Function to search book
int Search()
{
    system("cls");
    BORDER; Draw(1,42,6,3,_H); cout<<" SEARCH BOOK RECORD ";  Draw(1,41);
    Query();
    PromptEX(81,12,"Try Another Search?",_Search,_Menu);
}

// Function to edit book record
int Edit()
{
    system("cls");
    BORDER; Draw(1,42,6,3,_H); cout<<" EDIT BOOK RECORD ";  Draw(1,43);
    int position = Query();
    if(position!=EOF){
        Prompt(83,12,"Process to Edit?");
        while(true){
            bool flag = false;
            int opt = getch()-48;
            if(opt==1){
                FileManager<fstream> book(BookDataFile,Rnd);
                book().seekg(position,Beg);
                book.read(bkData);
                Draw(9,103,6,6,_H,14);
                Draw(1,15,72,5,_V);
                initpos(8,5,1);
                pos cout<<"\xDB\xDB\xB2\xB1 Old Record:";
                pos cout<<"\xDB\xB1 Subject Name   :  "<<book.name;
                pos cout<<"\xDB\xB1 Author Name    :  "<<book.author;
                pos cout<<"\xDB\xB1 Book Quantity  :  "<<book.quantity;
                pos cout<<"\xDB\xB1 Rack Number    :  "<<book.rack;
                Draw(0,46,26,11,_H);      // Horizontal Mid Line
                setpos(75,11);  cout<<"Console is now in scanning mode.";
                setpos(79,12);  cout<<"Please Enter Your Data...";
                setpos(6,11);   cout<<"\xB2\xB2\xB2\xB2\xB2 Update Record ";
                initpos(8,13,1);
                pos cout<<"\xDB\xDB\xB2\xB1 Edit Here: ";
                pos cout<<"\xDB\xB1 Subject Name   :  ";
                pos cout<<"\xDB\xB1 Author Name    :  ";
                pos cout<<"\xDB\xB1 Book Quantity  :  ";
                pos cout<<"\xDB\xB1 Rack Number    :  ";
                pos cout<<"\xDB\xB2\xB1\xB1\xB1\xB1\xB0\xB0\xB0\xB0";

                /* Get Input From User */
                initpos(29,14,1);
                pos Input(book.name,sizeof(book.name));
                pos Input(book.author,sizeof(book.author));
                pos Input(book.quantity);
                pos Input(book.rack);

                /* Prompt User for Confirmation */
                Draw(9,32,75,11,_H,2);
                setpos(78,7);   cout<<"\xB2\xB1\xB0 Data Scan Finished \xB0\xB1\xB2";
                Sleep(100);
                Prompt(81,12,"Do You Want to Save?");
                while(true){
                    int opt = getch()-48;
                    if(opt==1){
                        book().seekp(position,Beg);
                        book.write(bkData);     // write into Disk
                        Draw(9,18,82,7,_H);
                        Draw(9,20,81,12,_H,3,2);
                        setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
                        setpos(90,12);  cout<<"   ";
                        setpos(81,7);   cout<<" Successfully Saved ";
                        flag = true;
                        break;
                    }
                    else if(opt==2){
                        Draw(9,18,82,7,_H);
                        Draw(9,20,81,12,_H,3,2);
                        setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
                        setpos(90,12);  cout<<"   ";
                        setpos(81,7);   cout<<"  Process Canceled  ";
                        flag = true;
                        break;
                    }
                }
            }
            if(flag){
                break;
            }
            else if(opt==2){
                Draw(9,64,8,10,_H,9);
                Draw(9,22,79,12,_H,3,2);
                setpos(25,14);  cout<<"\xDB\xB2\xB1\xB0 Process Canceled! \xB0\xB1\xB2\xDB";
                break;
            }
        }
    }
    PromptEX(73,12,"Do You Want to Edit Another Record??",_Edit,_Menu);
}

// Function to delete book
int Delete()
{
    system("cls");
    BORDER; Draw(1,41,6,3,_H); cout<<" DELETE BOOK RECORD ";  Draw(1,42);
    int position = Query();
    if(position!=EOF){
        Prompt(80,12,"Are You Sure to Delete?");
        while(true){
            int opt = getch()-48;
            if(opt==1){
                FileManager<fstream> book(BookDataFile,Rnd);
                book().seekg(-static_cast<int>(sizeof(BookData)),End);
                book.read(bkData);
                book().seekp(position,Beg);
                book.write(bkData);
                book().seekp(-static_cast<int>(sizeof(BookData)),End);
                book.setEOF(static_cast<int>(book().tellp()));
                Draw(9,64,8,10,_H,9);
                Draw(9,25,80,12,_H,3,2);
                setpos(24,14);  cout<<"\xDB\xB2\xB1\xB0 Successfully Deleted \xB0\xB1\xB2\xDB";
                break;
            }
            else if(opt==2){
                Draw(9,64,8,10,_H,9);
                Draw(9,25,80,12,_H,3,2);
                setpos(25,14);  cout<<"\xDB\xB2\xB1\xB0 Process Canceled! \xB0\xB1\xB2\xDB";
                break;
            }
        }
    }
    PromptEX(81,12,"Delete Another Record?",_Delete,_Menu);
}

// Function to represent Issue Menu
int Book_Issue()
{
    Repeat:
    system("cls");
    BORDER; Draw(1,43,6,3,_H); cout<<" BOOK ISSUE MENU "; Draw(1,43);

    //--------------------- Design: Menu Items ---------------------//
    Draw(1,15,57,5,_V);
    initpos(8,8,2);
	pos cout<<"\xDB\xDB\xB2\xB1 1. Create an Issue";
	pos cout<<"\xDB\xDB\xB2\xB1 2. Browse Issued Record";
	pos cout<<"\xDB\xDB\xB2\xB1 3. Search Issued Record";
	pos cout<<"\xDB\xDB\xB2\xB1 4. Remove Issued Record";
	pos cout<<"\xDB\xDB\xB2\xB1 0. Back to Main Menu";
    setpos(72,11);  cout<<"Press Your Desired Key: ";
    //----------------- End of: Menu Items Design -----------------//

    int select = getch()-48;
    while(true){
        switch(select){
            case 1:
                select = Create_Issue();
                break;
            case 2:
                select = Browse_Issue();
                break;
            case 3:
                select = Search_Issue();
                break;
            case 4:
                select = Delete_Issue();
                break;
            case 0:
                return _Menu;
                break;
            default: {
                Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
                setpos(79,12);  cout<<"\aWarning!!! ";
                Sleep(700);     Draw(9,10,65,10,_H);
                setpos(68,11);  cout<<"You Have Pressed an Invalid Key.";
                setpos(65,12);  cout<<"Please Press a Valid Key (Hints: 0~4): ";
                select = getch()-48;
                break;
            }
        }
        if(select == _IssueMenu) goto Repeat;
    }
}

// Function to create book issue
int Create_Issue()
{
    system("cls");
    BORDER; Draw(1,44,6,3,_H); cout<<" CREATE ISSUE "; Draw(1,45);
    bool flag = false;
    int position = Query();
    if(position!=EOF){
        Prompt(81,12,"Process to Create?");
        while(true){
            int opt = getch()-48;
            if(opt==1){
                FileManager<fstream> issue(BookDataFile,Rnd);
                issue().seekg(position,Beg);
                issue.read(bkData);
                issue().close();
                // Handle Error
                try {
                    _dept.at(issue.dept-1);
                    _sem.at(issue.sem-1);
                }
                catch (const std::out_of_range&) {
                    Draw(9,103,6,5,_H,15);
                    Draw(2,105,5,20,_H);
                    setpos(40,12);  cout<<"\xDB\xB2\xB1\xB0 Something Went Wrong! \xB0\xB1\xB2\xDB";
                    getch();
                    return _Menu;
                }
                Draw(9,103,6,6,_H,14);
                Draw(1,15,72,5,_V);
                initpos(8,5,1);
                pos cout<<"\xDB\xDB\xB2\xB1 Book Record:";
                pos cout<<"\xDB\xB1 Category       :  "<<_dept[issue.dept-1]<<"-"<<issue.sem;
                pos cout<<"\xDB\xB1 Subject Code   :  "<<issue.code;
                pos cout<<"\xDB\xB1 Subject Name   :  "<<issue.name;
                pos cout<<"\xDB\xB1 Author Name    :  "<<issue.author;
                pos cout<<"\xDB\xB1 Book Quantity  :  "<<issue.quantity;
                pos cout<<"\xDB\xB1 Rack Number    :  "<<issue.rack;
                Draw(0,45,27,13,_H);      // Horizontal Mid Line
                setpos(75,11);  cout<<"Console is now in scanning mode.";
                setpos(79,12);  cout<<"Please Enter Your Data...";
                setpos(6,13);   cout<<"\xB2\xB2\xB2\xB2\xB2 Student Record ";
                initpos(8,15,1);
                pos cout<<"\xDB\xDB\xB2\xB1 Enter Here: ";
                pos cout<<"\xDB\xB1 Student ID     :  ";
                pos cout<<"\xDB\xB1 Student Name   :  ";
                pos cout<<"\xDB\xB2\xB1\xB1\xB1\xB1\xB0\xB0\xB0\xB0";

                /* Get Input from User */
                setpos(29,16);  Input(issue.stid);
                setpos(29,17);  Input(issue.stname,sizeof(issue.stname));

                /* Prompt User for Confirmation */
                Draw(9,32,75,11,_H,2);
                setpos(78,7);   cout<<"\xB2\xB1\xB0 Data Scan Finished \xB0\xB1\xB2";
                Sleep(100);
                Prompt(81,12,"Do You Want to Save?");
                while(true){
                    int opt = getch()-48;
                    if(opt==1){
                        Draw(9,18,82,7,_H);
                        Draw(9,20,81,12,_H,3,2);
                        setpos(90,12);  cout<<"   ";
                        setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
                        if(CheckIssue(issue.stid,issue.code)){
                            Draw(9,32,75,11,_H,2);
                            setpos(78,7);   cout<<"\xB2\xB1\xB0 Data Already Exists \xB0\xB1\xB2";
                        }
                        else if(CheckAvailability(issue.code,issue.dept,issue.quantity)){
                            Draw(9,32,75,11,_H,2);
                            setpos(77,7);   cout<<"\xB2\xB1\xB0 Book Is Not Available \xB0\xB1\xB2";
                        }
                        else {
                            // write into Disk
                            issue(IssueDataFile,Out|App);
                            issue.write(stData);
                            setpos(81,7);   cout<<" Successfully Saved ";
                        }
                        flag = true;
                        break;
                    }
                    else if(opt==2){
                        Draw(9,18,82,7,_H);
                        Draw(9,20,81,12,_H,3,2);
                        setpos(90,12);  Repeat(3){ cout<<"."; Sleep(100); }
                        setpos(90,12);  cout<<"   ";
                        setpos(81,7);   cout<<"  Process Canceled  ";
                        flag = true;
                        break;
                    }
                }
            }
            if(flag){
                break;
            }
            else if(opt==2){
                Draw(9,64,8,10,_H,9);
                Draw(9,22,79,12,_H,3,2);
                setpos(25,14);  cout<<"\xDB\xB2\xB1\xB0 Process Canceled! \xB0\xB1\xB2\xDB";
                break;
            }
        }
    }
    PromptEX(73,12,"Do You Want to Issue Another Record?",_CreateIssue,_IssueMenu);
}

// Function to browse book issue
int Browse_Issue()
{
    system("cls");
	BORDER; Draw(1,41,6,3,_H); cout<<" Browse Issued Record ";  Draw(1,40);

	cat select = Category();
	if(select.dept == _Menu){
        return _IssueMenu;
	}

    //--------------------- Design: Table ---------------------//
    setpos(6,5); cout<<" DEPT   SEM   CODE                    BOOK NAME                     STUDENT ID        STUDENT NAME";
    Draw(8,103,6,6,_H);
    Draw(3,12,12,5,_V); setpos(12,6); cout<<"\xC5";
    Draw(3,12,18,5,_V); setpos(18,6); cout<<"\xC5";
    Draw(3,12,25,5,_V); setpos(25,6); cout<<"\xC5";
    Draw(3,12,71,5,_V); setpos(71,6); cout<<"\xC5";
    Draw(3,12,86,5,_V); setpos(86,6); cout<<"\xC5";
    Draw(9,105,5,20,_H);
    //----------------- End of : Table Design------------------//

    FileManager<ifstream> issue(IssueDataFile);   // object to manipulate File
    int line = 7;
    while(!issue.read(stData).eof()){        // read data from Disk until EOF
        if(select.dept==8){
            if(select.sem==8){
                // Print Everything
            } else {
                if(select.sem!=issue.sem)
                    continue;
            }
        }
        else {
            if(select.sem==8){
                if(select.dept!=issue.dept)
                    continue;
            } else {
                if(select.dept!=issue.dept || select.sem!=issue.sem)
                    continue;
            }
        }
        // Handle Error
        try {
            _dept.at(issue.dept-1);
            _sem.at(issue.sem-1);
        }
        catch (const std::out_of_range&) {
            Draw(9,103,6,5,_H,15);
            Draw(2,105,5,20,_H);
            setpos(40,12);  cout<<"\xDB\xB2\xB1\xB0 Something Went Wrong! \xB0\xB1\xB2\xDB";
            getch();
            return _Menu;
        }
        // Print Here
        setpos(7,line);     cout<<_dept[issue.dept-1];
        setpos(14,line);    cout<<_sem[issue.sem-1];
        setpos(20,line);    cout<<issue.code;
        setpos(27,line);    cout<<issue.name;
        setpos(76,line);    cout<<issue.stid;
        setpos(88,line);    cout<<issue.stname;
        line++;
    }
    int total = line-7;
    if(line<17){
        line = 17;
    }
    // Extended Design : Dynamic
    Draw(8,103,6,line,_H);
    Draw(3,line-17,12,17,_V);  setpos(12,line); cout<<"\xC1";
    Draw(3,line-17,18,17,_V);  setpos(18,line); cout<<"\xC1";
    Draw(3,line-17,25,17,_V);  setpos(25,line); cout<<"\xC1";
    Draw(3,line-17,71,17,_V);  setpos(71,line); cout<<"\xC1";
    Draw(3,line-17,86,17,_V);  setpos(86,line); cout<<"\xC1";
    Draw(2,line-17,5,20,_V);
    Draw(2,line-17,109,20,_V);
    Draw(2,105,5,line+3,_H);
    // print summery
    setpos(15,line+1); cout<<"Total Issue: "<<total;
    setpos(80,line+1); cout<<"Press AnyKey : [Go Back] ";
    getch();
    return _IssueMenu;
}

// Function to search issue record
int Search_Issue()
{
    system("cls");
    BORDER; Draw(1,41,6,3,_H); cout<<" Search Issued Record ";  Draw(1,40);
    Draw(2,103,6,8,_H);
    Draw(0,103,6,9,_H,11);
    setpos(8,6); cout<<"\xDB\xDB\xB2\xB1 Enter Student or Subject [Code/Name]: ";

    //Get Input from User
    char searchKey[50];
    Input(searchKey,sizeof(searchKey));
    setpos(51,8); cout<<"Searching"; Repeat(5){ Sleep(100); cout<<'.'; }
    system("cls");
    BORDER; Draw(1,41,6,3,_H); cout<<" Search Issued Record ";  Draw(1,40);

    //--------------------- Design: Table ---------------------//
    setpos(6,5); cout<<" DEPT   SEM   CODE                    BOOK NAME                     STUDENT ID        STUDENT NAME";
    Draw(8,103,6,6,_H);
    Draw(3,12,12,5,_V); setpos(12,6); cout<<"\xC5";
    Draw(3,12,18,5,_V); setpos(18,6); cout<<"\xC5";
    Draw(3,12,25,5,_V); setpos(25,6); cout<<"\xC5";
    Draw(3,12,71,5,_V); setpos(71,6); cout<<"\xC5";
    Draw(3,12,86,5,_V); setpos(86,6); cout<<"\xC5";
    Draw(9,105,5,20,_H);
    //----------------- End of : Table Design -----------------//

    // Differentiate between ID & Name
    int searchID; bool ID = false;
    if(searchKey[0]>=48 && searchKey[0]<=57){
        searchKey[6] = '\0';
        searchID = atoi(searchKey);
        ID = true;
    }
    // Print the result
    FileManager<ifstream> issue(IssueDataFile);
    int line = 7;
    while(!issue.read(stData).eof()){
        if(ID){
            if(issue.code!=searchID && issue.stid!=searchID)
                continue;
        } else {
            if(strccmp(issue.name,searchKey) &&
               strccmp(issue.stname,searchKey) )
                continue;
        }
        // Handle Error
        try {
            _dept.at(issue.dept-1);
            _sem.at(issue.sem-1);
        }
        catch (const std::out_of_range&) {
            Draw(9,103,6,5,_H,15);
            Draw(2,105,5,20,_H);
            setpos(40,12);  cout<<"\xDB\xB2\xB1\xB0 Something Went Wrong! \xB0\xB1\xB2\xDB";
            getch();
            return _Menu;
        }
        // Print Here
        setpos(7,line);     cout<<_dept[issue.dept-1];
        setpos(14,line);    cout<<_sem[issue.sem-1];
        setpos(20,line);    cout<<issue.code;
        setpos(27,line);    cout<<issue.name;
        setpos(76,line);    cout<<issue.stid;
        setpos(88,line);    cout<<issue.stname;
        line++;
    }
    int total = line-7;
    if(line<17){
        line = 17;
    }
    // Extended Design : Dynamic
    Draw(8,103,6,line,_H);
    Draw(3,line-17,12,17,_V);  setpos(12,line); cout<<"\xC1";
    Draw(3,line-17,18,17,_V);  setpos(18,line); cout<<"\xC1";
    Draw(3,line-17,25,17,_V);  setpos(25,line); cout<<"\xC1";
    Draw(3,line-17,71,17,_V);  setpos(71,line); cout<<"\xC1";
    Draw(3,line-17,86,17,_V);  setpos(86,line); cout<<"\xC1";
    Draw(2,line-17,5,20,_V);
    Draw(2,line-17,109,20,_V);
    Draw(2,105,5,line+3,_H);
    // print summery
    setpos(15,line+1); cout<<"Issue Found: "<<total;
    setpos(80,line+1); cout<<"Press AnyKey : [Go Back] ";
    getch();
    return _IssueMenu;
}

// Function to delete issue record
int Delete_Issue()
{
    system("cls");
    BORDER; Draw(1,41,6,3,_H); cout<<" Delete Issued Record ";  Draw(1,40);
    Draw(2,103,6,10,_H);
    Draw(0,103,6,11,_H,9);
    setpos(8,6);    cout<<"\xDB\xDB\xB2\xB1 Enter Student [Code/Name]: ";
    setpos(8,8);    cout<<"\xDB\xDB\xB2\xB1 Enter Subject [Code/Name]: ";

    // Get Input from User
    char searchKey0[50], searchKey1[50];
    setpos(40,6);   Input(searchKey0,sizeof(searchKey0));
    setpos(40,8);   Input(searchKey1,sizeof(searchKey1));
    setpos(51,10);   cout<<"Searching"; Repeat(5){ Sleep(100); cout<<'.'; }
    Draw(9,103,6,11,_H,9); Draw(1,9,72,11,_V);
    setpos(12,10);   cout<<" Result ";
    setpos(20,10);   Draw(1,89);

    // Differentiate between ID & Name
    int searchID0, searchID1, positionCur;
    bool ID0 = false, ID1 = false, All = false, Deleted = false;
    if(searchKey0[0]>=48 && searchKey0[0]<=57){
        searchKey0[6] = '\0';
        searchID0 = atoi(searchKey0);
        ID0 = true;
    }
    if(searchKey1[0]>=48 && searchKey1[0]<=57){
        searchKey1[4] = '\0';
        searchID1 = atoi(searchKey1);
        ID1 = true;
        if(!searchID1) All = true;
    }

    // Delete the record
    FileManager<fstream> issue(IssueDataFile,Rnd);
    issue().seekg(0,Beg);
    while(!issue.read(stData).eof()){
        if(ID0){    // check student
            if(issue.stid!=searchID0)
                continue;
        } else {
            if(strccmp(issue.stname,searchKey0))
                continue;
        }
        if(All){
            // ignore book checking
        }
        else if(ID1){   // check book
            if(issue.code!=searchID1)
                continue;
        } else {
            if(strccmp(issue.name,searchKey1))
                continue;
        }
        positionCur = static_cast<int>(static_cast<int>(issue().tellg()) - static_cast<int>(sizeof(StudentData)));
        issue().seekg(-static_cast<int>(sizeof(StudentData)),End);
        issue.read(stData);
        issue().seekp(positionCur,Beg);
        issue.write(stData);
        issue().seekp(-static_cast<int>(sizeof(StudentData)),End);
        issue.setEOF(static_cast<int>(issue().tellp()));
        if(All){    // Delete All
            issue().seekg(positionCur,Beg);
            Deleted = true;
            continue;
        }
        Deleted = true;
        break;
    }
    if(Deleted){
        setpos(24,15);  cout<<"\xDB\xB2\xB1\xB0 Successfully Deleted \xB0\xB1\xB2\xDB";
    } else {
        setpos(25,15);  cout<<"\xDB\xB2\xB1\xB0 Record Not Found! \xB0\xB1\xB2\xDB";
    }
    PromptEX(80,13,"Delete Another Record?",_DeleteIssue,_IssueMenu);
}

// Function to query in book database
int Query()
{
    Draw(2,103,6,8,_H);
    Draw(0,103,6,9,_H,11);

    setpos(8,6); cout<<
    "\xDB\xDB\xB2\xB1 Choose Department [Press]:          \
     \xB2 1.CMT \xB2 2.ENT \xB2 3.ET \xB2 4.CT \xB2 5.MT \xB2 6.AIDT \xB2 7.TXT";
    int dept = getch()-'0';
    Draw(9,103,6,6,_H);
    setpos(8,6); cout<<"\xDB\xDB\xB2\xB1 Enter Subject [Code/Name]: ";

    // Get Input from User
    char searchKey[50];
    Input(searchKey,sizeof(searchKey));
    setpos(52,8); cout<<"Searching"; Repeat(3){ cout<<'.'; Sleep(100); }
    Draw(9,103,6,9,_H,11); Draw(1,11,72,9,_V);
    setpos(12,8); cout<<" Result ";
    setpos(20,8); Draw(1,89);

    // Differentiate between ID & Name
    int searchID; bool ID = false;
    if(searchKey[0]>=48 && searchKey[0]<=57){
        searchKey[6] = '\0';
        searchID = atoi(searchKey);
        ID = true;
    }

    // Search for the record
    FileManager<ifstream> book(BookDataFile);
    while(!book.read(bkData).eof()){
        if(ID){
            if(book.code!=searchID || dept!=book.dept)
                continue;
        } else {
            if(strccmp(book.name,searchKey) || dept!=book.dept)
                    continue;
        }
        // Handle Error
        try {
            _dept.at(book.dept-1);
            _sem.at(book.sem-1);
        }
        catch (const std::out_of_range&) {
            Draw(9,103,6,5,_H,15);
            Draw(2,105,5,20,_H);
            setpos(40,12);  cout<<"\xDB\xB2\xB1\xB0 Something Went Wrong! \xB0\xB1\xB2\xDB";
            getch();
            return _Menu;
        }
        // Print Here
        initpos(8,10,1);
        pos cout<<"\xDB\xDB\xB2\xB1 Object Found: ";
        pos cout<<"\xDB\xB1 Department     :  "<<_dept[book.dept-1];
        pos cout<<"\xDB\xB1 Semester       :  "<<_sem[book.sem-1];
        pos cout<<"\xDB\xB1 Subject Code   :  "<<book.code;
        pos cout<<"\xDB\xB1 Subject Name   :  "<<book.name;
        pos cout<<"\xDB\xB1 Author Name    :  "<<book.author;
        pos cout<<"\xDB\xB1 Book Quantity  :  "<<book.quantity;
        pos cout<<"\xDB\xB1 Rack Number    :  "<<book.rack;
        pos cout<<"\xDB\xB2";
        // return the file pointer position
        return static_cast<int>( static_cast<int>(book().tellg()) - static_cast<int>(sizeof(BookData)) );
    }
    setpos(25,14);  cout<<"\xDB\xB2\xB1\xB0 Record Not Found! \xB0\xB1\xB2\xDB";
    return EOF;     // End of File reached
}

// Function to choose a book category
cat Category()
{
    Draw(8,21,47,6,_H);
    Draw(1,13,57,7,_V);
    setpos(47,5);   cout<<"\xB2\xB1 Choose Category \xB1\xB2";
    initpos(8,6,1);
    pos cout<<"\xDB\xDB\xB2\xB1 Department:";
    pos cout<<"\xDB\xB2\xB1";
    pos cout<<"\xDB\xB1 1. Computer";
    pos cout<<"\xDB\xB1 2. Electronics";
    pos cout<<"\xDB\xB1 3. Electrical";
    pos cout<<"\xDB\xB1 4. Civil";
    pos cout<<"\xDB\xB1 5. Mechanical";
    pos cout<<"\xDB\xB1 6. Architecture";
    pos cout<<"\xDB\xB1 7. Textile";
    pos cout<<"\xDB\xB1 8. ALL Department";
    pos cout<<"\xDB\xB1 0. Main Menu";
    pos cout<<"\xDB\xB2\xB1";
    pos cout<<"\xDB\xDB\xDB\xB2\xB1\xB1\xB0\xB0\xB0";
    setpos(72,12);  cout<<"Press Your Desired Key: ";
    // Select a Department
	cat select;
	bool flag=false;
    while(true){
        select.dept = getch()-48;
        if(!select.dept){
            return select;
        }
        else if(select.dept>=1 && select.dept<=8){
            initpos(32,8,1);
            pos cout<<"\xDB\xDB\xB2\xB1 Semester:";
            pos cout<<"\xDB\xB1 1. First";
            pos cout<<"\xDB\xB1 2. Second";
            pos cout<<"\xDB\xB1 3. Third";
            pos cout<<"\xDB\xB1 4. Fourth";
            pos cout<<"\xDB\xB1 5. Fifth";
            pos cout<<"\xDB\xB1 6. Six";
            pos cout<<"\xDB\xB1 7. Seven";
            pos cout<<"\xDB\xB1 8. ALL Semester";
            pos cout<<"\xDB\xB1 0. Go Back";
            pos cout<<"\xDB\xB2\xB1\xB1\xB1\xB0\xB0";
            Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
            Sleep(300);
            setpos(72,12);  cout<<"Press Your Desired Key: ";
            // Select a Semester
            while(true){
                select.sem = getch()-48;
                if(!select.sem){
                    flag = true;
                    break;
                }
                else if(select.sem>=1 && select.sem<=8){
                    Draw(9,100,7,5,_H,15);    // flush inside the window
                    return select;
                }
                Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
                setpos(79,12);  cout<<"\aWarning!!! ";
                Sleep(700);     Draw(9,10,65,10,_H);
                setpos(68,11);  cout<<"You Have Pressed an Invalid Key.";
                setpos(65,12);  cout<<"Please Press a Valid Key (Hints: 0~8): ";
            }
        }
        if(!flag){
                Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
                setpos(79,12);  cout<<"\aWarning!!! ";
                Sleep(700);     Draw(9,10,65,10,_H);
                setpos(68,11);  cout<<"You Have Pressed an Invalid Key.";
                setpos(65,12);  cout<<"Please Press a Valid Key (Hints: 0~8): ";
        } else {
            Draw(9,18,32,8,_H,11);
            Draw(9,32,68,11,_H); Draw(9,38,65,12,_H);
            Sleep(300);
            setpos(72,12);  cout<<"Press Your Desired Key: ";
            flag = false;
        }
    }
}

// Function to get password from user
void Password(char pass[])
{
    ShowCursor;
    int index = 0; char get = 0;
    while(get=getch(), get!=ENTER || !index){
        if(get==BACKSPACE && index){
            index--;
            cout<<"\b \b";
        }
        else if( ( index < PassMax)      &&
                 ((get>='a' && get<='z') ||
                  (get>='A' && get<='Z') ||
                  (get>='0' && get<='9')) )
        {
            pass[index++] = get;
            putchar('*');
        }
    }
    pass[index] = '\0';
    HideCursor;
}

// Function to Update Encryption with new mask value
void UpdateEncryption(const char* oldPass, const char* newPass)
{
    FileManager<fstream> update(BookDataFile,Rnd);
    update().seekg(0,Beg);
    while(update.setMask(oldPass), !update.read(bkData).eof()){
        update().seekp(-static_cast<int>(sizeof(BookData)),Cur);
        update.setMask(newPass);
        update.write(bkData);
    }
    update().close();

    update(IssueDataFile,Rnd);
    update().seekg(0,Beg);
    while(update.setMask(oldPass), !update.read(stData).eof()){
        update().seekp(-static_cast<int>(sizeof(StudentData)),Cur);
        update.setMask(newPass);
        update.write(stData);
    }
    update.setMask(newPass);
}

// Function to get string with validation
void Input(char *str, size_t len)
{
    ShowCursor;
    FlushBuffer;
    cin.getline(str,len);
    if(!*str){
        cin.getline(str,len);
    }
    FlushBuffer;
    HideCursor;
}

// Function to get input with validation
template <typename T>
void Input(T &input)
{
    ShowCursor;
    FlushBuffer;
    cin>>input;
    FlushBuffer;
    HideCursor;
}

// Function to design the program
char Pattern[] = "\xB0\xB1\xB2\xB3\xDC\xDF\xCD\xEC\xC4 ";
void Draw(int p, int n, int x, int y, int direction, int repeat, int next)
{
    while(repeat--){
        if(direction==_V){
            for(int i=0; i<n; i++){
                setpos(x,y+i);
                putchar(Pattern[p]);
            }
            x += next;
            break;
        }
        if(direction==_H){
            setpos(x,y);
            y += next;
        }
        for(int i=0; i<n; i++){
            putchar(Pattern[p]);
        }
    }
}

// Function to check whether the book is already exists or not
bool CheckBook(int id, int dept)
{
    FileManager<ifstream> book(BookDataFile);
    while(!book.read(bkData).eof()){
        if(book.code==id && book.dept==dept)
            return true;
    }
    return false;
}

// Function to check whether the issue is already exists or not
bool CheckIssue(int stid, int bkid)
{
    FileManager<ifstream> issue(IssueDataFile);
    while(!issue.read(stData).eof()){
        if(issue.stid==stid && issue.code==bkid)
            return true;
    }
    return false;
}

// Function to check the Rack capacity
bool CheckCapacity(int rack, int quantity)
{
    int bcount = 0;
    FileManager<ifstream> check(BookDataFile);
    while(!check.read(bkData).eof()){
        if(rack==check.rack){
            bcount += check.quantity;
        }
    }
    check().close();
    check(AppDataFile);
    check.read(apData);
    return ((bcount+quantity) > check.rackMax );
}

// Function to check whether the book is available or not
bool CheckAvailability(int code, int dept, int quantity)
{
    int bcount = 0;
    FileManager<ifstream> check(IssueDataFile);
    while(!check.read(stData).eof()){
        if(code==check.code && dept==check.dept){
            bcount++;
        }
    }
    return (bcount>=quantity);
}

// Function to compare strings without case sensitivity
bool strccmp(const char* str, const char* cmp)
{
    while( ( (*str>='A' && *str<='Z')? *str+32 : *str )
            ==
           ( (*cmp>='A' && *cmp<='Z')? *cmp+32 : *cmp ) )
    {
        if(!*(str)){
            return false;   // found
        }
        str++;
        cmp++;
    }
    return true;   // not found
}

// Function to set Cursor visibility
void Cursor(bool state){
    CONSOLE_CURSOR_INFO lpCursor;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&lpCursor);
    if(state != lpCursor.bVisible){
        lpCursor.bVisible = state;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&lpCursor);
    }
}
