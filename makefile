# tool macros
CC = gcc
CXX ?= gcc
CFLAGS := -std=c++17 -Iheaders/ -Iexceptions/
CXXFLAGS :=  
DBGFLAGS := -g
COBJFLAGS := $(CFLAGS) -c -MD
LIBS := -lstdc++ -lstdc++fs -lpthread -lcrypt -lcurlpp -lcurl -lwiringPi -lwiringPiDev -lssl -lcrypto -ljwt -lm

# path macros
BIN_PATH := bin
OBJ_PATH := obj
SRC_PATH := source
DBG_PATH := debug
TEST_PATH := tests

# compile macros
TARGET_NAME := hsp
TEST_TARGET_PREFIX := test
ifeq ($(OS),Windows_NT)
	TARGET_NAME := $(addsuffix .exe,$(TARGET_NAME))
endif
TARGET := $(BIN_PATH)/$(TARGET_NAME)
TARGET_DEBUG := $(DBG_PATH)/$(TARGET_NAME)
TEST_TARGETS := $(patsubst $(TEST_PATH)/%.cpp, $(BIN_PATH)/%, $(foreach x, $(wildcard $(TEST_PATH)/*), $(x)))

# src files & obj files
SRC := $(foreach x, $(SRC_PATH), $(wildcard $(addprefix $(x)/*,.c*)))
TEST := $(filter-out $(SRC_PATH)/Main.cpp, $(SRC))
OBJ := $(filter-out $(OBJ_PATH)/RCSim.o, $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC))))))
OBJ_TEST := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(TEST)))))
OBJ_DEBUG := $(addprefix $(DBG_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
                  $(OBJ_DEBUG) \
				  $(OBJ_TEST)
CLEAN_LIST := $(TARGET) \
			  $(TARGET_DEBUG) \
			  $(TEST_TARGETS) \
			  $(DISTCLEAN_LIST)

# default rule
default: makedir all

# non-phony targets

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) -DRPI $(COBJFLAGS) -o $@ $<

$(TEST_TARGETS): bin/%: $(OBJ_TEST) $(DBG_PATH)/%.o
	$(CC) -o $@ $(OBJ_TEST) $(DBG_PATH)/$*.o $(CFLAGS) $(LIBS)

$(DBG_PATH)/%.o: $(TEST_PATH)/%.c*
	$(CC) $(COBJFLAGS) -o $@ $<

$(DBG_PATH)/%.o: $(SRC_PATH)/%.c*
	$(CC) $(COBJFLAGS) $(DBGFLAGS) -o $@ $<

$(TARGET_DEBUG): $(OBJ_DEBUG)
	$(CC) $(CFLAGS) $(DBGFLAGS) $(OBJ_DEBUG) $(LIBS) -o $@

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH) $(DBG_PATH)

.PHONY: all
all: $(TARGET)

.PHONY: test
tests: $(TEST_TARGETS)

.PHONY: debug
debug: $(TARGET_DEBUG)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)

-include $(OBJ:.o=.d)
-include $(OBJ_DEBUG:.o=.d)
-include $(OBJ_TEST:.o=.d)