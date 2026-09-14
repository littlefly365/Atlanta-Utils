SCRIPT=syspkg

all: $(SCRIPT)

$(SCRIPT):
	@install -m 755 syspkg.sh syspkg
clean:
	rm -rf $(SCRIPT)
install:
	install -Dm 755 $(SCRIPT) $(DESTDIR)$(BINDIR)/$(SCRIPT)
