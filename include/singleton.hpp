#pragma once

#include <macros.hpp>

template <typename T> class Singleton {
public:
  DELETE_COPY_MOVE(Singleton);

  static auto instance() noexcept -> T & {
    static T instance;
    return instance;
  }

protected:
  DEFAULT_CTOR_DTOR(Singleton);
};
