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
#include<fcntl.h>
#include<sys/stat.h>
#include<fstream>
#include<limits.h>
#include<string>

using namespace std;

pthread_mutex_t input_mutex;
sem_t input_sem;
int input_counter = 0;
char input_buff[1024]{'\0'};
int neuron=0;                           //Global Variables for Neurons, Waits, Buffers etc
float** L1_weight;
float input_node[2];
int f_idx=0;
ifstream config_file;

void *input_t_fun(void *arg)        //Thread Function to calculate product of neurons and weights using multithreading
{
    int* l= (int *) arg;                //Reading From Arguments

    int temp=(*l)*(neuron);
    for(int i=0;i<1;i++){

        for(int j=0;j<neuron;j++){
            L1_weight[*l][j]=input_node[*l]*L1_weight[*l][j];           //Calculating Product of Neurons and Waits
        }
    }

    sem_wait(&input_sem);                   //Semaphore Lock to Limit one Thread

    input_counter++;
    if(input_counter==2){
    pthread_mutex_unlock (&input_mutex);            //Unlock Main if all threads have been processed
    }

    sem_post(&input_sem);                   //Semaphore Unlock


    pthread_exit(NULL);
}

string getExecutablePath() {
    char result[PATH_MAX];
    ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
    std::string::size_type pos = std::string(result).find_last_of("/");
    return std::string(result).substr(0, pos);
}


int main()                     //Start of Main
{

    int max_layers=0;
    int current_layer=1;                //Variables for Layers and Maximum Layers

    cout<<"\t\t\tWelcome to UAT Neural-Network :"<<endl<<endl;

    while(1){

        cout<<"Enter Number of Layers = ";
        cin>>max_layers;
        cout<<"Enter Number of Neuron = ";              //Taking Input From User
        cin>>neuron;

        if(max_layers>=2 && max_layers<=8 && neuron>=2 && neuron<=8 ){              //Validating Input
            break;
        }else{
            cout<<endl;
            cout<<"Invalid Input "<<endl;
            cout<<endl;
        }

    }

    L1_weight= new float*[2];
    for(int i=0;i<2;i++){
        L1_weight[i]=new float[neuron];                 //Initializing Weights array
    }

    char argu_pip[10]={'a','r','g','u','m','e','n','t','s','\0'};       //Pipe to pass Arguments Between Layers
    mkfifo(argu_pip,0666);
    string exePath = getExecutablePath();
    string configPath = exePath + "/config.txt";
    for(int turn=0;turn<2;turn++){                          //To Run Neural Network Multiple Times

    config_file.open(configPath);     //Opening Configuration File
    cout<<"Entering Input Layer"<<endl;

    {                                           //Scope to Read from File
        if (config_file.is_open()) {

        string temp;
        temp.clear();
        getline(config_file,temp);              //Variables for storing weights and Line Indexes
        f_idx++;
        string ch;
        ch.clear();
        int t=0;

        for(int i=0;temp[i]!='\0';i++){

            if(temp[i]!=',') {
                ch=ch+temp[i];
            }
            else{
                input_node[t]=stof(ch);             //Reading From Files and Storing in Neuron Array
                t++;
                ch.clear();
            }
        }
        input_node[t]=stof(ch);

        }else {
            cout<<"Unable to Open Node File :(";
            return 0;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
        cout<<"Input Nodes : ";
        for(int i=0;i<2;i++){
            cout<<input_node[i]<<" ";           //Displaying Neurons Read From File
        }cout<<endl;
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    {
        if (config_file.is_open()) {                //This Scope Reads Layer Weights From File

            string temp;
            string ch;
            int t=0;

            for(int i=0;i<2;i++){

                temp.clear();
                ch.clear();
                t=0;
                getline(config_file,temp);              //Reading Line by Line
                f_idx++;

                for(int j=0;temp[j]!='\0';j++){

                    if(temp[j]!=',') {
                        ch=ch+temp[j];
                    }
                    else{
                        L1_weight[i][t]=stof(ch);           //Storing Weights in Array
                        t++;
                        if(t==neuron){
                            break;
                        }
                        ch.clear();
                    }
                }

                if(t!=neuron){
                    L1_weight[i][t]=stof(ch);
                }
            }
        }
        else{
            cout<<"Unable to Open Weight File :(";
            return 0;
        }

    }
    config_file.close();                                //Closing Configuration Files

    ///////////////////////////////////////////////////////////////////////////////////
    cout<<"Weights : "<<endl;
    for(int i=0;i<2;i++){
        for(int j=0;j<neuron;j++){
            cout<<L1_weight[i][j]<<" ";                 //Displaing Weights Read
        }
        cout<<endl;
    }
    //////////////////////////////////////////////////////////////////////////////////

                            //Start of Multithreading
    int s1=0;
    pthread_t input_neu[2];
    pthread_mutex_init (&input_mutex ,NULL) ;           //Initializing Mutex
    sem_init (&input_sem,0,1);                          //Initializing Semaphore
    int t[2]={0,1};
    
    pthread_mutex_lock (&input_mutex);          //Starting Somewhats of a Critical Sections
    
                for(int i=0;i<2;i++){
                pthread_create(&input_neu[i], NULL, input_t_fun, (void*)&t[i]);     //Creating Threads
                }
    
    pthread_mutex_lock (&input_mutex);//Locks the main untill are threads have been executed; unlocked by theard functions

    {
            float w=0;
            string wr;
            string wrc;             //String variables to write in buffer
            wr.clear();
            wrc.clear();

            for(int i=0;i<2;i++){

                for(int j=0;j<neuron;j++){
                    w=L1_weight[i][j];
                    wrc=to_string(w);           //Converting Neuron and Weights product into a string to write in buffer
                    wr=wr+wrc+',';
                }
            }
            for(int i=0;wr[i]!='\0';i++){
                input_buff[i]=wr[i];                //Writting in Buffer
            }
    }

    /////////////////////////////////////////////////////////////////////////////////////
    cout<<"Buffer : "<<endl;
    for(int i=0;input_buff[i]!='\0';i++){               //Displaying Buffer to be sent in Pipe
        cout<<input_buff[i];
    }
    cout<<endl<<endl;
    ///////////////////////////////////////////////////////////////////////////////////

    char pip[6]={'p','i','p','e','1','\0'};
    char p =(char)current_layer+48;                 //Pipe for Communication between Layers
    pip[4]=p;
    mkfifo(pip,0666);

    char w_pip[3]={'1','p','\0'};
    char pi =(char)current_layer+48;            //Pipe to Parrent wait for Child
    w_pip[0]=pi;
    char wait_buffer[100]{'\0'};
    mkfifo(w_pip,0666);


    current_layer=current_layer+1;          //Adding variable to move in Next Layer
  
    string arg_str;
    arg_str.clear();                        //Storing Arrguments in Pipe
    arg_str=arg_str+to_string(current_layer)+','+to_string(max_layers)+','+to_string(neuron)+','+to_string(f_idx)+',';
    char argu_buff[10]{'\0'};

    for(int i=0;arg_str[i]!='\0';i++){
        argu_buff[i]=arg_str[i];            //Converting Arguments in String 
    }

    int pid=fork();             //Forking

    if(pid>=0){                 //If Fork Successful

        if(pid==0){                                    //If Child

            execlp("./Layer","Layer",NULL);         //Exec To New Layer          
            exit(0);
        }
        else{
            //Parent Process

            int argu_open = open(argu_pip,O_WRONLY);
            write(argu_open,argu_buff,10);                      //Writting in Pipe to Pass Arguments
            close(argu_open);

            int wr_open = open(pip,O_WRONLY);
            write(wr_open,input_buff,1024);                      //Writting in Pipe to Pass Product of Neurons and Weights
            close(wr_open);

            int r_open = open(w_pip,O_RDONLY);
            read(r_open,wait_buffer,100);               //Pipe to make Parrent Wait for Child
            close(r_open);

            string line;
            line.clear();
            cout<<"Input Layer = "<<endl;                       //Displaying Answers recieved from Child
            cout<<"Fx1 =";

            for(int i=0;wait_buffer[i]!='\0';i++){

                if(wait_buffer[i]!=','){
                    cout<<wait_buffer[i];                   //Displaying Answers
                    line=line+wait_buffer[i];               //Storing in line to store answers in file
                }
                else{
                    cout<<endl;
                    line=line+',';
                    cout<<"Fx2 = ";
                }
                
            }cout<<endl<<endl;


            line=line+'\n';

            ifstream con_file(configPath);            //Opening file to edit
            string line_temp;
            getline(con_file,line_temp);
            line_temp.clear();
            while (getline(con_file, line_temp)) {              //Reading All other Lines
              line += line_temp + "\n";                 //Adding answer line with other Lines in file
            }
            con_file.close();
            
            ofstream con_file_out(configPath, std::ofstream::out);
            con_file_out << line;           //Writing New String in File
            con_file_out.close();

            sem_destroy(&input_sem);
            input_counter=0;                //Destroying Semaphore and Resting Variables for next Iteration
            f_idx=0;
            current_layer=1;

        }
    }
    else
    {
        cout<<"Fork Failed"<<endl;
    }

}
    cout<<endl;
    cout<<"Bye Bye :)"<<endl;                     //End of Program
    return 0;
}
