all: abstraction blueprint evaluation search

abstraction:
	@echo Building src/abstraction...
	$(MAKE) -C src/abstraction

blueprint:
	@echo Building src/blueprint...
	$(MAKE) -C src/blueprint

evaluation:
	@echo Building src/evaluation...
	$(MAKE) -C src/evaluation

search:
	@echo Building src/search...
	$(MAKE) -C src/search