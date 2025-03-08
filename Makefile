# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -g

# Linker flags (for Windows)
LDFLAGS = -lws2_32

# Executable names
SERVER = server
CLIENT = client

# Source files
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp

# Build both server and client
all: $(SERVER) $(CLIENT)

# Compile server
$(SERVER): $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $(SERVER_SRC) -o $(SERVER) $(LDFLAGS)

# Compile client
$(CLIENT): $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $(CLIENT_SRC) -o $(CLIENT) $(LDFLAGS)

# Clean compiled files
clean:
	rm -f $(SERVER) $(CLIENT)