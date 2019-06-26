#include "npshell.cpp"
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
    npshell shell;
    string input;
    
    while(1){
        cout << "%";
        getline(cin, input);
        //newline
        if(input.empty()) continue;
        shell.set_input(input);
        shell.process_single_line_cmd();
    }
    return 0;
}