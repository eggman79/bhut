#include <cstdint>
#include <limits>
#include <type_traits>
#include <array>
#include <vector>
#include <deque>
#include <string>
#include <string_view>
#include <unordered_map>
#include <iostream>

std::array<uint8_t, 1024> heap;
void* heap_ptr = (void*)heap.data();

std::array<uint8_t, 1024> stack;
void* stack_ptr = (void*)stack.data();

std::array<uint8_t, 1024> code;
void* code_ptr = (void*)code.data();

extern "C" void* push_instr;
extern "C" void* printi8;
extern "C" void* add_instr;
extern "C" void* jmp_instr;
extern "C" void* if_instr;
extern "C" void* call_instr;
extern "C" void* ret_instr;
extern "C" void* run_exit;
extern "C" void run(void* code);

constexpr std::array<void*, 9> instr = {
  &run_exit, &push_instr, nullptr, 
  &add_instr, &printi8, &if_instr, &jmp_instr, 
  &call_instr, &ret_instr};
void* instr_ptr = (void*)instr.data();

enum class Instr: uint8_t {
  Exit,
  Push,
  Pop,
  Add,
  Print,
  If,
  Jump,
  Call,
  Ret,
};

struct Class {
  using PropIndex = uint16_t;
  std::deque<std::string> ordered_props;
  std::unordered_map<std::string_view, PropIndex> props;

  void append(std::string_view prop_name) {
    ordered_props.emplace_back(prop_name);
    props.emplace(ordered_props.back(), ordered_props.size() - 1);
  }

  static constexpr PropIndex UndefinedIndex = std::numeric_limits<PropIndex>::max();

  PropIndex get_prop() const {
    return UndefinedIndex;    
  }

};

struct String : Class {

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
};

extern "C" void print_int64(int64_t value) {
  std::cout << value << std::endl;
}

int main() {
  Code code;

  code.append(Instr::Push);
  code.append(100ULL);
  code.append(Instr::Print);
  code.append(Instr::Push);
  const auto call_offset = code.code.size();
  code.append(0ULL);
  code.append(Instr::Call);
  code.append(Instr::Exit);

  const auto off = code.code.size();  
  code.append(Instr::Push);
  code.append(10000ULL);
  code.append(Instr::Print);
  code.append(Instr::Ret);

  *(void**)&code.code[call_offset] = (void*)&code.code[off];
  run(code.code.data());

  return 0;
}
