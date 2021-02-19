# NOTE - The rebuild script already pre-cleans, so no "clean" call needed.

default: helpscreen

helpscreen:
	@echo ""
	@echo "Targets:  clean  rebuild"
	@echo ""

clean:
	rm -f gallery.html
	make -C 20180115-v2 clean
	make -C 20181012-v2 clean
	make -C 20181017-panels clean
	make -C 20181023-adapters clean

rebuild:
	rm -f gallery.html
	make -C 20180115-v2 rebuild
	make -C 20181012-v2 rebuild
	make -C 20181017-panels rebuild
	make -C 20181023-adapters rebuild
	helpers/makegallery.pl --width=24% 20*

# This is the end of the file.
