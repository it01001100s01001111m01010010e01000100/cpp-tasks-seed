CXX_EXTRA_FLAGS=-I/usr/include/eigen3

include ../Makefile.inc

CXXFLAGS += $(CXX_EXTRA_FLAGS)

release : gauss

gauss : util.r.o Gauss_main.r.o Gauss_solve.r.o
	$(LD) -o $@ $^

debug : util.d.o Gauss_main.d.o Gauss_solve.d.o
	$(LD) -o $@ $^

utest : Gauss_test.d.o util.d.o Gauss_solve.d.o
	$(LD) -o $@ $^ $(GTEST_LIBS)

run_utest : utest
	./$^

run_itest : release
	echo "Integration tests..."
	bash ./integration_test.sh

tidy:
	../tidy.sh

all: depends run_utest run_itest

-include Makefile.hdrdep
