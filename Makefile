
PREFIX      ?= /usr/local
CFLAGS      += -Wall -std=gnu99
CPPFLAGS    += -DG_DISABLE_DEPRECATED \
               -DPREFIX=\"$(PREFIX)\"

# Usage:
#   $(eval $(call PKG, name, module1 [module2 [... moduleN]]))
#
# Creates variables:
#   PKG_name_MODULES
#   PKG_name_CFLAGS
#   PKG_name_LDLIBS
define PKG
PKG_$1_MODULES := $2
PKG_$1_CFLAGS  := $$(shell pkg-config $$(PKG_$1_MODULES) --cflags)
PKG_$1_LDLIBS  := $$(shell pkg-config $$(PKG_$1_MODULES) --libs)
endef

$(eval $(call PKG,WEBKIT,webkit2gtk-3.0 gtk+-3.0))
$(eval $(call PKG,WEBKIT_EXTENSION,webkit2gtk-web-extension-3.0))

all: groover groover.desktop

groover: CFLAGS  += $(PKG_WEBKIT_CFLAGS)
groover: LDFLAGS += $(PKG_WEBKIT_LDLIBS)
groover: groover.o groover.gresources.o

#%: %.o
#	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

groover.gresources.o: menus.xml

%.gresources.c: %.gresources.xml
	glib-compile-resources --generate-source --target=$@ $<

%.gresources.h: %.gresources.xml
	glib-compile-resources --generate-header --target=$@ $<

clean:
	$(RM) groover groover.o

install: all
	install -m 755 -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 -t $(DESTDIR)$(PREFIX)/bin groover
	install -m 755 -d $(DESTDIR)$(PREFIX)/share/applications
	install -m 644 -t $(DESTDIR)$(PREFIX)/share/applications groover.desktop

ifeq ($(origin TAG),command line)
VERSION := $(TAG)
else
VERSION := $(shell git tag 2> /dev/null | tail -1)
endif

dist:
ifeq ($(strip $(VERSION)),)
	@echo "ERROR: Either Git is not installed, or no tags were found"
else
	git archive --prefix=groover-$(VERSION)/ $(VERSION) | xz -c > groover-$(VERSION).tar.xz
endif

print-flags:
	@echo "$(PKG_WEBKIT_CFLAGS) $(PKG_WEBKIT_EXTENSION) $(CPPFLAGS)"

.PHONY: clean install dist print-flags

