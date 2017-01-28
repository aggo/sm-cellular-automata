XLIBS = -L/usr/X11R6/lib -lX11
XINCLUDE = -I/usr/X11R6/include/

main.c: main.c
	gcc main.c  -o main -lm -O3 
plot: plot.c 
	gcc plot.c -o plot -lm  $(XLIBS) $(XINCLUDE)
