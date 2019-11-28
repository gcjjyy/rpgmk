TARGET = rpgmk

LDFLAGS = -lsdl2

all: $(TARGET)

$(TARGET): rpgmk.c
	gcc -o $(TARGET) $(LDFLAGS) rpgmk.c

clean:
	@find . -name '$(TARGET)' -type f -delete

