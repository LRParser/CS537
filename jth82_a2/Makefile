GL_OPTIONS = -framework OpenGL -framework GLUT

FNAME = red_triangle

CXXINCS = -Wall -pedantic -Iinclude 

INIT_SHADER = common/InitShader.cpp
INIT_SHADER_OBJ = common/InitShader.o

EXAMPLELIST = $(wildcard *.cpp)
TARGETS = $(basename $(EXAMPLELIST))
DEPS = $(TARGETS:=.o)

all: $(FNAME:=.o) $(TARGETS)
	g++ $(INIT_SHADER_OBJ) $(DEPS) $(GL_OPTIONS) -o $(FNAME)

$(FNAME:=.o): $(FNAME:=.cpp)
	g++ -c $(FNAME:=.cpp) -o $(FNAME:=.o) -O2 $(CXXINCS)
	
$(TARGETS): $(INIT_SHADER_OBJ)
	g++ -c $@.cpp -o $@.o -O2 $(CXXINCS)
	
	
$(INIT_SHADER_OBJ): $(INIT_SHADER)
	g++ -c -Wall $(INIT_SHADER) -o $(INIT_SHADER_OBJ) -O2 $(CXXINCS)

clean:
	@rm -f *.o common/*.o
	
cleanall:
	rm *.o

run: clean all
	./red_triangle
