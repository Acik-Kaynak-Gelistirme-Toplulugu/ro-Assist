cmake_minimum_required(VERSION 3.16)
project(ro-assist VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Widgets Network Test)

set(RO_ASSIST_SOURCES
    include/roassist/mainwindow.h
    include/roassist/ui_texts.h
    include/roassist/update_helpers.h
    src/main.cpp
    src/mainwindow.cpp
    src/ui_texts.cpp
    src/update_helpers.cpp
    resources/resources.qrc
)

add_executable(ro-assist ${RO_ASSIST_SOURCES})
target_include_directories(ro-assist PRIVATE include)
target_link_libraries(ro-assist PRIVATE Qt6::Widgets Qt6::Network)

install(TARGETS ro-assist RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/linux/ro-assist.desktop
        DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES packaging/linux/ro-assist-autostart.desktop
        DESTINATION /etc/xdg/autostart)
install(FILES packaging/linux/io.github.project_ro_asd.ro_assist.metainfo.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
install(FILES resources/icons/ro-assist.svg
        DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

add_executable(ro-assist-ui-tests
    include/roassist/mainwindow.h
    include/roassist/ui_texts.h
    src/mainwindow.cpp
    src/ui_texts.cpp
    src/update_helpers.cpp
    resources/resources.qrc
    tests/mainwindow_integration_test.cpp
)
target_include_directories(ro-assist-ui-tests PRIVATE include)
target_link_libraries(ro-assist-ui-tests PRIVATE Qt6::Test Qt6::Widgets Qt6::Network)

add_executable(ro-assist-unit-tests
    include/roassist/update_helpers.h
    src/update_helpers.cpp
    tests/update_helpers_test.cpp
)
target_include_directories(ro-assist-unit-tests PRIVATE include)
target_link_libraries(ro-assist-unit-tests PRIVATE Qt6::Test Qt6::Core)

add_test(NAME ro-assist-unit-tests COMMAND ro-assist-unit-tests)
add_test(NAME ro-assist-ui-tests COMMAND ro-assist-ui-tests)
add_test(NAME ro-assist-smoke-test COMMAND ro-assist --smoke-test)
set_tests_properties(ro-assist-ui-tests PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;RO_ASSIST_SKIP_SYSTEM_CHECKS=1"
)
set_tests_properties(ro-assist-smoke-test PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;RO_ASSIST_SKIP_SYSTEM_CHECKS=1"
)

set(CPACK_GENERATOR "RPM")
set(CPACK_PACKAGE_NAME "ro-assist")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_CONTACT "Ebubekir Bulut <mutemet91@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Fedora Linux Sistem Asistanı")

include(CPack)
