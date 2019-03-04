# Project-specific settings
PROJECT := project_name
EMP_DIR := ../../../Empirical/source

# Flags to use regardless of compiler
CFLAGS_all := -Wall -Wno-unused-function -std=c++17 -I$(EMP_DIR)/

# Native compiler information
CXX_nat := g++
CFLAGS_nat := -O3 -DNDEBUG $(CFLAGS_all)
CFLAGS_nat_debug := -g $(CFLAGS_all)

# Emscripten compiler information
CXX_web := emcc
OFLAGS_web_all := -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" -s TOTAL_MEMORY=67108864 --js-library $(EMP_DIR)/web/library_emp.js -s EXPORTED_FUNCTIONS="['_main', '_empCppCallback']" -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=1 #--embed-file configs
OFLAGS_web := -Oz -DNDEBUG
OFLAGS_web_debug := -g4 -Oz -Wno-dollar-in-identifier-extension

CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web) $(OFLAGS_web_all)
CFLAGS_web_debug := $(CFLAGS_all) $(OFLAGS_web_debug) $(OFLAGS_web_all)


default: $(PROJECT)
native: $(PROJECT)
web: $(PROJECT).js
all: $(PROJECT) $(PROJECT).js

debug:	CFLAGS_nat := $(CFLAGS_nat_debug)
debug:	$(PROJECT)

debug-web:	CFLAGS_web := $(CFLAGS_web_debug)
debug-web:	$(PROJECT).js

web-debug:	debug-web

test-web: CFLAGS_web := $(CFLAGS_all) $(OFLAGS_web_debug) $(OFLAGS_web_all) -s WASM=0
test-web:	$(PROJECT).js

$(PROJECT):	source/native/$(PROJECT).cc
	$(CXX_nat) $(CFLAGS_nat) source/native/$(PROJECT).cc -o $(PROJECT)
	@echo To build the web version use: make web

$(PROJECT).js: source/web/$(PROJECT)-web.cc
	$(CXX_web) $(CFLAGS_web) source/web/$(PROJECT)-web.cc -o web/$(PROJECT).js

clean:
	rm -f $(PROJECT) web/$(PROJECT).js web/*.js.map web/*.js.map *~ source/*.o web/*.wasm web/*.wast

test: debug test-web
	./project_name | grep -q 'Hello, world!' && echo 'matched!' || exit 1
	echo "var page = new WebPage(); var fs = require('fs'); page.onLoadFinished = function() { console.log(page.content); phantom.exit(); }; page.open('./project_name.html', function() { page.evaluate(function() { }); });" > temp.js
	cd web && phantomjs ../temp.js | tr -d '\n' | grep -q "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>Project Name</title></head><body><div id=\"emp_base\"><span id=\"emp__0\"><h1>Hello, browser!</h1></span></div><script src=\"jquery-1.11.2.min.js\"></script><script type=\"text/javascript\" src=\"project_name.js\"></script></body></html>" && echo "matched!" || exit 1
	echo "var page = new WebPage(); var fs = require('fs'); page.onLoadFinished = function() { phantom.exit(); }; page.open('./project_name.html', function() { page.evaluate(function() { }); }); page.onConsoleMessage = function(msg, lineNum, sourceId) { console.log(msg); };" > temp.js
	cd web && phantomjs ../temp.js | grep -q "Hello, console!" || exit 1
	rm -f temp.js

# Debugging information
print-%: ; @echo '$(subst ','\'',$*=$($*))'
