#ifndef BOOST_THREAD_PTHREAD_TIMESPEC_HPP
#define BOOST_THREAD_PTHREAD_TIMESPEC_HPP
//  (C) Copyright 2007-8 Anthony Williams
//  (C) Copyright 2012 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <boost/thread/detail/config.hpp>
#include <boost/thread/thread_time.hpp>
#if defined BOOST_THREAD_USES_DATETIME
#include <boost/date_time/posix_time/conversion.hpp>
#endif
#include <pthread.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#ifdef BOOST_THREAD_USES_CHRONO
#include <boost/chrono/duration.hpp>
#include <boost/chrono/system_clocks.hpp>
#include <boost/chrono/ceil.hpp>
#endif

#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#     define BOOST_THREAD_TIMESPEC_MAC_API
#include <sys/time.h> //for gettimeofday and timeval
#else
#include <time.h>  // for clock_gettime
#endif

#include <boost/config/abi_prefix.hpp>

namespace boost
{
  namespace detail
  {

    inline timespec ns_to_timespec(boost::intmax_t const& ns)
    {
      boost::intmax_t s = ns / 1000000000l;
      struct timespec ts;
      ts.tv_sec = static_cast<long> (s);
      ts.tv_nsec = static_cast<long> (ns - s * 1000000000l);
      return ts;
    }
    inline boost::intmax_t timespec_to_ns(timespec const& ts)
    {
      return static_cast<boost::intmax_t>(ts.tv_sec) * 1000000000l + ts.tv_nsec;
    }

    class timespec_duration
    {
    public:
      explicit timespec_duration(timespec const& v) : value(v) {}
      explicit timespec_duration(boost::intmax_t const& ns) : value(ns_to_timespec(ns)) {}

#if defined BOOST_THREAD_USES_DATETIME
      timespec_duration(boost::posix_time::time_duration const& rel_time)
      {
        struct timespec d = { 0,0};
        d.tv_sec=rel_time.total_seconds();
        d.tv_nsec=(long)(rel_time.fractional_seconds()*(1000000000l/rel_time.ticks_per_second()));
        value =  d;
      }
#endif
#if defined BOOST_THREAD_USES_CHRONO
      template <class Rep, class Period>
      timespec_duration(chrono::duration<Rep, Period> const& d)
      {
        value.tv_sec  = static_cast<long>(chrono::duration_cast<chrono::seconds>(d).count());
        value.tv_nsec = static_cast<long>((chrono::ceil<chrono::nanoseconds>(d) - chrono::duration_cast<chrono::seconds>(d)).count());
      }
#endif

      timespec& get() { return value; }
      timespec const& get() const { return value; }
      boost::intmax_t getNs() const { return timespec_to_ns(value); }

      static inline timespec_duration zero()
      {
        timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
        return timespec_duration(ts);
      }
    private:
      timespec value;
    };

    inline bool operator==(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() == rhs.getNs();
    }
    inline bool operator!=(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() != rhs.getNs();
    }
    inline bool operator<(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() < rhs.getNs();
    }
    inline bool operator<=(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() <= rhs.getNs();
    }
    inline bool operator>(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() > rhs.getNs();
    }
    inline bool operator>=(timespec_duration const& lhs, timespec_duration const& rhs)
    {
      return lhs.getNs() >= rhs.getNs();
    }

    static inline timespec_duration timespec_milliseconds(long const& ms)
    {
      return timespec_duration(ms * 1000000l);
    }

    class real_timespec_timepoint
    {
    public:
      explicit real_timespec_timepoint(timespec const& v) : value(v) {}
      explicit real_timespec_timepoint(boost::intmax_t const& ns) : value(ns_to_timespec(ns)) {}

#if defined BOOST_THREAD_USES_DATETIME
      real_timespec_timepoint(boost::system_time const& abs_time)
      {
        boost::posix_time::time_duration const time_since_epoch = abs_time-boost::posix_time::from_time_t(0);
        value = timespec_duration(time_since_epoch).get();
      }
#endif
#if defined BOOST_THREAD_USES_CHRONO
      template <class Duration>
      real_timespec_timepoint(chrono::time_point<chrono::system_clock, Duration> const& abs_time)
      {
        value = timespec_duration(abs_time.time_since_epoch()).get();
      }
#endif

      timespec& get() { return value; }
      timespec const& get() const { return value; }
      boost::intmax_t getNs() const { return timespec_to_ns(value); }

    private:
      timespec value;
    };

    inline bool operator==(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() == rhs.getNs();
    }
    inline bool operator!=(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() != rhs.getNs();
    }
    inline bool operator<(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() < rhs.getNs();
    }
    inline bool operator<=(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() <= rhs.getNs();
    }
    inline bool operator>(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() > rhs.getNs();
    }
    inline bool operator>=(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return lhs.getNs() >= rhs.getNs();
    }

    inline real_timespec_timepoint operator+(real_timespec_timepoint const& lhs, timespec_duration const& rhs)
    {
      return real_timespec_timepoint(lhs.getNs() + rhs.getNs());
    }
    inline real_timespec_timepoint operator+(timespec_duration const& lhs, real_timespec_timepoint const& rhs)
    {
      return real_timespec_timepoint(lhs.getNs() + rhs.getNs());
    }
    inline timespec_duration operator-(real_timespec_timepoint const& lhs, real_timespec_timepoint const& rhs)
    {
      return timespec_duration(lhs.getNs() - rhs.getNs());
    }

    struct real_timespec_clock
    {
      static inline real_timespec_timepoint now()
      {
        timespec ts;

  #if defined(BOOST_THREAD_TIMESPEC_MAC_API)
        timeval tv;
        ::gettimeofday(&tv, 0);
        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;
  #else
        if ( ::clock_gettime( CLOCK_REALTIME, &ts ) )
        {
          ts.tv_sec = 0;
          ts.tv_nsec = 0;
          BOOST_ASSERT(0 && "Boost::Thread - Internal Error");
        }
  #endif
        return real_timespec_timepoint(ts);
      }
    };

#if defined(CLOCK_MONOTONIC)
  class mono_timespec_timepoint
  {
  public:
    explicit mono_timespec_timepoint(timespec const& v) : value(v) {}
    explicit mono_timespec_timepoint(boost::intmax_t const& ns) : value(ns_to_timespec(ns)) {}

#if defined BOOST_THREAD_USES_DATETIME
    // fixme: delete this function once it's no longer being used because it's
    // not a valid way to convert from system time to steady time
    //inline mono_timespec_timepoint(boost::system_time const& abs_time);
#endif
#if defined BOOST_THREAD_USES_CHRONO
    template <class Duration>
    inline mono_timespec_timepoint(chrono::time_point<chrono::steady_clock, Duration> const& abs_time);
#endif

    timespec& get() { return value; }
    timespec const& get() const { return value; }
    boost::intmax_t getNs() const { return timespec_to_ns(value); }

  private:
    timespec value;
  };

  inline bool operator==(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() == rhs.getNs();
  }
  inline bool operator!=(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() != rhs.getNs();
  }
  inline bool operator<(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() < rhs.getNs();
  }
  inline bool operator<=(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() <= rhs.getNs();
  }
  inline bool operator>(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() > rhs.getNs();
  }
  inline bool operator>=(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return lhs.getNs() >= rhs.getNs();
  }

  inline mono_timespec_timepoint operator+(mono_timespec_timepoint const& lhs, timespec_duration const& rhs)
  {
    return mono_timespec_timepoint(lhs.getNs() + rhs.getNs());
  }
  inline mono_timespec_timepoint operator+(timespec_duration const& lhs, mono_timespec_timepoint const& rhs)
  {
    return mono_timespec_timepoint(lhs.getNs() + rhs.getNs());
  }
  inline timespec_duration operator-(mono_timespec_timepoint const& lhs, mono_timespec_timepoint const& rhs)
  {
    return timespec_duration(lhs.getNs() - rhs.getNs());
  }

  struct mono_timespec_clock
  {
    // fixme: add support for mono_timespec_clock::now() on MAC OS X using code from
    // https://github.com/boostorg/chrono/blob/develop/include/boost/chrono/detail/inlined/mac/chrono.hpp
    static inline mono_timespec_timepoint now()
    {
      timespec ts;
      if ( ::clock_gettime( CLOCK_MONOTONIC, &ts ) )
      {
        ts.tv_sec = 0;
        ts.tv_nsec = 0;
        BOOST_ASSERT(0 && "Boost::Thread - Internal Error");
      }
      return mono_timespec_timepoint(ts);
    }
  };

//#if defined BOOST_THREAD_USES_DATETIME
//  // fixme: delete this function once it's no longer being used because it's
//  // not a valid way to convert from system time to steady time
//  mono_timespec_timepoint::mono_timespec_timepoint(boost::system_time const& abs_time)
//  {
//    boost::posix_time::time_duration const since_now = abs_time - boost::get_system_time();
//    value = (mono_timespec_clock::now() + timespec_duration(since_now)).get();
//  }
//#endif
#if defined BOOST_THREAD_USES_CHRONO
  template <class Duration>
  mono_timespec_timepoint::mono_timespec_timepoint(chrono::time_point<chrono::steady_clock, Duration> const& abs_time)
  {
    value = timespec_duration(abs_time.time_since_epoch()).get();
  }
#endif

#endif

#if defined BOOST_THREAD_HAS_CONDATTR_SET_CLOCK_MONOTONIC
  typedef mono_timespec_clock internal_timespec_clock;
  typedef mono_timespec_timepoint internal_timespec_timepoint;
#else
  typedef real_timespec_clock internal_timespec_clock;
  typedef real_timespec_timepoint internal_timespec_timepoint;
#endif

  }
}

#include <boost/config/abi_suffix.hpp>

#endif
