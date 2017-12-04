
#include "stdafx.h"


using std::cout;
using std::cin;

int myid, numprocs, size, rank;

double f(double a)
{
	return (4.0 / (1.0 + a*a));
};

static void My_MPI_Bcast(void * buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm) {

	int tag = 0;

	//MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	//MPI_Comm_rank(MPI_COMM_WORLD, &myid);

	if (myid == root)
	{
		for (int i = 0; i < numprocs; i++)
		{
			if (i != root)
			{
				MPI_Send(buffer, count, datatype, i, tag, comm);
			}
		}
	}
	else
	{
		MPI_Status status;
		MPI_Recv(buffer, count, datatype, root, tag, comm, &status);
	}
}

static void My_MPI_Gather(int send_buf[], int send_count, MPI_Datatype send_type, int recv_buf[], int recv_count, MPI_Datatype recv_type, int root, MPI_Comm comm)
{
	int tag = 0;

	if (rank == root)
	{
		for (int i = 0; i < size; i++)
		{
			for (int j = rank * send_count; j < rank * send_count + send_count; j++)
			{
				recv_buf[j] = send_buf[j % send_count];
			}
			if (i != rank)
			{
				MPI_Status status;
				int *tab = new int[send_count];
				MPI_Recv(tab, send_count, recv_type, i, tag, comm, &status);
				for (int j = i * send_count; j < i * send_count + send_count; j++)
				{
					recv_buf[j] = tab[j%send_count];
				}
			}
		}
	}
	else
	{
		MPI_Send(send_buf, send_count, send_type, root, tag, comm);
	}
}

void zadanie1()
{
	int done = 0, n, i;
	double PI25DT = 3.141592653589793238462643;
	double mypi, pi, h, sum, x;
	double startwtime, endwtime;
	int  namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Get_processor_name(processor_name, &namelen);

	cout << "Process " << myid << " on " << processor_name << "\n";

	n = 0;
	while (!done)
	{
	if (myid == 0)
	{
	cout << "Enter the number of intervals: (0 quits) ";
	cin >> n;

	startwtime = MPI_Wtime();
	}
	My_MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (n == 0)
	done = 1;
	else
	{
	h = 1.0 / (double)n;
	sum = 0.0;
	for (i = myid + 1; i <= n; i += numprocs)
	{
	x = h * ((double)i - 0.5);
	sum += f(x);
	}
	mypi = h * sum;

	MPI_Reduce(&mypi, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (myid == 0)
	{
	cout << "pi is approximately " << pi << " Error is " << fabs(pi - PI25DT) << "\n";

	endwtime = MPI_Wtime();
	cout << "wall clock time = " << endwtime - startwtime << "\n";
	}
	}
	}
	MPI_Finalize();
}
void zadanie3()
{
	int *global_array = NULL;
	int *local_array = NULL;
	int mult;
	int process_number;
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	//initialization of global_array in the memory of process 0
	if (rank == 0) {
		cout << "Enter the multiplier: ";
		cin >> mult;
		global_array = new int[size * mult];

		for (int i = 0; i < size * mult; i++)
			global_array[i] = i;

		cout << "\nProcessor " << rank << " has data: ";
		for (int i = 0; i < size * mult; i++)
			cout << global_array[i] << " ";
		cout << "\n";
	}
	fflush(stdout);


	//distributing the multiplier:
	MPI_Bcast(&mult, 1, MPI_INT, 0, MPI_COMM_WORLD);

	local_array = new int[mult];

	MPI_Scatter(global_array, mult, MPI_INT, local_array, mult, MPI_INT, 0, MPI_COMM_WORLD);

	for (int p = 0; p < size; p++)
	{
		//MPI_Barrier(MPI_COMM_WORLD);
		if (rank == p)
		{
			cout << "\nProcessor " << rank << " has data: ";
			for (int i = 0; i < mult; i++)
				cout << local_array[i] << " ";
			cout.put('\n');
		}
	}

	//doubling local data
	for (int i = 0; i < mult; i++)
		local_array[i] *= 2;
	//printf("Processor %d doubling the data, now has %d\n", rank, local_array);


	My_MPI_Gather(local_array, mult, MPI_INT, global_array, mult, MPI_INT, 0, MPI_COMM_WORLD);


	fflush(stdout);

	if (rank == 0) {
		cout << "\nProcessor " << rank << " has data: ";
		for (int i = 0; i<size * mult; i++)
			cout << global_array[i] << " ";
		cout << "\n";
	}

	if (rank == 0)
		delete global_array;

	delete local_array;

	MPI_Finalize();
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	//zadanie1();
	zadanie3();

	
	return 0;
}


