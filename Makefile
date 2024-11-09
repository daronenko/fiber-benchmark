PROJECT_NAME ?= fiber-benchmark

CMAKE_COMMON_FLAGS ?= -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# Available sanitizers: UBSAN, ASAN, TSAN
CMAKE_FLAGS ?= -DUBSAN=ON -DASAN=ON
NPROCS ?= $(shell nproc)

CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy

DOCKER ?= docker
DOCKER_COMPOSE ?= docker compose

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

target ?= fibers
port ?= 9090

.PHONY: run
# Usage: make run target=fibers port=8080
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

target ?= fibers
port ?= 9090

.PHONY: docker-cmake docker-build docker-run docker-clean
docker-cmake docker-build docker-run docker-clean: docker-%:
	@$(DOCKER_COMPOSE) -f docker/docker-compose.yml run -p $(port):$(port) --rm $(PROJECT_NAME)-container make $* target=$(target) port=$(port)

.PHONY: clean-docker
clean-docker:
	@$(DOCKER_COMPOSE) -f docker/docker-compose.yml down -v --rmi all
