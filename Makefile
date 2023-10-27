CXX = g++

CFLAGS = -ggdb3 -Wall -Wmissing-declarations -Wsuggest-attribute=noreturn -Wuseless-cast \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr, \

# CFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

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
