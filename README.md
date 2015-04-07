# Sojong #
소종하자 졸업하자

광선추적법을 이용하여 .obj 파일을 렌더링합니다.

## 1. 컴파일 ##
Microsoft Visual Studio 또는 GNU 환경에서 사용 가능합니다.

#### GNU ####
gcc와 make가 필요합니다. gcc 4.9.2+, make 4.1+ 을 권장합니다.

#### Visual Studio ####
루트 폴더와 include 폴더, kdtree 폴더에 있는 .c파일과 .h 파일을 모두 프로젝트에 포함시킨 후 컴파일합니다. 솔루션을 위한 디렉토리는 만들지 않는 것을 권장합니다.

## 2. 사용 방법 ##
표준 3D Object 파일(.obj)을 준비합니다. [이곳](http://people.sc.fsu.edu/~jburkardt/data/obj/obj.html)에서 몇몇 파일들을 찾아볼 수 있습니다.

#### GNU ####
Makefile에 컴파일과 테스트를 위한 환경이 준비되어 있습니다. make 명령으로 사용 가능한 옵션을 확인할 수 있습니다.
 - `make debug` : 디버그 모드로 컴파일합니다.
 - `make release` : 가능한 한 최적화를 시도하며 디버그 메시지를 출력하지 않습니다.
 - `make test [option=""]` : 프로그램을 테스트합니다. `option=""`으로 프로그램 옵션을 사용할 수 있습니다.

`make` 명령으로 다른 옵션들을 확인할 수 있습니다. 명령줄 옵션은 `make test option="-h"` 로 확인할 수 있습니다.

#### Visual Studio ####
컴파일 시 Release 모드로 컴파일하면 디버그 메시지를 출력하지 않습니다. 옵션은 `settings.h`에서 디파인된 값을 조절하는 것으로 사용할 수 있습니다. 각각의 옵션은 해당 파일의 주석을 확인하기 바랍니다.
