# Sojong #
소종하자 졸업하자

광선추적법을 이용하여 .obj 파일을 렌더링합니다.

## 1. 컴파일 ##
Microsoft Visual Studio 또는 GNU 환경에서 사용 가능합니다.

#### GNU ####
gcc와 make가 필요합니다. gcc 4.9.2 이상, make 4.1 이상 버전을 권장합니다.

#### Visual Studio ####
Microsoft Visual Studio 2013 (12.0.21005.1 REL) 버전을 권장합니다. 하위 버전 호환성은 테스트되지 않았습니다.

## 2. 실행 방법 ##
GNU 환경이라면 RayTracing.exe 실행 파일이, Visual Studio 환경이라면 Debug 또는 Release 폴더 안에 Sojong.exe 실행 파일이 생성됩니다. 콘솔 창에서 실행하는 것을 권장합니다.
다음과 같은 옵션을 사용할 수 있습니다.
  -c COUNT, --count=COUNT               생성할 frame 수를 설정합니다.
  -a ALGORITHM_NAME                     검색 알고리즘을 선택합니다. 현재는 naive, nlog2n, nlogn을 사용할 수 있습니다.
  -s SCALE                              이미지를 확대 또는 축소합니다. 기본값은 1.0입니다.
  -f FILENAME, --file=FILENAME          .obj 파일을 지정합니다.
  -h, --help                            도움말을 출력한 후 종료합니다.

## 3. 사용 방법 ##
표준 3D Object 파일(.obj)을 준비합니다. [이곳](http://people.sc.fsu.edu/~jburkardt/data/obj/obj.html)에서 몇몇 파일들을 찾아볼 수 있습니다.

#### GNU ####
Makefile에 컴파일과 테스트를 위한 환경이 준비되어 있습니다. make 명령으로 사용 가능한 옵션을 확인할 수 있습니다.
 - `make debug` : 디버그 모드로 컴파일합니다.
 - `make release` : 가능한 한 최적화를 시도하며 디버그 메시지를 출력하지 않습니다.
 - `make test [option=""]` : 프로그램을 테스트합니다. `option=""`으로 프로그램 옵션을 사용할 수 있습니다.

`make` 명령으로 다른 옵션들을 확인할 수 있습니다. 명령줄 옵션은 `make test option="-h"` 로 확인할 수 있습니다.

#### Visual Studio ####
컴파일 시 Release 모드로 컴파일하면 디버그 메시지를 출력하지 않습니다. 옵션은 `settings.h`에서 디파인된 값을 조절하는 것으로 사용할 수 있습니다. 각각의 옵션은 해당 파일의 주석을 확인하기 바랍니다.

## 4. 새로운 알고리즘 추가 방법 ##
새로운 알고리즘을 도입할 때에는 다음과 같이 수행합니다.

  1. 새로운 가속구조체를 도입하는 경우 적당한 폴더를 만듭니다.
  2. 가속구조체 생성과 해제, 교차 검사에 대한 함수를 만듭니다. 기존에 만들어져 있는 함수를 동일하게 사용하는 경우에는 만들지 않아도 됩니다. 형식은 다음과 같습니다:

    ```c
    void {알고리즘_이름}_clear_accel(Data *data);
    void {알고리즘_이름}_accel_build(Data *data);
    Hit {알고리즘_이름}_intersect_search(Data *data, Ray *ray);
    ```
  3. 위의 함수들을 헤더 파일에 추가한 뒤, 해당 헤더 파일을 algorithms.c 상단에 추가합니다.

    ```c
    ////////////////////////////////////////
    // search algorithms
    // {알고리즘 이름}
    #include "{알고리즘_이름}.h"
    ```
  4. algorithms.c의 `init_search_algo`함수에 알고리즘을 적당히 추가합니다. 기존에 만들어져 있는 함수를 동일하게 사용하는 경우 해당 함수를 기록합니다.

    ```c
    else if (strncmp(algo_name, "{알고리즘_이름}", {글자수}) == 0)
    {
      printf("{알고리즘_선택시_안내문구}\n");
      clear_accel = &{알고리즘_이름}_clear_accel;
      accel_build = &{알고리즘_이름}_accel_build;
      intersect_search = &{알고리즘_이름}_intersect_search;
    }
    ```
  5. 위의 1.에서 폴더를 추가한 경우 Makefile에 해당 폴더를 추가합니다.

    ```makefile
      ### {폴더이름} folder add
      SRCS += $(addprefix {폴더이름}/, $(shell ls {폴더이름} | grep c$$))
      DIR_CHECK += {폴더이름}
    ```
