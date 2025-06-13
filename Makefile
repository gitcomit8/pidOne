CC = gcc
CFLAGS = -Wall -Wextra -O2 -g
LDFLAGS = -lpthread -lcap-ng -lseccomp

SRC_DIR = src
BIN_DIR = bin

INIT_SRCS = $(SRC_DIR)/init.c \
            $(SRC_DIR)/parser.c \
            $(SRC_DIR)/graph.c \
            $(SRC_DIR)/supervisor.c \
            $(SRC_DIR)/ctl_dispatch.c

CTL_SRCS = $(SRC_DIR)/ctl.c

INIT_OBJS = $(INIT_SRCS:.c=.o)
CTL_OBJS = $(CTL_SRCS:.c=.o)

HEADERS = $(SRC_DIR)/parser.h \
          $(SRC_DIR)/graph.h \
          $(SRC_DIR)/supervisor.h \
          $(SRC_DIR)/ctl_dispatch.h

.PHONY: all clean

all: $(BIN_DIR)/init $(BIN_DIR)/pidonectl

$(BIN_DIR)/init: $(INIT_SRCS) $(HEADERS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(INIT_SRCS) $(LDFLAGS)

$(BIN_DIR)/pidonectl: $(CTL_SRCS) $(SRC_DIR)/ctl_dispatch.h
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(CTL_SRCS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)/*.o $(BIN_DIR)/init $(BIN_DIR)/pidonectl