set(CMAKE_MODULE_PATH .. .)

include(GaudiToolchainMacros)

init()
find_projects(projects tools data/toolchain/MULTI/MULTI_v1r0/CMakeLists.txt)

include(TestMacros)

assert_strequal(projects "MULTI;DERIVED;BASE")

get_filename_component(base_dir data/toolchain ABSOLUTE)
assert_strequal(BASE_ROOT_DIR "${base_dir}/BASE/BASE_v1r0")
assert_strequal(DERIVED_ROOT_DIR "${base_dir}/DERIVED/DERIVED_v1r0")
assert_strequal(MULTI_ROOT_DIR "${base_dir}/MULTI/MULTI_v1r0")

set_paths_from_projects(${projects})

assert(base_toolchain_called)
assert(derived_toolchain_called)
assert(multi_toolchain_called)

assert_strequal(entries "MULTI;DERIVED;BASE")
