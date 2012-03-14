#include "array-aho.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

typedef AhoCorasickTrie::Chars Chars;
typedef AhoCorasickTrie::Strings Strings;


Chars to_chars(char const* s)
{
   Chars chars;
   for (; *s; ++s) {
      chars.push_back(*s);
   }
   return chars;
}


bool test_contains(AhoCorasickTrie const& t, string s, bool expect = true)
{
   Chars k = to_chars(s.c_str());

   bool found = t.contains((char*)k.data(), k.size());
   if (found != expect) {
      cout << "FAIL: [" << s << "]" << (found ? " found" : " not found") << endl;
   }
   return found == expect;
}

void add_string(AhoCorasickTrie& trie, char const* s)
{
   Chars x = to_chars(s);
   trie.add_string((const char*)x.data(), x.size());
}

string aho_short(AhoCorasickTrie& trie, Chars text)
{
   int istart = 0;
   int iend = 0;
   AC_CHAR_TYPE const* data = text.data();

   trie.find_short((char const*)data, text.size(), &istart, &iend);

//   cout << "done find short" << endl;
//   cout << "end - start == " << out_end - out_start << endl;
   string rval;
   for (AC_CHAR_TYPE const* t = data + istart; t < data + iend; ++t) {
      rval.push_back((char)*t);
      cout << (char)*t;
   }
   cout << endl;
   return rval;
}

string aho_long(AhoCorasickTrie& trie, Chars text)
{
   int istart = 0;
   int iend = 0;
   AC_CHAR_TYPE const* data = text.data();

   trie.find_longest((char const*)data, text.size(), &istart, &iend);

   trie.find_longest((char const*)text.data(), text.size(),
                     &istart,
                     &iend);

   cout << "end - start == " << iend - istart << endl;
   string rval;
   for (AC_CHAR_TYPE const* t = data + istart; t < data + iend; ++t) {
      rval.push_back((char)*t);
      cout << (char)*t;
   }
   cout << endl;
   return rval;
}

void test_aho_canon()
{
   AhoCorasickTrie trie;
   add_string(trie, "ABCJ");
   add_string(trie, "ABCK");
   add_string(trie, "BCDE");
   add_string(trie, "CD");

//   trie.print();

   trie.compile();

   Chars text = to_chars("ABCDJ");
   string s = aho_short(trie, text);
   assert(s == "CD");
}


void test_aho_short_long()
{
   AhoCorasickTrie trie;
   add_string(trie, "SHE");
   add_string(trie, "HERS");
   add_string(trie, "HE");
   add_string(trie, "HIS");

//   trie.print();

   trie.compile();

   Chars text = to_chars("HERS");
   aho_short(trie, text);
   aho_long(trie, text);
}


void test_aho_continue_util(AhoCorasickTrie& trie, Chars text)
{
   cout << "test aho continue" << endl;
   size_t len_remaining = text.size();
   int found_istart = 0;
   int found_iend = 0;
   do {
      found_istart = found_iend;
      trie.find_short((char const*)text.data(), text.size(),
                      &found_istart, &found_iend);

      cout << "CONTINUE FOUND: ";
      for (AC_CHAR_TYPE const* t = text.data() + found_istart; t < text.data() + found_iend; ++t) {
         cout << (char)*t;
      }
      cout << endl;
   } while (found_istart < found_iend);
}


string test_trie(AhoCorasickTrie& trie, Chars text)
{
   trie.compile();

   int found_istart = 0;
   int found_iend = 0;

   trie.find_short((char const*)text.data(), text.size(),
                   &found_istart,
                   &found_iend);

   string rval;
   for (AC_CHAR_TYPE const* t = text.data() + found_istart;
        t < text.data() + found_iend; ++t) {
      rval.push_back((char)*t);
      cout << (char)*t;
   }
   cout << endl;
   return rval;
}

void test_empty()
{
   AhoCorasickTrie trie;

   cout << "test empty trie" << endl;
   string s = test_trie(trie, to_chars(""));
   assert(s == "");
}

void test_find_nothing()
{
   cout << "test find nothing" << endl;
   AhoCorasickTrie trie;

   add_string(trie, "SHE");
   add_string(trie, "HERS");
   add_string(trie, "HE");
   add_string(trie, "HIS");

   string s = test_trie(trie, to_chars("BANANA"));
   assert(s == "");
}

void test_ahox()
{
   cout << "test aho x" << endl;
   AhoCorasickTrie trie;
   add_string(trie, "ABC");
   add_string(trie, "XABCJ");
   add_string(trie, "XABCK");
   add_string(trie, "BCDE");
   add_string(trie, "CD");

   trie.compile();

   Chars text = to_chars("XABCDJQ");
   string s = aho_short(trie, text);
   assert(s == "ABC");
}

void test_not_at_front()
{
   AhoCorasickTrie trie;
   add_string(trie, "foo");
   add_string(trie, "bar");
   trie.compile();

   Chars text = to_chars("this is a foo message");
   string s = aho_short(trie, text);
   assert(s == "foo");
}

void test_aho_continue()
{
   //   {
   //   AhoCorasickTrie trie;
   //   add_string(trie, "quux");
   //   add_string(trie, "foo");
   //   add_string(trie, "bar");
   //   add_string(trie, "baz");
   //   trie.compile();
   //
   //   Chars text = to_chars("quux foo bar baz");
   //   test_aho_continue_util(trie, text);
   //   }

   {
   AhoCorasickTrie trie;
   add_string(trie, "python");
   add_string(trie, "perl");
   add_string(trie, "scheme");
   add_string(trie, "java");
   add_string(trie, "pythonperl");
   trie.compile();

   Chars text = to_chars("pythonperl");
   test_aho_continue_util(trie, text);
   }
}

void test_contained()
{
   AhoCorasickTrie trie;
   add_string(trie, "he");
   add_string(trie, "hers");
   add_string(trie, "erstwhile");
   Chars text = to_chars("xxherstwhilexx");
   test_aho_continue_util(trie, text);
}

void test_tail_chain()
{
   AhoCorasickTrie trie;
   add_string(trie, "pq");
//   add_string(trie, "p");
   add_string(trie, "q");
   Chars text = to_chars("pq");
   test_aho_continue_util(trie, text);
}

void test_mid_chain()
{
   AhoCorasickTrie trie;
   add_string(trie, "pqp");
   add_string(trie, "q");
   Chars text = to_chars("pqp");
   test_aho_continue_util(trie, text);
}

int main()
{
//   test_ahox();
//   test_find_nothing();
//   test_empty();
//   test_aho_continue();
   //   test_not_at_front();
//   test_contained();
//   test_tail_chain();
   test_mid_chain();
}
