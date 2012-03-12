

######!/usr/bin/env python
"""Unit tests for Aho Corasick keyword string searching.
Many tests copyright Danny Yoo, the rest copyright Jeff Donner.
Because I think Danny's tests were GPL, so now is this whole file.
So, if you distribute this test code you must also distribute .. uh,
this test code (this doesn't apply to the whole package).
Jeff Donner, jeffrey.donner@gmail.com
"""

import sys
import noaho
import unittest


class AhoCorasickTest(unittest.TestCase):
    def setUp(self):
        self.tree = noaho.NoAho()

    def tearDown(self):
        self.tree = None

    def test_keyword_as_prefix_of_another(self):
        """According to John, there's a problem with the matcher.
        this test case should expose the bug."""
        self.tree.add('foobar')
        self.tree.add('foo')
        self.tree.add('bar')
#        self.tree.compile()
        self.assertEqual((3, 6, None), self.tree.find('xxxfooyyy'))
        self.assertEqual((0, 3, None), self.tree.find('foo'))
        self.assertEqual((3, 6, None), self.tree.find('xxxbaryyy'))

    def test_another_find(self):
        """Just to triangulate the search code.  We want to make sure
        that the implementation can do more than one search, at
        least."""
        self.tree.add("Python")
        self.tree.add("PLT Scheme")
#        self.tree.compile()
        self.assertEqual((19, 25, None),
                         self.tree.find("I am learning both Python and PLT Scheme"))
        self.assertEqual((0, 10, None),
                         self.tree.find("PLT Scheme is an interesting language."))


    def test_simple_construction(self):
        self.tree.add("foo")
        self.tree.add("bar")
#        self.tree.compile()
        self.assertEqual((10, 13, None),
                         self.tree.find("this is a foo message"))


    def test_find_longest(self):
        self.tree.add("a")
        self.tree.add("alphabet");
#        self.tree.compile()
        self.assertEqual((0, 1, None), self.tree.find("alphabet soup"));
        self.assertEqual((0, 8, None), self.tree.find_long("alphabet soup"))
        self.assertEqual((13, 14, None), self.tree.find_long("yummy, I see an alphabet soup bowl"))

    def test_find_with_whole_match(self):
        """Make sure that longest search will match the whole string."""
        longString = "supercalifragilisticexpialidocious"
        self.tree.add(longString)
#        self.tree.compile()
        self.assertEqual((0, len(longString), None),
                         self.tree.find(longString))

    def test_find_longest_with_whole_match(self):
        """Make sure that longest search will match the whole string."""
        longString = "supercalifragilisticexpialidocious"
        self.tree.add(longString)
#        self.tree.compile()
        self.assertEqual((0, len(longString), None),
                         self.tree.find_long(longString))

    def test_find_longest_with_no_match(self):
        self.tree.add("foobar")
#        self.tree.compile()
        self.assertEqual(None, self.tree.find_long("fooba"))


    def test_with_expected_non_match(self):
        """Check to see that we don't always get a successful match."""
        self.tree.add("wise man")
#        self.tree.compile()
        self.assertEqual(
            None,
            self.tree.find("where fools and wise men fear to tread"))


    def test_empty_construction(self):
        """Make sure that we can safely construct and dealloc a tree
        with no initial keywords.  Important because the C
        implementation assumes keywords exist on its dealloc, so we
        have to do some work on the back end to avoid silly segmentation
        errors."""
        tree = noaho.NoAho()
        del tree

    def test_embedded_nulls(self):
        """Check to see if we can accept embedded nulls"""
        self.tree.add("hell\0 world")
#        self.tree.compile()
        self.assertEqual(None, self.tree.find("ello\0 world"))
        self.assertEqual((0, 11, None), self.tree.find("hell\0 world"))


    def test_embedded_nulls_again(self):
        self.tree.add("\0\0\0")
#        self.tree.compile()
        self.assertEqual((0, 3, None), self.tree.find("\0\0\0\0\0\0\0\0"))


    def test_find_all_and_find_all_longest(self):
        self.tree.add("python")
        self.tree.add("perl")
        self.tree.add("scheme")
        self.tree.add("java")
        self.tree.add("pythonperl")
#        self.tree.compile()
        self.assertEqual([(0, 6, None), (6, 10, None), (10, 16, None), (16, 20, None)],
                         list(self.tree.findall("pythonperlschemejava")))
        self.assertEqual([(0, 10, None), (10, 16, None), (16, 20, None)],
                         list(self.tree.findall_long("pythonperlschemejava")))
        self.assertEqual([],
                         list(self.tree.findall("no pascal here")))
        self.assertEqual([],
                         list(self.tree.findall_long("no pascal here")))


    def test_add_find_add_find(self):
        pass

    def test_payload(self):
        class RandomClass(object):
            def __init__(self):
                pass
        obj = RandomClass()
        self.tree.add("python", "yes-python")
        self.tree.add("perl", "")
        self.tree.add("scheme", None)
        self.tree.add("lisp", [1,2,3])
        # no payload, comes out None
        self.tree.add("C++")
        self.tree.add("dylan", obj)

        self.assertEqual((0, 6, "yes-python"), self.tree.find("python"))
        self.assertEqual((0, 4, ""), self.tree.find("perl"))
        self.assertEqual((0, 6, None), self.tree.find("scheme"))
        self.assertEqual((0, 4, [1,2,3]), self.tree.find("lisp"))
        self.assertEqual((0, 3, None), self.tree.find("C++"))
        self.assertEqual((0, 5, obj), self.tree.find("dylan"))


    # reminder that we need to implement this
    @unittest.expectedFailure
    def test_subset(self):
        self.tree.add("he")
        self.tree.add("hers")
        self.assertEqual([(0, 2, None), (0, 4, None)], list(self.tree.findall("hers")))

    # reminder that we need to figure out which version we're in, and
    # test Python 2 unicode explicitly
    @unittest.expectedFailure
    def test_unicode_in_python2(self):
        self.assertEqual(True, False)


def main(args):
    unittest.main()


if __name__ == '__main__':
    main(sys.argv[1:])
