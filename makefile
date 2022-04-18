# INCLUDE is where you find the headerfiles for the libs you're using. They are used during compilation.
# Example: -I /usr/include/boost/
INCLUDE=

# LIBDIR is where you can find the linkable objects or whatever. They are used for the linking stage.
LIBDIR=-L /usr/lib/x86_64-linux-gnu/

# LIBS are the libs you are using written with a -l and then ignoring the lib-part at the beginning of the file's name.
# So "libboost_date_time.a" will be just "-lboost_date_time"
LIBS=

# Sources are the source code files. Only the .cpp files, becuase the .h files are included into them during pre-processing.
SOURCES=$(wildcard src/*.cpp)
OBJDIR=obj/
OBJECTS=$(patsubst src/%.cpp, $(OBJDIR)%.o, $(SOURCES))
# This last line creates an identical list of objects based on the list of .cpp files.

a.out: $(OBJECTS)
	g++ $(LIBDIR) $(LIBS) $(OBJECTS)

$(OBJECTS): obj/%.o : src/%.cpp
	g++ -g $(INCLUDE) -c $< -o $@

clean:
	rm obj/*.o

cleanall:
	rm obj/*.o a.out
