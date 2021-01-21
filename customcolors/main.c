#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../shared/inline-hook/inlineHook.h"
#include "../shared/utils/utils.h"

//myTest classのUpdate()をHookして、以下の処理に入れ替える。
//ここでは、Update()で何もしない、処理に入れ替えることにする。

//dump.csから該当のメソッドのアドレスを読み取って引数に与える。

MAKE_HOOK(Update, 0x46C9E0, void, void* self)
{
    log("Mod");
}

__attribute__((constructor)) void lib_main()
{
    INSTALL_HOOK(Update);
    log("custom-colors loaded!");
}
