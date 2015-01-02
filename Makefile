
CFLAGS += -D_GNU_SOURCE

SRC = lex.c
OBJ = $(SRC:.c=.o)
HDR = lex.h

LIB = lex.a

all: $(LIB)

$(LIB): $(OBJ)
	ar rcs $(LIB) $(OBJ)

$(OBJ): $(HDR)

clean:
	rm $(LIB) $(OBJ)
