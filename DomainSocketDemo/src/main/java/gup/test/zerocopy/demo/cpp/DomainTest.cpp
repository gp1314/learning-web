#include "jni.h"
#include "gup_test_zerocopy_demo_DomainTest.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#define MAXLINE 80
char *client_path = "client.socket";
char *server_path = "server.socket";
char *socket_path = "server.socket";

JNIEXPORT void JNICALL Java_gup_test_zerocopy_demo_DomainTest_sendFileDescriptor
  (JNIEnv *env, jobject obj, jobject fdobject){
    struct  sockaddr_un cliun, serun;
    int len;
    int sockfd;
    int fd[1];
    struct msghdr msg;
    struct cmsghdr *cmsg = NULL;
    char buf[CMSG_SPACE(sizeof(fd))];
    struct iovec iov;
    int dummy;
    int size;

    jclass fd_class;
    jfieldID fd_descriptor;
    jmethodID fd_constructor;

    memset(&msg, 0, sizeof(struct msghdr));

    iov.iov_base = &dummy;
    iov.iov_len = 1;

    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = buf;
    msg.msg_controllen = sizeof(buf);

    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
        perror("client socket error");
        exit(1);
    }

    memset(&cliun, 0, sizeof(cliun));
    cliun.sun_family = AF_UNIX;
    strcpy(cliun.sun_path, client_path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(cliun.sun_path);
    unlink(cliun.sun_path);
    if (bind(sockfd, (struct sockaddr *)&cliun, len) < 0) {
        perror("bind error");
        exit(1);
    }

    memset(&serun, 0, sizeof(serun));
    serun.sun_family = AF_UNIX;
    strcpy(serun.sun_path, server_path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
    if (connect(sockfd, (struct sockaddr *)&serun, len) < 0){
        perror("connect error");
        exit(1);
    }

    fd_class = (*env)->FindClass(env, "java/io/FileDescriptor");
    fd_class = (*env)->NewGlobalRef(env, fd_class);
    fd_descriptor = (*env)->GetFieldID(env, fd_class, "fd", "I");
    fd_constructor = (*env)->GetMethodID(env, fd_class, "<init>", "()V");

    fd[0] = (*env)->GetIntField(env, fdobject, fd_descriptor);
    printf("send: %d \n", fd[0]);
    (*env)->DeleteLocalRef(env, fdobject);

    memcpy(CMSG_DATA(cmsg), fd, sizeof(fd));
    msg.msg_controllen = cmsg->cmsg_len;

    size = sendmsg(sockfd, &msg, 0);
    sleep(3);
    close(sockfd);
    return;
  }

/*
 * Class:     gup_test_zerocopy_demo_DomainTest
 * Method:    getFileDescriptor
 * Signature: ()Ljava/io/FileDescriptor;
 */
JNIEXPORT jobject JNICALL Java_gup_test_zerocopy_demo_DomainTest_getFileDescriptor
  (JNIEnv *env, jobject obj){
    struct sockaddr_un serun, cliun;
    socklen_t cliun_len;
    int listenfd, connfd, size;
    int i, n;
    int fd[1];

    jclass fd_class;
    jfieldID fd_descriptor;
    jmethodID fd_constructor;
    jobject jret = NULL;

    int dummy = 0;
    struct iovec iov;
    char buf[CMSG_SPACE(sizeof(fd))];
    struct msghdr msg;
    struct cmsghdr *cmsg;

    iov.iov_base = &dummy;
    iov.iov_len = 1;

    memset(&msg, 0, sizeof(msg));
    msg.msg_iov= &iov;
    msg.msg_iovlen= 1;
    msg.msg_control= buf;
    msg.msg_controllen = sizeof(buf);
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(fd));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;

    memcpy(CMSG_DATA(cmsg), fd, sizeof(fd));

    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      exit(1);
    }

    memset(&serun, 0, sizeof(serun));
    serun.sun_family = AF_UNIX;
    strcpy(serun.sun_path, socket_path);
    size = offsetof(struct sockaddr_un, sun_path) + strlen(serun.sun_path);
    unlink(socket_path);
    if (bind(listenfd, (struct sockaddr *)&serun, size) < 0) {
      perror("bind error");
      exit(1);
    }
    printf("UNIX domain socket bound\n");

    if (listen(listenfd, 20) < 0) {
      perror("listen error");
      exit(1);
    }
    printf("Accepting connections ...\n");

    cliun_len = sizeof(cliun);
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliun, &cliun_len)) < 0){
      perror("accept error");
    }
    printf("success to accept connections ...\n");

    n = recvmsg(connfd, &msg, 0);
    if (n < 0) {
      perror("read error");
    } else if(n == 0) {
      printf("EOF\n");
    }
    printf("success to read size: %d    ", n);

    memcpy(fd, CMSG_DATA(cmsg), sizeof(fd));
    printf("received: %d \n", fd[0]);

    close(connfd);
    close(listenfd);

    fd_class = (*env)->FindClass(env, "java/io/FileDescriptor");
    fd_class = (*env)->NewGlobalRef(env, fd_class);
    fd_descriptor = (*env)->GetFieldID(env, fd_class, "fd", "I");
    fd_constructor = (*env)->GetMethodID(env, fd_class, "<init>", "()V");

    jret = (*env)->NewObject(env, fd_class, fd_constructor);
    (*env)->SetIntField(env, jret, fd_descriptor, fd[0]);
    return jret;
  }


JNIEXPORT jlong JNICALL Java_gup_test_zerocopy_demo_DomainTest_mmap
  (JNIEnv *env, jobject obj, jobject fdobject, jlong length){
    int fd;
    jclass fd_class;
    jfieldID fd_descriptor;
    jmethodID fd_constructor;
    void *addr = 0;

    fd_class = (*env)->FindClass(env, "java/io/FileDescriptor");
    fd_class = (*env)->NewGlobalRef(env, fd_class);
    fd_descriptor = (*env)->GetFieldID(env, fd_class, "fd", "I");
    fd_constructor = (*env)->GetMethodID(env, fd_class, "<init>", "()V");

    fd = (*env)->GetIntField(env, fdobject, fd_descriptor);
    printf("mmap fd: %d\n",fd);
    addr = mmap(NULL, length, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        printf("Failed to mmap fd: %d \n", fd);
    }
    return (jlong)(intptr_t)addr;
  }


JNIEXPORT void JNICALL Java_gup_test_zerocopy_demo_DomainTest_unmap
  (JNIEnv *env, jobject obj, jlong jaddr, jlong length){
    void *addr;

    addr = (void*)(intptr_t)jaddr;
    if (munmap(addr, length) < 0) {
      printf("Failed to unmmap! \n");
    }
  }