
######!/usr/bin/env python
"""Unit tests for Aho Corasick keyword string searching.
Many tests copyright Danny Yoo, the rest copyright Jeff Donner.
Because I think Danny's tests were GPL, so, now, is this whole file.
So, if you distribute this test code you must also distribute .. uh,
this test code (this doesn't apply to the whole package).
Jeff Donner, jeffrey.donner@gmail.com
"""

import sys
from noaho import NoAho
import unittest


class AhoCorasickTest(unittest.TestCase):
    def setUp(self):
        self.tree = NoAho()

    def tearDown(self):
        self.tree = None


    def test_keyword_as_prefix_of_another(self):
        """According to John, there's a problem with the matcher.
        this test case should expose the bug."""
        self.tree.add('foobar')
        self.tree.add('foo')
        self.tree.add('bar')
        self.assertEqual((3, 6, None), self.tree.find_short('xxxfooyyy'))
        self.assertEqual((0, 3, None), self.tree.find_short('foo'))
        self.assertEqual((3, 6, None), self.tree.find_short('xxxbaryyy'))


    def test_another_find(self):
        """Just to triangulate the search code.  We want to make sure
        that the implementation can do more than one search, at
        least."""
        self.tree.add("Python")
        self.tree.add("PLT Scheme")
        self.assertEqual((19, 25, None),
                         self.tree.find_short("I am learning both Python and PLT Scheme"))
        self.assertEqual((0, 10, None),
                         self.tree.find_short("PLT Scheme is an interesting language."))


    def test_simple_construction(self):
        self.tree.add("foo")
        self.tree.add("bar")
        self.assertEqual((10, 13, None),
                         self.tree.find_short("this is a foo message"))


    def test_find_longest(self):
        self.tree.add("a")
        self.tree.add("alphabet");
        self.assertEqual((0, 1, None), self.tree.find_short("alphabet soup"));
        self.assertEqual((0, 8, None), self.tree.find_long("alphabet soup"))
        self.assertEqual((13, 14, None), self.tree.find_long("yummy, I see an alphabet soup bowl"))


    def test_find_with_whole_match(self):
        """Make sure that longest search will match the whole string."""
        longString = "supercalifragilisticexpialidocious"
        self.tree.add(longString)
        self.assertEqual((0, len(longString), None),
                         self.tree.find_short(longString))


    def test_find_longest_with_whole_match(self):
        """Make sure that longest search will match the whole string."""
        longString = "supercalifragilisticexpialidocious"
        self.tree.add(longString)
        self.assertEqual((0, len(longString), None),
                         self.tree.find_long(longString))

    def test_find_longest_with_no_match(self):
        self.tree.add("foobar")
        self.assertEqual((None, None, None), self.tree.find_long("fooba"))


    def test_with_expected_non_match(self):
        """Check to see that we don't always get a successful match."""
        self.tree.add("wise man")
        self.assertEqual((None, None, None),
                         self.tree.find_short("where fools and wise men fear to tread"))


    def test_reject_empty_key(self):
        self.assertRaises(ValueError, self.tree.add, "")


    def test_empty_construction(self):
        """Make sure that we can safely construct and dealloc a tree
        with no initial keywords.  Important because the C
        implementation assumes keywords exist on its dealloc, so we
        have to do some work on the back end to avoid silly segmentation
        errors."""
        tree = NoAho()
        del tree


    def test_embedded_nulls(self):
        """Check to see if we can accept embedded nulls"""
        self.tree.add("hell\0 world")
        self.assertEqual((None, None, None), self.tree.find_short("ello\0 world"))
        self.assertEqual((0, 11, None), self.tree.find_short("hell\0 world"))


    def test_embedded_nulls_again(self):
        self.tree.add("\0\0\0")
        self.assertEqual((0, 3, None), self.tree.find_short("\0\0\0\0\0\0\0\0"))


    def test_findall_and_findall_longest(self):
        self.tree.add("python")
        self.tree.add("perl")
        self.tree.add("scheme")
        self.tree.add("java")
        self.tree.add("pythonperl")
        self.assertEqual([(0, 6, None), (6, 10, None), (10, 16, None), (16, 20, None)],
                         list(self.tree.findall_short("pythonperlschemejava")))
        self.assertEqual([(0, 10, None), (10, 16, None), (16, 20, None)],
                         list(self.tree.findall_long("pythonperlschemejava")))
        self.assertEqual([],
                         list(self.tree.findall_short("no pascal here")))
        self.assertEqual([],
                         list(self.tree.findall_long("no pascal here")))

    def test_bug_no_2_competing_longests(self):
        """Previously we'd return the /last/ key found, now we look forward
        while there are contiguous candidate keys, and actually return the
        longest.
        """
        self.tree.add('cisco', 'cisco')
        self.tree.add('em', 'em')
        self.tree.add('cisco systems australia', 'cisco systems')
        self.assertEqual([(0, 5, 'cisco'), (10, 12, 'em')],
                         list(self.tree.findall_long('cisco systems')))

    def test_add_and_find_mix_freely(self):
        text = """We got pickles and crocks, We got bagels and lox"""
        self.tree.add('lox')
        self.assertEqual((45, 48, None), self.tree.find_long(text))
        self.tree.add('pickles')
        self.assertEqual((7, 14, None), self.tree.find_long(text))


    def test_explicit_compilation_still_ok(self):
        # ... but vestigial
        text = """We got pickles and crocks, We got bagels and lox"""
        self.tree["lox"] = None
        self.tree.compile()
        self.assertEqual((45, 48, None), self.tree.find_long(text))
        self.tree["pickles"] = None
        self.assertEqual((7, 14, None), self.tree.find_long(text))


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

        self.assertEqual((0, 6, "yes-python"), self.tree.find_short("python"))
        self.assertEqual((0, 4, ""), self.tree.find_short("perl"))
        self.assertEqual((0, 6, None), self.tree.find_short("scheme"))
        self.assertEqual((0, 4, [1,2,3]), self.tree.find_short("lisp"))
        self.assertEqual((0, 3, None), self.tree.find_short("C++"))
        self.assertEqual((0, 5, obj), self.tree.find_short("dylan"))


    def test_dict_style_get_and_set(self):
        self.tree['foo'] = 5
        self.assertEqual(5, self.tree['foo'])


    def test_dict_style_set_empty_key(self):
        # equivalent to self.tree[''] = None
        # __setitem__ implements this part of the [] protocol
        self.assertRaises(ValueError, self.tree.__setitem__, '', None)


    def test_dict_style_set_nonstring_key(self):
        # equivalent to self.tree[''] = None
        # __setitem__ implements this part of the [] protocol
        self.assertRaises(ValueError, self.tree.__setitem__, 6, None)
        self.assertRaises(ValueError, self.tree.__setitem__, None, None)
        self.assertRaises(ValueError, self.tree.__setitem__, [], None)


    def test_dict_style_get_unseen_key(self):
        # __getitem__ implements this part of the [] protocol
        self.assertRaises(KeyError, self.tree.__getitem__, 'unseen')
        self.assertRaises(KeyError, self.tree.__getitem__, '')


    def test_dict_style_containment(self):
        self.tree['foo'] = 5
        self.assertEqual(True, 'foo' in self.tree)
        self.assertEqual(False, '' in self.tree)
        self.assertEqual(False, 'fo' in self.tree)
        self.assertEqual(False, 'o' in self.tree)
        self.assertEqual(False, 'oo' in self.tree)
        self.assertEqual(False, 'f' in self.tree)


    def test_dict_style_len(self):
        self.tree['a'] = None
        self.tree['b'] = [1,2]
        self.tree['c'] = 12
        self.assertEqual(3, len(self.tree))


    # reminder that we need to figure out which version we're in, and
    # test Python 2 unicode explicitly
    @unittest.expectedFailure
    def test_unicode_in_python2(self):
        self.assertEqual(True, False)

    # key iteration is unimplemented
    @unittest.expectedFailure
    def test_iteration(self):
        self.tree.add("Harry")
        self.tree.add("Hermione")
        self.tree.add("Ron")
        self.assertEqual(set("Harry", "Hermione", "Ron"), set(self.tree.keys()))


    # reminder that we need to implement findall_short
    @unittest.expectedFailure
    def test_subset(self):
        self.tree.add("he")
        self.tree.add("hers")
        self.assertEqual([(0, 2, None), (0, 4, None)], list(self.tree.findall_short("hers")))


def main(args):
    unittest.main()


if __name__ == '__main__':
    main(sys.argv[1:])
