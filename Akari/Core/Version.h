#pragma once                                                                                                                                                                                                       
#define stringify(a) stringify_(a)                                                                                                                                                                                                       
#define stringify_(a) #a                                                                                                                                                                                                       
#define BUILD_VERSION_MAJOR 1                                                                                                                                                                                                      
#define BUILD_VERSION_MINOR 0                                                                                                                                                                                                       
#define BUILD_VERSION_PATCH 1                                                                                                                                                                                                       
#define BUILD_NUMBER 385 
#define BUILD_STRING "Akari Version " stringify(BUILD_VERSION_MAJOR) "." stringify(BUILD_VERSION_MINOR) "." stringify(BUILD_VERSION_PATCH) " (" stringify(BUILD_NUMBER) ") (Built - " __DATE__ " " __TIME__ ")"                                                                                                                                                                                                       
