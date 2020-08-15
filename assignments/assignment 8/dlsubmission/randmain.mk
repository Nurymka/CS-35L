randlibhw.so: randlibhw.c
	$(CC) $(CFLAGS) -shared -fPIC $^ -o $@
randlibsw.so: randlibsw.c
	$(CC) $(CFLAGS) -shared -fPIC $^ -o $@
randmain: randmain.o randcpuid.o
	$(CC) $(CFLAGS) -ldl -Wl,-rpath=$(PWD) $^ -o $@
