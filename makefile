
main: src/test_main.c src/tokenizer.hpp src/file-to-buf.hpp
	$(CXX) -ggdb src/test_main.c -o test

clean: 
	rm test
