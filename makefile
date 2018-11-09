TARGET   = graphs_dbi
CPP      = g++
CPPFLAGS = -std=c++0x -Wall
LINKER   = g++
LFLAGS   = -Wall -l sqlite3 -l readline

OBJ     := obj/graphs_dbi.o obj/DatabaseInterface.o obj/BettiTable.o obj/Graph.o obj/parser.o
SRC	:= src/graphs_dbi.cpp src/DatabaseInterface.cpp src/BettiTable.cpp src/Graph.cpp src/parser.cpp
rm      = rm -f


all: bin/$(TARGET)

bin/$(TARGET): $(OBJ)
	@$(LINKER) $(OBJ) $(LFLAGS) -o $@
	@echo "Linking complete"

$(OBJ): obj/%.o : src/%.cpp
	@$(CPP) $(CPPFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully"

.PHONY: clean
clean:
	@$(rm) $(OBJ)
	@echo "Cleanup complete"

.PHONY: remove
remove: clean
	@$(rm) bin/$(TARGET)
	@echo "Executable removed"
