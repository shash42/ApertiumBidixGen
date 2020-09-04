AUTOMAKE_OPTIONS = subdir-objects
SUBDIRS = src
dist_doc_DATA = README

#AM_CXXFLAGS = -std=c++17 -lstdc++fs

bin_PROGRAMS = bidixgen parsebidix parserdfcsv
bidixgen_SOURCES = src/CLI.cpp
parsebidix_SOURCES = src/BidixParsing.cpp
parserdfcsv_SOURCES = src/RDFCSVParsing.cpp
