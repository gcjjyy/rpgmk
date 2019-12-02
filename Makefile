IMAGEVIEW = imageview
PATNVIEW = patnview

LDFLAGS = -lsdl2

all: $(IMAGEVIEW) $(PATNVIEW)

$(IMAGEVIEW): imageview.c rpgmk.c
	gcc -o $(IMAGEVIEW) $(LDFLAGS) imageview.c rpgmk.c

$(PATNVIEW): patnview.c rpgmk.c
	gcc -o $(PATNVIEW) $(LDFLAGS) patnview.c rpgmk.c

clean:
	@find . -name '*.o' -type f -delete
	@find . -name '$(IMAGEVIEW)' -type f -delete
	@find . -name '$(PATNVIEW)' -type f -delete

