////////////////////////////////////////////////
//         Operating Systems Project          //
// Muhammad Umair Khalid.......21I-0455 B-CS  //
// Ali Umer....................21I-0380 B-CS  //
// Tayyab Sohail...............21I-2478 B-CS  //
////////////////////////////////////////////////

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<pthread.h>
#include<iostream>
#include<semaphore.h>
#include<unistd.h>
#include<sys/wait.h>            //Header Files
#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<fstream>

using namespace std;

float* o_nodes;                 //Global Variables for Nodes Array and Neuron
int neuron=0;

int main(int argc, char* argv[]){

char argu_pip[10]={'a','r','g','u','m','e','n','t','s','\0'};           //To Pass Arguments between Layers using Pipes
char argu_buff[10]{'\0'};

int argu_open = open(argu_pip,O_RDONLY);
read(argu_open,argu_buff,10);                   //Reading Arguments from Previous Layer using Pipes
close(argu_open);

int current_layer=int(argu_buff[0]) - 48;
int neu=int(argu_buff[2]) - 48;                     //Initializing Variables
neuron=neu;


char o_pip[6]={'o','p','i','p','e','\0'};
char buffer[1024]{'\0'};                             //To read Product of Output weights and Neurons from Previous Layer

int r_open = open(o_pip,O_RDONLY);
read(r_open,buffer,1024);                    //Reading I1w1, I2w2 .... From Previous Layer
close(r_open);


o_nodes=new float[neuron];
for(int i=0;i<neuron;i++){              //Initializing Neuron Array
    o_nodes[i]=0;
}


{                                               //This scope assigns values to neuron array
    string b;
    b.clear();
    int idx1=0;
    for(int i=0; buffer[i]!='\0';i++){
        if(buffer[i]!=',') {
                b=b+buffer[i];
            }
        else{
            o_nodes[idx1]=o_nodes[idx1]+stof(b);                //Assigning values from buffer to nuron array      
            idx1++;
            b.clear();
        }
    }
}                                                   //End of Scope


float x= 0;
for(int i=0;i<neuron;i++){          //Adding all the values of Final Layer Neurons
    x=x+o_nodes[i];
}

float x1=0;
float x2=0;             //Declaring Variables for Final Answers
float x_sqr=0;

x_sqr=x*x;
x1=(x_sqr+x+1)/2;                   //Calculating Final Answers
x2=(x_sqr-x)/2;

cout<<endl<<endl;
cout<<"Fx1 = "<<x1<<endl;                   //Displaying values of F(X1) and F(X2)
cout<<"Fx2 = "<<x2<<endl;
cout<<endl;

    char w_pip[3]={'1','p','\0'};
    char p =(char)current_layer+48;
    w_pip[0]=p;
    char wait_buffer[100]{'\0'};

    string w,wr;
    w.clear();
    wr.clear();
    w=to_string(x1);                    //Converting from float to string to write in buffer
    wr=wr+w+',';
    w=to_string(x2);
    wr=wr+w;

    for(int i=0;wr[i]!='\0';i++){                   //Writting in buffer to send back values previous Layer Using Pipes
        wait_buffer[i]=wr[i];
    }

    int wr_open = open(w_pip,O_WRONLY);
    write(wr_open,wait_buffer,100);                 //Writing in Pipe to resume previous Process
    close(wr_open);

    return 0;
}
