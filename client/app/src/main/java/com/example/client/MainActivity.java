package com.example.client;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.Socket;
import java.net.SocketException;
import java.util.Enumeration;

import android.support.v4.app.*;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.os.Bundle;
import android.os.Environment;

public class MainActivity extends AppCompatActivity {

    EditText editTextAddress;
    TextView IP_ADDRESS;
    String sss;
    Button buttonConnect;
    TextView textPort;
    ImageView iv;

    static final int SocketServerPORT = 8080;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        editTextAddress = (EditText) findViewById(R.id.address);
        IP_ADDRESS = (TextView) findViewById(R.id.ip_address);
        textPort = (TextView) findViewById(R.id.port);
        textPort.setText("port: " + SocketServerPORT);
        buttonConnect = (Button) findViewById(R.id.connect);


        IP_ADDRESS.setText(getIpAddress());



        buttonConnect.setOnClickListener(new OnClickListener(){

            @Override
            public void onClick(View v) {
                ClientRxThread clientRxThread =
                        new ClientRxThread(
                                editTextAddress.getText().toString(),
                                SocketServerPORT);

                clientRxThread.start();
            }});
    }

    //setting IP address:

    private String getIpAddress() {
        String ip = "";
        try {
            Enumeration<NetworkInterface> enumNetworkInterfaces = NetworkInterface
                    .getNetworkInterfaces();
            while (enumNetworkInterfaces.hasMoreElements()) {
                NetworkInterface networkInterface = enumNetworkInterfaces
                        .nextElement();
                Enumeration<InetAddress> enumInetAddress = networkInterface
                        .getInetAddresses();
                while (enumInetAddress.hasMoreElements()) {
                    InetAddress inetAddress = enumInetAddress.nextElement();

                    if (inetAddress.isSiteLocalAddress()) {
                        ip += "Local address is: "
                                + inetAddress.getHostAddress();
                    }

                }

            }

        } catch (SocketException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            ip += "Something Wrong! " + e.toString() + "\n";
        }

        return ip;
    }


    private class ClientRxThread extends Thread {
        String dstAddress;
        int dstPort;

        ClientRxThread(String address, int port) {
            dstAddress = address;
            dstPort = port;
        }

        @Override
        public void run() {

            Socket socket = null;

            try {
                socket = new Socket(dstAddress, dstPort);

                File file = new File(
                        Environment.getExternalStorageDirectory(),
                        "Received_frame.jpg");
                FileOutputStream fos = new FileOutputStream(file);
                DataInputStream dis = new DataInputStream(socket.getInputStream());
                int size=0;
                int bytes;
                try {
                    size=dis.readInt();
                    for (int b=0; b<= size;b++) {
                        bytes = dis.read();
                        fos.write(bytes);
                    }


                }  finally {
                    if(fos!=null){
                        fos.close();
                    }

                }
               


                socket.close();

                MainActivity.this.runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        ImageView myImage = (ImageView) findViewById(R.id.recive_image);
                        Bitmap myBitmap = BitmapFactory.decodeFile("/storage/emulated/0/Received_frame.jpg");
                        myImage.setImageBitmap(myBitmap);
                        Toast.makeText(MainActivity.this,
                                "Finished",
                                Toast.LENGTH_LONG).show();
                    }});

            } catch (IOException e) {

                e.printStackTrace();

                final String eMsg = "Something wrong: " + e.getMessage();
                MainActivity.this.runOnUiThread(new Runnable() {

                    @Override
                    public void run() {
                        Toast.makeText(MainActivity.this,
                                eMsg,
                                Toast.LENGTH_LONG).show();
                    }});

            } finally {
                if(socket != null){
                    try {
                        socket.close();
                    } catch (IOException e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                    }
                }

            }

        }

    }
}