add_library (Eval evaluation.cpp evaluation.h parser.cpp parser.h pugixml.hpp pugixml.cpp pugiconfig.hpp)
add_executable (demo main.cpp)
target_link_libraries (demo Eval)
