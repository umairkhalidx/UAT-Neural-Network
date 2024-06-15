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
#include<unistd.h>                      //Header Files
#include<sys/wait.h>
#include<sys/types.h>
#include <fcntl.h>
#include<sys/stat.h>
#include<fstream>
#include<limits.h>
#include<string>

using namespace std;


string getExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string::size_type pos = std::string(result).find_last_of("/");
    return std::string(result).substr(0, pos);
}


pthread_mutex_t mutex1;
sem_t sem1;
int thread_counter = 0;
float** weight;
float* o_weights;
float* nodes;                       //Global Variables for
int neuron=0;
char pipe_buff[1024]{'\0'};
int f_idx=0;

string exePath = getExecutablePath();
string configPath = exePath + "/config.txt";
ifstream config_file(configPath);

void *o_thread_fun(void *arg) {                 //Output Layer Thread Function
    
    int* l= (int *) arg;
    int temp=(*l)*(neuron);                 //Reading Thread Number From File

    for(int j=0;j<1;j++){
        o_weights[*l]=nodes[*l]*o_weights[*l];          //Calculating Product of Neuron and Weights
    }

    sem_wait(&sem1);                //Locking Semaphore to limit one thread in CS

    thread_counter++;
    if(thread_counter==neuron){
    pthread_mutex_unlock (&mutex1);                 //Unlocking Main Process
    }
    sem_post(&sem1);                    //Unlocking Semaphore

    pthread_exit(NULL);

}



void *thread_fun(void *arg) {               //Thread Function to Calculate Product of Neuron and Weights

    int* l= (int *) arg;
    int temp=(*l)*(neuron);                 //Reading Thread Number From File

    for(int i=0;i<1;i++){

        for(int j=0;j<neuron;j++){
            weight[*l][j]=nodes[*l]*weight[*l][j];          //Calculating Product of Neuron and Weights
        }
    }

    sem_wait(&sem1);                //Locking Semaphore to limit one thread in CS

    thread_counter++;
    if(thread_counter==neuron){
    pthread_mutex_unlock (&mutex1);                 //Unlocking Main Process
    }
    sem_post(&sem1);                    //Unlocking Semaphore

    pthread_exit(NULL);

}

int main(int argc, char* argv[])                   //Start of Main
{

    char argu_pip[10]={'a','r','g','u','m','e','n','t','s','\0'};       //Pipe to read Arguments Between Layers
    char argu_buff[10]{'\0'};

    int argu_open = open(argu_pip,O_RDONLY);
    read(argu_open,argu_buff,10);                       //Reading Layer Arguments from File
    close(argu_open);

    int current_layer=int(argu_buff[0]) - 48;
    int max_layers=int(argu_buff[2]) - 48;              //Stroing Values from Argument Buffer in Variables
    int neu=int(argu_buff[4]) - 48;
    neuron=neu;

    string a_str;
    a_str.clear();
    for(int i=6;argu_buff[i]!='\0';i++){
        a_str=a_str+argu_buff[i];
    }
    int fid=stoi(a_str);
    f_idx=fid;


    {
        string tp;
        tp.clear();
        while(fid!=0){                      //Moving Line pointer in File Accordingly

            getline(config_file,tp);
            fid--;

        }
    }

    current_layer=current_layer-1;
    char pip[6]={'p','i','p','e','1','\0'};                 //Remaking Pipe name for communication Between Current
    char p =(char)current_layer+48;
    pip[4]=p;
    current_layer=current_layer+1;

    char buffer[1024]{'\0'};
    int r_open = open(pip,O_RDONLY);
    read(r_open,buffer,1024);              //Reding Product of Neurons and Weights from Pervious Layer using Pipe Buffer
    close(r_open);


    weight= new float*[neuron];
    for(int i=0;i<neuron;i++){
        weight[i]=new float[neuron];                    //Initializing Weights Array
    }

    nodes=new float[neuron];
    for(int i=0;i<neuron;i++){                          //Initializing Neuron Array
        nodes[i]=0;
    }

    {                       //Scope to Calculating New Neuron Values

        string b;
        b.clear();
        int idx1=0;

        for(int i=0; buffer[i]!='\0';i++){                  //Run Till BUffer Not Empty
        
            if(buffer[i]!=',') {
                    b=b+buffer[i];
            }
            else{
                    nodes[idx1]=nodes[idx1]+stof(b);            //Calculating New Neuron Values
                    idx1++;
                    if(idx1==neuron){
                        idx1=0;
                    }
                    b.clear();
            }
        }
    }                   //End of Scope


if(current_layer<max_layers){                   //Only run if Nor Last Layer Else exec to Output Layer

    cout<<"Entering Layer "<<current_layer<<endl;

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Neurons : "<<endl;
    for(int j=0; j<neuron;j++){                 //Displaying Neuron Values
            cout<<nodes[j]<<" ";
    }cout<<endl;
    ////////////////////////////////////////////////////////////////////////////////////////////////////


    {                                   //Scope to Read Weights

        if (config_file.is_open()) 
        {

            string temp;
            string ch;
            int t=0;

            for(int i=0;i<neuron;i++){

                temp.clear();
                ch.clear();
                t=0;
                getline(config_file,temp);
                for(int j=0;temp[j]!='\0';j++){

                    if(temp[j]!=',') {
                        ch=ch+temp[j];
                    }
                    else{
                        weight[i][t]=stof(ch);
                        t++;
                         if(t==neuron){
                            break;
                        }
                        ch.clear();
                    }
                }

                if(t!=neuron){
                    weight[i][t]=stof(ch);
                }
        }

        }else{
            cout<<"Unable to Open Weight File :(";
            return 0;
        }
    }
                                //Start of Multithreading

    pthread_t neu_thread[neuron];
    pthread_mutex_init (&mutex1 ,NULL) ;           //Initializing Mutexe
    sem_init (&sem1,0,1);                          //Initializing Semaphore
    int t[8]={0,1,2,3,4,5,6,7};

    pthread_mutex_lock (&mutex1);          //Starting Somewhats of a Critical Sections

                for(int i=0;i<neuron;i++){
                pthread_create(&neu_thread[i], NULL, thread_fun, (void*)&t[i]);     //Creating Threads

                }
    pthread_mutex_lock (&mutex1);//Locks the main untill are threads have been executed; unlocked by theard functions

////////////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Weights : "<<endl;
    for(int i=0;i<neuron;i++){

        for(int j=0;j<neuron;j++){                      //Displaying Neurons
            cout<<weight[i][j]<<" ";
        }
        cout<<endl;
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////

     {
                float w=0;

                string wr;
                string wrc;
                wr.clear();             //String variables to write in buffer
                wrc.clear();
                for(int i=0;i<neuron;i++){

                for(int j=0;j<neuron;j++){
                    w=weight[i][j];
                    wrc=to_string(w);           //Converting Neuron and Weights product into a string to write in buffer
                    wr=wr+wrc+',';
                }
                }
                for(int i=0;wr[i]!='\0';i++){
                    pipe_buff[i]=wr[i];                //Writting in Buffer
                }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    cout<<"Buffer : "<<endl;
    for(int i=0;pipe_buff[i]!='\0';i++){               //Displaying Buffer to be sent in Pipe
        cout<<pipe_buff[i];
    }
    cout<<endl<<endl;
   /////////////////////////////////////////////////////////////////////////////////////

    config_file.close();                //Closing File stream


    char pip1[6]={'p','i','p','e','1','\0'};                 //Pipe for Communication between Layers
    pip1[4]=pip[4];
    pip1[4]++;
    f_idx=f_idx+8;
    mkfifo(pip1,0666);


    char w_pip[3]={'1','p','\0'};
    char pi =(char)current_layer+48;            //Pipe to Parrent wait for Child
    w_pip[0]=pi;
    char wait_buffer[100]{'\0'};
    mkfifo(w_pip,0666);


    current_layer=current_layer+1;          //Adding variable to move in Next Layer

    string arg_str;
    arg_str.clear();

    arg_str=arg_str+to_string(current_layer)+','+to_string(max_layers)+','+to_string(neuron)+',';
    string ft;                        //Storing Arrguments in Pipe
    ft.clear();

    ft=ft+to_string(f_idx);
    for(int i=0;ft[i]!='\0';i++){                      //Storing Arrguments in Pipe
        arg_str=arg_str+ft[i];

    }

    char argu_buff[10]{'\0'};
    for(int i=0;arg_str[i]!='\0';i++){           //Converting Arguments in String
        argu_buff[i]=arg_str[i];
    }


    int pid=fork();             //Forking

        if(pid>=0){                 //If Fork Successfull

             if(pid==0){                                    //If Child

                execlp("./Layer","Layer",NULL);         //Exec To New Layer          
                exit(0);

            }
            else{
                //Parent Process
                 int argu_open = open(argu_pip,O_WRONLY);
                write(argu_open,argu_buff,10);                      //Writting in Pipe to Pass Arguments
                close(argu_open);


                int wr_open = open(pip1,O_WRONLY);
                write(wr_open,pipe_buff,1024);                      //Writting in Pipe to Pass Product of Neurons and Weights
                close(wr_open);

                int r_open = open(w_pip,O_RDONLY);
                read(r_open,wait_buffer,100);               //Pipe to make Parrent Wait for Child
                close(r_open);

                cout<<"Layer = "<<current_layer-1<<endl;
                cout<<"Fx1 =";
                for(int i=0;wait_buffer[i]!='\0';i++){
                    if(wait_buffer[i]!=','){                       //Displaying Answers recieved from Child
                        cout<<wait_buffer[i];
                    }
                    else{
                        cout<<endl;
                        cout<<"Fx2 = ";
                    }

                }cout<<endl<<endl;

                w_pip[0]--;
                int w_open = open(w_pip,O_WRONLY);
                write(w_open,wait_buffer,100);//Writing in Pipe to return values and Continue previous layer's Parent
                close(w_open);

            }
        }
        else
        {
            cout<<"Fork Failed"<<endl;
        }
}else{                                  //If In Last Layer

    cout<<"Entering Ouptput Layer "<<endl;

////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Neurons : "<<endl;
    for(int j=0; j<neuron;j++){
            cout<<nodes[j]<<" ";                //Displaying Neuron Values
    }cout<<endl;
////////////////////////////////////////////////////////////////////////////////////////////////
    o_weights=new float[neuron]{0};                 //Initializing Weights Array
    config_file.close();

    config_file.open(configPath);             //Opening File Stream
    
    f_idx=51;
    {
        string tp;
        tp.clear();
        while(f_idx!=0){

            getline(config_file,tp);                //Moving File Pointer to Desired Line
            f_idx--;

        }
    }

    {                               //Scope to Read Layer Weights From File

    if (config_file.is_open()) {

        string temp;
        string ch;                  //Variables for File HAndling
        int t=0;

        for(int i=0;i<1;i++){
            temp.clear();
            ch.clear();
            t=0;
            getline(config_file,temp);                  //Reading File Line by Line
            
            for(int j=0;temp[j]!='\0';j++){

                if(temp[j]!=',') {
                    ch=ch+temp[j];
                }
                else{
                    o_weights[t]=stof(ch);              //Storing Layer Weights in Array
                    t++;
                     if(t==neuron){
                        break;
                    }
                    ch.clear();
                }

            }
            if(t!=neuron){
                o_weights[t]=stof(ch);
            }
        }
        config_file.close();                    //CLosing File Pointer


        }else{
            cout<<"Unable to Open Weight File :(";
            return 0;
        }
    }
////////////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Weights : "<<endl;
    for(int i=0;i<neuron;i++){
        cout<<o_weights[i]<<" ";                //Displaying Weights
    }
    cout<<endl;
///////////////////////////////////////////////////////////////////////////////////////////////////////

                            //Start of Multithreading

    pthread_t o_thread[neuron];
    pthread_mutex_init (&mutex1 ,NULL);           //Initializing Mutex
    sem_init (&sem1,0,1);                          //Initializing Semaphore
    int t[8]={0,1,2,3,4,5,6,7};

    pthread_mutex_lock (&mutex1);          //Starting Somewhats of a Critical Sections

        for(int i=0;i<neuron;i++){

            pthread_create(&o_thread[i], NULL, o_thread_fun, (void*)&t[i]);     //Creating Threads
        }

    pthread_mutex_lock (&mutex1);//Locks the main untill are threads have been executed; unlocked by theard functions

    {
                float w=0;
                string wr;
                string wrc;             //String variables to write in buffer
                wr.clear();
                wrc.clear();

                for(int j=0;j<neuron;j++){
                    w=o_weights[j];
                    wrc=to_string(w);           //Converting Neuron and Weights product into a string to write in buffer
                    wr=wr+wrc+',';
                }

                for(int i=0;wr[i]!='\0';i++){
                    pipe_buff[i]=wr[i];               //Writting in Buffer
                }
    }
///////////////////////////////////////////////////////////////////////////////////////////////////////////
    cout<<"Buffer : "<<endl;
    for(int i=0;pipe_buff[i]!='\0';i++){               //Displaying Buffer to be sent in Pipe
       cout<<pipe_buff[i];
    }
    cout<<endl;
////////////////////////////////////////////////////////////////////////////////////////////////////////

    char o_pip[6]={'o','p','i','p','e','\0'};           //Pipe For Communication Between This Layer and Output Layer
    mkfifo(o_pip,0666);

    char w_pip[3]={'1','p','\0'};
    char p =(char)current_layer+48;            //Pipe to Parrent wait for Child
    w_pip[0]=p;
    char wait_buffer[100]{'\0'};
    mkfifo(w_pip,0666);

    string arg_str;
    arg_str.clear();
    arg_str=arg_str+to_string(current_layer)+','+to_string(neuron)+',';
    char argu_buff[10]{'\0'};                        //Storing Arrguments in Pipe


    for(int i=0;arg_str[i]!='\0';i++){
        argu_buff[i]=arg_str[i];            //Converting Arguments in String 
    }

    int pid=fork();             //Forking

        if(pid>=0){                 //If Fork Successfull

             if(pid==0){                                    //If Child

                execlp("./Output","Output",NULL);         //Exec To Output Layer     
                exit(0);
            }
            else{
                //Parent Process
                int argu_open = open(argu_pip,O_WRONLY);
                write(argu_open,argu_buff,10);                      //Writting in Pipe to Pass Arguments
                close(argu_open);

                int wr_open = open(o_pip,O_WRONLY);
                write(wr_open,pipe_buff,1024);                     //Writting in Pipe to Pass Product of Neurons and Weights
                close(wr_open);

                int r_open = open(w_pip,O_RDONLY);
                read(r_open,wait_buffer,100);               //Pipe to make Parrent Wait for Child
                close(r_open);

                cout<<"Output Layer "<<endl;
                cout<<"Fx1 =";
                for(int i=0;wait_buffer[i]!='\0';i++){             //Displaying Answers recieved from Output Layer
                if(wait_buffer[i]!=','){
                    cout<<wait_buffer[i];                  //Displaying Answers
                }
                else{
                    cout<<endl;
                    cout<<"Fx2 = ";
                }
                
            }cout<<endl<<endl;

                w_pip[0]--;
                int w_open = open(w_pip,O_WRONLY);
                write(w_open,wait_buffer,100);//Writing in Pipe to return values and Continue previous layer's Parent
                close(w_open);
            }
        }
        else
        {
            cout<<"Fork Failed"<<endl;
        }
}
    return 0;
}
