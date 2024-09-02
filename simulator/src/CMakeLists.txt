add_executable(vm vm.c simulation.c)
target_include_directories(
  vm PUBLIC include
)
target_compile_options(vm PRIVATE -O2)
