cat << 'EOF' > Makefile
CC = gcc
CFLAGS = -I. -Wall -Wextra
TARGET = install_script
SOURCES = install_script.c \
					headers.c/uefi_check.c \
          headers.c/mounting.c \
          headers.c/partitioning/run_partition_call.c \
          headers.c/partitioning/partition_script_uefi_64.c \
          headers.c/partitioning/format_partitioned_space.c \
          headers.c/dir_making_options/pacstrap/pacstrap_call.c \
          headers.c/dir_making_options/pacstrap/pacstrap_option.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
EOF

