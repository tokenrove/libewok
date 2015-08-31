
CFLAGS = -std=gnu11 -Wall -Wextra -g -O3 -I.

.PHONY: default clean

TESTS := test/test_logical test/ewah-compare-raw

default: $(TESTS)

EWOK_O := bitmap.o ewah_rlw.o ewah_io.o ewah_bitmap.o
test/test_logical: test/test_logical.o $(EWOK_O)
test/ewah-compare-raw: test/ewah-compare-raw.o indexed_ewah.o $(EWOK_O)

clean:
	$(RM) *.o test/*.o $(TESTS)
