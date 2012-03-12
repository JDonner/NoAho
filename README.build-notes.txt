# python 2
$ python setup.py build
running build
running build_ext
building 'noaho' extension
creating build/temp.linux-x86_64-2.7
gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -c noaho.cpp -o build/temp.linux-x86_64-2.7/noaho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
gcc -pthread -fno-strict-aliasing -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python2.7 -c array-aho.cpp -o build/temp.linux-x86_64-2.7/array-aho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
creating build/lib.linux-x86_64-2.7
g++ -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions build/temp.linux-x86_64-2.7/noaho.o build/temp.linux-x86_64-2.7/array-aho.o -o build/lib.linux-x86_64-2.7/noaho.so
$ cp build/lib.linux-x86_64-2.7/noaho.so .
$ python test-noaho.py
.............xx.
----------------------------------------------------------------------
Ran 16 tests in 0.004s

OK (expected failures=2)
# python 3
$ rm -rf build
$ python3 setup.py build
running build
running build_ext
building 'noaho' extension
creating build
creating build/temp.linux-x86_64-3.2
gcc -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python3.2mu -c noaho.cpp -o build/temp.linux-x86_64-3.2/noaho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
gcc -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python3.2mu -c array-aho.cpp -o build/temp.linux-x86_64-3.2/array-aho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
creating build/lib.linux-x86_64-3.2
g++ -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions build/temp.linux-x86_64-3.2/noaho.o build/temp.linux-x86_64-3.2/array-aho.o -o build/lib.linux-x86_64-3.2/noaho.cpython-32mu.so
$ cp build/lib.linux-x86_64-3.2/noaho.cpython-32mu.so .
$ python3 test-noaho.py
.............xx.
----------------------------------------------------------------------
Ran 16 tests in 0.007s

OK (expected failures=2)

# Generating .cpp files with Cython
$ rm -f noaho.cpp && rm -rf build && rm -rf .objs && rm -f *.so && python3 cython_setup.py build_ext --inplace
running build_ext
cythoning noaho.pyx to noaho.cpp
building 'noaho' extension
creating build
creating build/temp.linux-x86_64-3.2
gcc -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python3.2mu -c noaho.cpp -o build/temp.linux-x86_64-3.2/noaho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
gcc -pthread -DNDEBUG -g -fwrapv -O2 -Wall -Wstrict-prototypes -fPIC -I/usr/include/python3.2mu -c array-aho.cpp -o build/temp.linux-x86_64-3.2/array-aho.o
cc1plus: warning: command line option ‘-Wstrict-prototypes’ is valid for C/ObjC but not for C++ [enabled by default]
g++ -pthread -shared -Wl,-O1 -Wl,-Bsymbolic-functions -Wl,-Bsymbolic-functions build/temp.linux-x86_64-3.2/noaho.o build/temp.linux-x86_64-3.2/array-aho.o -o /mnt/raid/projects/my-aho-corasick/cython-aho/noaho.cpython-32mu.so

$ cython --version
Cython version 0.15.1
$ which cython
/usr/scripts/cython
$ echo $PYTHONPATH
/home/jd/software/Cython-0.15.1/build/lib.linux-x86_64-3.2:

# This Cython was generated with Python 3; that's apparently necessary
