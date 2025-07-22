# Makefile for Assignment 5 with explicit build/eval tests

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
TARGET = a5

.PHONY: all clean \
	build1 build2 build3 build4 \
	eval1 eval2 eval3 eval4 \
	invalid1 invalid2 invalid3 test

# Build the executable
all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o

main.o: main.c hbt.h
	$(CC) $(CFLAGS) -c main.c

# Clean up
clean:
	rm -f main.o $(TARGET) tmp_build*.b tmp_eval*.txt tmp_inv*.txt

# --- Build-mode tests ---
build1: all
	@./$(TARGET) -b examples/ops0.b tmp_build0.b && \
	  cmp -s tmp_build0.b examples/tree0.b && \
	  echo "build1 PASS" || echo "build1 FAIL"

build2: all
	@./$(TARGET) -b examples/ops1.b tmp_build1.b && \
	  cmp -s tmp_build1.b examples/tree1.b && \
	  echo "build2 PASS" || echo "build2 FAIL"

build3: all
	@./$(TARGET) -b examples/ops2.b tmp_build2.b && \
	  cmp -s tmp_build2.b examples/tree2.b && \
	  echo "build3 PASS" || echo "build3 FAIL"

build4: all
	@./$(TARGET) -b examples/ops3.b tmp_build3.b && \
	  cmp -s tmp_build3.b examples/tree3.b && \
	  echo "build4 PASS" || echo "build4 FAIL"

# --- Eval-mode tests for valid trees ---
eval1: all
	@OUT=$$(./$(TARGET) -e examples/tree0.b) && \
	  [ "$$OUT" = "1,1,1" ] && echo "eval1 PASS" || echo "eval1 FAIL"

eval2: all
	@OUT=$$(./$(TARGET) -e examples/tree1.b) && \
	  [ "$$OUT" = "1,1,1" ] && echo "eval2 PASS" || echo "eval2 FAIL"

eval3: all
	@OUT=$$(./$(TARGET) -e examples/tree2.b) && \
	  [ "$$OUT" = "1,1,1" ] && echo "eval3 PASS" || echo "eval3 FAIL"

eval4: all
	@OUT=$$(./$(TARGET) -e examples/tree3.b) && \
	  [ "$$OUT" = "1,1,1" ] && echo "eval4 PASS" || echo "eval4 FAIL"

# --- Eval-mode tests for invalid trees ---
invalid1: all
	@OUT=$$(./$(TARGET) -e examples/invalidtree0.b) && \
	  [ "$$OUT" = "1,0,0" ] && echo "invalid1 PASS" || echo "invalid1 FAIL"

invalid2: all
	@OUT=$$(./$(TARGET) -e examples/invalidtree1.b) && \
	  [ "$$OUT" = "1,0,1" ] && echo "invalid2 PASS" || echo "invalid2 FAIL"

invalid3: all
	@OUT=$$(./$(TARGET) -e examples/invalidtree2.b) && \
	  [ "$$OUT" = "1,1,0" ] && echo "invalid3 PASS" || echo "invalid3 FAIL"

# Run all tests
test: build1 build2 build3 build4 \
	eval1 eval2 eval3 eval4 \
	invalid1 invalid2 invalid3
