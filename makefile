# Simple Makefile example

# Variables
CC = gcc
CFLAGS = -g
CLIENT_TARGET = client
SERVER_TARGET = server
LIBS = -lcrypto

CLIENT_SRCS = network_tool_client.c
SERVER_SRCS = network_tool_server.c

CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
SERVER_OBJS = $(SERVER_SRCS:.c=.o)

# Default target
all: $(CLIENT_TARGET) $(SERVER_TARGET)


# Link object files to create the executable
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	rm -f $(CLIENT_OBJS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	rm -f $(SERVER_OBJS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Clean up build files
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean