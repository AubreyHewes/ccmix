/* 
 * File:   ccmix.cpp
 * Author: fbsagr
 *
 * Created on April 10, 2014, 1:43 PM
 */

#include "SimpleOpt.h"
#include "mix_file.h"
//#include "SimpleGlob.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifdef _MSC_VER

#include <windows.h>
#include <tchar.h>
#include "win32/dirent.h"

#else

#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#define TCHAR		char
#define _T(x)		x
#define _tprintf	printf
#define _tmain		main

#endif

#ifdef WINDOWS
#define DIR_SEPARATOR '\\'
#else
#define DIR_SEPARATOR '/'
#endif

using namespace std;

enum { OPT_HELP, OPT_EXTRACT, OPT_CREATE, OPT_GAME, OPT_FILES, OPT_DIR };
typedef enum { NONE, EXTRACT, CREATE, ADD, REMOVE } t_mixmode;

//get program directory from argv[0]
static const string getProgramDir(const char* const program_location) {
    register int i;
    const char* const loc = program_location;

    size_t size = strlen(loc);

    for (i = (size - 1); (i >= 0) && (loc[i] != DIR_SEPARATOR); --i);

    if (loc[i] == DIR_SEPARATOR) {
        char* curdir = (char*) malloc((i + 1) * sizeof (char));
        if (curdir != NULL) {
            strncpy(curdir, loc, (size_t) i);
            curdir[i] = '\0';
            return string(curdir);
        }
    }

    const char* curdir = ".";
    return string(curdir);
}

//search and test a few locations for a global mix database
string findGMD(const string program_dir, const string home_dir)
{
    string gmd_loc = "global mix database.dat";
    vector<string> gmd_dir(3);
    gmd_dir[0] = home_dir;
    gmd_dir[1] = "/usr/share/ccmix";
    gmd_dir[2] = program_dir;
    // Test if a file exists, used to find global mix database
    for (int i = 0; i < gmd_dir.size(); i++) {
        string gmd_test = gmd_dir[i] + DIR_SEPARATOR + gmd_loc;
        if (FILE *file = fopen(gmd_test.c_str(), "r")) {
            fclose(file);
            return gmd_test;
        }
    }
    return gmd_loc;
}

// This just shows a quick usage guide in case an incorrect parameter was used
// or not all required args were provided.
inline void ShowUsage(TCHAR** argv)
{
    cout << "Usage: " << argv[0] << " [-x|-c] (-f FILE) (-d DIR) MIXFILE" << endl;
    cout << "Try `" << argv[0] << " -?` or `" << argv[0] << 
            " --help` for more information." << endl;
}

// Shows more detailed help if help flags were used in invocation.
inline void ShowHelp(TCHAR** argv)
{
    cout << "/n***ccmix program usage***\n" << endl;
    cout << "Usage: " << argv[0] << 
            " [COMMAND] (-f FILE) (-d DIR) (-g GAME) MIXFILE\n" << endl;
    cout << "Commands:" << endl;
    cout << "*extract*" << endl;
    cout << "Extracts the contents of the specified mix file to the current "
            "directory.\n"
            "-f specifies a single file to extract.\n"
            "-d specifies an alternative directory to extract to.\n" << endl;
    cout << "*create*" << endl;
    cout << "Creates a new mix file from the contents of the current folder.\n"
            "-f specifies a single file as initial file to add to the new mix.\n"
            "-d specifies an alternative directory to create mix from.\n" << endl;
}

//quick inline to respond to more than one command being specified.
inline void NoMultiMode(TCHAR** argv)
{
    _tprintf(_T("You cannot specify more than one command at once.\n"));
    ShowUsage(argv);
}


CSimpleOpt::SOption g_rgOptions[] = {
    { OPT_EXTRACT,  _T("extract"),      SO_NONE    },
    { OPT_CREATE,   _T("create"),       SO_NONE    },
    { OPT_FILES,    _T("-f"),           SO_REQ_SEP }, // "-f ARG"
    { OPT_DIR,      _T("-d"),           SO_REQ_SEP }, // "-d ARG"
    { OPT_GAME,     _T("-g"),           SO_REQ_SEP }, // "-g ARG"
    { OPT_HELP,     _T("-?"),           SO_NONE    }, // "-?"
    { OPT_HELP,     _T("--help"),       SO_NONE    }, // "--help"
    SO_END_OF_OPTIONS                       // END
};
    
int _tmain(int argc, TCHAR** argv)
{
    if(argc <= 1) {
        ShowUsage(argv);
        return 0;
    }
    
    string file = "";
    string dir = "";
    string input_mixfile = string(argv[argc - 1]);
    const string program_path(argv[0]);
#ifdef WINDOWS
    string user_home_dir = getenv("HOMEDRIVE") + getenv("HOMEPATH");
#else    
#ifndef HOME
    string user_home_dir(getenv("HOME"));
#else
    string user_home_dir(getpwuid(getuid())->pw_dir);
#endif
#endif
    
    t_game game = game_td;
    t_mixmode mode = NONE;
    
    CSimpleOpt args(argc, argv, g_rgOptions);
    
    while (args.Next()) {
        if (args.LastError() != SO_SUCCESS) {
            _tprintf(_T("Invalid argument: %s\n"), args.OptionText());
            ShowUsage(argv);
            return 1;
        }
        
        switch (args.OptionId()) {
        case OPT_HELP:
            ShowHelp(argv);
            return 0;
        case OPT_FILES:
            if (args.OptionArg() != NULL) {
                file = string(args.OptionArg());
            } else {
                _tprintf(_T("-f option requires a filename.\n"));
                return 1;
            }
            break;
        case OPT_DIR:
            if (args.OptionArg() != NULL) {
                dir = string(args.OptionArg());
            } else {
                _tprintf(_T("-d option requires a directory name.\n"));
                return 1;
            }
            break;
        case OPT_CREATE:
            if (mode != NONE) { NoMultiMode(argv); return 1; }
            mode = CREATE;
            break;
        case OPT_EXTRACT:
            if (mode != NONE) { NoMultiMode(argv); return 1; }
            mode = EXTRACT;
            break;
        default:
            if (args.OptionArg()) {
                _tprintf(_T("option: %2d, text: '%s', arg: '%s'\n"),
                    args.OptionId(), args.OptionText(), args.OptionArg());
            }
            else {
                _tprintf(_T("option: %2d, text: '%s'\n"),
                    args.OptionId(), args.OptionText());
            }
            break;
        }
    }
    
    cout << input_mixfile << endl;
    MixFile in_file(findGMD(getProgramDir(program_path.c_str()), user_home_dir));
    if (!in_file.open(input_mixfile)){
        cout << "Cannot open specified mix file" << endl;
        return 1;
    }
    
    cout << in_file.printFileList(1);
    
    switch(mode) {
    case EXTRACT:
        if (dir != "") {
            cout << input_mixfile << "\nExtract to dir " << dir << endl;
            in_file.extractAll(dir);
            return 0;
        } else {
            cout << input_mixfile << "\nExtract to current dir" << endl;
            in_file.extractAll("./");
            return 0;
        }
        break;
    default:
        cout << "command switch default, this shouldn't happen!!" << endl;
        return 1;
    }
    
    return 0;
}

