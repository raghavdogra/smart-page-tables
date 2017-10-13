CPP_FILES := $(wildcard source/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS :=
CC_FLAGS := -std=c++11

simulate: $(OBJ_FILES)
	g++ $(LD_FLAGS) -o $@ $^

obj/%.o: source/%.cpp
	g++ $(CC_FLAGS) -c -o $@ $<

clean :
	\rm -fr obj/*
	\rm -fr simulate
	\rm -fr *~
	\rm -fr src/*~
	\rm -fr include/*~
