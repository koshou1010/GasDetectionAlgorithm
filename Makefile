CXX ?= g++

all: main


main: test.cpp algo.cpp classifier.cpp svm.cpp
	$(CXX) -o main test.cpp algo.cpp classifier.cpp svm.cpp
# mk: make_dataset.cpp algo.cpp
# 	$(CXX) -o mk make_dataset.cpp algo.cpp
