#pragma once

#include <cstddef>
#include <utility>

namespace TXX
{
  template <size_t n>
  struct rbptr
  {
    size_t p;
    
    rbptr &operator++() {
      p++;
      if (p == n) p = 0;
      return *this;
    }
    
    rbptr &operator+=(size_t x) {
      p = (p + x) % n;
      return *this;
    }
    
    rbptr operator+(size_t i) const {
      return rbptr { (p + i) % n };
    }
    
    size_t operator-(const rbptr &o) const {
      if (o.p > p) {
	return n + p - o.p;
      }
      
      return p - o.p;
    }
    
    bool operator>(const rbptr &o) const {
      return p > o.p;
    }
    
    bool operator<(const rbptr &o) const {
      return p < o.p;
    }
    
    bool operator==(const rbptr &p2) const {
      return p == p2.p;
    }

    operator int() const {
      return p;
    }
  };
  
  template <typename T, size_t n>
  class ring_buffer_base
  {
    using ptr = rbptr<n>;
    
    static constexpr ptr endbuf { n };


    T buffer[n];
    ptr start, end;

    T &operator[](const ptr &p) {
      return buffer[p.p];
    }

  public:
    int pushc(T c) {
      if ((end + 1) == start) {
        ++start;
      }

      (*this)[end] = c;
      ++end;
      return 0;
    }

    bool empty() {
      return start == end;
    }

    bool full() {
      return end + 1 == start;
    }

    std::pair<const T *, size_t> get_seg() {
      const char *p = &(*this)[start];
      size_t l = 0;

      if (start > end) {
        l = endbuf - start;
      } else if (start < end) {
        l = end - start;
      }

      start += l;
      return std::make_pair(p, l);
    }

  };

  template <size_t n>
  class ring_buffer : public ring_buffer_base<char, n>
  {
  };
};
