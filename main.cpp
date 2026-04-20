#include <cstdint>
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
extern "C" void* add;
extern "C" void* run_exit;
extern "C" void run(void* code);

std::array<void*, 4> instr = {&run_exit, &push, nullptr, &add};
void* instr_ptr = (void*)instr.data();

enum class Instr: uint8_t {
  Exit,
  Push,
  Pop,
  Add,
};

struct Code {
  std::vector<uint8_t> code;

  template <typename T>
  void append(T value) {
    if constexpr (sizeof(value) == 1) {
      code.emplace_back(value);
    } else {
      constexpr auto value_size = sizeof(value);
      code.resize(code.size() + value_size);
      *(T*)&code[code.size() - value_size] = value;
    }
  }
};

int main() {
  Code code;
  code.append((uint8_t)1);
  code.append((uint64_t)12);
  code.append((uint8_t)1);
  code.append((uint64_t)13);
  code.append((uint8_t)3);
  code.append((uint8_t)0);
  run(code.code.data());
  return 0;
}
