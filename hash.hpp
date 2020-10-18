/**********************************************************************
 *  This file is part of hash-combine.
 *
 *        Copyright (C) 2019-2020 Julian Rüth
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *********************************************************************/

// Apparently boost's hash_combine is not very good
// (https://stackoverflow.com/questions/35985960/c-why-is-boosthash-combine-the-best-way-to-combine-hash-values)
// It's maybe not so bad in practice but pulling in the entire hash.hpp header
// just for this one function seems a bit too much. (The hash_fwd.hpp header
// does not contain the implementation of hash_combine.)
// So we rolled our own hash_combine since strangely we could not find a
// header-only for that yet.

#ifndef FLATSURF_HASH_COMBINE_HPP
#define FLATSURF_HASH_COMBINE_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>
#include <tuple>

namespace {

namespace flatsurf {

// A convenience function to be able to write flatsurf::hash(t) instead of std::hash<T>{}(t).
template <typename T>
size_t hash(T&& t) {
  return std::hash<std::decay_t<T>>{}(std::forward<T>(t));
}

size_t hash_combine() {
  return 0;
}

template <typename ...Tail>
size_t hash_combine(size_t head, Tail&&...tail) {
  if (sizeof...(tail) == 0) {
    return head;
  } else {
    // http://mostlymangling.blogspot.com/2019/01/better-stronger-mixer-and-test-procedure.html, public domain
    const auto rrxmxmx = [](size_t v) {
      const auto ror64 = [](size_t word, unsigned int shift) {
        return (word >> (shift & 63)) | (word << ((-shift) & 63));
      };

      v ^= ror64(v, 49) ^ ror64(v, 24);
      v *= 0x9FB21C651E98DF25L;
      v ^= v >> 28;
      v *= 0x9FB21C651E98DF25L;
      return v ^ v >> 28;
    };

    return rrxmxmx(head) ^ hash_combine(tail...);
  }
}

}  // namespace flatsurf

}

#endif
