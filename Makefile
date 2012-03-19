CXXFLAGS=-O2 -Wall -Wextra -Wno-unused-variable
CXX=g++
INCLUDE=-I/usr/include
SOURCES=array-aho.cpp test-array-aho.cpp
OBJDIR=.objs
OBJECTS=$(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))

TARGET=test-aho

$(OBJDIR):
	mkdir $(OBJDIR)

# $@ is the target (objs/foo.o), and $< is the input foo.cpp
$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

all: $(TARGET)

# don't need this: RPATH=$(BOOST_LIBDIR)
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

clean:
	rm -f $(TARGET)
	rm -f $(OBJECTS)

#http://www.cs.duke.edu/~ola/courses/programming/libraries.html
# libs: g++ $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
# g++ -shared -Wl,-soname, $(TARGET).so $(OBJS) $(LIBS)
