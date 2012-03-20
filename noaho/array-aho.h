#ifndef AHO_CORASICK_TRIE_H
#define AHO_CORASICK_TRIE_H

// Copyright (C) 2012 Jeff Donner
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy,
// modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// (MIT 'expat' license)


#if YOUR_COMPILER_SUPPORTS_UNORDERED_MAP
#  include <unordered_map>
#else
#  include "HashMap.h"
#endif

#include <vector>
#include <cassert>
#include <cstddef>
#include <iosfwd>

// We could include Python.h like this and return PyObject*s directly
// but, the author just prefers the cleanliness of not having Python
// dependencies in here. It does complicate noaho.pyx though, so maybe
// it's foolish. I seem to recall it working, at least.
// for PyObject
//#include "Python.h"
// typedef PyObject* PayloadT;

typedef int AC_CHAR_TYPE;

typedef void* PayloadT;
class AhoCorasickTrie;

struct Node {
   friend class AhoCorasickTrie;
   typedef int Index;

   Node(PayloadT payload = 0)
   : length(0)
   , ifailure_state(0)
   , payload(payload)
   // 1 is the initial reserve; it will grow automatically.
   // Danny Yoo has the right idea, of using a linear search
   // when the number of children is small (his cutoff: <= 3)
   // The result is still O(1), as it's an upper-bounded length.
   // Above that length of course it falls back to a hash map.
   , children(1)
   {}

#if YOUR_COMPILER_SUPPORTS_UNORDERED_MAP
   typedef std::unordered_map<AC_CHAR_TYPE, Index> Children;
#else
   typedef PocoJgd::HashMap<AC_CHAR_TYPE, Index> Children;
#endif

   Index child_at(AC_CHAR_TYPE c) const {
      Children::const_iterator child = children.find(c);
      // since these are indices, 0 is valid, so invalid is < 0
      return child == children.end() ? -1 : child->second;
   }

   void set_child(AC_CHAR_TYPE c, Index idx) {
      assert(idx);
      children[c] = idx;
   }

   unsigned short length;
   Index ifailure_state;
   PayloadT payload;

   Children children;
};

std::ostream& operator<<(std::ostream& os, Node const& node);

class AhoCorasickTrie
{
public:
   typedef typename Node::Index Index;
   typedef std::vector<AC_CHAR_TYPE> Chars;
   typedef std::vector<Chars> Strings;

public:
   AhoCorasickTrie()
   : is_compiled(false)
   {
      // born with root node
      nodes.push_back(Node());
   }

   void add_string(char const* s, size_t n, PayloadT payload = 0);

   PayloadT find_short(char const* s, size_t n,
                       int* inout_start,
                       int* out_end);

   PayloadT find_longest(char const* s, size_t n,
                         int* inout_start,
                         int* out_end);

   // Only makes fail links but, I'm hitching on the idea from regexps.
   // You never need to use this, it's done automatically, it's just here
   // in case you want manual control.
   void compile() { ensure_compiled(); }

   void print() const;

   int contains(char const*, size_t n) const;

   int num_keys() const;

   /// Returns either a valid ptr (including 0) or, -1 cast as a ptr.
   PayloadT get_payload(char const* s, size_t n) const;


private:
   static bool is_valid(Index ichild) { return 0 <= ichild; }
   void ensure_compiled() { if (not is_compiled) make_failure_links(); }

   /// Does the actual 'compilation', ie builds the failure links
   void make_failure_links();
   /// Clears the failure links. Given that we do a breadth-first
   /// traversal I'm not sure this is necessary, actually.
   void clear_failure_links();

   Index child_at(Index i, AC_CHAR_TYPE a) const {
      Index ichild = nodes[i].child_at(a);
      // The root is a special case - every char that's not an actual
      // child of the root, points back to the root.
      if (not is_valid(ichild) and i == 0)
         ichild = 0;
      return ichild;
   }

   Node::Index add_node() {
      nodes.push_back(Node());
      return nodes.size() - 1;
   }

   Strings follow_failure_chain(Node::Index inode,
                                Chars chars, int istart,
                                int ifound_at) const;

   // root is at 0 of course.
   // <Node>s are stored by value so, it's expensive when you're
   // building the tree (but still O(n)), but, it saves some bytes in
   // pointers (esp. on 64 bit machines) and enhances continuity (ie
   // prefetchability of cache)). At least, that's the author's
   // untested reason for doing it this way.
   typedef std::vector<Node> Nodes;
   Nodes nodes;
   mutable bool is_compiled;
};

// for debugging
std::ostream& operator<<(std::ostream& os, AhoCorasickTrie::Chars const& text);

std::ostream& operator<<(std::ostream& os, AhoCorasickTrie::Strings const& texts);

#endif // AHO_CORASICK_TRIE_H
