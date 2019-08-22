add_subdirectory(DCore EXCLUDE_FROM_ALL)

set(PYTHON_VERSION "3" CACHE STRING "Python version")
string(REGEX REPLACE "([2-3])[0-9]?" "Python\\1" PYTHON_MODULE "${PYTHON_VERSION}")

set(Python_USE_STATIC_LIBS ${Boost_USE_STATIC_LIBS})
find_package("${PYTHON_MODULE}" COMPONENTS Development Interpreter)

find_package(Boost 1.65.1 COMPONENTS "python${PYTHON_VERSION}")
if(NOT Boost_FOUND)
   find_package(Boost 1.65.1 REQUIRED COMPONENTS python)
else()
   set(PYTHON_SUFFIX "${PYTHON_VERSION}")
endif()

file(GLOB HEADERS "*.hpp")

add_library( dcore SHARED
             common.cpp
             module.cpp
             ${HEADERS}
           )

target_link_libraries( dcore PRIVATE graphene_wallet "Boost::python${PYTHON_SUFFIX}" ${${PYTHON_MODULE}_LIBRARIES} )
target_include_directories( dcore PRIVATE ${${PYTHON_MODULE}_INCLUDE_DIRS} )

if(Python_USE_STATIC_LIBS)
   target_compile_definitions( dcore PRIVATE BOOST_PYTHON_STATIC_LIB )
endif()

set_target_properties( dcore PROPERTIES PREFIX "" )

if( APPLE )
   set_target_properties( dcore PROPERTIES SUFFIX ".so" )
elseif( WIN32 )
   set_target_properties( dcore PROPERTIES SUFFIX ".pyd" )
endif()

if( WIN32 )
   # to install into system use CMAKE_INSTALL_PREFIX=
   if( CMAKE_INSTALL_PREFIX )
      set(LIBRARY_DESTINATION_PATH Lib)
      set(MODULE_DESTINATION_PATH Lib/site-packages)
   else()
      set(LIBRARY_DESTINATION_PATH ${${PYTHON_MODULE}_SITEARCH})
      set(MODULE_DESTINATION_PATH ${${PYTHON_MODULE}_SITELIB})
   endif()
else()
   # to install into system use CMAKE_INSTALL_PREFIX=/
   string(SUBSTRING ${${PYTHON_MODULE}_SITEARCH} 1 -1 LIBRARY_DESTINATION_PATH)
   string(SUBSTRING ${${PYTHON_MODULE}_SITELIB} 1 -1 MODULE_DESTINATION_PATH)
endif()

install(TARGETS dcore
   RUNTIME DESTINATION ${LIBRARY_DESTINATION_PATH}
   LIBRARY DESTINATION ${LIBRARY_DESTINATION_PATH}
   ARCHIVE DESTINATION ${LIBRARY_DESTINATION_PATH} EXCLUDE_FROM_ALL
)

install(FILES __init__.py DESTINATION ${MODULE_DESTINATION_PATH}/DCore)
