all:
	clang++ -O3 pstr.c++&& ./a.out > benchmark.txt && cat benchmark.txt
