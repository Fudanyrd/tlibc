.PHONY: install clean
install:
	@cd null; $(MAKE); $(MAKE) install; cd ..; \
	cd zero; $(MAKE); $(MAKE) install; cd ..; \

clean:
	@cd zero; $(MAKE) clean; cd ..; \
	cd null; $(MAKE) clean; cd ..;
