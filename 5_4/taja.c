#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#define PASSWORDSIZE 12
int main(void)
{
	int fd;
	int nread, cnt = 0, errcnt = 0;
	char ch, text[] = "The magic thing is that youcan change it.";
		struct termios init_attr, new_attr;
	fd = open(ttyname(fileno(stdin)), O_RDWR);
	tcgetattr(fd, &init_attr);
	new_attr = init_attr;
	new_attr.c_lflag &= ~ICANON;
	new_attr.c_lflag &= ~ECHO;
	new_attr.c_cc[VMIN] = 1;
	new_attr.c_cc[VTIME] = 0;
	time_t t1, t2;
	struct tm *tp1, *tp2;
	int ttemp1= 0, ttemp2=0, typeNum=0;

	time(&t1);
	tp1 = localtime(&t1);
	ttemp1 = tp1->tm_sec;

	if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
		fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
	}

	printf("다음 문장을 그대로 입력하세요.\n%s\n", text);
	while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
		if (ch == text[cnt++]){
			write(fd, &ch, 1);
			typeNum++;
		}else {
			write(fd, "*", 1);
			errcnt++;
		}
	}
	time(&t2);
	tp2 = localtime(&t2);
	ttemp2 = tp2->tm_sec;
	printf("타이핑 오류의 횟수는 %d\n", errcnt);
	int temp=ttemp2-ttemp1;
	printf("분당 타자수는 %lf 타\n", (double)typeNum/(double)temp);
	tcsetattr(fd, TCSANOW, &init_attr);
	close(fd);
}
