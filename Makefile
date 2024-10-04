CC := g++
CFLAGS := -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

SOURCES := source/stack.cpp source/main.cpp source/log.cpp
OBJ := build/stack.obj build/main.obj build/log.obj

BUILD := build

INCLUDES := include
OUTPUT := stack

$(OUTPUT): $(OBJ)
	$(CC) $(OBJ) -o $@

build/stack.obj:
	@$(CC) $(CFLAGS) -I $(INCLUDES) -c source/stack.cpp -o $@

build/main.obj:
	@$(CC) $(CFLAGS) -I $(INCLUDES) -c source/main.cpp -o $@

build/log.obj:
	@$(CC) $(CFLAGS) -I $(INCLUDES) -c source/log.cpp -o $@

clean:
	rm build/*.obj $(OUTPUT)
