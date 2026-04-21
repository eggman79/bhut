#include <cstdint>
#include <cinttypes>
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

std::array<uint8_t, 1024> call_stack;
void* call_stack_ptr = (void*)call_stack.data();

std::array<uint8_t, 1024> code;
void* code_ptr = (void*)code.data();

extern "C" void* push_instr;
extern "C" void* pop_instr;
extern "C" void* printi8;
extern "C" void* add_instr;
extern "C" void* sub_instr;
extern "C" void* jmp_instr;
extern "C" void* if_instr;
extern "C" void* call_instr;
extern "C" void* ret_instr;
extern "C" void* dup_instr;
extern "C" void* run_exit;
extern "C" void run(void* code);

constexpr std::array<void*, 11> instr = {
  &run_exit, &push_instr, &pop_instr,
  &add_instr, &sub_instr,
  &printi8, &if_instr, &jmp_instr, 
  &call_instr, &ret_instr, &dup_instr};

void* instr_ptr = (void*)instr.data();

enum class Instr: uint8_t {
  Exit,
  Push,
  Pop,
  Add,
  Sub,
  Print,
  If,
  Jump,
  Call,
  Ret,
  Dup,
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

  void update_call_addr(size_t call_addr, size_t method_addr) {
    *(void**)&code[call_addr] = (void*)&code[method_addr];
  }
};

struct Strings {
  std::vector<std::string> items;
};

extern "C" void print_int64(int64_t value) {
  printf("%" PRId64 "\n", value);
}

int main() {
  Code code;

  code.append(Instr::Push);
  code.append(1ULL);
  code.append(Instr::Push);
  code.append(2ULL);
  code.append(Instr::Push);

  const auto call_addr = code.code.size();
  code.append(0ULL);
  code.append(Instr::Call);
  code.append(Instr::Dup);
  code.append(Instr::Dup);
  code.append(Instr::Print);
  code.append(Instr::Print);
  code.append(Instr::Print);
code.append(Instr::Push);
  code.append(10ULL);

  code.append(Instr::Push);
  code.append(10ULL);

  code.append(Instr::Push);
  const auto call_addr3 = code.code.size();

  code.append(0ULL);
  code.append(Instr::Call);

  code.append(Instr::Push);
  code.append(10ULL);
  code.append(Instr::Push);

  const auto call_addr2 = code.code.size();
  code.append(0ULL);
  code.append(Instr::Call);
  code.append(Instr::Print);
  code.append(Instr::Exit);

  const auto method_addr = code.code.size();
  code.append(Instr::Add);
  code.append(Instr::Ret);


  const auto method2_addr = code.code.size();

  code.append(Instr::Add);
  code.append(Instr::Ret);

  code.update_call_addr(call_addr, method_addr);
  code.update_call_addr(call_addr2, method_addr);
  code.update_call_addr(call_addr3, method_addr);

  run(code.code.data());

  return 0;
}
