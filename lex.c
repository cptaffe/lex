// lexer

#include <stdio.h>
#include <stdlib.h>
#include <errno.h> // errorno
#include <string.h> // memset
#include <stdint.h> // integer types
#include <unistd.h> // read/write

extern int errno;

#include "lex.h"

// allocate and initialize the lexer struct
// returns a pointer to an allocated lex if successful,
// else returns NULL.
lex *lex_init(size_t sz, FILE *file, void *(*func)(lex *)) {
	lex *l;

	// allocate lex
	if ((l = malloc(sizeof (lex))) == NULL) {
		return NULL;
	} else {
		// set size of buffer
		l->size = sz;

		// set file to read from
		l->file = file;

		// set first state function
		l->func = func;

		// zero lens
		l->len = 0;
		l->llen = 0;

		// null data
		l->data = NULL;

		// allocate sz bytes for buf
		if ((l->lexed = malloc(sz)) == NULL) {
			free(l); // clean up.
			return NULL;
		} else {
			return l;
		}
	}
}

// frees the lex struct
void lex_free(lex *l) {
	free(l->lexed);
	free(l);
}

// dump currently lexed characters while
// preserving back'd characters.
// errors by returning a non-zero value.
int lex_dump(lex *l) {
	// if there are back'd characters that need preserving.
	if ((l->llen -= l->len) > 0) {
		// copy back preserved (back'd) characters
		// using memmove because it allows for overlapping buffers.
		if ((l->lexed = memmove(l->lexed, &l->lexed[l->len], l->llen)) == NULL) {

			// note the error
			#ifdef DEBUG
			fprintf(stderr, "lex_dump: movement of lex buffer failed: %s.\n", strerror(errno));
			#endif

			return 1; // error
		}
	}

	// reset length
	l->len = 0;

	return 0; // success
}

// returns an allocated string with a copy of the
// currently lexed characters, also dumps.
char *lex_emit(lex *l) {
	char *copy;

	// allocate an extra byte for null termination
	if ((copy = malloc(l->len + 1)) == NULL) {

		// note the error
		#ifdef DEBUG
		fprintf(stderr, "lex_emit: allocation of emittable buffer failed: %s.\n", strerror(errno));
		#endif

		return NULL; // error
	} else {
		// using memcpy because memory does not overlap.
		if ((copy = memcpy(copy, l->lexed, l->len)) == NULL) {

			// note the error
			#ifdef DEBUG
			fprintf(stderr, "lex_emit: copying of lex buffer failed: %s.\n", strerror(errno));
			#endif

			return NULL; // error
		} else {
			copy[l->len] = '\0'; // null terminate
			if (lex_dump(l)) {
				return NULL; // error
			} else {
				return copy;
			}
		}
	}
}

// gets the next character, either a back'd
// character or a character from the file.
int lex_next(lex *l) {

	// reallocate 'lexed' buffer when it runs out of space
	// simply doubles the size.
	if (l->len >= l->size) {

		// double buffer size
		l->size *= 2;
		if ((l->lexed = realloc(l->lexed, l->size)) == NULL) {
			// reallocation of lex buffer failed
			// error by signalling end of input.

			// note the error
			#ifdef DEBUG
			fprintf(stderr, "lex_next: reallocation of lex buffer failed: %s.\n", strerror(errno));
			#endif

			// error
			return -2;
		}
	}

	// if there are back'd characters
	if (l->llen > l->len) {
		return l->lexed[l->len++];
	} else {
		// read character from file
		char c = getc(l->file);

		// don't save EOF, not a real character.
		if (c != EOF) {
			l->lexed[l->len++] = c;
			l->llen++; // increase lexed length
		}

		return (int) c;
	}
}

// backup a character, leaving llen at its current position.
int lex_back(lex *l) {
	if (l->len > 0) {
		l->len--;
		return 0; // success
	} else {
		return 1; // error
	}
}

// get the next character (either back'd or from file),
// and backup so it can be next'd again.
int lex_peek(lex *l) {
	int c = lex_next(l);
	if (c < -1) {

		// note error
		#ifdef DEBUG
		fprintf(stderr, "lex_peek: cannot get next character: %s.\n", strerror(errno));
		#endif

		// error
		return -2;
	} else {

		// do not back on EOF
		if (c != EOF && lex_back(l)) {

			// note error
			#ifdef DEBUG
			fprintf(stderr, "lex_peek: cannot back character: %s.\n", strerror(errno));
			#endif

			// error.
			return -3;
		} else {
			return c;
		}
	}
}

// finite state machine!
void lex_state(lex *l) {
	// loop until a state function returns NULL.
	while (l->func != NULL) {
		// type conversion for simplicity.
		l->func = (void *(*)(lex *)) l->func(l);
	}
}
