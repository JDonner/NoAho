CXXFLAGS=-g -Wall -Wextra -Wno-unused-variable -pthread
CXX=g++
INCLUDE=-I/usr/include
TEST_AHO_CPP_SOURCES=array-aho.cpp gtest-array-aho.cpp
TEST_AHO_C_SOURCES=unicodectype.c
OBJDIR=.objs
#C_OBJECTS=$(addprefix $(OBJDIR)/, $(TEST_AHO_C_SOURCES:.c=.o))
TEST_AHO_OBJECTS=$(addprefix $(OBJDIR)/, $(TEST_AHO_C_SOURCES:.c=.o))
TEST_AHO_OBJECTS+=$(addprefix $(OBJDIR)/, $(TEST_AHO_CPP_SOURCES:.cpp=.o))
OBJECTS=$(TEST_AHO_OBJECTS)

GTEST_DIR = /usr/src/gtest
# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = /usr/include/gtest/*.h \
                /usr/include/gtest/internal/*.h

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

TEST_AHO=gtest-noaho

$(OBJDIR):
	mkdir $(OBJDIR)

# $@ is the target (objs/foo.o), and $< is the input foo.cpp
$(OBJDIR)/%.o: %.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $(INCLUDE) -o $@ $<

all: $(TEST_AHO)

$(TEST_AHO): $(TEST_AHO_OBJECTS) gtest_main.a
	$(CXX) -g $^ -lpthread -o $@
clean:
	rm -f $(TEST_AHO)
	rm -f $(OBJECTS)

#http://www.cs.duke.edu/~ola/courses/programming/libraries.html
# libs: g++ $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
# g++ -shared -Wl,-soname, $(TARGET).so $(OBJS) $(LIBS)
