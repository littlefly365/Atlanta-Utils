SCRIPT=sysupdate

all: $(SCRIPT)

$(SCRIPT):
	@install -m 755 sysupdate.sh sysupdate
clean:
	rm -rf $(SCRIPT)
install:
	install -Dm 755 $(SCRIPT) $(DESTDIR)$(BINDIR)/$(SCRIPT)
