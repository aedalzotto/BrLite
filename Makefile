TARGET = brlite

SRCDIR = src
INCDIR = $(SRCDIR)/include

INCLUDE = -I$(INCDIR)

CXX = g++
CXXFLAGS = -Wall -O2 $(INCLUDE)
LDFLAGS  = -lsystemc

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst %.cpp, %.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	@printf "Linking %s ...\n" "$@"
	@$(CXX) $^ -o $@ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	@printf "Compiling %s...\n" "$(dir $<)$*.cpp"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	@printf "Cleaning up\n"
	@rm -f src/*.o
	@rm -f $(TARGET)
