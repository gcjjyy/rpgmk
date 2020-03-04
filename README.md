# rpgmk
RPG쯔꾸르 DANTE PC의 16컬러 이미지 데이터를 읽어 출력하는 프로그램

# 빌드
```sh
$ brew install sdl2
$ make
```

# 사용법
```sh
$ ./imageview CHR00.DAT PALET.DAT 32 32
$ ./imageview ENEMY00.DAT PALET2.DAT 80 96
$ ./imageview OPNTITLE.DAT OPNTITLE.RGB 640 400
$ ./patnview
```

# 스크린샷
![example_01.png](example_01.png)
![example_02.png](example_02.png)
![example_03.png](example_03.png)
![example_04.png](example_04.png)

# 비고: RPGMK의 이미지 파일 특징
RPGMK의 이미지 파일은 한 픽셀이 4비트를 차지하고 있는 비압축 형식의 포맷이다. 하지만 보안 때문인지, 다르 어떤 이유 때문인지는 모르지만, 데이터를 매우 복잡하게 꼬아놓았기 때문에, 분석하는데 매우 애를 먹었다.

먼저 가장 쉬운 ENEMY00.DAT 같은 경우를 보면, 64x80의 이미지 한 장이 들어 있는데, 세로로 4개의 점에 대한 데이터를 총 16조각 내어 저장한다. 세로로 4개이기 때문에 비트수는 총 16비트가 된다. 또한 이 16개의 비트가 4x4크기의 행렬이라고 했을 때, 그 행렬 그대로 저장하는 것이 아니라, 행렬을 전치행렬로 만들어서 저장한다.

따라서, (0001)같은 비트를 각각 쪼개어 세로로 배치시켜서 저장한다는 의미이다.
