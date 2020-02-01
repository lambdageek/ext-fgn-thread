
Example that creates a native thread and calls Mono

There are a couple of `#if 0` blocks in [foo.c](foo.c) that can be changed.

Build with:
```
$ make
```

Run with:
```
MONO_PATH=path/to/net_4_x MONO_CONFIG=path/to/etc/config lldb path/to/mono -- Prog.exe
```

