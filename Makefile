CC = gcc
CFLAGS = -Iinclude -Ithird_party/glad/include -Ithird_party/stb -Ithird_party/glm -std=c11 -Wall -Wextra -g -fsanitize=address,undefined
LIBS = -lglfw -lGL -lm -ldl -pthread -lcriterion -fsanitize=address,undefined

SRC = $(wildcard src/*.c) third_party/glad/src/glad.c
LIBSRC = $(filter-out src/main.c,$(wildcard src/*.c))
# object files go into build/obj so they don't pollute root
OBJDIR = build/obj
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(SRC)))
LIBOBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(notdir $(LIBSRC)))

BIN = bin/family-tree
TESTS = tests/test_person.c

.PHONY: all app test clean

all: app

# build application: link all object files
app: $(BIN)

# build static lib from library object files
libfamily.a: $(LIBOBJS)
	@echo "ar rcs $@ $^"
	@ar rcs $@ $^

# compile objects into build/obj
$(OBJDIR)/%.o: src/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/glad.o: third_party/glad/src/glad.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)

# app linking: link object files (including the glad object)
$(BIN): $(OBJS)
	@mkdir -p bin
	$(CC) $(OBJS) -o $@ $(LIBS)

test: libfamily.a tests/test_person.c
	@mkdir -p build
	$(CC) $(CFLAGS) $(TESTS) libfamily.a -o build/test_person $(LIBS)
	./build/test_person

clean:
	rm -rf $(OBJDIR) libfamily.a bin build
