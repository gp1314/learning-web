package gup.test.zerocopy.demo;

import java.io.FileDescriptor;


public class DomainTest {

    public static final long LENGTH = 16;
    public native void sendFileDescriptor(FileDescriptor fd);

    public native FileDescriptor getFileDescriptor();

    public native long mmap(FileDescriptor fd,long length);

    public native void unmap(long addr, long length);

}
