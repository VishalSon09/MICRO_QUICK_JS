#ifdef CONFIG_ARM32
#endif

#ifdef CONFIG_WIN32
#endif


CFLAGS=-Wall -Werror -std=c11 -pedantic 
MQJS_OBJS=
LIBS=-lm

mqjs$(EXE): $(MQJS_OBJS)
	$(CC) $(CFLAGS) -o mqjs$(EXE) $(MQJS_OBJS) $(LIBS)

clean:
	rm -f *.o *.d *~ tests/*.d tests/*~ test_builtin.bin mqjs_stdlib.h \
	mquickjs_build_atoms mquickjs_atom.h mqjs_example example_stdlib   \
	example_stdlib.h $(PROGS) $(TEST_PROGS)
