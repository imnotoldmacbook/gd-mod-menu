cmake_minimum_required(VERSION 3.5...3.30)

project(cocos-headers CXX)

# This is now just an interface library with headers
# Import library generation moved to main CMakeLists.txt

add_library(cocos2d INTERFACE)

target_include_directories(cocos2d INTERFACE
  cocos2dx
  cocos2dx/include
  cocos2dx/kazmath/include
  cocos2dx/platform/win32
  cocos2dx/platform/third_party/win32
  cocos2dx/platform/third_party/win32/OGLES
  extensions
)