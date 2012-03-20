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

#include "array-aho.h"
#include <queue>
#include <iostream>
#include <utility>

// http://www.cs.uku.fi/~kilpelai/BSA05/lectures/slides04.pdf

using namespace std;


std::ostream& operator<<(std::ostream& os, Node const& node)
{
   for (Node::Children::const_iterator i = node.children.begin(),
           end = node.children.end();
        i != end; ++i) {
      os << (char)i->first << ';';
   }
   os << ((0 < node.length) ? " (term) " : "()");
   os << " and failure: " << node.ifailure_state;
   os << std::endl;
   return os;
}

std::ostream& operator<<(std::ostream& os, AhoCorasickTrie::Chars const& text)
{
   for (AhoCorasickTrie::Chars::const_iterator i = text.begin(), end = text.end();
        i != end; ++i) {
      os << (char)*i;
   }
   return os;
}

std::ostream& operator<<(std::ostream& os, AhoCorasickTrie::Strings const& texts)
{
   for (AhoCorasickTrie::Strings::const_iterator i = texts.begin(), end = texts.end();
        i != end; ++i) {
      os << '[' << *i << ']' << endl;
   }
   return os;
}


AhoCorasickTrie::Chars to_chars(char const* s, int num_ucs4_chars)
{
   AC_CHAR_TYPE const* ucs4 = (AC_CHAR_TYPE const*)s;
   AC_CHAR_TYPE const* u = ucs4;

   AhoCorasickTrie::Chars chars;
   for (u = ucs4; u < ucs4 + num_ucs4_chars; ++u) {
      chars.push_back(*u);
   }
   return chars;
}


void AhoCorasickTrie::add_string(char const* char_s, size_t n,
                                 PayloadT payload)
{
   AC_CHAR_TYPE const* c = reinterpret_cast<AC_CHAR_TYPE const*>(char_s);

   Index iparent = 0;
   Index ichild = 0;
   for (size_t i = 0; i < n; ++i, ++c) {
      // Don't need the node here, it's behind the scenes.
      // on the other hand we don't care about the speed of adding
      // strings.
      ichild = nodes[iparent].child_at(*c);
      if (not is_valid(ichild)) {
         ichild = add_node();
         nodes[iparent].set_child(*c, ichild);
      }
      iparent = ichild;
   }
   nodes[ichild].payload = payload;
   nodes[ichild].length = n;

   if (is_compiled) {
      clear_failure_links();
      is_compiled = false;
   }
}


int AhoCorasickTrie::contains(char const* char_s, size_t n) const
{
   AC_CHAR_TYPE const* c = reinterpret_cast<AC_CHAR_TYPE const*>(char_s);
   Index inode = 0;
   for (size_t i = 0; i < n; ++i, ++c) {
      inode = nodes[inode].child_at(*c);
      if (inode < 0) {
         return 0;
      }
   }

   return nodes[inode].length ? 1 : 0;
}

int AhoCorasickTrie::num_keys() const
{
   int num = 0;
   for (Nodes::const_iterator it = nodes.begin(), end = nodes.end();
        it != end; ++it) {
      if (it->length)
         ++num;
   }

   return num;
}

PayloadT AhoCorasickTrie::get_payload(char const* s, size_t n) const
{
   PayloadT payload;
   AC_CHAR_TYPE const* ucs4 = (AC_CHAR_TYPE const*)s;
   AC_CHAR_TYPE const* u = ucs4;

   Node::Index inode = 0;
   for (u = ucs4; u < ucs4 + n; ++u) {
      inode = nodes[inode].child_at(*u);
      if (inode < 0)
         return (PayloadT)-1;
   }
   if (nodes[inode].length)
      return nodes[inode].payload;
   else
      return (PayloadT)-1;
}


// After:
//   http://www.quretec.com/u/vilo/edu/2005-06/Text_Algorithms/index.cgi?f=L2_Multiple_String&p=ACpre
void AhoCorasickTrie::make_failure_links()
{
   queue<Node*> q;
   for (Node::Children::const_iterator i = nodes[(Index)0].children.begin(),
           end = nodes[(Index)0].children.end();
        i != end; ++i) {
      Node* child = &nodes[i->second];
      q.push(child);
      child->ifailure_state = (Index)0;
   }
   // root fails to root
   nodes[0].ifailure_state = 0;
   //   q.push(root);

   while (not q.empty()) {
      Node* r = q.front();
      q.pop();
      for (Node::Children::const_iterator is = r->children.begin(),
              end = r->children.end();
           is != end; ++is) {
         AC_CHAR_TYPE a = is->first;
         Node* s = &nodes[is->second];
         q.push(s);
         Index ifail_state = r->ifailure_state;
         Index ifail_child = this->child_at(ifail_state, a);
         while (not is_valid(ifail_child)) {
            ifail_state = nodes[ifail_state].ifailure_state;
            ifail_child = this->child_at(ifail_state, a);
         }
         s->ifailure_state = ifail_child;
         // we were not an accept state, but now we are.
         //         s->output = s->ifailure_state.output;
         if (not s->length) {
            s->length = nodes[s->ifailure_state].length;
            s->payload = nodes[s->ifailure_state].payload;
         }
      }
   }

   is_compiled = true;
}

void AhoCorasickTrie::clear_failure_links()
{
   for (Nodes::iterator i = nodes.begin(), end = nodes.end();
        i != end; ++i) {
      i->ifailure_state = (Index)0;
   }
   // root fails to root

   is_compiled = false;
}


PayloadT AhoCorasickTrie::find_short(char const* char_s, size_t n,
                                     int* inout_istart,
                                     int* out_iend)
{
   ensure_compiled();
   Index istate = 0;
   PayloadT last_payload = 0;
   AC_CHAR_TYPE const* original_start = reinterpret_cast<AC_CHAR_TYPE const*>(char_s);
//int original_istart = *inout_istart;
   AC_CHAR_TYPE const* start = original_start + *inout_istart;
   AC_CHAR_TYPE const* end = original_start + n;

   for (AC_CHAR_TYPE const* c = start; c < end; ++c) {
      Index ichild = this->child_at(istate, *c);
      while (not is_valid(ichild)) {
         istate = nodes[istate].ifailure_state;
         ichild = this->child_at(istate, *c);
      }

      istate = ichild;
      if (nodes[istate].length and nodes[istate].length <= c + 1 - start) {
         *out_iend = c - original_start + 1;
         *inout_istart = *out_iend - nodes[istate].length;

// This is what Aho-Corasick proper does (but it may do more; this
// start at implementation below catches only embedded suffixes, eg
// given keys 'pq' and 'q' and text 'pq' it will catch 'pq' and 'q',
// but given keys 'pqp' and 'q', and text 'pqp', will only catch 'q'
// (if using short; otherwise of course will catch only 'pqp').
// First move at attempt full Aho-Corasick
//Chars chars = to_chars(char_s, n);
//Strings chain = follow_failure_chain(istate, chars, original_istart, *out_iend);

         return nodes[istate].payload;
      }
   }
   return last_payload;
}


PayloadT AhoCorasickTrie::find_longest(char const* char_s, size_t n,
                                       int* inout_istart,
                                       int* out_iend)
{
   ensure_compiled();
   Index istate = 0;
   bool have_match = false;

   PayloadT payload = 0;
   AC_CHAR_TYPE const* original_start = reinterpret_cast<AC_CHAR_TYPE const*>(char_s);
   AC_CHAR_TYPE const* start = original_start + *inout_istart;
   AC_CHAR_TYPE const* end = original_start + n;

   for (AC_CHAR_TYPE const* c = start; c < end; ++c) {
      Index ichild = this->child_at(istate, *c);
      while (not is_valid(ichild)) {
         if (have_match) {
            return payload;
         }
         istate = nodes[istate].ifailure_state;
         ichild = this->child_at(istate, *c);
      }

      istate = ichild;
      if (nodes[istate].length and nodes[istate].length <= c + 1 - start) {
         have_match = true;
         *out_iend = c - original_start + 1;
         *inout_istart = *out_iend - nodes[istate].length;
         payload = nodes[istate].payload;
      }
   }

   return payload;
}

AhoCorasickTrie::Strings
AhoCorasickTrie::follow_failure_chain(Node::Index inode,
                                      AhoCorasickTrie::Chars chars, int istart,
                                      int ifound_at) const
{
   typedef vector<AC_CHAR_TYPE> Chars;
   Strings strings;

   do {
cout << "inode:(" << inode << ")" << flush;
      int len = nodes[inode].length;
      if (len) {
cout << "istart:" << istart << "; ifound:" << ifound_at << "; len: " << len << endl;
         int lo = ifound_at - len,
            hi = ifound_at ;
cout << lo << ".." << hi << endl;
         Chars s(chars.begin() + lo, chars.begin() + hi);
         cout << s << endl;
         strings.push_back(s);
      }
      inode = nodes[inode].ifailure_state;
   } while (inode != 0);
   return strings;
}

// This is crap
void AhoCorasickTrie::print() const
{
//   cout << "maybe printing" << endl;
   typedef pair<AC_CHAR_TYPE, Index> Pair;
   queue<Pair> q;
   q.push(make_pair((AC_CHAR_TYPE)'@', 0));
   while (not q.empty()) {
      Pair p = q.front();
      q.pop();
      AC_CHAR_TYPE f = p.first;
      if (f == '$') {
         cout << endl;
         continue;
      }
      else {
         cout << (char)p.first << " ";
      }
      Index inode = p.second;
      if (is_valid(inode)) {
         for (Node::Children::const_iterator i = nodes[inode].children.begin(),
                 end = nodes[inode].children.end();
              i != end; ++i) {
            // structurally the same; will it work?
            q.push(make_pair(i->first, i->second));
         }
         // mark level
         q.push(make_pair<AC_CHAR_TYPE, Index>('$', 0));
      }
   }
}
