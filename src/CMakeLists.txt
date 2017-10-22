add_executable(pdfslicer
    main.cpp
    application.hpp
    application.cpp
    aboutdialog.hpp
    aboutdialog.cpp
    page.hpp
    page.cpp
    commandmanager.hpp
    commandmanager.cpp
    document.hpp
    document.cpp
    view.hpp
    view.cpp
    viewchild.hpp
    viewchild.cpp
    previewwindow.hpp
    previewwindow.cpp
    window.hpp
    window.cpp)

target_link_libraries(pdfslicer
    ${GTKMM_LIBRARIES}
    ${CAIROMM_LIBRARIES}
    ${POPPLER_LIBRARIES}
    PDFWriter
    CTPL)