# Copyright (C) 2012 Jeff Donner
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge,
# publish, distribute, sublicense, and/or sell copies of the Software,
# and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions: The above copyright notice and
# this permission notice shall be included in all copies or substantial
# portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT
# WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
# TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
# ('expat' version of the MIT license)

# rm -f noaho.cpp && rm -rf build && rm -rf .objs && rm -f *.so && python3 cython_deep_build_setup.py build_ext --inplace

from cpython.ref cimport Py_INCREF
from cpython.ref cimport Py_DECREF
from cpython.version cimport PY_MAJOR_VERSION

cdef get_as_ucs4(object text):
    if isinstance(text, unicode) or (PY_MAJOR_VERSION < 3 and isinstance(text, str)):
        # [4:] to skip byte-order mark (BOM) - we're always native.
        ucs4_data = text.encode('utf-32')[4:]
    else:
        raise ValueError("Requires unicode or str text input, got %s" % type(text))

    # http://wiki.cython.org/FAQ#HowdoIpassaPythonstringparameterontoaClibrary.3F
    return ucs4_data, len(ucs4_data) / 4


cdef py_from_void_payload(void* void_payload):
    cdef object py_payload
    if void_payload == <void*>0:
        py_payload = None
    else:
        py_payload = <object>void_payload
    return py_payload


cdef extern from "array-aho.h":
    cdef cppclass AhoCorasickTrie:
        AhoCorasickTrie()
# http://groups.google.com/group/cython-users/browse_thread/thread/d0c57ee21a278db7/ffb90297821483d8?lnk=gst&q=map+return+object#ffb90297821483d8
        void add_string(char* text, int len, void* payload)
        void compile()
        void* find_short(char* text, int len,
                         int* out_start, int* out_end)
        void* find_longest(char* text, int len,
                           int* out_start, int* out_end)
        int num_keys()
        void* get_payload(char* text, int len)
        int contains(char* text, int len)


cdef class NoAho:
    cdef AhoCorasickTrie *thisptr
    cdef object payloads_to_decref
    def __cinit__(self):
        self.thisptr = new AhoCorasickTrie()
        self.payloads_to_decref = []

    def __dealloc__(self):
        for payload in self.payloads_to_decref:
            Py_DECREF(payload)
        del self.thisptr

    def __len__(self):
        return self.thisptr.num_keys()

    def __contains__(self, key_text):
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        ucs4_data, num_ucs4_chars = get_as_ucs4(key_text)
        return self.thisptr.contains(ucs4_data, num_ucs4_chars)

    def __getitem__(self, text):
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        cdef void* void_payload
        cdef long long payload_as_int
        ucs4_data, num_ucs4_chars = get_as_ucs4(text)
        void_payload = self.thisptr.get_payload(ucs4_data, num_ucs4_chars)
        payload_as_int = <long long>void_payload
        # No valid ptr has the lowest 3 bits set, thus we can smuggle
        # a signal in there that the key wasn't present (-1, which has
        # all bits set).
        if payload_as_int == -1:
            raise KeyError(text)
        py_payload = py_from_void_payload(void_payload)
        return py_payload

    def __setitem__(self, text, py_payload):
        self.add(text, py_payload)

# This is harder...
#    def __iter__(self):
#        return

    def add(self, text, py_payload = None):
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        cdef void* void_payload

        ucs4_data, num_ucs4_chars = get_as_ucs4(text)

        if num_ucs4_chars == 0:
            raise ValueError("Key cannot be empty (would cause Aho-Corasick automaton to spin)")

        if py_payload is None:
            void_payload = <void*>0
        else:
            Py_INCREF(py_payload)
            self.payloads_to_decref.append(py_payload)
            void_payload = <void*>py_payload

        self.thisptr.add_string(ucs4_data, num_ucs4_chars, void_payload)

# Nice-to-have...
#    def add_many(self):
#        pass

    def compile(self):
        self.thisptr.compile()

    def find_short(self, text):
        cdef int start, end
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        cdef void* void_payload
        cdef object py_payload
        ucs4_data, num_ucs4_chars = get_as_ucs4(text)
        start = 0
        end = 0
        void_payload = self.thisptr.find_short(ucs4_data, num_ucs4_chars,
                                               &start, &end)
        py_payload = py_from_void_payload(void_payload)
        if start == end:
            return None, None, None
        else:
            return start, end, py_payload

    def find_long(self, text):
        cdef int start, end
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        cdef void* void_payload
        cdef object py_payload
        ucs4_data, num_ucs4_chars = get_as_ucs4(text)
        start = 0
        end = 0
        void_payload = self.thisptr.find_longest(ucs4_data, num_ucs4_chars,
                                                 &start, &end)
        py_payload = py_from_void_payload(void_payload)
        if start == end:
            return None, None, None
        else:
            return start, end, py_payload

# http://thread.gmane.org/gmane.comp.python.cython.user/1920/focus=1921
    def findall_short(self, text):
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        ucs4_data, num_ucs4_chars = get_as_ucs4(text)
        # 0 is flag for 'short'
        return AhoIterator(self, ucs4_data, num_ucs4_chars, 0)

    def findall_long(self, text):
        cdef bytes ucs4_data
        cdef int num_ucs4_chars
        ucs4_data, num_ucs4_chars = get_as_ucs4(text)
        # 1 is flag for 'long'
        return AhoIterator(self, ucs4_data, num_ucs4_chars, 1)


# iterators (though, there was another, better one! -- try __citer__ and __cnext__)
# http://thread.gmane.org/gmane.comp.python.cython.user/2942/focus=2944

# http://groups.google.com/group/cython-users/browse_thread/thread/69b6eeb930826bcb/0b20e6e265e719a3?lnk=gst&q=iterator#0b20e6e265e719a3
cdef class AhoIterator:
    cdef NoAho aho_obj
    cdef bytes ucs4_data
    cdef int num_ucs4_chars
    cdef int start, end, want_longest

    def __init__(self, aho_obj, ucs4_data, num_ucs4_chars, want_longest):
        self.aho_obj = aho_obj
        self.ucs4_data = ucs4_data
        self.num_ucs4_chars = num_ucs4_chars
        self.start = 0
        self.end = 0
        self.want_longest = want_longest

    # I belieeeeve we don't need a __dealloc__ here, that Cython bumps
    # Python objects (self.aho_obj when I assign, and derefs them
    # when we die.

    def __iter__(self):
        return self

    def __next__(self):
        cdef void* void_payload
        cdef object py_payload
        cdef int out_start, out_end

        # I figured a runtime switch was worth not having 2
        # iterator types.
        if self.want_longest:
            void_payload = self.aho_obj.thisptr.find_longest(
                self.ucs4_data, self.num_ucs4_chars,
                &self.start, &self.end)
        else:
            void_payload = self.aho_obj.thisptr.find_short(
                self.ucs4_data, self.num_ucs4_chars,
                &self.start, &self.end)

        py_payload = py_from_void_payload(void_payload)
        if self.start < self.end:
            # set up for next time
            out_start = self.start
            out_end = self.end
            self.start = self.end
            return out_start, out_end, py_payload
        else:
            raise StopIteration
