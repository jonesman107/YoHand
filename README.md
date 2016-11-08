# Setup

Installation can be done with the following commands:

```
git clone https://github.com/libpd/libpd.git
cd libpd
git submodule init
git submodule update
# on OS X change 'g++' to 'clang' in the cpplib target in ./Makefile
make UTIL=true
make cpplib UTIL=true
```

# Bugs

Occasional segfaults occur.  This seems to be a bug in PD.  To fix it, change lines 70 and 71 in `libpd/pure-data/src/m_sched.c` from

```
while (x2->c_next != x) x2 = x2->c_next;
x2->c_next = x->c_next;
```

to

```
while (x2 && x2->c_next != x) x2 = x2->c_next;
if (x2) x2->c_next = x->c_next;
```

then rebuild libpd, and `make clean && make` YoHand.
