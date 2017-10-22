#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <dirent.h>

#define LEN 4096

char* get_filetype(const char* file);
void send_error(int status,char* title);
void send_header(int status,char* title, char* filetype);
void decode(char* to, char* from);
void encode(char* to, char* from);

int main(int argc,const char* argv[])
{
    if(argc < 2)
        send_error(500,"server error : argc < 2");

    //if(access(argv[1],R_OK | F_OK) != 0)
    if(chdir(argv[1]) < 0)
        send_error(500,"server error : chdir error");

    char line[LEN],type[LEN],path[LEN],protocol[LEN];
    if(fgets(line,sizeof(line),stdin) == NULL)
        send_error(500,"server error : type path protocol");
    if(sscanf(line,"%[^ ] %[^ ] %[^ ]",type,path,protocol) != 3)
        send_error(400,"bad request");
    if(strcasecmp(type,"GET") != 0)
        send_error(400,"method not allow");
    if(path[0] != '/')
        send_error(404,"file not found");

    while(fgets(line,sizeof(line),stdin) != NULL)
    {
        if(strcmp(line,"\n") == 0 || strcmp(line,"\r\n") == 0)
            break;
    }

    char file[LEN];
    struct stat st;
    file[0] = '.';
    decode(file+1,path);
    //printf("%s\n",&path[1]);
    //printf("%s\n",file);
    if(stat(file,&st) < 0)
    {
        printf("file : %s\r\n",file);
        send_error(500,"server error : stat");
    }
    if(S_ISDIR(st.st_mode))
    {
        send_header(200,"OK","text/html;charset=utf-8");
        printf("<html>"
                    "<head><title>Index of %s</title></head>"
                    "<body bgcolor=\"#cc99cc\">"
                        "<h4>Index of %s</h4>"
                        "<pre>"
                    ,file,file);
        struct dirent** dl;
        int nf = scandir(file,&dl,NULL,alphasort);
        if(nf < 0)
            perror("scandir");
        else
        {
            struct stat fst;
            char stfile[LEN];
            for(int i=0;i<nf; ++i)
            {
                strcpy(stfile,file);
                strcat(stfile,"/");
                strcat(stfile,dl[i]->d_name);
                if(lstat(stfile,&fst) < 0)
                    printf("<a href=\"%s%s/\">%-32.32s/</a>",file+1,dl[i]->d_name,dl[i]->d_name);
                else if(S_ISDIR(fst.st_mode))
                    printf("<a href=\"%s%s/\">%-32.32s/</a> \t\t%14lld",file+1,dl[i]->d_name,dl[i]->d_name,(long long)fst.st_size);
                else
                    printf("<a href=\"%s%s\">%-32.32s</a> \t\t%14lld",file+1,dl[i]->d_name,dl[i]->d_name,(long long)fst.st_size);
                printf("<br/>");
            }
        }
        printf("</pre>"
               "</body>"
               "</html>");
    }
    else
    {
    //普通文件
    FILE* fp = fopen(file,"r");
    if(fp == NULL)
        send_error(500,"server error : open file");
    send_header(200,"send header",get_filetype(file));
    int ch;//这里必须用int判断EOF，我真是菜鸡。
    while((ch = getc(fp)) != EOF)
    {
        putchar(ch);
    }
    fflush(stdout);
    fclose(fp);
    fp = NULL;
    }
//    printf("test success !\n");

    return 0;
}

int hex2d(char hex)
{
    if(hex >= '0' && hex <= '9')
        return hex-'0';
    else if(hex >= 'a' && hex <= 'f')
        return hex-'a'+10;
    else if(hex >= 'A' && hex <= 'F')
        return hex-'A'+10;
    else
        return hex;
}

void decode(char* to, char* from)
{
    if(to == NULL || from == NULL)
        return;

    while(*from != '\0')
    {
        if(from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            *to = hex2d(from[1])*16 + hex2d(from[2]);
            from += 3;
        }
        else
        {
            *to = *from;
            ++from;
        }
        ++to;
    }
    *to = '\0';
}

void encode(char* to, char* from)
{
    if(to == NULL && from == NULL)
        return;

    while(*from != '\0')
    {
        if(isalnum(*from) || strchr("/._-~",*from) != NULL)
        {
            *to = *from;
            ++to;
            ++from;
        }
        else
        {
            sprintf(to,"%%%02x",*from);
            to += 3;
            from += 3;
        }
    }
    *to = '\0';
}

char* get_filetype(const char* file)
{
    if(file == NULL)
        return NULL;
    char* dot = strrchr(file,'.');
    if(*dot == '\0')
        return "text/plain; charset=utf-8";
    else if(strcmp(dot,".html") == 0)
        return "text/html; charset=utf-8";
    else if(strcmp(dot, ".jpg") == 0)
        return "image/jpeg";
    else if(strcmp(dot, ".gif") == 0)
        return "image/gif";
    else if(strcmp(dot, ".png") == 0)
        return "image/png";
    else if(strcmp(dot, ".wav") == 0)
        return "audio/wav";
    else if(strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    else if(strcmp(dot, ".mov") == 0)
        return "video/quicktime";
    else if(strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    else
        return "text/plain; charset=utf-8";
}

void send_header(int status, char* title, char* filetype)
{
    if(title == NULL || filetype == NULL)
    {
        title = "ERROR";
        filetype = "text/plain; charset=utf-8";
    }
    printf("HTTP/1.1 %d %s\r\n",status,title);
    printf("Content-Type:%s\r\n",filetype);
    printf("\r\n");
}

void send_error(int status,char* title)
{
    if(title == NULL)
        title = "ERROR";
    send_header(status,title,"text/html; charset=utf-8");
    printf("<html>\n"
                "<head><title>%d %s</title></head>\n"
                "<body bgcolor=\"#cc99cc\">\n"
                    "<h4>error!</h4>\n"
                    "<hr>\n"
                    "<address>\n"
                        "<a href=\"http://blog.csdn.net/gongluck93/\">gongluck</a>\n"
                    "</address>\n"
                "</body>\n"
            "</html>",
           status,title);
    fflush(stdout);
    exit(1);
}
