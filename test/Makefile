COMPILER				:=	$(CXX)
COMPILER_FLAG			:=	-std=c++98 -pedantic -fPIC -Wall -Wextra -Werror $(CPPFLAGS) $(CXXFLAGS)

INCLUDES				:=	-I../include
LIBRARIES_DEP			:=	../lib/configator.a
LIVRARIES				:=	$(LIBRARIES_DEP) -lgtest -lgtest_main -lgmock -lpthread
DEPENDANCIES			:=	..

SOURCES_EXTENTION		:=	cpp

BUILD_DIRECTORY			:=	./build
BINARY_DIRECTORY		:=	./bin

SOURCES					=	$(wildcard *.$(SOURCES_EXTENTION))
OBJECTS					:=	$(addprefix $(BUILD_DIRECTORY)/,$(SOURCES:.$(SOURCES_EXTENTION)=.o))
TESTS					:=	$(addprefix $(BINARY_DIRECTORY)/,$(SOURCES:.$(SOURCES_EXTENTION)=.gtest))

all: $(DEPENDANCIES)
	$(MAKE) $(TESTS)

clean:
	$(RM) -r $(BUILD_DIRECTORY) $(BINARY_DIRECTORY)

re: clean
	$(MAKE) all

$(DEPENDANCIES):
	$(MAKE) -C $@

.PHONY: all clean re $(DEPENDANCIES)

$(BINARY_DIRECTORY):
	mkdir -p $@

$(BUILD_DIRECTORY):
	mkdir -p $(sort $(dir $(OBJECTS)))

$(BUILD_DIRECTORY)/%.o: %.$(SOURCES_EXTENTION) $(LIBRARIES_DEP) | $(BUILD_DIRECTORY)
	$(COMPILER) $(COMPILER_FLAG) -MMD -o $@ -c $< $(INCLUDES)

$(BINARY_DIRECTORY)/%.gtest: $(BUILD_DIRECTORY)/%.o | $(BINARY_DIRECTORY)
	$(COMPILER) $(COMPILER_FLAG) -o $@ $< $(LIVRARIES)

#.PRIMARY: $(DEPENDANCIES)
.SECONDARY: $(OBJECTS)

-include $(OBJECTS:.o=.d)