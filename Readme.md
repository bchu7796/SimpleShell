# SimpleShell
This is an implementation of unix shell with special piping machanism. 

### Supported Commands

1. noop - this command does nothing.
2. number - this command adds a number to each line of input.
3. removetag - this command remove html tag from the input without modifying the input.
4. removetag0 - this command is same as removetag, but it outputs message to stderr.
5. exit - this command terminate the shell program.
6. ls - this command lists all files in working directory.
7. cat - this command shows the input on the screen.

### Pipe
1. Regular Pipe - "|" is the regular pipe, it means that the  standard ouput of last command will be piped to the next command.
2. Special Pipe - "|N" (0 < N < 1000, where N is a natural number) is the special pipe, the standard output of the last command will be piped to the next Nth line.

### Output Redirect
This shell program also implement ">" operator to redirect standard output of last command to a file.

### Command Format
There must be a space between every command, argument, and operator. Special pipe is an exception, there should not be any spaces between "|" and "N".

### Usage
1. ```git clone https://github.com/bchu7796/SimpleShell.git```
2. ```cd SimpleShell```
3. ```make```

### Reference
NCTU CS - Network Programming 2018.
