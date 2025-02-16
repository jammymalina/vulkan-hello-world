#ifndef RESULT_H
#define RESULT_H

#define Result(T, E)        \
  struct Result_##T##_##E { \
    bool is_ok;             \
    union {                 \
      T value;              \
      E error;              \
    };                      \
  }

#define Ok(T, E)              \
  (struct Result_##T##_##E) { \
    .is_ok = true, .value = (T)_OK_IMPL
#define _OK_IMPL(...) \
  __VA_ARGS__         \
  }

#define Err(T, E)             \
  (struct Result_##T##_##E) { \
    .is_ok = false, .error = (E)_ERR_IMPL
#define _ERR_IMPL(...) \
  __VA_ARGS__          \
  }

typedef const char* ErrorMessage;

#endif  // RESULT_H
