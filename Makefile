.PHONY: all
all: opt

opt:
	$(MAKE) -C src/libs/libchip8 opt
	$(MAKE) -C src/main opt

debug:
	$(MAKE) -C src/libs/libchip8 debug
	$(MAKE) -C src/main debug
	$(MAKE) -C src/testchip8 debug
	src/testchip8/testchip8

coverage:
	$(MAKE) -C src/libs/libchip8 coverage
	$(MAKE) -C src/main coverage
	$(MAKE) -C src/testchip8 coverage
	src/testchip8/testchip8

.PHONY: test
test:
	$(MAKE) -C src/libs/libchip8 all
	$(MAKE) -C src/testchip8 all
	src/testchip8/testchip8

.PHONY: clean
clean:
	$(MAKE) -C src/libs/libchip8 clean
	$(MAKE) -C src/main clean
	$(MAKE) -C src/testchip8 clean
