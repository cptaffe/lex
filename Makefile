
CFLAGS += -D_GNU_SOURCE

SRC = lex.c
OBJ = $(SRC:.c=.o)
HDR = lex.h

LIB = liblex.o

all: $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

lib%.o: %.o
	$(LD) -r -o $@ $<

$(LIB): $(OBJ)

$(OBJ): $(HDR)

clean:
	$(RM) $(LIB) $(OBJ)
