test:
	cd tests && make test
	cd tests && make fulldebug
	cd tests && make opt
	# cd tests && make test-web
	source third-party/emsdk_portable_repo/emsdk_portable/emsdk_env.sh && cd examples/ProjectTemplate && make test

doc: build-doxygen-xml
	cd doc && ./make_docs.sh

coverage:
	cd tests && make test-coverage

build-doxygen-xml:
	./third-party/doxygen/build/bin/doxygen Doxyfile

install-dependencies:
	git submodule init
	git submodule update
	cd third-party && make

install-doc-dependencies:
	git submodule init
	git submodule update
	cd third-party && make install-doc-dependencies

install-test-dependencies:
	cd third-party && make install-test-dependencies

install-coverage-dependencies:
	cd third-party && make install-coverage-dependencies

clean:
	rm -rf build/*
	cd tests && make clean
clean-dep:
	cd third-party && make clean
