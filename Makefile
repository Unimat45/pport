SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)
$(SUBDIRS):
		$(MAKE) -C $@

.PHONY: all $(SUBDIRS)

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done