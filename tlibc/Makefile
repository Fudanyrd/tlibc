# auto generated, do not modify.
include Makefile.config

all: _cat _crash _echo _kill _link _ls _mkdir _pause _pwd _seq _sh _stat _wc _yes
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

_cat: cat.o sys.o 
	$(LD) cat.o sys.o -o _cat 
_crash: crash.o sys.o 
	$(LD) crash.o sys.o -o _crash 
_echo: echo.o sys.o stdio.o string.o 
	$(LD) echo.o sys.o stdio.o string.o -o _echo 
_kill: kill.o sys.o atoi.o 
	$(LD) kill.o sys.o atoi.o -o _kill 
_link: link.o sys.o 
	$(LD) link.o sys.o -o _link 
_ls: ls.o sys.o stdio.o string.o 
	$(LD) ls.o sys.o stdio.o string.o -o _ls 
_mkdir: mkdir.o sys.o 
	$(LD) mkdir.o sys.o -o _mkdir 
_pause: pause.o sys.o 
	$(LD) pause.o sys.o -o _pause 
_pwd: pwd.o sys.o string.o 
	$(LD) pwd.o sys.o string.o -o _pwd 
_seq: seq.o sys.o stdio.o string.o atoi.o 
	$(LD) seq.o sys.o stdio.o string.o atoi.o -o _seq 
_sh: sh.o sys.o stdio.o string.o system.o 
	$(LD) sh.o sys.o stdio.o string.o system.o -o _sh 
_stat: stat.o sys.o stdio.o string.o 
	$(LD) stat.o sys.o stdio.o string.o -o _stat 
_wc: wc.o sys.o stdio.o string.o 
	$(LD) wc.o sys.o stdio.o string.o -o _wc 
_yes: yes.o sys.o 
	$(LD) yes.o sys.o -o _yes 


.PHONY: clean collect
clean:
	-rm -f *.o *.d _*
collect:
	@echo cat crash echo kill link ls mkdir pause pwd seq sh stat wc yes
