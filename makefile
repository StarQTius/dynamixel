SHELL = /bin/bash -O extglob

cpp_flags = \
	-g \
	--coverage \
	-Wall \
	-Wextra \
	-Werror \
	-Iinclude \
	-Ilib/unpadded/include \
	-Ilib/Unity/src \

c_flags = \
	-std=c99 \
	-Ilib/Unity/src \

libs = \
	-lstdc++ \

install:
	([[ "$(DIR)" = /* ]] && [ -d $(DIR) ]) || (echo "$(DIR) is not a valid absolute path"; exit 1)
	cp -v -r include/* $(DIR)

install_dependencies:
	([[ "$(DIR)" = /* ]] && [ -d $(DIR) ]) || (echo "$(DIR) is not a valid absolute path"; exit 1)
	git submodule init lib/unpadded
	git submodule update --remote
	cd lib/unpadded && make install DIR=$(DIR) && make install_dependencies DIR=$(DIR)

check11: obj/cpp11/main.o obj/lib/unity.o
	gcc --coverage $^ $(libs) -o run_ut11
	./run_ut11

check14: obj/cpp14/main.o obj/lib/unity.o
	gcc --coverage $^ $(libs) -o run_ut14
	./run_ut14

check17: obj/cpp17/main.o obj/lib/unity.o
	gcc --coverage $^ $(libs) -o run_ut17
	./run_ut17

check20: obj/cpp20/main.o obj/lib/unity.o
	gcc --coverage $^ $(libs) -o run_ut20
	./run_ut20

check: check11 check14 check17 check20

gcov: check
	mkdir -p gcov
	cd gcov && gcov ../test/main.cpp -m -o ../obj

clean:
	rm obj/cpp?(11|14|17|20)/*.?(o|gcda|gcno)

mrproper: clean
	git submodule deinit --all

obj/cpp11/main.o: test/main.cpp
	gcc -std=c++11 $(cpp_flags) -DUT_ONLY -c $^ -o obj/cpp11/main.o

obj/cpp14/main.o: test/main.cpp
	gcc -std=c++14 $(cpp_flags) -DUT_ONLY -c $^ -o obj/cpp14/main.o

obj/cpp17/main.o: test/main.cpp
	gcc -std=c++17 $(cpp_flags) -DUT_ONLY -c $^ -o obj/cpp17/main.o

obj/cpp20/main.o: test/main.cpp
	gcc -std=c++20 $(cpp_flags) -DUT_ONLY -c $^ -o obj/cpp20/main.o

obj/lib/unity.o:
	gcc $(c_flags) -c lib/Unity/src/unity.c -o obj/lib/unity.o
