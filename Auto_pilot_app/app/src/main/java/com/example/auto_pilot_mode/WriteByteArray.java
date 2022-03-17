package com.example.auto_pilot_mode;


// Java Program to convert
// byte array to file


import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class WriteByteArray {

    public static void toFile (byte[] bytes, File destination){

        try {
            FileOutputStream fos=new FileOutputStream(destination);
            fos.write(bytes);
            fos.close();
        }
         catch (Exception e) {
            System.out.println("there is an error!");
        }


    }


}
