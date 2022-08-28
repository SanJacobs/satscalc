# INCLUDE is where you find the header files for the libs you're using. They are used during compilation.
# Example: -I /usr/include/boost/
INCLUDE=

# Compiler. Pretty self-explanatory.
CXX = g++

# Enabling C++17 mode so I can have more <algorithm> stuff
CVERSION=-std=c++17

# Native compilation flags and crosscompilation flags for Windows
CFLAGS=-g
WINFLAGS=-g -gcodeview

# Executable name stuff
V=v0.2.0-alpha
NAME=satscalc

# LIBDIR is where you can find the linkable objects or whatever. They are used for the linking stage.
# -L /usr/lib/x86_64-linux-gnu/
LIBDIR=

# LIBS are the libs you are using written with a -l and then ignoring the lib-part at the beginning of the file's name.
# So "libboost_date_time.a" will be just "-lboost_date_time"
LIBS=

# Sources are the source code files. Only the .cpp files, becuase the .h files are included into them during pre-processing.
SOURCES=$(wildcard src/*.cpp)
OBJDIR=obj/
OBJECTS=$(patsubst src/%.cpp, $(OBJDIR)%.o, $(SOURCES))
# This last line creates an identical list of objects based on the list of .cpp files.

a.out: $(OBJECTS)
	$(CXX) $(LIBDIR) $(CVERSION) $(LIBS) $(OBJECTS) -o $(NAME)

$(OBJECTS): obj/%.o : src/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCLUDE) $(CVERSION) $(CFLAGS) -c $< -o $@

windows: 
	zig c++ -target x86_64-windows-gnu src/*.cpp $(CVERSION) $(WINFLAGS) -o $(NAME).exe

windows32: 
	zig c++ -target i386-windows-gnu src/*.cpp $(CVERSION) $(WINFLAGS) -o $(NAME).exe

mac: 
	zig c++ -target x86_64-macos-gnu src/*.cpp $(CVERSION) $(CFLAGS) -o $(NAME)

release:
	mkdir -p bin/linux
	make CFLAGS=-O2 NAME=bin/linux/$(NAME)-$(V)
	mkdir -p bin/mac
	make mac CFLAGS=-O2 NAME=bin/mac/$(NAME)-$(V)
	mkdir -p bin/windows
	make windows WINFLAGS="-O2 --static" NAME=bin/windows/$(NAME)-$(V)
	make windows32 WINFLAGS="-O2 --static" NAME=bin/windows/$(NAME)-$(V)-win32

clean:
	rm -f obj/*.o

cleanall:
	rm -rf obj/*.o bin/ $(NAME)

install: a.out
	cp satscalc /usr/local/bin/satscalc

test: a.out
	./test.sh

