TYPE = mdaPiano
ifeq ($(TYPE),mdaPiano)
	BUILD_TYPE = PIANO
else
	BUILD_TYPE = EPIANO
endif

BUNDLE = lv2-$(TYPE).lv2
INSTALL_DIR = /usr/local/lib/lv2

SOURCES = *.cpp
FLAGS = -fPIC -DPIC -O $(WARNINGS) -D$(BUILD_TYPE)
WARNINGS = -g -Wall -pedantic

$(BUNDLE): manifest.ttl $(TYPE).ttl $(TYPE).so $(TYPE)/presets $(TYPE)/data/*.raw
	rm -rf $(BUNDLE)
	mkdir $(BUNDLE)
	cp -r $^ $(BUNDLE)

$(TYPE).so: $(SOURCES) $(TYPE).peg
	$(CXX) $(FLAGS) -shared $(SOURCES) `pkg-config --cflags --libs lvtk-plugin-1` -o $@

$(TYPE).peg: $(TYPE).ttl
	ttl2c $^ $(TYPE).peg

install: $(BUNDLE)
	mkdir -p $(INSTALL_DIR)
	rm -rf $(INSTALL_DIR)/$(BUNDLE)
	cp -R $(BUNDLE) $(INSTALL_DIR)

clean:
	rm -rf $(BUNDLE) $(addprefix $(TYPE),.so .peg)
