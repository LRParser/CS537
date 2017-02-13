GL_OPTIONS = -framework OpenGL -framework GLUT

name1 = assignment5

CXXINCS = -Wall -pedantic -Iinclude 

INIT_SHADER = common/InitShader.cpp
INIT_SHADER_OBJ = common/InitShader.o

assignment4: $(INIT_SHADER_OBJ)
	g++ $(INIT_SHADER_OBJ) $(name1).cpp $(GL_OPTIONS) -O2 $(CXXINCS) -o $(name1) 
	
all: assignment4

$(INIT_SHADER_OBJ): $(INIT_SHADER)
	g++ -c -Wall $(INIT_SHADER) -o $(INIT_SHADER_OBJ) -O2 $(CXXINCS)

clean:
	@rm -f *.o common/*.o $(name1)

run: clean all
	./$(name1)
