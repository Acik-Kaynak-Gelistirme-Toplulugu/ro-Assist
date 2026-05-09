cmake_minimum_required(VERSION 3.16)
project(ro-assist VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Test)

set(RO_ASSIST_SOURCES
    include/roassist/mainwindow.h
    include/roassist/update_helpers.h
    src/main.cpp
    src/mainwindow.cpp
    src/update_helpers.cpp
    resources/resources.qrc
)

add_executable(ro-assist ${RO_ASSIST_SOURCES})
target_include_directories(ro-assist PRIVATE include)
target_link_libraries(ro-assist PRIVATE Qt6::Widgets Qt6::Network)

install(TARGETS ro-assist DESTINATION bin)

add_executable(ro-assist-tests
    include/roassist/update_helpers.h
    src/update_helpers.cpp
    tests/update_helpers_test.cpp
)
target_include_directories(ro-assist-tests PRIVATE include)
target_link_libraries(ro-assist-tests PRIVATE Qt6::Test Qt6::Core)

add_test(NAME ro-assist-unit-tests COMMAND ro-assist-tests)
add_test(NAME ro-assist-smoke-test COMMAND ro-assist --smoke-test)
set_tests_properties(ro-assist-smoke-test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "ro-assist")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Ebubekir Bulut <mutemet91@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fedora Linux Sistem Asistanı")

include(CPack)
