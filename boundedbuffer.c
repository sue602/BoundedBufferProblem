/*
 * Portions created by SGI are Copyright (C) 2000 Silicon Graphics, Inc.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met: 
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Silicon Graphics, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../DARWIN_15.4.0_DBG/st.h"

int MAX_COUNT = 100;
int count = 0;
st_cond_t condition;
st_mutex_t mutex;

void
producer(void * d)
{
  printf("producer =%d\n",(int)d);
  while(1)
  {
    st_mutex_lock(mutex);
    if(count>=MAX_COUNT)
    {
      printf("=>wait produce=%d count=%d\n",(int)d,count);
      st_mutex_unlock(mutex);
      st_cond_wait(condition);
    }
    else
    {
      count++;
      printf("=>produce=%d count=%d\n",(int)d,count);
      st_mutex_unlock(mutex);
      st_cond_signal(condition);
      st_usleep(1000 * 100);
    }
    
  }
}

void
consumer(void * d)
{
  printf("consumer =%d\n",(int)d);
  while(1)
  {
    st_mutex_lock(mutex);
    if(0 >= count)
    {
      printf("<=========wait consumer=%d count=%d\n",(int)d,count);
      st_mutex_unlock(mutex);
      st_cond_wait(condition);
    }
    else
    {
        count--;
        printf("<=========consumer=%d count=%d\n",(int)d,count);
        st_mutex_unlock(mutex);
        st_cond_signal(condition);
        st_usleep(1000 * 100);
    }
    
  }
}

/*
 * Asynchronous DNS host name resolution. This program creates one
 * ST thread for each host name (specified as command line arguments).
 * All threads do host name resolution concurrently.
 */
int main(int argc, char *argv[])
{
  if (st_init() < 0) {
    perror("st_init");
    exit(1);
  }
  condition = st_cond_new();
  mutex = st_mutex_new();

  int maxQueueP = 10;
  int maxQueueC = 15;
  st_thread_t p[6],c[6];

  int i = 0;
  for (; i < maxQueueP; ++i)
  {
    if (  (p[i] = st_thread_create(producer,i,1,0) ) == NULL) {
      perror("st_thread_create producer failed");
      exit(1);
    }
  }

  i = 0;
  for (; i < maxQueueC; ++i)
  {
    if ( (c[i] = st_thread_create(consumer,i,1,0) ) == NULL) {
      perror("st_thread_create consumer failed");
      exit(1);
    }
  }
  

  i = 0;
  for (; i < maxQueueP; ++i)
  {
    st_thread_join(p[i],0);
  }
  i = 0;
  for (; i < maxQueueC; ++i)
  {
    st_thread_join(c[i],0);
  }

  

  /* NOTREACHED */
  return 1;
}

