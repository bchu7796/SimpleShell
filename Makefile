CC = g++

NP_SHELL_CMD_SRC = $(wildcard cmd_src/*.cpp)
NP_SHELL_CMD = $(patsubst cmd_src/%.cpp,bin/%,$(NP_SHELL_CMD_SRC))

all: work

work: $(NP_SHELL_CMD)  ls cat npshell

npshell: main.cpp
	$(CC) $< -o $@

$(NP_SHELL_CMD): bin/
	$(CC) $(patsubst bin/%,cmd_src/%.cpp,$@) -o $@

ls cat: bin/ls bin/cat

bin/ls bin/cat:
	cp /bin/ls /bin/cat bin/

bin/:
	mkdir bin/

.PHONY: debug
debug:
	$(CC) -g npshell.cpp -o npshell

.PHONY: clean
clean:
	rm -rf bin
	rm npshell




