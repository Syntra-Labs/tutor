.PHONY: all build clean test install

all: build

build:
	@if [ -d "crates/lgcc" ]; then \
		cd crates/lgcc && cargo build --release && cp target/release/lgcc ../../bin/lgcc; \
	fi
	python3 -m pip install -e . --no-deps 2>/dev/null || true

test:
	./bin/tutor test 000a

install: build
	mkdir -p $(HOME)/.local/bin
	ln -sf $(PWD)/bin/tutor $(HOME)/.local/bin/tutor
	ln -sf $(PWD)/bin/tutorvim $(HOME)/.local/bin/tutorvim
	@if [ -f "$(PWD)/bin/lgcc" ]; then ln -sf $(PWD)/bin/lgcc $(HOME)/.local/bin/lgcc; fi
	@echo "Installed tutor and tutorvim to $(HOME)/.local/bin/"

clean:
	@if [ -d "crates/lgcc" ]; then cd crates/lgcc && cargo clean; fi
	rm -rf build dist *.egg-info
