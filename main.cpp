// #define UNICODE
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>

const std::string config_file_name = ".\\config";
const std::string internal_save_folder = ".\\saves\\";
const std::string bb_save_folder = std::string{getenv("userprofile")} + "\\appdata\\LocalLow\\Jan Malitschek\\BetonBrutal\\Game\\";
const std::string backup_save_folder = ".\\saves\\backup\\";
const std::string save_files[2] = {"Stats.dat", "DLC1Stats.dat"};
static int current_command = 0;
static bool redirect_input = false;
static std::string input_str = "";
static std::vector<int> config_input = {};

const std::map<int,std::string> config_keys{
    {0x01, "Left mouse button"},     {0x02, "Right mouse button"},
    {0x04, "Middle mouse button"},   {0x05, "X1 mouse button"},
    {0x06, "X2 mouse button"},       {0x08, "BACKSPACE"},
    {0x09, "TAB"},                   {0x0D, "ENTER"},
    {0x10, "SHIFT"},                 {0x11, "CTRL"},
    {0x12, "ALT"},                   {0x14, "CAPS LOCK"},
    {0x1B, "ESC"},                   {0x20, "SPACEBAR"},
    {0x21, "PAGE UP"},               {0x22, "PAGE DOWN"},
    {0x24, "HOME"},                  {0x25, "LEFT ARROW"},
    {0x26, "UP ARROW"},              {0x27, "RIGHT ARROW"},
    {0x28, "DOWN ARROW"},            {0x2D, "INS"},
    {0x2E, "DEL"},                   {0x30, "0"},
    {0x31, "1"},                     {0x32, "2"},
    {0x33, "3"},                     {0x34, "4"},
    {0x35, "5"},                     {0x36, "6"},
    {0x37, "7"},                     {0x38, "8"},
    {0x39, "9"},                     {0x41, "A"},
    {0x42, "B"},                     {0x43, "C"},
    {0x44, "D"},                     {0x45, "E"},
    {0x46, "F"},                     {0x47, "G"},
    {0x48, "H"},                     {0x49, "I"},
    {0x4A, "J"},                     {0x4B, "K"},
    {0x4C, "L"},                     {0x4D, "M"},
    {0x4E, "N"},                     {0x4F, "O"},
    {0x50, "P"},                     {0x51, "Q"},
    {0x52, "R"},                     {0x53, "S"},
    {0x54, "T"},                     {0x55, "U"},
    {0x56, "V"},                     {0x57, "W"},
    {0x58, "X"},                     {0x59, "Y"},
    {0x5A, "Z"},                     {0x60, "Num 0"},
    {0x61, "Num 1"},                 {0x62, "Num 2"},
    {0x63, "Num 3"},                 {0x64, "Num 4"},
    {0x65, "Num 5"},                 {0x66, "Num 6"},
    {0x67, "Num 7"},                 {0x68, "Num 8"},
    {0x69, "Num 9"},                 {0x6A, "Multiply"},
    {0x6B, "Add"},                   {0x6C, "Separator"},
    {0x6D, "Subtract"},              {0x6E, "Decimal"},
    {0x6F, "Divide"},                {0x70, "F1"},
    {0x71, "F2"},                    {0x72, "F3"},
    {0x73, "F4"},                    {0x74, "F5"},
    {0x75, "F6"},                    {0x76, "F7"},
    {0x77, "F8"},                    {0x78, "F9"},
    {0x79, "F10"},                   {0x7A, "F11"},
    {0x7B, "F12"},                   {0x7C, "F13"},
    {0x7D, "F14"},                   {0x7E, "F15"},
    {0x7F, "F16"},                   {0x80, "F17"},
    {0x81, "F18"},                   {0x82, "F19"},
    {0x83, "F20"},                   {0x84, "F21"},
    {0x85, "F22"},                   {0x86, "F23"},
    {0x87, "F24"},                   {0xA0, "LSHIFT"},
    {0xA1, "RSHIFT"},                {0xA2, "LCONTROL"},
    {0xA3, "RCONTROL"} 
};

std::map<std::string, int> assigned_values = {
    {"save", 0x70},
    {"load", 0x71},
    {"quicksave", 0x74},
    {"quickload", 0x78},
    {"max_qsave_history", 0x33}
};

HHOOK _hook;
KBDLLHOOKSTRUCT kbdStruct;

int setHook(int mode);
void releaseHook();
int inputProcessing(int key_stroke, int scan_code);
int configInputProcessing(int key_stroke, int scan_code);

void exitRoutine(int exit_code);
BOOL WINAPI ctrlRoutine(DWORD fdwCtrlType);
int loadConfig();
void listSaveFiles();
void quicksaveBackup(unsigned char which_stats);
int save(std::string input_file_name);
int load(std::string input_file_name);

int configCommand();
void helpCommand();
int importCommand(std::string import_file);
int argumentHandler(int argc, char *argv[]);




LRESULT __stdcall hookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    if(nCode >= 0){
		if(wParam == WM_KEYDOWN){
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            
			inputProcessing(kbdStruct.vkCode, kbdStruct.scanCode);
		}
	}
	return CallNextHookEx(_hook,nCode,wParam,lParam);
}

LRESULT __stdcall configHookCallback(int nCode, WPARAM wParam, LPARAM lParam){
    if(nCode >= 0){
		if(wParam == WM_KEYDOWN){
			kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);
            
			configInputProcessing(kbdStruct.vkCode, kbdStruct.scanCode);
		}
	}
	return CallNextHookEx(_hook,nCode,wParam,lParam);
}

int setHook(int mode){
	if(mode == 0){
        if((_hook = SetWindowsHookEx(WH_KEYBOARD_LL,hookCallback,NULL,0)) == NULL){
            return 1;
        }
    }if(mode == 1){
        if((_hook = SetWindowsHookEx(WH_KEYBOARD_LL,configHookCallback,NULL,0)) == NULL){
            return 1;
        }
    }
    return 0;
}

void releaseHook(){
	UnhookWindowsHookEx(_hook);
}

int inputProcessing(int key_stroke, int scan_code){	
	HWND foreground = GetForegroundWindow();

    if(redirect_input && foreground == GetConsoleWindow()){
        bool lowercase = ((GetKeyState(0x14) & 0x0001) == 0);
        if((GetKeyState(0x10) & 0x1000) != 0 || (GetKeyState(0xa0) & 0x1000) != 0 || (GetKeyState(0xa1) & 0x1000) != 0){
            lowercase = !lowercase;
        }
        if(key_stroke == 0x0d && input_str.size() > 0){       // finish input
            redirect_input = false;
            printf("\n");
        }else if(key_stroke == 0x08 && !input_str.empty()){   // backspace
            input_str.pop_back();
            printf("\b \b");
        }else if(key_stroke > 0x40 && key_stroke < 0x5b){     // a-b
            char tmp_character = (char)(key_stroke + ((lowercase) ? 0x20 : 0));
            input_str += tmp_character;
            printf("%c", tmp_character);
        }else if(key_stroke > 0x2f && key_stroke < 0x3a){     // 0-9
            input_str += (char)key_stroke;
            printf("%c", (char)key_stroke);
        }else if(key_stroke > 0x5f && key_stroke < 0x6a){     // 0-9 NUM
            char tmp_character = (char)(key_stroke - 0x30);
            input_str += tmp_character;
            printf("%c", tmp_character);
        }else if(key_stroke == 0xbd || key_stroke == 0x5f){   // - / _
            char tmp_character = (lowercase) ? '-' : '_';
            input_str += tmp_character;
            printf("%c", tmp_character);
        }
    }

    if(!redirect_input && !input_str.empty()){
        int status = 0;
        if(current_command == assigned_values.at("save")){
            status = save(input_str);
            if(status != 0){
                exitRoutine(1);
            }else{
                printf("\nsave file \"%s\" successfully created\n", input_str.c_str());
            }
            input_str.clear();
            current_command = 0;
        }else if(current_command == assigned_values.at("load")){
            status = load(input_str);
            if(status == -1){
                printf("something went wrong!\n");
                exitRoutine(1);
            }else if(status == 1){
                printf("save file \"%s\" not found. try again\nenter file name: ", input_str.c_str());
                input_str.clear();
                redirect_input = true;
                return 0;
            }else{
                printf("\nsave file \"%s\" successfully loaded\n", input_str.c_str());
            }
            input_str.clear();
            current_command = 0;
        }else{
            printf("unexpected behavior!\n");
            exitRoutine(1);
        }
    }

	if(foreground){
        char window_title[100];
		GetWindowTextA(foreground,(LPSTR)window_title,100);
        if(memcmp(window_title,"BetonBrutal",12) == 0){
            int status = 0;
            if(key_stroke == assigned_values.at("quicksave")){
                status = save("quicksave");
                if(status != 0){
                    exitRoutine(1);
                }
            }else if(key_stroke == assigned_values.at("quickload")){
                status = load("quicksave");
                if(status != 0){
                    printf("something went wrong!\n");
                    exitRoutine(1);
                }
            }else if(key_stroke == assigned_values.at("save")){
                redirect_input = true;
                current_command = assigned_values.at("save");
                system("cls");
                listSaveFiles();
                printf("-------- save file --------\nenter file name: ");
            }else if(key_stroke == assigned_values.at("load")){
                redirect_input = true;
                current_command = assigned_values.at("load");
                system("cls");
                listSaveFiles();
                printf("-------- load file --------\nenter file name: ");
            }
        }
	}
    return 0;
}

int configInputProcessing(int key_stroke, int scan_code){
    HWND foreground = GetForegroundWindow();

    // in this case current_command used to identify end of input
    if(foreground == GetConsoleWindow()){
        if(current_command == 1 && key_stroke == 0x08){
            int tmp_input_size = config_keys.at(config_input[config_input.size()-1]).size();
            for(int i=0; i<tmp_input_size; i++){
                printf("\b");
            }
            printf("%*c", tmp_input_size,' ');
            for(int i=0; i<tmp_input_size; i++){
                printf("\b");
            }
            config_input.pop_back();
            current_command = 0;
            return 0;
        }
        if(current_command == 1 && key_stroke == 0x0D){
            printf("\n");
            redirect_input = false;
            current_command = 0;
        }
        if(current_command == 0 && redirect_input){
            if(config_keys.count(key_stroke) != 0){
                printf("%s", config_keys.at(key_stroke).c_str());
                bool tmp_flag = false;
                for(int i=0; i<(int)config_input.size(); i++){
                    if(key_stroke == config_input[i]){
                        printf("\nkey \"%s\" is already assigned, try again\n", config_keys.at(config_input[i]).c_str());
                        redirect_input = false;
                        tmp_flag = true;
                    }
                }
                if(!tmp_flag){
                    config_input.push_back(key_stroke);
                    current_command = 1;
                }
            }
        }

        int config_input_size = config_input.size();
        if(!redirect_input && !current_command && config_input_size == 1){
            printf("press normal load key: ");
            redirect_input = true;
        }else if(!redirect_input && !current_command && config_input_size == 2){
            printf("press quick save key: ");
            redirect_input = true;
        }else if(!redirect_input && !current_command && config_input_size == 3){
            printf("press quick load key: ");
            redirect_input = true;
        }else if(!redirect_input && !current_command && config_input_size == 4){
            releaseHook();
            PostQuitMessage(WM_QUIT);
            return 0;
        }
    }   
    return 0;
}



void exitRoutine(int exit_code){
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    exit(exit_code);
}

BOOL WINAPI ctrlRoutine(DWORD fdwCtrlType){
    switch(fdwCtrlType){
    case CTRL_C_EVENT:
        exitRoutine(0);
    case CTRL_BREAK_EVENT:
        exitRoutine(0);
    default:
        return false;
    }
}

int loadConfig(){
    std::ifstream config_file(config_file_name.c_str());
    std::string tmp_str;
    while(std::getline(config_file,tmp_str)){
        if(tmp_str.substr(0,2) != "//"){
            int tmp_sep_index = tmp_str.find(':');
            if(tmp_sep_index == -1){
                config_file.close();
                return 1;
            }
            std::string function = tmp_str.substr(0,tmp_sep_index);
            int tmp_space_index = function.find(' ');
            if(tmp_space_index != -1){
                function.erase(tmp_space_index, function.size()-tmp_space_index);
            }
            if(assigned_values.count(function) == 0){
                config_file.close();
                return 2;
            }
            tmp_sep_index += 1;
            std::string key = tmp_str.substr(tmp_sep_index);
            for(int i=key.size()-1; i>=0; --i){
                if(key[i] == ' '){
                    key.erase(i,1);
                }
            }
            bool tmp_flag = false;
            for(auto &key_pair : config_keys){
                if(key_pair.second == key){
                    assigned_values[function] = key_pair.first;
                    tmp_flag = true;
                    break;
                }
            }
            if(!tmp_flag){
                config_file.close();
                return 3;
            }
        }
    }
    config_file.close();
    assigned_values["max_qsave_history"] -= 0x30;

    return 0;
}

void listSaveFiles(){
    unsigned char which_stats;
    std::ifstream which_stats_file((bb_save_folder + "WhichStats.dat").c_str(),std::ios::binary);
    which_stats_file.read((char*)&which_stats,1);
    which_stats_file.close();

    std::vector<std::string> file_names;
    std::vector<FILETIME> file_lwtimes;
    WIN32_FIND_DATA find_data; 
    
    HANDLE handle_find = FindFirstFile((backup_save_folder + "*").c_str(),&find_data); 
    if(handle_find != INVALID_HANDLE_VALUE){
        while(FindNextFile(handle_find,&find_data) != 0){
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                std::string tmp_file_name = std::string(find_data.cFileName);
                if(tmp_file_name[0] == std::to_string((int)which_stats)[0] && (int)tmp_file_name.find("_b") != -1){
                    tmp_file_name.erase(0,1);
                    file_names.push_back(tmp_file_name);
                    file_lwtimes.push_back(find_data.ftLastWriteTime);
                }
            }
        }
    }else{
        printf("something went wrong!\n");
        exitRoutine(1);
    }
    
    handle_find = FindFirstFile((internal_save_folder + "*").c_str(),&find_data); 
    if(handle_find != INVALID_HANDLE_VALUE){
        while(FindNextFile(handle_find,&find_data) != 0){
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                std::string tmp_file_name = std::string(find_data.cFileName);
                if(tmp_file_name[0] == std::to_string((int)which_stats)[0] && (int)tmp_file_name.find('.') == -1){
                    tmp_file_name.erase(0,1);
                    file_names.push_back(tmp_file_name);
                    file_lwtimes.push_back(find_data.ftLastWriteTime);
                }
            }
        }
    }else{
        printf("something went wrong!\n");
        exitRoutine(1);
    }

    int max_file_name_size = 0;
    for(int i=0; i<(int)file_names.size(); i++){
        int tmp_file_name_size = file_names[i].size();
        if(max_file_name_size < tmp_file_name_size){
            max_file_name_size = tmp_file_name_size;
        }
    }
    
    printf("existing save files:\n");
    for(int i=0; i<(int)file_names.size(); i++){
        SYSTEMTIME tmp_lwtime;
        FileTimeToSystemTime(&file_lwtimes[i],&tmp_lwtime);
        int space_number = max_file_name_size - file_names[i].size() + 10;
        printf("- %s%*c", file_names[i].c_str(), space_number,' ');
        printf("%i.%i.%i %02i:%02i\n", tmp_lwtime.wYear,tmp_lwtime.wMonth,tmp_lwtime.wDay, tmp_lwtime.wHour,tmp_lwtime.wMinute);
    }
}

void quicksaveBackup(unsigned char which_stats){
    int last_quicksave = -1;
    FILETIME tmp_lwtime;
    WIN32_FIND_DATA find_data; 
    HANDLE handle_find = FindFirstFile((backup_save_folder + "*").c_str(), &find_data); 
    if(handle_find != INVALID_HANDLE_VALUE){
        while(FindNextFile(handle_find, &find_data) != 0){
            if(!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
                std::string tmp_file_name = std::string(find_data.cFileName);
                if(tmp_file_name[0] == std::to_string((int)which_stats)[0] && (int)tmp_file_name.find("quicksave") != -1){
                    int save_number = tmp_file_name[tmp_file_name.size()-1] - '0';
                    if(save_number >= 1 && save_number <= assigned_values.at("max_qsave_history")){
                        if(last_quicksave == -1){
                            tmp_lwtime = find_data.ftLastWriteTime;
                        }
                        int compare_flag = CompareFileTime(&find_data.ftLastWriteTime,&tmp_lwtime);
                        if(compare_flag == 0 || compare_flag == 1){
                            tmp_lwtime = find_data.ftLastWriteTime;
                            last_quicksave = save_number;
                        }
                    }
                }
            }
        }
    }else{
        printf("something went wrong!\n");
        exitRoutine(1);
    }

    last_quicksave = (last_quicksave == -1) ? 0 : last_quicksave;
    last_quicksave = (last_quicksave == assigned_values.at("max_qsave_history")) ? 1 : last_quicksave+1;
    CopyFile((internal_save_folder + std::to_string((int)which_stats) + "quicksave").c_str(), \
             (backup_save_folder + std::to_string((int)which_stats) + "quicksave_b" + std::to_string(last_quicksave)).c_str(), false);

}

int save(std::string input_file_name){
    if((int)input_file_name.find("quicksave_b") != -1){
        printf("\nyou cant use \"quicksave_b\" in file name!\n");
        return 1;
    }
    unsigned char which_stats;
    std::ifstream which_stats_file((bb_save_folder + "WhichStats.dat").c_str(), std::ios::binary);
    which_stats_file.read((char*)&which_stats,1);
    which_stats_file.close();
    
    std::ofstream internal_save_file((internal_save_folder + std::to_string((int)which_stats) + input_file_name).c_str(), std::ios::binary);
    std::ifstream tmp_input((bb_save_folder + save_files[(int)which_stats]).c_str(), std::ios::binary);
    std::vector<unsigned char> tmp_buff(std::istreambuf_iterator<char>(tmp_input),{});
    internal_save_file << save_files[(int)which_stats].c_str() << '\0';
    unsigned int tmp_buff_size = tmp_buff.size();
    internal_save_file.write(reinterpret_cast<const char *>(&tmp_buff_size),4);
    std::copy(tmp_buff.begin(),tmp_buff.end(),std::ostreambuf_iterator<char>(internal_save_file));
    tmp_input.close();
    internal_save_file.close();

    if(input_file_name == "quicksave" && assigned_values.at("max_qsave_history") != 0){
        quicksaveBackup(which_stats);
    }

    return 0;
}

int load(std::string input_file_name){
    unsigned char which_stats;
    std::ifstream which_stats_file((bb_save_folder + "WhichStats.dat").c_str(),std::ios::binary);
    which_stats_file.read((char*)&which_stats,1);
    which_stats_file.close();

    std::ifstream internal_save_file;
    if((int)input_file_name.find("_b") != -1){
        internal_save_file.open((backup_save_folder + std::to_string((int)which_stats) + input_file_name).c_str(),std::ios::binary);
    }else{
        internal_save_file.open((internal_save_folder + std::to_string((int)which_stats) + input_file_name).c_str(),std::ios::binary);
    }

    if(!(internal_save_file.is_open())){
        return 1;
    }

    std::string save_file_name;
    unsigned int tmp_buff_size;
    for(int i=0; true; i++){
        char bt;
        internal_save_file.read(&bt,1);
        if(bt == 0){ break; }
        save_file_name += bt;
    }
    internal_save_file.read(reinterpret_cast<char *>(&tmp_buff_size),4);
    std::ofstream save_file((bb_save_folder + save_file_name).c_str(),std::ios::binary);
    for(unsigned int i=0; i<tmp_buff_size; i++){
        unsigned char bt;
        internal_save_file.read((char*)&bt,1);
        save_file.write(reinterpret_cast<const char *>(&bt),1);
    }
    save_file.close();
    internal_save_file.close();

    return 0;
}


void helpCommand(){
    printf("\
bb_save - simple save manager for beton brutal (dlc`s too)\n\
\n\
options:\n\
-h / --help    - print this text\n\
-c / --config  - configure keys\n\
-i / --import  - import stats file from another person (specify path to file after this option)\n\
\n\
all functions work properly when you press esc before\n\
functions:\n\
normal save  - after pressing you need to open console where program is running and enter name how you\n\
               want to save current progress. to not accidentally overwrite existing save, above\n\
               printing list of existed saves (but you can overwrite save).\n\
normal load  - after pressing you need to open console where program is running and enter the name of\n\
               save, that you want to load, save listed only for current map.\n\
quick save   - after pressing you create a save of current progress without need to name a save.\n\
quick load   - after pressing load save that was created without assigning it a name.\n\
\n\
default keys:\n\
normal save  - f1\n\
normal load  - f2\n\
quick sqve   - f5\n\
quick load   - f9\n");
}

int configCommand(){
    printf("press normal save key: ");
    redirect_input = true;
    if(setHook(1) == 1){
        printf("cant set hook!\n");
        exitRoutine(1);
    }
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0) != 0){}

    std::ifstream config_file(config_file_name.c_str());
    std::ofstream tmp_file(".\\tmp_file");
    std::string tmp_str;
    while(std::getline(config_file,tmp_str)){
        if(tmp_str.substr(0,2) != "//"){
            if(tmp_str.substr(0,4) == "save"){
                tmp_str = tmp_str.substr(0,5);
                tmp_str += config_keys.at(config_input[0]);
            }else if(tmp_str.substr(0,4) == "load"){
                tmp_str = tmp_str.substr(0,5);
                tmp_str += config_keys.at(config_input[1]);
            }else if(tmp_str.substr(0,9) == "quicksave"){
                tmp_str = tmp_str.substr(0,10);
                tmp_str += config_keys.at(config_input[2]);
            }else if(tmp_str.substr(0,9) == "quickload"){
                tmp_str = tmp_str.substr(0,10);
                tmp_str += config_keys.at(config_input[3]);
                tmp_file.write(tmp_str.c_str(),tmp_str.size());
                break;
            }
        }
        tmp_file.write(std::string(tmp_str + "\n").c_str(),tmp_str.size()+1);
    }
    config_file.close();
    tmp_file.close();
    remove(config_file_name.c_str());
    rename(".\\tmp_file", config_file_name.c_str());
    
    printf("\nnew keys successfully assigned\n");

    return 0;
}

int importCommand(std::string import_file){
    int which_stats = -1;
    printf("map ids:\nBeton brutal - 0\nBeton bath   - 1\nenter which map this save is for: ");
    scanf("%i", &which_stats);
    if(which_stats == -1){
        printf("incorrect map number\n");
        exitRoutine(1);
    }
    char tmp_str[100];
    printf("enter save name: ");
    scanf("%100s", tmp_str);
    std::string file_name = std::string(tmp_str);

    std::ofstream internal_save_file((internal_save_folder + std::to_string(which_stats) + file_name).c_str(), std::ios::binary);
    std::ifstream import_save_file(import_file.c_str(), std::ios::binary);
    if(!import_save_file.is_open()){
        printf("cant find file \"%s\"\n", import_file.c_str());
        exitRoutine(1);
    }
    std::vector<unsigned char> tmp_buff(std::istreambuf_iterator<char>(import_save_file),{});
    internal_save_file << save_files[which_stats].c_str() << '\0';
    unsigned int tmp_buff_size = tmp_buff.size();
    internal_save_file.write(reinterpret_cast<const char *>(&tmp_buff_size),4);
    std::copy(tmp_buff.begin(),tmp_buff.end(),std::ostreambuf_iterator<char>(internal_save_file));
    import_save_file.close();
    internal_save_file.close();

    printf("\nfile successfully imported\n");

    return 0;
}

int argumentHandler(int argc, char *argv[]){
    if(argc < 0 || argc > 3){
        return 1;
    }else if(argc == 1){
        return 0;
    }
    if((strcmp(argv[1], "-h") == 0) || strcmp(argv[1], "--help") == 0){
        if(argc > 2){return 1;}
        helpCommand();
    }else if((strcmp(argv[1], "-c") == 0) || (strcmp(argv[1], "--config") == 0)){
        if(argc > 2){return 1;}
        configCommand();
    }else if((strcmp(argv[1], "-i") == 0) || (strcmp(argv[1], "--import") == 0)){
        importCommand(argv[2]);
    }else{
        return 1;
    }

    return 2;
}



int main(int argc, char *argv[]){
    int status;
    status = argumentHandler(argc, argv);
    if(status == 1){
        printf("incorrect arguments!\n");
        return 1;
    }else if(status == 2){
        exitRoutine(0);
    }
    status = loadConfig();
    if(status == 1){
        printf("incorrect config!\n");
        return 1;
    }else if(status == 2){
        printf("incorrect config function!\n");
        return 1;
    }else if(status == 3){
        printf("incorrect config key!\n");
        return 1;
    }
    status = (int)SetConsoleCtrlHandler(ctrlRoutine,true);
    if(status == 0){
        printf("cant set ctrl handler!\n");
        return 1;
    }

    // debug
    // for(auto &i : assigned_values){
    //     printf("%s %i\n", i.first.c_str(), i.second);
    // }printf("\n");

    status = setHook(0);
    if(status == 1){
        printf("cant set hook!\n");
        return 1;
    }else{
        printf("program is running\n");
    }

    MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) != 0){}
 
    return 0;
}