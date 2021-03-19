#include "jni.h"
#include "gup_test_zerocopy_demo_JniTest.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
JNIEXPORT jobject JNICALL Java_gup_test_zerocopy_demo_JniTest_getFileDescriptor
  (JNIEnv *env, jobject obj, jstring jpath, jint jlength){
      int fd = -1;
      int res;
      char buf[jlength];
      char target[PATH_MAX];
      const char *path = NULL;
      jclass fd_class;
      jfieldID fd_descriptor;
      jmethodID fd_constructor;
      jobject jret = NULL;
      path = (*env)->GetStringUTFChars(env, jpath, NULL);
      snprintf(target, PATH_MAX, "%s",path);

      fd = open(target, O_CREAT | O_EXCL | O_RDWR, 0700);
      memset(buf, 0, sizeof(buf));
      write(fd, buf, jlength);
      lseek(fd, 0, SEEK_SET);
      fd_class = (*env)->FindClass(env, "java/io/FileDescriptor");
      fd_class = (*env)->NewGlobalRef(env, fd_class);
      fd_descriptor = (*env)->GetFieldID(env, fd_class, "fd", "I");
      fd_constructor = (*env)->GetMethodID(env, fd_class, "<init>", "()V");

      jret = (*env)->NewObject(env, fd_class, fd_constructor);
      (*env)->SetIntField(env, jret, fd_descriptor, fd);
      return jret;
  }
