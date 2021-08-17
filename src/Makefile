# call Makefile in base directory if 'make' is executed here
base_dir := ..

%: ; $(MAKE) -C $(base_dir)$(MAKEFLAGS) $@
.PHONY: all
all: ; $(MAKE) -C $(base_dir)$(MAKEFLAGS)


# FIXME - calling target that matches a file in current directory
#  doesn't call the base dir makefile
#  (can't do wildcard .PHONY)
