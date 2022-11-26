#include <sys/types.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define BUFSIZE 16
#define QNAME1 "/my_queue1"
#define QNAME2 "/my_queue2"
#define PRIORITY 1

void *sendth(void *args){
	char send_data[50];
	mqd_t qd;
	struct mq_attr q_attr;
	q_attr.mq_maxmsg = 10; 
	q_attr.mq_msgsize = BUFSIZE;

	if ((qd = mq_open(QNAME1, O_CREAT | O_RDWR, 0600, &q_attr)) == -1) {perror("mq_open failed");exit(1);}
	while(1){
		scanf("%s", send_data);
		if (mq_send(qd, send_data, strlen(send_data), PRIORITY) == -1) {perror("mq_send failed"); exit(1);}
		if (!strcmp(send_data,"exit")) {break;}
	}
	if (mq_close(qd) == -1) {perror("mq_close failed"); exit(1);}
}
void *receiveth(void *args){
	mqd_t qd; char recv_data[BUFSIZE];
	struct mq_attr q_attr, old_q_attr;
	int prio;
	char buf[BUFSIZE];
	q_attr.mq_maxmsg = 10;
	q_attr.mq_msgsize = BUFSIZE; 

	if ((qd = mq_open(QNAME2, O_RDWR | O_NONBLOCK,0600, NULL)) == -1) {perror("mq_open failed");exit(1);}
	q_attr.mq_flags = 0; 
	if (mq_setattr(qd, &q_attr, NULL)) {perror("mq_setattr failed");exit(1);}
	if (mq_getattr(qd, &old_q_attr)) {perror("mq_getattr failed");exit(1);}

	while (1) {
		if (mq_receive(qd, recv_data, BUFSIZE, &prio) == -1) { perror("mq_send failed"); exit(1); }
		printf("received from message queue : %s\n", recv_data); 
		if (recv_data == "exit") { break; }
	}

	if (mq_close(qd) == -1) {perror("mq_close failed");exit(1);}
	if (mq_unlink(QNAME2) == -1) {perror("mq_unlink failed");exit(1);}
}

int main()
{
	int status1=0, status2=0;
	pthread_t tid1, tid2;

	status1 = pthread_create (&tid1, NULL, sendth, NULL);
	if (status1 != 0) perror ("Create thread");

	status2 = pthread_create (&tid2, NULL, receiveth, NULL);
	if (status2 != 0) perror ("Create thread");

	pthread_exit (NULL);
}
