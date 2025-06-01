# Makefile

CC       := gcc
CFLAGS   := -Wall -O2 -std=c11 -I./src -DCGLM_HAVE_BUILTIN_ASSUME_ALIGNED=1
LIBS     := -lGLEW -lGL -lX11 -lGLU -lglfw -lcglm -lassimp -lm

SRC_DIR  := src
SRCS     := \
    $(SRC_DIR)/main.c \
    $(SRC_DIR)/camera.c \
    $(SRC_DIR)/shader.c \
    $(SRC_DIR)/model.c \
    $(SRC_DIR)/collision.c \
    $(SRC_DIR)/fog.c \
    $(SRC_DIR)/texture.c

OBJS     := $(SRCS:.c=.o)

# Default target
all: app

# Link
app: $(OBJS)
	$(CC) $(OBJS) -o $@ $(LIBS)

# Compile each .c into .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*.o app

