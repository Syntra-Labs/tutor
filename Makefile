.PHONY: all build clean test install

all: build

build:
	cd crates/lgcc && cargo build --release
	cp crates/lgcc/target/release/lgcc bin/lgcc

test:
	./bin/tutor test

install: build
	mkdir -p $(HOME)/.local/bin
	ln -sf $(PWD)/bin/tutor $(HOME)/.local/bin/tutor
	ln -sf $(PWD)/bin/tutorvim $(HOME)/.local/bin/tutorvim
	ln -sf $(PWD)/bin/lgcc $(HOME)/.local/bin/lgcc
	@echo "Installed tutor, tutorvim, and lgcc to $(HOME)/.local/bin/"

clean:
	cd crates/lgcc && cargo clean
	rm -f bin/lgcc
