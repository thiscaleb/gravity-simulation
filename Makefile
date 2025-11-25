# Maybe make this more robust in the future.
# Auto detect perhaps?
OPENGL_VERSION = 4.6

ifeq ($(OPENGL_VERSION), 4.6)
GLAD_PATH = lib/glad_4_6_core
else
GLAD_PATH = lib/glad_3_1_compat
endif

CC      = gcc
CFLAGS  = -Wall -g -O3 -Iinclude -I$(GLAD_PATH)/include
LIBS    = -lm -ldl -lglfw -lGL -lyaml 

# break this out into individual object files at some point
SRC     = src/main.c src/physics/gravity.c $(GLAD_PATH)/src/glad.c src/math/math_funcs.c src/utils/constants.c src/utils/utils.c src/math/vector2.c src/physics/cr3bp.c src/math/matrix4.c src/utils/config_parser.c src/utils/shaders_parser.c
OUTDIR  = out
TARGET  = $(OUTDIR)/sim

all: $(TARGET)

$(OUTDIR):
	mkdir -p $(OUTDIR)

$(TARGET): $(SRC) | $(OUTDIR)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
	rm -f $(OUTDIR)/*

check: $(TARGET)
	valgrind --leak-check=full ./$(TARGET) -m n-body -n 4 -t 5000 


clean_test: 
	rm test

test: 
	$(CC) $(CFLAGS) sphere_test.c $(GLAD_PATH)/src/glad.c src/utils/shaders_parser.c $(LIBS) -o test
