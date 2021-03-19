package gup.test.zerocopy.demo;

import sun.misc.Unsafe;

import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.lang.reflect.Field;


public class JniTest {

    public native FileDescriptor getFileDescriptor(String path, int length);

    private static final Unsafe unsafe = safetyDance();

    private static Unsafe safetyDance() {
        try {
            Field f = Unsafe.class.getDeclaredField("theUnsafe");
            f.setAccessible(true);
            return (Unsafe)f.get(null);
        } catch (Throwable e) {
            System.err.println("failed to load misc.Unsafe" +  e.getMessage());
        }
        return null;
    }

    static {
        try
        {
            System.loadLibrary("JniTest");
            System.loadLibrary("DomainTest");
        }catch (UnsatisfiedLinkError e){
            System.err.println( "Cannot load hello library:\n " + e.toString() );
        }
    }

    public static void main(String[] args) throws Exception{
        JniTest jniTest = new JniTest();
        FileDescriptor fd = jniTest.getFileDescriptor("/home/hadoop/gup_jni_test",8192);
//        String testString = "gup test for jni...";
//        System.out.println("");
//        try{
//            outputStream.write(testString.getBytes(),0,testString.getBytes().length);
//        }catch (IOException e){
//            System.err.println("Failed to write! " + e.getMessage());
//        }finally {
//            outputStream.close();
//        }
        DomainTest domainTest = new DomainTest();
        domainTest.sendFileDescriptor(fd);
        long baseArrdess = -1;
        try{
            baseArrdess = domainTest.mmap(fd,DomainTest.LENGTH);
            System.out.println("baseArrdess: " + baseArrdess);
            unsafe.putLongVolatile(null,baseArrdess,99999999L);
        }finally {
            Thread.sleep(15000);
            if (baseArrdess != -1) {
                domainTest.unmap(baseArrdess,DomainTest.LENGTH);
            }
        }
//        outputStream.close();
    }
}
