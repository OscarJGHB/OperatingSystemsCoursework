
// -----------------------------------
// CSCI 340 - Operating Systems
// Spring 2025
// web.c header file
// 
// Homework 3
//
// -----------------------------------

#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "web.h"

// global variable used by the write_callback function
size_t length = 0;


// ------------------------------------
// Function prototype that is used by the curl library to
// retrieve the webpage (in the response message) and 
// store in the web_t webpage field.
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

size_t write_callback(void *ptr, size_t size, size_t nmemb, char** buffer ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	*buffer = (char*)realloc( *buffer, ( length + size*nmemb )*sizeof( char* ) ); 

	strcat( *buffer, (char*) ptr );

	length += (size*nmemb);

	return size * nmemb;

} // end write_callback function

// ------------------------------------
// Function prototype that connects to the web address 
// specified in the web_t url field and retrieves
// the html webpage (using the curl c-library)
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//

int get_webpage( web_t* web_struct ) {

	// -------------------------------------
	// Please do not modify this code
	// -------------------------------------

	CURL *curl;
  	CURLcode error_code = FAIL;

	length = 0;

  	curl = curl_easy_init();

  	if ( curl ) {

	    	web_struct->webpage = malloc( sizeof( char ) * length );

	    	curl_easy_setopt(curl, CURLOPT_URL, web_struct->url );
	    	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &web_struct->webpage );
	 
	    	error_code = curl_easy_perform( curl );

		curl_easy_cleanup( curl );

	}
    
	return ( error_code == OK ) ? OK : FAIL;

} // end get_response function



// ------------------------------------
// Function prototype that parses webpage/URL 
// pointed at by client socket
// 
//
// Arguments:	web_t pointer
//
// Return:     	OK on success, FAIL on error
//
void parse( web_t* web_struct ) {
	int i;
	int link_arr_size = 32;

	//what to search for
	char* target = "<a";
	char* inner_target = "href"; //separate in case extra spaces
	char* href_target = "trump";
	int target_len = strlen(target);
	int inner_target_len = strlen(inner_target);
	int href_target_len = strlen(href_target);
	int is_href = 0;
	
	char* webpage_ptr = web_struct->webpage; //points at webpage
	char* href_ptr; //points at the href to parse

	web_struct->links = malloc( sizeof(char*) * link_arr_size );
	*(web_struct->links)= "\0"; //initialize in case no mention of trump
	

	char** link_arr = web_struct->links;
	int* num_links = &(web_struct->link_cnt);


	while(*webpage_ptr != '\0')
	{
		if(strncmp(webpage_ptr,target,target_len)== 0) //anchor found
		{
			i = 0; 
			is_href = 0;

			webpage_ptr = webpage_ptr + target_len; //move out <a //go past anchor tag

			//jumps over spaces
			while (*webpage_ptr == ' ' && *webpage_ptr != '\0') 
			{
				webpage_ptr++;
			}

			while(*webpage_ptr !=  '>' && *webpage_ptr != '\0') //gets to end of anchor
			{	
				if(strncmp(webpage_ptr,inner_target, inner_target_len) == 0) //finds where href is kept
				{
					is_href = 1;
					i = 0; 
					webpage_ptr = webpage_ptr + inner_target_len; //jumps over href=
				}
				if(is_href && *webpage_ptr == ' ') //in case there is an obstruction between <a and href=
				{
					break;
				}
				else
				{
					i++;
					webpage_ptr++;	
				}
				
			}

			if(is_href){
				
				href_ptr = webpage_ptr-i; //ptr to strt
				
				while(*(webpage_ptr-1) == ' ' || *(webpage_ptr-1) == '>' || *(webpage_ptr-1) == '"' || *(webpage_ptr-1) == '\'')
				{
					webpage_ptr--;
					i--;
				}
				
				while(*href_ptr == '\'' || *href_ptr=='"' || *href_ptr == ' ' || *href_ptr == '=')
				{
					href_ptr++;
					i--;
				}

				//find the sole / to get past the url for the link
				while( *href_ptr != '>' && *(href_ptr+1) != '>') 
				{
					if(*href_ptr == '/' && *(href_ptr+1) == '/')
					{
						href_ptr+=2;
						i-=2;
					}
					else if(*href_ptr == '/' && *(href_ptr+1) != '/')
					{
						break;
					}
					else
					{
						href_ptr++;	
						i--;
					}	
				}


				

				
				for(int j = 0; j < i-href_target_len;j++)
				{
					if(strncmp(href_ptr+j,href_target,href_target_len) ==0) //trump
					{
						if(*num_links >= link_arr_size)
						{
							link_arr_size *= 2;
							web_struct->links = realloc(web_struct->links,sizeof(char*) * link_arr_size);
							link_arr = web_struct->links;
						}


						link_arr[*num_links] = malloc(sizeof(char) * i+1); //allocates enough space for i char and 1 null
						
						strncpy(link_arr[*num_links], href_ptr,i);
						link_arr[*num_links][i] = '\0';
						//printf("Trump Acquired on %s: %s and added to index %d\n",web_struct->url, link_arr[*num_links],web_struct->link_cnt);
						(*num_links)++;

						break; 
					}
				}
			}
		}
		else
		{
			webpage_ptr++;
		}
		
	}
	

	



	if ( WEB_DEBUG ) {
		//printf("URL=%s\n", web_struct->url ); 
		//printf("CNT=%d\n", web_struct->link_cnt );
		//printf("WEBPAGE=%s\n", web_struct->webpage );
	}
	

	


} // end parse function
