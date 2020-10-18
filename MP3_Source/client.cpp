/* 
  File: client.cpp

  Author: R. Parkekr
      Department of Computer Science
      Texas A&M University
  Date  : 2020/09/22

  Client main program for MP2 in CSCE 313
*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

  /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include <cassert>
#include <string>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <errno.h>
#include <unistd.h>

#include "reqchannel.hpp"
#include "pc_buffer.hpp"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/
typedef struct RTFArgs{
  int n_reqs;
  string patient_name;
  PCBuffer * buffer;
} RTFArgs;

typedef struct WTFArgs{
  PCBuffer * buffer;
  RequestChannel * rc;
  int thread_id;
  PCBuffer * joe_stats;
  PCBuffer * jane_stats;
  PCBuffer * john_stats;
} WTFArgs;

typedef struct STFArgs{
  string name;
  PCBuffer * pcb;
  int n_reqs;
} STFArgs;

//PCBuffer buffer;

// defaults for command line arguments
int bufferSize = 500;
int numWorkerThread = 10;
int numRequests = 10000;
int histograms[3][10];

/*--------------------------------------------------------------------------*/
/* CONSTANTS */
/*--------------------------------------------------------------------------*/

  /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */
/*--------------------------------------------------------------------------*/

  /* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* LOCAL FUNCTIONS -- SUPPORT FUNCTIONS */
/*--------------------------------------------------------------------------*/

std::string int2string(int number) {
  std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

void print_time_diff(const std::string _label,
           const struct timeval & _tp1,
           const struct timeval & _tp2) {
  // Prints to stdout the difference, in seconds and museconds, between
  // two timevals.
  
  std::cout << _label;
  long sec = _tp2.tv_sec - _tp1.tv_sec;
  long musec = _tp2.tv_usec - _tp1.tv_usec;
  if (musec < 0) {
    musec += 1000000;
    sec--;
  }
  std::cout << " [sec = " << sec << ", musec = " << musec << "]" << std::endl;
  
}

std::string generate_data() {
  // Generate the data to be returned to the client.
  return int2string(rand() % 100);
}

void * request_thread(void * args) {
  RTFArgs * rtfa = (RTFArgs *) args;
  //cout << "Patient name: " << rtfa->patient_name << endl;
  for(int i = 0; i < rtfa->n_reqs; i++) {
    string req = "data " + rtfa->patient_name;
    //cout << "Requesting: " << req << endl;
    rtfa->buffer->Deposit(req);
  }
  rtfa->buffer->Deposit("Done");
}

void * worker_thread(void * args) {
  WTFArgs * wtfa = (WTFArgs *) args;
  for(;;) {
    string req = wtfa->buffer->Retrieve();
    if(req == "Done") {
      break;
    }
    string reply = wtfa->rc->send_request(req);
    if(req == "data Joe Smith") {
      wtfa->joe_stats->Deposit(reply);
    }
    if(req == "data Jane Smith") {
      wtfa->jane_stats->Deposit(reply);
    }
    if(req == "data John Doe") {
      wtfa->john_stats->Deposit(reply);
    }
    cout << "req = " << req << endl;
  }
  wtfa->rc->send_request("quit");
}

void * stat_thread(void * args) {
  STFArgs * stfa = (STFArgs *) args;
  for(int i = 0; i < stfa->n_reqs; i++) {
    int reply = stoi(stfa->pcb->Retrieve()); 
    cout << "statistics for " << stfa->name << " reply: " << reply << endl;
    if(stfa->name == "Joe Smith") {
      histograms[0][reply / 10]++;
    }
    if(stfa->name == "Jane Smith") {
      histograms[1][reply / 10]++;
    }
    if(stfa->name == "John Doe") {
      histograms[2][reply / 10]++;
    }
  }
}

void print_hists() {
  for(int i = 0; i < 3; i++) {
    cout << "     Histogram for patient " << i << endl;

    cout << setw(10) << "0-9" << setw(10) << "10-19" 
          << setw(10) << "20-29" << setw(10) << "30-39"
          << setw(10) << "40-49" << setw(10) << "50-59"
          << setw(10) << "60-69" << setw(10) << "70-79"
          << setw(10) << "80-89" << setw(10) << "90-99" 
          << "\n";

    for(int j = 0; j < 10; j++) {
      cout << setw(10) << histograms[i][j];
    }
    cout << endl << endl;
  }
}

/*--------------------------------------------------------------------------*/
/* MAIN FUNCTION */
/*--------------------------------------------------------------------------*/

int main(int argc, char * argv[]) {

  //Handle 
  int cmd;
  while((cmd = getopt(argc, argv, "b:n:w:")) != -1) {
    switch(cmd) {
      case 'b':
        bufferSize = atoi(optarg);
        break;
      case 'n':
        numRequests = atoi(optarg);
        break;
      case 'w':
        numWorkerThread = atoi(optarg);
        break;
      case '?':
        cout << "Invalid argument aborting" << endl << endl;
        return 1;
      default:
        cout << "Command line error" << endl;
        cout << "Options:\n-b=# : Size of PCBuffer\n-n=# : Number of requests\n-w=# : Number of worker threads" << endl;
        return 1;
    }
  }

  if(fork() == 0) {
    //char * args[]={"./dataserver", nullptr};
    //execvp(args[0], args);
    execvp("./dataserver", NULL);
  }

  //cout << bufferSize << endl << endl << endl;
  //buffer = new PCBuffer(bufferSize);


  std::cout << "CLIENT STARTED:" << std::endl;

  std::cout << "Establishing control channel... " << std::flush;
  RequestChannel chan("control", RequestChannel::Side::CLIENT);
  std::cout << "done." << std::endl;

  PCBuffer buffer(bufferSize);
  PCBuffer joe_stats(bufferSize);
  PCBuffer jane_stats(bufferSize);
  PCBuffer john_stats(bufferSize);
  pthread_t requestThread[3];
  pthread_t workerThread[numWorkerThread];
  pthread_t statThread[3];

  std::cout << "Create worker threads" << std::endl;
  WTFArgs workerData[numWorkerThread];
  for(int i = 0; i < numWorkerThread; i++) {
    workerData[i].buffer = &buffer;
    workerData[i].thread_id = i;
    workerData[i].joe_stats = &joe_stats;
    workerData[i].jane_stats = &jane_stats;
    workerData[i].john_stats = &john_stats;

    string reply5 = chan.send_request("newthread");
    cout << "Reply to request 'newthread' is " << reply5 << "" << endl;
    workerData[i].rc = new RequestChannel(reply5, RequestChannel::Side::CLIENT);
    pthread_create(&workerThread[i], NULL, worker_thread, (void *)&workerData[i]);
  }


  cout << "Create request threads" << endl;
  RTFArgs requestData[3];
  requestData[0].n_reqs = numRequests;
  requestData[0].patient_name = "Joe Smith";
  requestData[0].buffer = &buffer;
  requestData[1].n_reqs = numRequests;
  requestData[1].patient_name = "Jane Smith";
  requestData[1].buffer = &buffer;
  requestData[2].n_reqs = numRequests;
  requestData[2].patient_name = "John Doe";
  requestData[2].buffer = &buffer;
  
  pthread_create(&requestThread[0], NULL, request_thread, (void*)&requestData[0]);
  pthread_create(&requestThread[1], NULL, request_thread, (void*)&requestData[1]);
  pthread_create(&requestThread[2], NULL, request_thread, (void*)&requestData[2]);

  cout << "Create statistics threads" << endl;
  STFArgs statData[3];
  statData[0].name = "Joe Smith";
  statData[0].pcb = &joe_stats;
  statData[0].n_reqs = numRequests;
  statData[1].name = "Jane Smith";
  statData[1].pcb = &jane_stats;
  statData[1].n_reqs = numRequests;
  statData[2].name = "John Doe";
  statData[2].pcb = &john_stats;
  statData[2].n_reqs = numRequests;
  pthread_create(&statThread[0], NULL, stat_thread, (void *)&statData[0]);
  pthread_create(&statThread[1], NULL, stat_thread, (void *)&statData[1]);
  pthread_create(&statThread[2], NULL, stat_thread, (void *)&statData[2]);

  for(int i = 0; i < 3; i++) {
    pthread_join(requestThread[i], NULL);
  }

  for(int i = 0; i < numWorkerThread; i++) {
    pthread_join(workerThread[i], NULL);
  }

  for(int i = 0; i < 3; i++) {
    pthread_join(statThread[i], NULL);
  }

  //cout << "test" << endl;


  /* -- Start sending a sequence of requests */

  /*std::string reply1 = chan.send_request("hello");
  std::cout << "Reply to request 'hello' is '" << reply1 << "'" << std::endl;

  std::string reply2 = chan.send_request("data Joe Smith");
  std::cout << "Reply to request 'data Joe Smith' is '" << reply2 << "'" << std::endl;

  struct timeval timeStart_1;
  struct timeval timeEnd_1;

  assert(gettimeofday(&timeStart_1, 0) == 0);

  std::string reply3 = chan.send_request("data Jane Smith");
  std::cout << "Reply to request 'data Jane Smith' is '" << reply3 << "'" << std::endl;

  assert(gettimeofday(&timeEnd_1, 0) == 0);

  for(int i = 0; i < 100; i++) {
  std::string request_string("data TestPerson" + int2string(i));
  std::string reply_string = chan.send_request(request_string);
  std::cout << "reply to request " << i << ":" << reply_string << std::endl;;
  }

  struct timeval timeStart_2;
  struct timeval timeEnd_2;

  assert(gettimeofday(&timeStart_2, 0) == 0);

  generate_data();

  assert(gettimeofday(&timeEnd_2, 0) == 0);

 */


  std::string reply4 = chan.send_request("quit");
  std::cout << "Reply to request 'quit' is '" << reply4 << std::endl;
/*
  print_time_diff("Time taken for computation remotely: ", timeStart_1, timeEnd_1);
  print_time_diff("Time taken for computation localy: ", timeStart_2, timeEnd_2);
*/
  usleep(1000000);
  print_hists();

  cout << "End of client" << endl;
}
