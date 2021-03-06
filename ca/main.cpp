/**
* File "main.cpp"
* Created by Sina on Sun Mar  1 17:19:44 2015.
*/

#include "sstream"
#include "Header.h"
#include "user.h"

int main(){
    int socketfd, socket_accept_fd, port_number,read_status,max_fd;
    map <int, string> toUser;
    vector<User> users;
    char buffer[MAX_MSG_SIZE];
    int serverFD = -1;
    bzero(&buffer,MAX_MSG_SIZE);
    struct sockaddr_in server_address, client_address;
    fd_set server,read_fds;
    KeyPair cakp = getKeyPair(2048);
    FD_ZERO(&server);
    FD_ZERO(&read_fds);
    port_number = atoi(CA_PORT);

    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0) 
        failure("problem in openning socket!");

    build_server(socketfd, &server_address,port_number);
    FD_SET(0,&server);
    FD_SET(socketfd,&server);
    max_fd = socketfd;

    while(1){
        read_fds = server;
        if(select(max_fd+1,&read_fds,NULL,NULL,NULL) < 0)
            failure("problem in sockets select!");
        int i;
        for( i=0; i<=max_fd ; i++){
            if(FD_ISSET(i , &read_fds)){
                if(i==0){
                    // input 
                }
                else if(i==serverFD){
                    //server
                    read(i, buffer, MAX_MSG_SIZE);
                    string uname = buffer;
                    if(isUserExist(users, uname)){
                        User user = findUser(users, uname);
                        send_message("OK",i);
                        string cer = reciveAndDec(i, false, cakp);
                        if(user.checkCer(cer)){
                            send_message("OK", i);
                        } else {
                            send_message("certificates does not match.", i);
                        }
                    } else {
                        send_message("no such user.", i);
                    }
                }
                else if(i!=socketfd){
                    //client
                    unsigned char encMsg[MAX_MSG_SIZE];
                    read(i, buffer, MAX_MSG_SIZE);
                    string user = buffer;
                    string addr;
                    addr += "../users/"+user+"_pub.pem";
                    stringstream pubkeystream;
                    cout<<"reading public key: "<<addr<<"\n";
                    ifstream pubReader(addr.c_str());
                    pubkeystream << pubReader.rdbuf();
                    pubReader.close();
                    string pubkey = pubkeystream.str();
                    if(pubkey!=""){
                        send_message("OK", i);
                    }
                    else{
                        send_message("public key not found\n", i);
                        continue;
                    }
                    
                    KeyPair kp;
                    kp.setPub(pubkey);

                    string msg = reciveAndDec(i, true, kp);
                    cout<<msg<<endl;
                    User newUser(msg);
                    if(isUserExist(users, newUser)){
                        encAndSend(i,true,"User Already Exists", kp);
                        cout<<"user does not registered!\n";
                    }else{
                        cout<<"registering user.\n";
                        //encAndSend(i, true, "OK", kp);
                        send_message("OK", i);

                        string ret = reciveAndDec(i, true, kp);
                        if(ret=="sendme"){
                            cout<<"generating certificate.\n";
                            string cer = newUser.getCer();
                            cout<<"sending certificate!\n";
                            encAndSend(i, true, cer, cakp);
                            users.push_back(newUser);
                            cout<<"user registered\n";
                        }
                        else{
                            cout<<"something unknown went wrong!\n";
                        }
                    }
                }
                else{
                    //there is a new connection
                    socklen_t client_address_size;
                    client_address_size = sizeof(client_address);
                    socket_accept_fd = accept(socketfd, (struct sockaddr *) &client_address, &client_address_size);
                    if (socket_accept_fd ==-1)
                        failure("problem in accept");
                    FD_SET(socket_accept_fd,&server);
                    if(socket_accept_fd>max_fd)
                        max_fd = socket_accept_fd;
                    read(socket_accept_fd,buffer,MAX_MSG_SIZE);
                    if(!strcmp(buffer, "client")){
                        send_message("OK", socket_accept_fd);
                    } else if(!strcmp(buffer,"server")){
                        send_message("OK", socket_accept_fd);
                        serverFD = socket_accept_fd;
                    }
                    cout<<"new client or server connected.\n";
                }
            }
        }//for
    }//while

    return 0;
}

