add_subdirectory(DCore EXCLUDE_FROM_ALL)

set(PYTHON_VERSION "3" CACHE STRING "Python version")
set(Python_USE_STATIC_LIBS ${Boost_USE_STATIC_LIBS})
find_package(Python3 COMPONENTS Development Interpreter)

set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.65.1 REQUIRED COMPONENTS "python${PYTHON_VERSION}")

file(GLOB HEADERS "*.hpp")

add_library( dcore SHARED
             account.cpp
             asset.cpp
             chain.cpp
             common.cpp
             module.cpp
             nft.cpp
             ${HEADERS}
           )

target_link_libraries( dcore PRIVATE graphene_wallet "Boost::python${PYTHON_VERSION}" ${Python3_LIBRARIES} )
target_include_directories( dcore PRIVATE ${Python3_INCLUDE_DIRS} )

if(Python_USE_STATIC_LIBS)
   target_compile_definitions( dcore PRIVATE BOOST_PYTHON_STATIC_LIB )
endif()

set_target_properties( dcore PROPERTIES PREFIX "" )

if( APPLE )
   set_target_properties( dcore PROPERTIES SUFFIX ".so" )
elseif( WIN32 )
   set_target_properties( dcore PROPERTIES SUFFIX ".pyd" )
endif()

# to install into system use CMAKE_INSTALL_PREFIX=
if( CMAKE_INSTALL_PREFIX )
  if( WIN32 )
      set(LIBRARY_DESTINATION_PATH Lib)
      set(MODULE_DESTINATION_PATH Lib/site-packages)
   elseif( APPLE )
      string(REGEX MATCH "lib/python3\.[0-9]/.*" LIBRARY_DESTINATION_PATH ${Python3_SITEARCH})
      string(REGEX MATCH "lib/python3\.[0-9]/.*" MODULE_DESTINATION_PATH ${Python3_SITELIB})
   else()
      string(SUBSTRING ${Python3_SITEARCH} 1 -1 LIBRARY_DESTINATION_PATH)
      string(SUBSTRING ${Python3_SITELIB} 1 -1 MODULE_DESTINATION_PATH)
   endif()
else()
   set(LIBRARY_DESTINATION_PATH ${Python3_SITEARCH})
   set(MODULE_DESTINATION_PATH ${Python3_SITELIB})
endif()

install(TARGETS dcore
   RUNTIME DESTINATION ${LIBRARY_DESTINATION_PATH}
   LIBRARY DESTINATION ${LIBRARY_DESTINATION_PATH}
   ARCHIVE DESTINATION ${LIBRARY_DESTINATION_PATH} EXCLUDE_FROM_ALL
)

install(FILES __init__.py DESTINATION ${MODULE_DESTINATION_PATH}/DCore)
