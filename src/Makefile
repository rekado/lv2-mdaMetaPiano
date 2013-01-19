BUNDLE = lv2-mdaPiano.lv2
INSTALL_DIR = /usr/local/lib/lv2

SOURCES = *.cpp
FLAGS = -fPIC -DPIC -O $(WARNINGS)
WARNINGS = #-g -Wall -pedantic

$(BUNDLE): manifest.ttl mdaPiano.ttl mdaPiano.so presets data/*.raw
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp -r $^ $(BUNDLE)

mdaPiano.so: $(SOURCES) mdaPiano.peg
	$(CXX) $(FLAGS) -shared $(SOURCES) `pkg-config --cflags --libs lv2-plugin` -o $@

mdaPiano.peg: mdaPiano.ttl
	lv2peg $^ mdaPiano.peg

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

clean:
	rm -rf $(BUNDLE) mdaPiano.so mdaPiano.peg
