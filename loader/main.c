#include <android/log.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <dirent.h> 
#include <linux/limits.h> 
#include <sys/sendfile.h> 
#include <dlfcn.h> 

#define MOD_PATH "/sdcard/Android/data/com.DefaultCompany.Mod_Test/files/mods/" 
#define MOD_TEMP_PATH "/data/data/com.DefaultCompany.Mod_Test/cache/curmod.so" 


void load_mods()
{
    __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Loading mods!");
    const char* mods = "/sdcard/Android/data/com.beatgames.beatsaber.demo/files/mods/";
    struct stat statBuf;
    //mkdir does not seem to work 
    if (stat(mods, &statBuf) == 0)
    {
        __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Directory for Mods exist");
    }
    else
    {
        __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Directory for Mods does not exist");
        __android_log_print(ANDROID_LOG_INFO, "QuestHook", "mkdir = %d", mkdir("/sdcard/Android/data/com.beatgames.beatsaber.demo/files/mods/", 0777));
    }
    DIR* dir;
    struct dirent* file_list;
    __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Entry is created");
    dir = opendir(mods);
    if (dir == NULL)
    {
        __android_log_write(ANDROID_LOG_INFO, "QuestHook", "No mod");
    }
    else
    {
        file_list = readdir(dir);
        while (file_list != NULL)
        {
            // Only attempts to load .so files 
            if (strlen(file_list->d_name) > 3 && !strcmp(file_list->d_name + strlen(file_list->d_name) - 3, ".so"))
            {
                char full_path[PATH_MAX] = "/sdcard/Android/data/com.beatgames.beatsaber.demo/files/mods/";
                strcat(full_path, file_list->d_name);
                __android_log_print(ANDROID_LOG_INFO, "QuestHook", "Loading mod: %s", full_path);
                // Get filesize of mod 
                int infile = open(full_path, O_RDONLY);
                off_t filesize = lseek(infile, 0, SEEK_END);
                lseek(infile, 0, SEEK_SET);
                // Unlink old file 
                unlink(MOD_TEMP_PATH);
                // Creates temporary copy (we can't execute stuff in /sdcard so we need to copy it over) 
                int outfile = open(MOD_TEMP_PATH, O_WRONLY | O_CREAT, 0600);
                sendfile(outfile, infile, 0, filesize);
                close(infile);
                close(outfile);
                // Mark copy as executable 
                chmod(MOD_TEMP_PATH, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP);
                // and load it 
                dlopen(MOD_TEMP_PATH, RTLD_NOW);
            }

            file_list = readdir(dir);
        }
        closedir(dir);
    }
    __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Done loading mods!");
}



__attribute__((constructor)) void lib_main()
{
    __android_log_write(ANDROID_LOG_INFO, "QuestHook", "Welcome Mod!");
    load_mods();
}