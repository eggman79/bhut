#include <cstdint>
#include <type_traits>
#include <array>
#include <vector>
#include <iostream>

std::array<uint8_t, 1024> heap;
void* heap_ptr = (void*)heap.data();

std::array<uint8_t, 1024> stack;
void* stack_ptr = (void*)stack.data();

std::array<uint8_t, 1024> code;
void* code_ptr = (void*)code.data();

extern "C" void* push;
extern "C" void* printi8;
extern "C" void* add;
extern "C" void* iff;
extern "C" void* run_exit;
extern "C" void run(void* code);

constexpr std::array<void*, 6> instr = {&run_exit, &push, nullptr, &add, &printi8, &iff};
void* instr_ptr = (void*)instr.data();

enum class Instr: uint8_t {
  Exit,
  Push,
  Pop,
  Add,
  Print,
  If,
};

namespace detail {
    // Non-enum: identity type
    template<typename T, bool IsEnum = std::is_enum_v<T>>
    struct underlying_type { using type = T; };
    
    // Enum: std::underlying_type
    template<typename T>
    struct underlying_type<T, true> : std::underlying_type<T> {};
}

template<typename T>
using integral_type_t = typename detail::underlying_type<T>::type;

struct Code {
  std::vector<uint8_t> code;

  template <typename T>
    void append(T value) {
      if constexpr (sizeof(value) == 1) {
        code.emplace_back((integral_type_t<T>)value);
      } else {
        constexpr auto value_size = sizeof(value);
        code.resize(code.size() + value_size);
        *(integral_type_t<T>*)&code[code.size() - value_size] = value;
      }
    }

  void commit() {

  }
};

extern "C" void print_int64(int64_t value) {
  std::cout << value << std::endl;
}

int main() {
  Code code;
  code.append(Instr::Push);
  code.append(1ULL);
  code.append(Instr::If);
  const auto off1 = code.code.size();

  code.append(0ULL);
  code.append(Instr::Push);
  code.append(100ULL);
  code.append(Instr::Print);

  const auto off2 = code.code.size();
  *(uintptr_t*)&code.code[off1] = off2 - off1 + 1;

  code.append(Instr::Push);
  code.append(101ULL);
  code.append(Instr::Print);
  code.append(Instr::Exit);
  run(code.code.data());
  return 0;
}
