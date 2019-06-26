#ifndef NPSHELL_HPP
#define NPSHELL_HPP

#include <iostream>
#include <string>
#include <queue>
#include <list>
#include <stdlib.h>
#include <unistd.h>
#include <csignal> 
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <cstring>
#include <stdio.h>

#endif

void sighandle(int sig){}

class single_line_result{
    private:
        bool redirect_flag;
        int counter;
        std::string result;
    public:
        single_line_result();

        void set_redirect_flag(bool value);

        bool get_redirect_flag();

        void set_counter(int value);

        int get_counter();

        void set_result(std::string value);

        std::string get_result();
};


class npshell{
    private:
        std::list<single_line_result> single_line_results;
        std::queue<std::string> cmds;
        std::string input;

        std::string get_input();

        void set_env();

        void print_env();
        

        std::string get_cmd_from_string(std::string single_cmd, size_t space_position);

        std::string get_argument_from_string(std::string single_cmd, size_t space_position);

        void parse_input();

        void execute_cmd();

    public:
        void set_input(std::string value);

        int process_single_line_cmd();
};
