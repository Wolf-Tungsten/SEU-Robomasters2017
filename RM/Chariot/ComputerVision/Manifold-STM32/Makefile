# Makefile
# Version 1.0
# Writed by Deyou Kong, 2017-06-26
# Check by Deyou Kong, 2017-06-26


TARGET = Run.out

CXX = g++

DIR_RELEASE = ./Release

LDFLAGS = -Wl,-rpath,./ -L./ -lpthread -lrt #$(OPENCV_LINK)


# OpenCV

OPENCV_FLAG = `pkg-config opencv --cflags`
OPENCV_LINK = `pkg-config opencv --libs` `pkg-config cuda-6.5 --libs` `pkg-config cudart-6.5 --libs`

# Custom

DIR_CUSTOM = .
DIR_CUST_INC = $(DIR_CUSTOM)
DIR_CUST_SRC = $(DIR_CUSTOM)

CUST_OBJECTS = $(addprefix $(DIR_RELEASE)/,CConfig.o Chariot.o ChariotEx.o Demo.o)
CUST_CXXFLAGS = -g -Wall --std=c++11 $(addprefix -I,$(DIR_CUST_INC)) #$(OPENCV_FLAG)


# Relationships

$(TARGET) : $(CUST_OBJECTS)
	$(CXX) -o $(DIR_RELEASE)/$(TARGET) $(CUST_OBJECTS) $(LDFLAGS)

$(DIR_RELEASE)/CConfig.o : $(DIR_CUST_SRC)/CConfig.cpp $(DIR_CUST_SRC)/CConfig.h
	$(CXX) $(CUST_CXXFLAGS) -o $(DIR_RELEASE)/CConfig.o -c $(DIR_CUST_SRC)/CConfig.cpp

$(DIR_RELEASE)/Chariot.o : $(DIR_CUST_SRC)/Chariot.cpp $(DIR_CUST_SRC)/Chariot.h
	$(CXX) $(CUST_CXXFLAGS) -o $(DIR_RELEASE)/Chariot.o -c $(DIR_CUST_SRC)/Chariot.cpp

$(DIR_RELEASE)/ChariotEx.o : $(DIR_CUST_SRC)/ChariotEx.cpp $(DIR_CUST_SRC)/ChariotEx.h
	$(CXX) $(CUST_CXXFLAGS) -o $(DIR_RELEASE)/ChariotEx.o -c $(DIR_CUST_SRC)/ChariotEx.cpp

$(DIR_RELEASE)/Demo.o : $(DIR_CUST_SRC)/Demo.cpp
	$(CXX) $(CUST_CXXFLAGS) -o $(DIR_RELEASE)/Demo.o -c $(DIR_CUST_SRC)/Demo.cpp


# Command
	
clean:
	rm -f $(DIR_RELEASE)/*
	rm -f *~
