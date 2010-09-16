
// [HK] added <stdlib.h> to have prototype of exit function
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

void enter_directory_from_path (char * p, size_t bufsize)
{
    char* d = (char*)malloc(bufsize);
    strcpy(d,p);

    *(strrchr(d,'/')) = '\0';
    if (chdir(d)) {
        fprintf(stderr,"cannot enter directory : %s\n", d);
        exit(1);
    }
    free(d);
}

int resolvepath(const char *p, char *buf, size_t bufsize)
{
    struct stat* stat_info = (struct stat*) malloc(sizeof(struct stat));
    char* path = (char*)malloc(bufsize);
    char* lpath = (char*)malloc(bufsize);
    char* file_name = (char*)malloc(bufsize);
    char* cwd = (char*)malloc(bufsize);
    int stat_status;

    fprintf(stderr,"entering resolvepath with %s\n", p);

    // first make a copy since we do not want to touch p
    strcpy(path,p);

    getcwd(cwd,bufsize);
    enter_directory_from_path(path,bufsize);

    stat_status = lstat(path, stat_info);
    if (stat_status) {
        fprintf(stderr,"cannot stat file %s", path);
        exit(1);
    }
    
    while ( S_ISLNK((*stat_info).st_mode)) {
          
        // access the link content
        int n = readlink(path, lpath, bufsize);
        *(lpath+n)='\0';
        
        // access the file name
        strcpy(file_name,strrchr(lpath,'/')+1);

        // access the directory
        enter_directory_from_path(lpath,bufsize);
        lpath = getcwd(lpath,bufsize);
        
        // build the new path
        strcpy(path, lpath);
        strcat(path, "/");
        strcat(path, file_name);

        stat_status = lstat(path, stat_info);
        if (stat_status) {
            fprintf(stderr,"cannot stat file %s", path);
            exit(1);
        }
    }

    strcpy(buf,path);

    // return to original directory
    chdir(cwd);

    free(path); free(lpath) ; free(file_name); free(stat_info); free(cwd);

    return strlen(buf);
}
