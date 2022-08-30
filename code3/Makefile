SALTICIDAE=$(realpath ./salticidae)
Salticidae_Include_Paths = -I$(SALTICIDAE)/include
Salticidae_Lib_Paths = -L$(SALTICIDAE)
#/lib

CFLAGS = `pkg-config --cflags libcrypto openssl libuv` # gio-2.0 openssl
LDLIBS = `pkg-config --libs   libcrypto openssl libuv` #-lgmp # gio-2.0 openssl

App_Cpp_Files := $(wildcard App/*.cpp)
App_Cpp_Files := $(filter-out App/Server.cpp, $(App_Cpp_Files))
App_Include_Paths := -IApp $(Salticidae_Include_Paths)

App_C_Flags := -fPIC -Wno-attributes $(App_Include_Paths)
App_Cpp_Flags := $(App_C_Flags) -std=c++14 $(CFLAGS)
App_Link_Flags := $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae

App_Cpp_Objects := $(App_Cpp_Files:.cpp=.o)

App_Name := server

all: $(App_Name)

$(App_Name): App/Server.o $(App_Cpp_Objects)
	@$(CXX) $^ -o $@ $(LDLIBS) $(Salticidae_Lib_Paths) -lsalticidae $(Salticidae_Include_Paths)
	@echo "LINK <= $@"

App/%.o: App/%.cpp
	@$(CXX) $(App_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

.PHONY: clean

clean:
	@rm -f $(App_Name) $(App_Cpp_Objects) App/Server.o
