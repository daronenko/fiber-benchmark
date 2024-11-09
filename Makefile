CMAKE_COMMON_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# Available sanitizers: UBSAN, ASAN, TSAN
CMAKE_FLAGS ?= -DUBSAN=ON -DASAN=ON
NPROCS ?= $(shell nproc)

CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy

CMAKE_DEBUG_FLAGS += -DCMAKE_BUILD_TYPE=Debug $(CMAKE_COMMON_FLAGS)
CMAKE_RELEASE_FLAGS += -DCMAKE_BUILD_TYPE=Release $(CMAKE_COMMON_FLAGS)

.PHONY: cmake
cmake:
	@cmake -B build $(CMAKE_FLAGS)

build/CMakeCache.txt: cmake

target ?= undefined

.PHONY: build
# Usage: make build target=fibers
build: build/CMakeCache.txt
	@cmake --build build -j $(NPROCS) --target server-$(target)

target ?= undefined
port ?= undefined

.PHONY: run
# Usage: make run target=fibers
run:
	@build/server/$(target)/server-$(target) $(port)

.PHONY: clean
clean:
	@rm -rf build

.PHONY: format
format:
	@find server -name '*pp' -type f | xargs $(CLANG_FORMAT) -i

.PHONY: lint
lint:
	@find server -name '*pp' -type f | xargs $(CLANG_TIDY) -p build