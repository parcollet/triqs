include_directories(${CMAKE_SOURCE_DIR}/google_test)

 file(GLOB TestList RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

 foreach(TestName1 ${TestList})
  string(REPLACE ".cpp" "" TestName ${TestName1})
  add_executable(${TestName} ${CMAKE_CURRENT_SOURCE_DIR}/${TestName}.cpp)
  target_link_libraries(${TestName} PRIVATE gtest)
  target_compile_options(${TestName} PRIVATE -std=c++17)
  add_cpp_test(${TestName})
  
 
 endforeach(TestName1  ${TestList})


