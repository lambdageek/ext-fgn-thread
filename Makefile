
SOEXT:=dylib
#SOEXT:=so

all: Prog.exe libFoo.$(SOEXT)

.PHONY: all clean

Prog.exe: Prog.cs
	csc $<

libFoo.$(SOEXT): foo.c
	$(CC) -o $@ -shared -g $<

clean:
	-rm -f Prog.exe
	-rm -f libFoo.$(SOEXT)
	-rm -rf libFoo.dylib.dSYM/
