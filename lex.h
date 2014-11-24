
#ifndef BF_LEX_H_
#define BF_LEX_H_

// The lex struct is passed to all lex functions.
struct lex {
	// file is the current input file, given to lex_init
	FILE *file;
	// size is the specified size of the buffer, given to lex_init
	size_t size;
	// current length of to be emitted text
	int len;
	// length of lexed text, can differ from len when back'd
	int llen;
	// text buffer
	char *lexed;
	// state function
	void *(*func)(struct lex *);
};

typedef struct lex lex;

// allocate and initiate lexer
// takes the length of the lex buffer, the file to lex from,
// and a state function.
lex *lex_init(size_t sz, FILE *file, void *(*func)(lex *));

// free lexer
void lex_free(lex *l);

// run lexer state machine
// calls the state function passed to lex_init (l->func),
// looping to call state functions returned by it until one
// returns NULL.
void lex_state(lex *l);

// utility functions for lexing,
// these utilize the lex struct.

// dumps characters currently lexed,
// returns a non-zero value on error.
int lex_dump(lex *l);

// returns an allocated string of the currently
// lexed characters, then dumps the currently lexed characters.
// returns NULL on error.
char *lex_emit(lex *l);

// lexes one character into the lex buffer,
// returns the characer.
// on error returns negative value less than -1 (EOF).
int lex_next(lex *l);

// backs one character from the lex buffer (if it can),
// returns non-zero value on failure.
int lex_back(lex *l);

// peeks ahead one character,
// guarantees it will be avaliable on the next call to next.
// on error returns a negative value less than -1 (EOF).
int lex_peek(lex *l);

#endif // BF_LEX_H_
