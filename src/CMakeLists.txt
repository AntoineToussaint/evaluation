add_library (Eval evaluation.cpp evaluation.h parser.cpp parser.h pugixml.hpp pugixml.cpp pugiconfig.hpp)
add_executable (evaluation main.cpp)
target_link_libraries (evaluation Eval)
