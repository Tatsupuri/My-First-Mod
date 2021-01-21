// thx https://github.com/jbro129/Unity-Substrate-Hook-Android

#include <jni.h>

#include "utils.h"
#include <android/log.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <unistd.h>


long baseAddr(char *soname)  // credits to https://github.com/ikoz/AndroidSubstrate_hookingC_examples/blob/master/nativeHook3/jni/nativeHook3.cy.cpp
{
    __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "baseAddr is called");


    // /prpc/self がちゃんと機能していないようだったのでgetpid()で取得

    __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "PID = %d", getpid());
   
    char pid[8];
    char path[24] = "/proc/";

    sprintf(pid, "%d", getpid());

    strcat(path, pid);
    strcat(path, "/maps");



    void *imagehandle = dlopen(soname, RTLD_LOCAL | RTLD_LAZY);
    
    if (soname == NULL) {
        __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "soname is null");
        return NULL;
    }
    if (imagehandle == NULL){
        __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "imagehandle is null");
        return NULL;
    }

    uintptr_t * irc = NULL;
    FILE *f = NULL;
    char line[200] = {0};
    char *state = NULL;
    char *tok = NULL;
    char * baseAddr = NULL;
    
    //if ((f = fopen("/proc/self/maps", "r")) == NULL)
    if ((f = fopen(path, "r")) == NULL)
    {
        __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "no maps file");
        return NULL;
    }
    
    
    
    while (fgets(line, 199, f) != NULL)
    {
        tok = strtok_r(line, "-", &state);//strtok_r はトークンで分解
        baseAddr = tok;

        tok = strtok_r(NULL, "\t ", &state);
        tok = strtok_r(NULL, "\t ", &state); // "r-xp" field
        tok = strtok_r(NULL, "\t ", &state); // "0000000" field
        tok = strtok_r(NULL, "\t ", &state); // "01:02" field
        tok = strtok_r(NULL, "\t ", &state); // "133224" field
        tok = strtok_r(NULL, "\t ", &state); // path field

        //__android_log_print(ANDROID_LOG_INFO, "BaseAddr", "tok = %s", tok);

        //tokにはpath fieldが格納されている
        if (tok != NULL) {
            //__android_log_print(ANDROID_LOG_INFO, "BaseAddr", "tok = %s", tok);
            int i;
            for (i = (int)strlen(tok)-1; i >= 0; --i) 
            {
                //空白などを0に置き換える
                if (!(tok[i] == ' ' || tok[i] == '\r' || tok[i] == '\n' || tok[i] == '\t'))
                    break;
                tok[i] = 0;
            }
            //__android_log_print(ANDROID_LOG_INFO, "BaseAddr", "tok_modified = %s", tok);
            
            {
                size_t toklen = strlen(tok);
                size_t solen = strlen(soname);

                //tokがlibil2cpp.soのパスと一致しているとき、そのアドレスを返す
                if (toklen > 0) 
                {
                    if (toklen >= solen && strcmp(tok + (toklen - solen), soname) == 0) //strcmpは二つの文字列の比較、一致していたら0を返す
                    {
                        fclose(f);


                        __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "baseAddr = %s", baseAddr);
                        return (long)strtoll(baseAddr,NULL,16);//baseAddrを16進数にして,longlong型にして返す、それができない時はNullを返す。返り値はlongにキャストされている。
                    }
                }
            }
        }

    }
    fclose(f);

    __android_log_print(ANDROID_LOG_INFO, "BaseAddr", "some error is happend");
    return NULL;
}

long location; // save lib.so base address so we do not have to recalculate every time causing lag.

long getRealOffset(long offset) // calculate dump.cs address + lib.so base address.
{
    log("getRealOffset is called");


    // libil2cpp.so のアドレスを取得し、オフセットの正しい値を計算する。
    if (location == 0)
    {
        log("location will be set");
        
        //soの場所が暗号化？されているので、シンボリックリンクから参照元のパスを取得する

        char org_path[1024];
        ssize_t len = readlink("/data/user/0/com.DefaultCompany.Mod_Test/lib", org_path, sizeof(org_path) - 1);

        //readlinkはnull終端させないので文字列として扱うにはnullを付け足す。
        org_path[len] = '\0';

        //フルパスを取得
        strcat(org_path,"/libil2cpp.so");
        
        log("so: %s", org_path);
        
        location = baseAddr(org_path);
        //log("location = %ld", location);
    }

    //log("real offset is %ld", location + offset);
    return location + offset;
}



void csstrtostr(cs_string* in, char* out)
{
    for(int i = 0; i < in->len; i++) {
        out[i] = in->str[i*2];
    }
    out[in->len] = '\0';
}