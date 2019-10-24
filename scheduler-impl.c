#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lab5_queue.h"

/**
ProcessNum  4
pidnum:123, arrival_time:50, execution_time:90
pidnum:13, arrival_time:80, execution_time:220
pidnum:1023, arrival_time:10, execution_time:1660
pidnum:12, arrival_time:80, execution_time:8
*/
void outprint(int time_x, int time_y, int pid, int arrival_time, int remaining_time);
void calculator_for_realtime(Process* proc, LinkedQueue** ProcessQueue, int proc_num, int queue_num, int period, int flag[proc_num],int tmp_time);
LinkedQueue* Sort_ProcessQueue(LinkedQueue* ProcessQueue,Process* proc, int proc_num, int queue_num);

void scheduler(Process* proc, LinkedQueue** ProcessQueue, int proc_num, int queue_num, int period){
	printf("Process number: %d\n", proc_num);
	for (int i = 0;i < proc_num; i++)
		printf("%d %d %d\n", proc[i].process_id, proc[i].arrival_time, proc[i].execution_time);

	printf("\nQueue number: %d\n", queue_num);
	printf("Period: %d\n", period);
	for (int i = 0;i < queue_num; i++){
        	printf("%d %d %d\n", i, ProcessQueue[i]->time_slice, ProcessQueue[i]->allotment_time);
 	}

	int tmp_time = proc[0].arrival_time;
	int flag[proc_num];
	int flag3[proc_num];//show the time
	int new_service_time[proc_num];
	int special_count = 0;//for special_case
	int period_count = 0;

	for(int i = 0;i < proc_num;i++){
		proc[i].completion_time = proc[0].arrival_time + proc[i].execution_time;
		proc[i].waiting_time = 0;
		proc[i].service_time = 0;
		new_service_time[i] = 0;
		flag[i] = 0;
		flag3[i] = 0;
	}

	int proc_int_count = 0;
	int count[queue_num-1];


	count[queue_num-1] = ProcessQueue[queue_num-1]->allotment_time;

	for(int i = queue_num-2;i >= 0;i--){//done
		count[i] = count[i+1] + ProcessQueue[i]->allotment_time;
	}
	count[0] = 1000000;
	do{
		int flag2 = 0;//use to know that this process queue have to done!!!

		for(int i = 0;i < proc_num;i++) // insert the data
		{
			//printf("tmp_time = %d, proc[i].arrival_time = %d,flag[i] = %d\n",tmp_time,proc[i].arrival_time, flag[i]);
			if( tmp_time >= proc[i].arrival_time && flag[i] == 0)// first we need to let them to queue[1]
			{
				EnQueue(ProcessQueue[queue_num-1], proc[i]);
				//printf("enqueue new queue\n");
				proc_int_count++;
				flag[i] = 1;//the process is running
				flag3[i] = queue_num-1;
				if(proc_num != 1)
					ProcessQueue[queue_num-1] = Sort_ProcessQueue(ProcessQueue[queue_num-1],proc,proc_num,queue_num);
			}

			for(int j = 1;j < queue_num;j++)// make it in the other priority
			{

				LinkedQueue *ptr = ProcessQueue[queue_num-1-j];
				if((new_service_time[i] == count[j]) && (flag[i] == 1 )&& flag3[i] != (j - 1) && (ptr->proc.completion_time != tmp_time)&& (flag3[i] != 0)){
					Process tmp_proc = FrontQueue(ProcessQueue[j]);
					while(tmp_proc.process_id != proc[i].process_id){
						EnQueue(ProcessQueue[j],FrontQueue(ProcessQueue[j]));
						DeQueue(ProcessQueue[j]);
						tmp_proc = FrontQueue(ProcessQueue[j]);
					}
					EnQueue(ProcessQueue[j-1],proc[i]);
					DeQueue(ProcessQueue[j]);
					flag3[i] = j-1;
					if(proc_num != 1){
						ProcessQueue[j-1] = Sort_ProcessQueue(ProcessQueue[j-1],proc,proc_num,queue_num);
					//	ProcessQueue[j] = Sort_ProcessQueue(ProcessQueue[j],proc,proc_num,queue_num);
					}

				}
			}

		}

		for(int j = 0;j < queue_num;j++){


			LinkedQueue* pt = ProcessQueue[queue_num-1-j];//pt[j] is iterator of ProcessQueue


			if((pt->next != NULL) && (flag2 == 0)){//check is it a queue list or void,if not void ,go while loop,for every case of the queue

				//I need to try to implement TDM in this case

				while(pt->next != NULL && flag2 != 1)//for every queue in this case,i need to just add them all
				{
					pt = pt->next;
					int time_slice = ProcessQueue[queue_num-1-j]->time_slice;
					for(int i = 0;i < proc_num;i++)
					{
						if(pt->proc.process_id == proc[i].process_id)
							pt->proc = proc[i];
					}
					
					if( ( (tmp_time + time_slice) > pt->proc.completion_time)){
						time_slice = pt->proc.completion_time - tmp_time;// if done less than the time_slice
					}

					if( ( (pt->proc.service_time + time_slice) > count[queue_num-1-j]) ){
						time_slice = count[queue_num-1-j] - pt->proc.service_time;
					}


					if( ( (tmp_time + time_slice) > (period_count+1)*period)){
						time_slice = (period_count+1)*period - tmp_time;
					}
					
					tmp_time += time_slice; // need to state the time

					pt->proc.service_time += time_slice;//add with service_time and mean that it is running
					pt->proc.waiting_time -= time_slice;//subtract the waiting time becuase it already run


					for(int i = 0;i < proc_num;i++)
					{

						if(pt->proc.process_id == proc[i].process_id){
							proc[i] = pt->proc;
							proc[i].completion_time -= time_slice;
							new_service_time[i] += time_slice;
							if(time_slice != 0)
								outprint(tmp_time-time_slice,tmp_time,proc[i].process_id,proc[i].arrival_time,proc[i].execution_time - proc[i].service_time);
						}

						if( tmp_time >= proc[i].arrival_time && flag[i] == 0){
							while(pt->next != NULL){
								pt = pt->next;
							}
						}

					}
					int flag7 = 0;
					for(int i = 0;i < proc_num;i++){

					
						int num = flag3[i];
						LinkedQueue *ptr = ProcessQueue[queue_num-1-j];
						ptr = ptr->next;

						if(flag[i]!=2 )
							proc[i].completion_time += time_slice;// other num of that
						if(flag[i]==1)
							proc[i].waiting_time += time_slice;// now process need to do that



						if((proc[i].completion_time == tmp_time) && (flag7 == 0) && (flag[i] != 2)){

							Process tmp_proc = FrontQueue(ProcessQueue[num]);

							if(tmp_proc.completion_time == tmp_time){
								DeQueue(ProcessQueue[num]);
								flag7 = 1;

							}

							while(tmp_proc.completion_time != tmp_time && (flag7 == 0)){
								if(tmp_proc.completion_time != tmp_time){
									EnQueue(ProcessQueue[num],tmp_proc);
									DeQueue(ProcessQueue[num]);
								}
								else
									flag7 = 1;
								tmp_proc = FrontQueue(ProcessQueue[num]);

							}
							if(flag7 == 0){
								QueuePrint(ProcessQueue[num]);
								DeQueue(ProcessQueue[num]);
							}

							flag[i] = 2;

							flag7=1;
						}
					}

				}
				flag2 = 1;

			}


		}

		int exit_num = 0;

		if(tmp_time == (period_count+1) * period){
			period_count++;
			
			for(int i = 0;i <queue_num-1;i++){
				LinkedQueue *ptr = ProcessQueue[i];

				while(!IsEmpty(ProcessQueue[i])){
					Process tmp_proc = FrontQueue(ProcessQueue[i]);
					EnQueue(ProcessQueue[queue_num-1],tmp_proc);
					DeQueue(ProcessQueue[i]);
				}

			}
			for(int i = 1;i < queue_num;i++){
				count[i] += period;
			}

			for(int i = 0;i < proc_num;i++){
				flag3[i] = queue_num-1;
				new_service_time[i] = period*period_count;
			}
		}
		for(int i = 0;i < proc_num;i++)
		{
			if(flag[i] == 2)
				exit_num++;
		}
		if(exit_num == proc_num) break;//end with the system
	}while(1);

}


LinkedQueue* Sort_ProcessQueue(LinkedQueue* ProcessQueue,Process* proc, int proc_num, int queue_num){

	//i will use the selection in this case
	LinkedQueue *pt = ProcessQueue;
	int count_num[proc_num];
	int len = 0;
	int i = 0;
	int j = 0;

	while(pt->next != NULL){
		pt = pt->next;
		count_num[i] = pt->proc.process_id ;
		len++;
		i++;
	}
	for(int i = 0;i < len;i++){
		for(int j = i;j < len;j++){
			if(count_num[i] < count_num[j]){
				int temp = count_num[i];
				count_num[i] = count_num[j];
				count_num[j] = temp;
			}
		}
	}
	while(!IsEmpty(ProcessQueue)){
		DeQueue(ProcessQueue);
	}
	for(int k = 0;k < len;k++){
		for(int i = 0;i < proc_num;i++){
			if(count_num[k] == proc[i].process_id){
				EnQueue(ProcessQueue,proc[i]);
				i = proc_num;
			}
		}
	}
	return ProcessQueue;

}


