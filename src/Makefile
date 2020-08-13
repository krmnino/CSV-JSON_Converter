TARGET1 = CSVtoJSON
TARGET2 = JSONtoCSV

OBJS1 = CSVtoJSON.o 
OBJS2 = JSONtoCSV.o

CXX = g++
CXXFLAGS = -std=c++17

all: $(TARGET1) $(TARGET2)

$(TARGET1):	$(OBJS1)	
		$(CXX) $(CXXFLAGS) $(OBJS1) -g -o $(TARGET1) $(LDFLAGS)

$(TARGET2):	$(OBJS2)	
		$(CXX) $(CXXFLAGS) $(OBJS2) -g -o $(TARGET2) $(LDFLAGS)
		rm -f *.o

clean:
		rm -f *.o $(TARGET1) $(TARGET2)

CSVtoJSON.o: 	CSVtoJSON.cpp
JSONtoCSV.o:	JSONtoCSV.cpp

