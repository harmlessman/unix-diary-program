/*
   unix project
   diary program입니다.
   2021-12-19
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

// 프로그램을 실행하는 경로
char *path = NULL;
// 바꿀 비밀번호를 입력받는 변수
char *p = NULL;
// 선택할 dir이름을 입력받는 변수
char *dirname = NULL;
// 선택할 txt파일 이름을 입력받는 변수
char *txtname = NULL;
char str[100]; //명령을 받는 문자열
// 현재 날짜를 입력받는 문자열
char year[100];
char month[100];
char day[100];
// 일기 제목을 입력받는 변수
char *name = NULL;
// 비밀번호를 입력받는 문자열
char password[100]={0};

// 바꿀 비밀번호를 입력받는 문자열
char temp[100]={0};
// 바꿀 비밀번호를 입력받는 변수
char *c1 = NULL;
char *c2 = NULL;
// 바꿀 비밀번호를 입력받는 문자열
char c[100] = {0};

// 읽는 파일의 내용을 저장하는 문자열
char fulltxt[10000]={0};

// signal 중 sigint의 handler
void exi();
// password.txt가 있는지 알아보는 함수
int ex(const char *f);
// signal 중 sigalrm의 handler
void made();


int main()
{
	// 프로그램을 실행하는 경로를 저장
	char strbuf[300]={0};
	path = getcwd(strbuf, 300);
	// 현재 날짜를 받기 위해서
	time_t timer;
	struct tm* t;
	timer = time(NULL);
	t = localtime(&timer);
	
	int pid, status, cpid;
	
	// 프로그램 시작 시 실행되는 시작부분 (로딩창)
	int i;
	initscr();
   	clear();
	move(10, 21);
  	addstr("***Hello, Diary!***");
	move(12,27);
	refresh();
	addstr("[");
	move(12, 33);
	//refresh();
	addstr("]");
	for(i=28; i<34; i++ ){
		
		move(12, i);
		addstr("*");
		sleep(1);
		refresh();
   	}	
	endwin();
	
	
	
	
	
	
	// 현재 경로를 출력해줍니다.
	printf("현재 경로 : ");
	printf("%s\n", path);
	
	printf("Welcome to your diary!!\n");
	
	// 종료(4번)이 입력되기 전까지 계속 실행
	while(true)
	{
		// 기존 path로 지정
		chdir(path);
		// password.txt파일이 있는지 없는지 검사. 없다면 초기 비밀번호를 생성
		if (ex("password.txt")==0)//initial password = s1234
		{
			FILE *f = fopen("password.txt", "w");
			fprintf(f, "s1234");
			fclose(f);
		}
		// password.txt 파일이 있다면 이전패스워드 가지고옴
		FILE *f = fopen("password.txt", "r"); 
		fscanf(f, "%s", password);
		fclose(f);
		
		// 선택지를 출력 후 숫자를 입력받음
		char newpath[300]={0};
		printf("\n원하는 작업을 선택해주세요!\n");
		printf("1. 다이어리 작성\n");
		printf("2. 다이어리 관리\n");
		printf("3. 비밀번호 변경\n");
		printf("4. 종료\n");
		
		fgets(str, 100, stdin);
		
		
		// 1을 입력받았을 때,
		if (strcmp(str,"1\n")==0)
		{
			printf("지금 위치에 diray dir를 만드는 중입니다.....\n");
			// signal (sigalrm 사용)
			signal(SIGALRM, made);
			alarm(1);
			pause();
			
			// 현재 시간을 가져오고 year에 이어 붙입니다.
			// 그리고 새로운 path(newpath)문자를 만듭니다.
			sprintf(year, "%d", t->tm_year+1900);
			sprintf(month, "%d", t->tm_mon+1);
			sprintf(day, "%d", t->tm_mday);
			strcat(year, month);
			strcat(year, day);
			
			strcpy(newpath, path);
			strcat(newpath, "/");
			strcat(newpath, year);
				
			
			printf("current path : %s\n", newpath);
			// fork()를 이용하여 mkdir실행
			if (fork()==0)
			{
				execlp("mkdir", "mkdir", "-p", year, NULL);
				exit(0);
			}
			else
			{
				cpid = wait(&status);
				chdir(newpath);
			}
			// newpath로 이동합니다.
			
			
			
			printf("일기 제목을 입력해주세요.. : ");
			fgets(str, 100, stdin);
			printf("%s", str);
			// fgets를 사용하면 \n값도 저장되기 때문에 \n를
			// 때주는 작업입니다.
			name = strtok(str, "\n");
			strcat(name, ".txt");
			chdir(newpath);
			FILE *fp;
			fp = fopen(name, "w+");
			wait(NULL);
			if (fork()!=0)
			{
				cpid = wait(&status);
				printf("\n일기 작성 완료!\n");
			}
			else
			{
				// 일기 내용을 입력받습니다.
				// ctrl-d가 입력되면 종료됩니다. (signal 사용)
				signal(SIGINT, exi);
				printf("일기장 생성 완료!\n");
				printf("내용을 입력해주세요\n");
				printf("일기를 다 쓰셨으면 ctrl+D 키를 눌러주십시오\n");
				// 내용 입력받음
				while ( fgets(str, 100, stdin) )
				{
					fprintf(fp, "%s", str);
				}
				
				exit(0);
			}
			// 기존의 path로 되돌아갑니다.
			chdir(path);
			fclose(fp);
			
			
		}
		else if (strcmp(str,"2\n")==0)
		{
			// 2를 입력받았을 때
			// 우선 관리부분이기 때문에 비밀번호를 입력받습니다.
			printf("비밀번호를 입력해주세요\n");
			printf("현재 비밀번호 : ");
			fgets(temp, 100, stdin);
			p = strtok(temp, "\n");
			// 입력받은 비밀번호와 기존의 비밀번호가 같다면
			if (strcmp(password, p)==0)
			{
				printf("로그인 성공!\n");
				if(fork()!=0)
				{
					
					cpid = wait(&status);
					// 관리할 디렉토리를 입력받음
					printf("어떤 diary를 관리하시겠습니까?\n");
					fgets(str, 100, stdin);
					// 관리할 디렉토리로 가는 path를 생성 (newpath)
					dirname = strtok(str, "\n");
					strcpy(newpath, path);
					strcat(newpath, "/");
					strcat(newpath, dirname);
					
					// 관리할 디렉토리를 입력받은 후 0 또는 1을
					// 입력받습니다. 0은 파일삭제, 1은 파일읽기
					printf("0 : 삭제\n1 : 열기\n");
					fgets(str, 100, stdin);
					// 만약 0을 입력받고, newpath로 이동이
					// 되었다면
					if(chdir(newpath)==0 && strcmp(str, "0\n")==0)
					{
						// 부모process
						if(fork()!=0)
						{
							cpid = wait(&status);
							// 삭제하고 싶은 일기 입력
							printf("삭제하고 싶은 일기를 선택하세요.\n");
							fgets(str, 100, stdin);
							txtname = strtok(str, "\n");
							if (remove(txtname)==0)
								printf("삭제완료!\n");
							else
								printf("삭제실패!");
						
						
						}
						else // 자식process
						{
							// 파일 리스트 출력
							execlp("ls", "ls", NULL);
							exit(0);
						}
					}
					else if(chdir(newpath)==0 && strcmp(str, "1\n")==0)
					{
						// 만약 1을 입력받고, newpath로 이동이 되었다면
						// 부모 process
						if(fork()!=0)
						{
							cpid = wait(&status);
							// 파일내용 입력받는 문자열 초기화
							for (int k=0; k<10000; k++)
								fulltxt[k] = 0;
							// 읽을 일기파일 입력
							printf("읽고 싶은 일기를 선택하세요.\n");
							fgets(str, 100, stdin);
							txtname = strtok(str, "\n");
							// 파일 내용을 fulltxt에 저장한 후 출력
							FILE *ff = fopen(txtname, "r");
							fread(fulltxt, 1, 10000, ff);
							printf("%s", fulltxt);
						
						}
						else // 자식 process
						{
							execlp("ls", "ls", NULL);
							exit(0);
						}
					}
					else
					{
						//만약 입력한 디렉토리가 없다면
						printf("존재하는 디렉토리를 입력해주세요.\n");
					}
					
				}
				else
				{
					// 디렉토리 리스트 출력
					execlp("ls", "ls", NULL);
					exit(0);
				}
			}
			else
			{
				// 만약 비밀번호를 틀렸다면
				printf("비밀번호를 다시 확인해주세요\n");
			}
		}
		else if (strcmp(str,"3\n")==0) //3번을 골랐을 경우
		{
			printf("비밀번호를 변경합니다.\n");
			printf("현재 비밀번호: ");
			// 비밀번호를 입력합니다.
			fgets(str, 100, stdin);
			p = strtok(str, "\n");
			
			// 입력한 값과 비밀번호가 일치한다면 실행
			if (strcmp(password, p)==0)
			{
				printf("새 비밀번호 : ");
				fgets(str, 100, stdin);
				c1 = strtok(str, "\n");
				printf("새 비밀번호 재확인: ");
				fgets(c, 100, stdin);
				c2 = strtok(c, "\n");
				if (strcmp(c1,c2)!=0)
					printf("새 비밀번호를 정확히 입력해주세요\n");
				else if (strcmp(password, c1)==0)
					printf("처음 비밀번호와 동일한 비밀번호입니다.\n");
				else
				{
					printf("비밀번호 변경이 완료되었습니다.\n");
					// password.txt에 변경된 비밀번호를 작성합니다.
					strcpy(password, c1);
					FILE *fp = fopen("password.txt", "w");
					fprintf(fp, "%s", password); //성공적으로 패스워드 변경했을 시, 패스워드 저장
					fclose(fp);
				}
				
			}
			else
			{
				printf("비밀번호를 다시 확인해주세요.\n");
			}
			
			
		}
		else if (strcmp(str,"4\n")==0) //4번을 입력받았다면
		{
			// 종료
			printf("안녕히가세요!!\n");
			exit(0);
		}
		else
		{
			// 1 2 3 4 을 입력받은게 아니라면
			printf("1 2 3 4 중 선택해서 입력해주세요\n");
		}
	}
	
	
	
}

void exi()
{
	exit(0);
}


int ex(const char *f) //파일 유무 판단
{
    FILE *file;
    if ((file = fopen(f, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void made()
{
	printf(" ");
}





