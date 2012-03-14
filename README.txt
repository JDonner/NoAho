Non-Overlapping Aho-Corasick

Features:
- 'short' and 'long' search; short probably isn't that useful without overlap.
- Allows you to associate an arbitrary Python-object payload with each keyword;
- Works with unicode and str in Python 2, and unicode in Python 3
  (it's all UCS4 under the hood).
- Can be used commercially, it has a minimal license.
- Does the 'compilation' (generation of Aho-Corasick failure links) on-demand

To build and install, use either
  # Python 2
  python setup.py install (or build, and copy the .so to where you want it)
or
  # Python 3
  python3 setup.py install (or build, and copy the .so to where you want it)


'text' below applies to str or unicode, in Python 2 or 3
    t.add(text, optional payload)
    (start, end, payload) = t.find_short(text)
    (start, end, payload) = t.find_long(text)
    (start, end, payload) = t.findall_short(text)
    (start, end, payload) = t.findall_long(text)

    If no payload was given, it's None.
    The 'find[all]_short' forms are named as long and awkwardly as
    they are, to leave plain 'find[all]' free if overlapping ever gets
    implemented.

Written in C++ and Cython.

You should not need to use Cython to build and use this module, but if
you want to change it and regenerate, there's a build command line at the top of
noaho.pyx.  To allow the generated .cpp files to be used in both Py2
and Py3, use a Python 3-built Cython. I used Cython-0.15.1, but even
as far back as 0.13 should work (for that version though, you'll have
to manually use a c++ compiler to do the last step, of linking).


NOTE: this does not find overlapping keywords (of course you can overlap
things manually but, that defeats the purpose).

For the fullest spec of what the code will and will not so, check out
test-noaho.py (python[3] test-noaho.py)

Untested: whether the payload handling is proper, ie that there are no
memory leaks. It should be correct though.

Contains

