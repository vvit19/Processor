CXX = g++

CFLAGS = -ggdb3 -Wall -Wmissing-declarations -Wsuggest-attribute=noreturn -Wuseless-cast \
#-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr, \

ASM_FOLDER     = ./asm/
DISASM_FOLDER  = ./disasm/
SPU_FOLDER     = ./spu/
UTILS_FOLDER   = ./utils/
CODEGEN_FOLDER = ./codegen/

# CUR_FOLDER = $(ASM_FOLDER)
# CUR_FOLDER = $(DISASM_FOLDER)
CUR_FOLDER = $(SPU_FOLDER)

TARGET = run

IFLAGS = -I$(CUR_FOLDER)include/ -I$(UTILS_FOLDER)include/ -I$(CODEGEN_FOLDER)

SRC_FOLDER  = $(CUR_FOLDER)src/
OBJ_FOLDER  = $(CUR_FOLDER)obj/
SRC_UTILS_F = $(UTILS_FOLDER)src/
OBJ_UTILS_F = $(UTILS_FOLDER)obj/

SRC    = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ    = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))

SRC_UT = $(wildcard $(SRC_UTILS_F)*.cpp)
OBJ   += $(patsubst $(SRC_UTILS_F)%.cpp, $(OBJ_UTILS_F)%.o, $(SRC_UT))

$(OBJ_UTILS_F)%.o : $(SRC_UTILS_F)%.cpp
	@$(CXX) $(IFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_FOLDER)%.o : $(SRC_FOLDER)%.cpp
	@$(CXX) $(IFLAGS) $(CFLAGS) -c $< -o $@

$(TARGET) : $(OBJ)
	@$(CXX) $(IFLAGS) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

clean:
	rm $(TARGET) $(OBJ)
