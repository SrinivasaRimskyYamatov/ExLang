CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC
LDFLAGS = -ldl -lm

all: exlang

exlang: main.o interpreter.o ast.o environment.o lexer.o parser.o value.o
	$(CXX) $(CXXFLAGS) -rdynamic -Wl,--export-dynamic -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build specific extension
%.so:
	@if [ -f libs/$*.cpp ]; then \
		$(CXX) $(CXXFLAGS) -shared -fPIC -Wl,--export-dynamic -o $@ libs/$*.cpp -I. -I.. $(LDFLAGS); \
	else \
		echo "Error: libs/$*.cpp not found"; \
	fi

clean:
	rm -f *.o exlang *.so libs/*.o

.PHONY: all clean

.PHONY: all clean