GL_OPTIONS = -framework OpenGL -framework GLUT

name1 = assignment3

CXXINCS = -Wall -pedantic -Iinclude 

INIT_SHADER = common/InitShader.cpp
INIT_SHADER_OBJ = common/InitShader.o

EXAMPLELIST = assignment3.cpp
TARGETS = assignment3
DEPS = assignment3.o

assignment3: $(INIT_SHADER_OBJ)
	g++ $(INIT_SHADER_OBJ) assignment3.cpp $(GL_OPTIONS) -O2 $(CXXINCS) -o assignment3
	
all: assignment3

$(INIT_SHADER_OBJ): $(INIT_SHADER)
	g++ -c -Wall $(INIT_SHADER) -o $(INIT_SHADER_OBJ) -O2 $(CXXINCS)

clean:
	@rm -f *.o common/*.o $(name1) $(name2)
	
cleanall:
	rm *.o

run: clean all
	./assignment3
