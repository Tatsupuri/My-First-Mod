#include <android/log.h>
#include <stdlib.h>
#include <time.h>
#include <stdalign.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "../shared/inline-hook/inlineHook.h"
#include "../shared/utils/utils.h"

//myTest class��Update()��Hook���āA�ȉ��̏����ɓ���ւ���B
//�����ł́AUpdate()�ŉ������Ȃ��A�����ɓ���ւ��邱�Ƃɂ���B

//dump.cs����Y���̃��\�b�h�̃A�h���X��ǂݎ���Ĉ����ɗ^����B

MAKE_HOOK(Update, 0x46C9E0, void, void* self)
{
    log("Mod");
}

__attribute__((constructor)) void lib_main()
{
    INSTALL_HOOK(Update);
    log("custom-colors loaded!");
}
