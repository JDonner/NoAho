Non-Overlapping Aho-Corasick Trie

Features:
- 'short' and 'long' (longest matching key) searches, both one-off and
  iteration over all non-overlapping keyword matches in some text.
- Works with both unicode and str in Python 2, and unicode in Python 3.
  NOTE: As everything is simply single UCS4 / UTF-32 codepoints under the
  hood, all substrings and input unicode must be normalized, ie any separate modifying marks must be
  folded into each codepoint. See:
     http://stackoverflow.com/questions/16467479/normalizing-unicode
  Or, theoretically, you could put into the tree all forms of the
  keywords you expect to see in your text. This is untested but it
  would probably work.
- Allows you to associate an arbitrary Python object payload with each
  keyword, and supports dict operations len(), [], and 'in' for the
  keywords (though no del or traversal).
- Does the 'compilation' (generation of Aho-Corasick failure links) of
  the trie on-demand, ie you could mix adding keywords and searching
  text, freely, but mostly it just relieves you of worrying about
  compiling.
- Can be used commercially, it's under the minimal, MIT license (if you
  somehow need a different license, ask me, I mean for it to be used).

Anti-Features:
- Will not find overlapped keywords (eg given keywords "abcde" and
  'defgh", will not find "defgh" in "abcdefgh"; would find both in
  "abcdedefgh"), unless you move along the string manually, one
  character at a time, which would defeat the purpose. The package
  'Acora' is an alternative package for this use.
- Lacking overlap, find[all]_short is kind of useless.
- Lacks key iteration and deletion from the mapping (dict) protocol
- Memory leaking untested (should be ok but ...)
- No /testcase/ for unicode in Python 2 (did manual test however)
- Unicode chars represented as ucs4, and, each character has its own
  hashtable, so it's relatively memory-heavy, but that's life nowadays.
- Requires a C++ compiler.

Bug reports and patches welcome of course!



To build and install, use either
  pip install noaho
or
  # Python 2
  python2 setup.py install # (or ... build, and copy the .so to where you want it)
  pip install
or
  # Python 3
  python3 setup.py install # (or ... build, and copy the .so to where you want it)


API:
    from noaho import NoAho
    trie = NoAho()
'text' below applies to str and unicode in Python 2, or unicode in Python 3 (all there is)
    trie.add(key_text, optional payload)
    (key_start, key_end, key_value) = trie.find_short(text_to_search)
    (key_start, key_end, key_value) = trie.find_long(text_to_search)
    (key_start, key_end, key_value) = trie.findall_short(text_to_search)
    (key_start, key_end, key_value) = trie.findall_long(text_to_search)
    # keyword = text_to_search[key_start:key_end]
    trie['keyword] = key_value
    key_value = trie.find_long(text_to_search)
    assert len(trie)
    assert keyword in trie

Examples:
    >>> a = NoAho()
    >>> a.add('ms windows')
    >>> a.add('ms windows 2000', "this is canonical")
    >>> a.add('windows', None)
    >>> a.add('windows 2000')
    >>> a['apple'] = None
    >>> text = 'windows 2000 ms windows 2000 windows'
    >>> for k in a.findall_short(text):
    ...     print text[k[0]:k[1]]
    ...
    windows
    ms windows
    windows
    >>> for k in a.findall_long(text):
    ...     print text[k[0]:k[1]]
    ...
    windows 2000
    ms windows 2000
    windows

Mapping (dictionary) methods:
    trie = NoAho()
    trie['apple'] = apple_handling_function
    trie['orange'] = Orange()
    trie.add('banana') # payload will be None
    trie['pear'] # will give key error
    assert isinstance(trie['orange'], Orange)
    assert 'banana' in trie
    len(trie)
    # No del;
    # no iteration over keys

The 'find[all]_short' forms are named as long and awkwardly as they are,
to leave plain 'find[all]' free if overlapping matches are ever implemented.


For the fullest spec of what the code will and will not do, check out
test-noaho.py (run it with: python[3] test-noaho.py)

Untested: whether the payload handling is complete, ie that there are no
memory leaks. It should be correct though.


Regenerating the Wrapper:
- Needs a C++ compiler and Cython.

You should not need to use Cython to build and use this module, but if
you want to make changes to the module itself, there is a script:

  test-all-configurations.sh

which should with minor configuration tweaking, rebuild and test
against both python 2 and 3. It assumes you've made a couple of
virtualenvs, and have a Cython tarball in the top directory.  Note
that the python you used to install Cython should be the same as the
one you use to do the regeneration, because the regeneration setup
includes a module Cython.Distutils, from the installation.

Cython generates python-wrapper noaho.cpp from noaho.pyx (be careful
to distinguish it from the misnamed array-aho.* (it uses hash tables),
which is the original C++ code).
