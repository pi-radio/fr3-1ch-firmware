#include <threadxx/ring_buffer.hpp>
#include <threadxx/dbgstream.hpp>
#include <stdarg.h>

#ifndef THREADXX_DEBUG_BUFFER_SIZE
  #define THREADXX_DEBUG_BUFFER_SIZE  1024
#endif

namespace dbg
{
  static std::vector<renderer *> renderers {};
  static bool initialized = false;

  TXX::ring_buffer<THREADXX_DEBUG_BUFFER_SIZE> dbgbuf;

  template <size_t n>
  class dbgbuffer : public std::streambuf
  {
    friend void add_renderer(renderer *);
    char_type last;

  protected:
    int sync() override
    {
      // Cache as much as possible until we get a renderer
      if (renderers.size() == 0) {
        return 0;
      }

      while(!dbgbuf.empty()) {
        auto seg = dbgbuf.get_seg();

        for (auto r : renderers)
          r->render(seg.first, seg.second);
      }

      return 0;
    }

    inline bool test_eof(const char_type c)
    {
      return traits_type::eq_int_type(c, traits_type::eof());
    }

    void pushc(char_type c)
    {
      if (dbgbuf.full()) {
        sync();
      }

      dbgbuf.pushc(c);
    }

    virtual int_type overflow(int_type c) override
    {
      if (test_eof(c)) {
        sync();
      }

      if (c == '\n' && last != '\r') {
        pushc('\r');
      }

      pushc(c);

      return 1;
    }

  public:
    dbgbuffer()
    {
      for (auto c : "\e[2J\e[H") {
        if (c) {
          pushc(c);
        }
      }

      initialized = true;
    }

  };




  static typename std::aligned_storage<sizeof(dbgbuffer<THREADXX_DEBUG_BUFFER_SIZE>),
                                       alignof(dbgbuffer<THREADXX_DEBUG_BUFFER_SIZE>)>::type buffer_buf;
  dbgbuffer<THREADXX_DEBUG_BUFFER_SIZE> &buffer = reinterpret_cast<dbgbuffer<THREADXX_DEBUG_BUFFER_SIZE> &>(buffer_buf);

  static typename std::aligned_storage<sizeof (std::ostream), alignof (std::ostream)>::type
  dbgout_buf; // memory for the stream object
  std::ostream& dbgout = reinterpret_cast<std::ostream &> (dbgout_buf);  

  void add_renderer(renderer *r) {
    renderers.push_back(r);

    if (initialized) {
      buffer.sync();
    }
  }

  static int _counter;

  
  _dbginitializer::_dbginitializer()
  {
    using namespace std;
    if (_counter++ == 0) {
      new (&buffer) dbgbuffer<THREADXX_DEBUG_BUFFER_SIZE>();
      new (&dbgout) ostream (&buffer);
    }
  }
  
  _dbginitializer::~_dbginitializer()
  {
    using namespace std;
    if (--_counter == 0) dbgout.~ostream();
  }
}

extern "C" int dbgprint(const char *fmt, ...)
{
  size_t retval;

  va_list args;

  va_start(args, fmt);

  retval = vsnprintf(NULL, 0, fmt, args);

  va_end(args);

  std::vector<char> serbuf(retval + 1);

  va_start(args, fmt);

  retval = vsnprintf(&serbuf[0], retval + 1, fmt, args);

  va_end(args);

  char prev = 0;

  for (size_t i = 0; i < retval; i++) {
    if (serbuf[i] == '\n' && prev != '\r') {
      dbg::dbgout.put('\r');
    }

    dbg::dbgout.put(serbuf[i]);

    if (serbuf[i] == '\n')
      dbg::dbgout.flush();

    prev = serbuf[i];
  }

  return retval;

}
