package gup.test.zerocopy.demo;

import sun.misc.Unsafe;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Field;

/**
 * @author £ºgup
 * @date £º2021-03-13 14:04
 * @description£º${description}
 * @modified By£º
 * @version: $version$
 */
public class ClientTest {

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
            System.loadLibrary("DomainTest");
        }catch (UnsatisfiedLinkError e){
            System.err.println( "Cannot load hello library:\n " + e.toString() );
        }
    }

    public static void main(String[] args) throws Exception {
        DomainTest domainTest = new DomainTest();
        FileDescriptor fd = domainTest.getFileDescriptor();
        FileOutputStream outputStream = new FileOutputStream(fd);
//        String testString = "gup test for jni...";
//        System.out.println("");
//        try{
//            outputStream.write(testString.getBytes(),0,testString.getBytes().length);
//        }catch (IOException e){
//            e.printStackTrace();
//            System.err.println("Failed to write! " + e.getMessage());
//        }finally {
//            outputStream.close();
//        }
        long baseArrdess = -1;
        Thread.sleep(5000);
        try{
            baseArrdess = domainTest.mmap(fd,DomainTest.LENGTH);
            System.out.println("baseArrdess: " + baseArrdess);
            long value = unsafe.getLongVolatile(null,baseArrdess);
            System.out.println("client value: " + value);
        }finally {
            if (baseArrdess != -1){
                domainTest.unmap(baseArrdess,DomainTest.LENGTH);
            }
            outputStream.close();
        }
    }
}
