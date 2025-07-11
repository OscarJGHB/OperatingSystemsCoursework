// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2025
// server.c source file
// Assignment 2
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

// ------------------------------------
// Function prototype that creates a socket and 
// then binds it to the specified port_number 
// for incoming client connections
// 
// Arguments:	port_number = port number server socket will be bound to.
//
// Return:      Socket file descriptor (or -1 on failure)
//
int bind_port(unsigned int port_number)
{
  // -------------------------------------
  // NOTHING TODO HERE :)
  // -------------------------------------
  // DO NOT MODIFY !

  int socket_fd;
  int set_option = 1;

  struct sockaddr_in server_address;
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&set_option,
	     sizeof(set_option));

  if (socket_fd < 0) return FAIL;

  bzero((char *) &server_address, sizeof(server_address));

  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_number);

  if (bind(socket_fd, (struct sockaddr *) &server_address,
	     sizeof(server_address)) == 0) {
    return socket_fd;
  } else {
    return FAIL;
  }
} // end bind_port function

void rewrite(char* ptr, const char* reset_val)
{
  //ptr should be at beginning of source already
  int i;
  for(i=0;i<strlen(reset_val); i++) //as long as less than reset val
  {
    *ptr = *(reset_val+i); //overwrites the char array with reset val
    ptr++;
  }
  *ptr = '\0';
  //atp srhould be pointing to end, adding a null to signal end of char arr
  //ptr will point at null as a starting point


}


// ------------------------------------
// Function prototype that accepts a client
// socket connection
// 
// Arguments:	server file descriptor
//
// Return:      Termination status of client (0 = No Errors, -1 = Error)
//
int accept_client(int server_socket_fd)
{
  int exit_status = OK;
  int client_socket_fd = -1;
  socklen_t client_length; 
  struct sockaddr_in client_address;
  char request[2048];

  client_length = sizeof(client_address);

  client_socket_fd = accept(server_socket_fd,
			     (struct sockaddr *) &client_address,
			     &client_length);
  // -------------------------------------
  // TODO:
  // -------------------------------------
  // Modify code to fork a child process 
  // -------------------------------------

  pid_t pid;
  pid = fork();

  if (pid != 0){close(client_socket_fd);}

  else if (pid ==0){ //child
    close(server_socket_fd);
    if (client_socket_fd >= 0) {
      char entity_body[1024] = "\0";
      bzero(request, 2048);
      read(client_socket_fd, request, 2047);
      if (OK) printf("Here is the http message:\n%s\n\n", request);
      
      
 
      
      ////////////////////////// gets the method/////////////
      char method[10];
      int i = 0;
      char* p = request;

      //gets requested method
      while(*p != '\0' && *p != '\n' && *p != ' ')
      {
        
        method[i] = *p;
        p++;
        i++;
      }
      method[i] = '\0';

      /////////////////get method//////////////////

      if(strcmp(method,"GET") == 0){
        

        char url_to_parse[512];
        while(*p == ' ' && *p != '\n') {p++;} //atp p is after the method, so the following line, until a space/nl, is what we want to parse
        p++;

        
        i = 0; //set up position counter
        while(*p != ' ' && *p != '\n' && *p != '\0')
        {
          url_to_parse[i] = *p;
          
          i++;
          p++;
        }
        url_to_parse[i] = '\0'; //adds null chr to end, signaling end of string

        
        p = url_to_parse; 

        if(*p == '?') //checks if query
        {
          strcat(entity_body,"<html><body><h1>GET Request</h><h2>CSCI 340 (Operating Systems) Homework 2</h2><table border=1 width=\"50%\">");
      
          p++; //get onto ltr of key
          //create key and value, then pair obj
          char pairs[32][512];
          char keys[32][256];
          char vals[32][256];
          char key[256];
          char val[256];
          char* p1 = key; //points at beginng
          i = 0; //to point at pos in key and values array
          
          
          rewrite(p1,"<td><b>"); //overwrites the string, replacing first part
          p1 = p1 + strlen(key);

          while (*p != '\0') //while not at end of parsing
          {
            //if we encounter another & then we will refresh the key and value
            if(*p == '&') //begins a new key/val
            {
              strcat(val,"</td>"); //got out of equal sign, onto new key
              strcpy(vals[i],val);
              i++;
              p1 = key;
              rewrite(p1,"<td><b>");
              p1 = p1 + strlen(key);
              p++;
            }
            else if(*p == '=')
            {
              strcat(key,"</b></td>"); //got out of key
              strcpy(keys[i],key);

              p1 = val;
              rewrite(p1,"<td>");
              p1 = p1 + strlen(val);
              p++;
            }
            else{
              *p1 = *p;
              *(p1+1) = '\0';
              p++;
              p1++;
              }
            
           

          }

          strcat(val,"</td>"); //catch the last one
          strcpy(keys[i], key);
          strcpy(vals[i], val);
          i++;
          
          int j;
          for(j=0;j<i;j++) //for each pair add on
          {
            strcat(pairs[j],"<tr>");
            strcat(pairs[j],keys[j]);
            strcat(pairs[j],vals[j]);
            strcat(pairs[j],"</tr>");

            strcat(entity_body, pairs[j]);
          }

          strcat(entity_body, "</table></body></html>");
          


        }
        else //requesting an html file by name
        { 
          FILE *fp; //file
          fp = fopen(url_to_parse, "r"); //opens txt files
          if(fp == NULL)
          {
            strcat(entity_body,"<html><body><h1>NO FILE LOCATED</h></body></html>");

          }
          else
          {
            char line[255]; 
            while(fgets(line, 255, fp) != NULL)
            {
              p = line;
              strcat(entity_body,p);
            }
            fclose(fp);
          }
        }

////////////////////post method////////////////////////////////////
      }
      else if(strcmp(method,"POST") == 0)
      {
        while(p!= request + (int)strlen(request)-5)
        {
          if (strncmp(p, "\r\n\r\n", 4) == 0){break;}
          p++;
        }
        p+=4;
        strcat(entity_body,"<html><body><h1>POST Request</h><h2>CSCI 340 (Operating Systems) Homework 2</h2><table border=1 width=\"50%\">");
      
        //create key and value, then pair obj
        char pairs[32][512];
        char keys[32][256];
        char vals[32][256];
        char key[256];
        char val[256];
        char* p1 = key;
        int i = 0; 

        rewrite(p1,"<td><b>"); 
        p1 = p1 + strlen(key);

        while (*p != '\0') 
        {
          if(*p == '&') 
          {
            
            strcat(val,"</td>"); 
            strcpy(vals[i],val);
            i++; 
            p1 = key;
            rewrite(p1,"<td><b>");
            p1 = p1 + strlen(key);
            p++;
          }
          else if(*p == '=')
          {
            strcat(key,"</b></td>"); 
            strcpy(keys[i],key);

            p1 = val;
            rewrite(p1,"<td>");
            p1 = p1 + strlen(val);
            p++;
          }
          else{
            *p1 = *p;
            *(p1+1) = '\0';
            p++;
            p1++;
            }
          
          

        }

        strcat(val,"</td>"); //slaps onto last val

        strcpy(keys[i], key);
        strcpy(vals[i], val);
        i++;
        
        int j;
        for(j=0;j<i;j++) //for each pair add on
        {
          strcat(pairs[j],"<tr>");
          strcat(pairs[j],keys[j]);
          strcat(pairs[j],vals[j]);
          strcat(pairs[j],"</tr>");

          strcat(entity_body, pairs[j]);
        }

      }
      

      
      char response[2048];
      sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s",
        (int)strlen(entity_body), entity_body);
      
      if (OK) printf("%s\n", response);
      
      write(client_socket_fd, response, strlen(response));

      close(client_socket_fd);
    } else {
      exit_status = FAIL;
    }
    exit(0);
  }
  if(OK)printf("Exit status = %d\n", exit_status);

  return exit_status;
} // end accept_client function
