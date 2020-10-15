#include "npshell.hpp"

using namespace std;

single_line_result::single_line_result(){
        redirect_flag = false;
        counter = 0;
}

void single_line_result::set_redirect_flag(bool value){
    redirect_flag = value;
}

bool single_line_result::get_redirect_flag(){
    return redirect_flag;
}

void single_line_result::set_counter(int value){
    counter = value;
}

int single_line_result::get_counter(){
    return counter;
}

void single_line_result::set_result(string value){
    result = value;
}

string single_line_result::get_result(){
    return result;
}

void npshell::set_input(string value){
    input = value;
}

string npshell::get_input(){
    return input;
}

void npshell::set_env(){
    //TODO
}

void npshell::print_env(){
    //TODO
}


string npshell::get_cmd_from_string(string single_cmd, size_t space_position){
    string cmd;
    cmd = single_cmd.substr(0, space_position);
    return cmd;
}

string npshell::get_argument_from_string(string single_cmd, size_t space_position){
    string argument;
    argument = single_cmd.substr(space_position + 1, single_cmd.length() - space_position - 1); 
    return argument;
}

void npshell::parse_input(){
    size_t end_position = 0;
    size_t start_position = 0;
    size_t redir_position = 0;
    string input = get_input();
    string single_cmd;

    single_line_results.push_back(*(new single_line_result));
    while(end_position < input.length()){
        end_position = (input.find_first_of('|', start_position) == string::npos)? \
        input.length() : input.find_first_of('|', start_position);
        //process |N
        if(start_position != 0 && input[start_position] != ' '){
            single_line_results.back().set_counter(atoi(input.substr(start_position, end_position - start_position).c_str()));    
            single_line_results.back().set_redirect_flag(true);
            break;
        }
        //skip space after |
        else if(input[start_position] == ' ') start_position += 1;               
        //if this is not the last command in the input line,
        //there will be a space at the end.
        //to avoid the space,
        //the second argument of substr should minus 1
        single_cmd = (end_position == input.length()) ? \
        input.substr(start_position, end_position - start_position) : input.substr(start_position, end_position - start_position - 1);
        //check redirection operater
        if((redir_position = single_cmd.find_first_of('>', 0)) != single_cmd.npos){
            cmds.push(single_cmd.substr(0, redir_position - 1));
            cmds.push(single_cmd.substr(redir_position, single_cmd.length() - redir_position));
            break;
        }
        else{
            cmds.push(single_cmd);
            start_position = end_position + 1;
        }
    }
    return;
}

void npshell::execute_cmd(){
    string cmd;
    string argument;
    string previous_result;
    size_t space_position;
    char result_buf[4096];
    pid_t child_pid;
    bool is_previous_input = true;
    int previous_line_fds[2];
    int result_fds[2];

    //clear result_buf
    memset(result_buf, 0, sizeof(result_buf));
    //process pipe from previous input line
    for(list<single_line_result>::iterator it = single_line_results.begin(); it != single_line_results.end(); it++){
        if(it->get_counter() == 0 && it->get_redirect_flag()){
            previous_result += it->get_result();
            it = single_line_results.erase(it);
        }
        else if(it->get_counter() != 0){
            it->set_counter(it->get_counter() - 1);
        }
    }

    while(!cmds.empty()){
        space_position = cmds.front().find_first_of(' ', 0);
        cmd = get_cmd_from_string(cmds.front(), space_position);
        //commands inside npshell
        if(cmd == "setenv"){
            set_env();
            cmds.pop();
            continue;
        }
        else if(cmd == "printenv"){
            print_env();
            cmds.pop();
            continue;
        }
        else if(cmd == "exit"){
            exit(0);
        }
        else if(cmd == ">"){
            FILE* fd;
            fd = fopen(get_argument_from_string(cmds.front(), space_position).c_str(), "w");
            if(fd){
                fputs(result_buf, fd);
                fclose(fd);
            }
            cmds.pop();
            break;
        }              
        //the path of commands' execute files
        string path = "./bin/" + cmd;
        pipe(previous_line_fds);
        pipe(result_fds);
        signal(SIGUSR1, sighandle);
        if((child_pid = fork()) == 0){
            //child process
            struct timespec req, rem;
            req.tv_sec = 0;
            req.tv_nsec = 1;

            close(previous_line_fds[1]);
            close(result_fds[0]);
            nanosleep(&req, &rem);

            dup2(previous_line_fds[0], STDIN_FILENO);
            dup2(result_fds[1], STDOUT_FILENO);                   
            if(space_position == cmds.front().npos){                            
                //commands without argument
                if(execl(path.c_str(), cmd.c_str(), NULL) < 0) perror("error on exec");
            }
            else{
                //commands with argument
                argument = get_argument_from_string(cmds.front(), space_position);
                if(execl(path.c_str(), cmd.c_str(), argument.c_str(), NULL) < 0) perror("error on exec");
            }            
            //close file descriptors
            close(previous_line_fds[0]);
            close(result_fds[1]);
            exit(0);
        }
        else{
            //pass previous input result to the child process
            signal(SIGUSR1, SIG_DFL);
            close(result_fds[1]);
            close(previous_line_fds[0]);
            if(is_previous_input){   
                strcpy(result_buf, previous_result.c_str());
            }
            if(write(previous_line_fds[1], result_buf, sizeof(result_buf)) == -1){
                cout << "error writing to pipe" << endl; 
            }
            close(previous_line_fds[1]);
            kill(child_pid, SIGUSR1);
            memset(result_buf, 0, sizeof(result_buf));
            read(result_fds[0], result_buf, sizeof(result_buf));
            close(result_fds[0]);
            wait(&child_pid);
            
            //end of this single line
            if(cmds.size() == 1){
                if(!single_line_results.back().get_redirect_flag()){
                    cout << result_buf;
                    single_line_results.pop_back();
                } 
                else{
                    single_line_results.back().set_result(single_line_results.back().get_result() + result_buf);
                }
            }
            is_previous_input = false;
            cmds.pop();
        }
    }
}

int npshell::process_single_line_cmd(){ 
    parse_input();
    execute_cmd();
    return 0;
}
