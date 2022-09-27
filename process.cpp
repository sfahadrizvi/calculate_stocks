


#include <iostream>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#define CACHE_SIZE 1024*16*1024
#define MAX_LINE_SIZE 100

struct stock
{
	long long time_gap;  //this is the maximum time gap
	long long timestamp; //stores previous timestamp
	long long volume;    // stores the volume 
	long long max_price; //maximum price
	long long total_price; //the price of shares multiplied by the number of shares. this is used in the weighted average calculation
	char symbol[3]; //3 character symbol

};


/*
this funciton is used by the sort algorithm*/
bool compare_stock(const stock& lhs, const stock& rhs) {
	int a = strcmp(rhs.symbol, lhs.symbol);
	if (a <= 0)
		return false;
	else
		return true;
}

std::vector<struct stock> stock_list;//global vector to hold the stocks

int buffer_length=0; //defines the number of characters read
int buffer_index=0; //index of the buffer to be used to parse lines from the input
int line_index=0;   //index of the line. this is used to scan a line from the input
char *file_buffer;  // instead of reading using scanf or gets I am using fread to read a chunk of memory. Reading into a buffer improves file reading spead as reading line by line is slow.
					// the size of this buffer should be multiple of page size to reduce pagefaults.
char *line_buffer;  //scan each line into a buffer to be parsed into tokens. 

/*
Reads the next line from the file. If the current buffer is completely read then it will read the next section of the input file. 
The idea is to read chunk of data and create a buffer that can be cached. Reading in chunks also improves harddrive performance.

*/
inline int get_next_line()
{
	while (1)
	{ 
		if (buffer_index == buffer_length) //read the next buffer
		{
			buffer_length = fread(file_buffer, 1, CACHE_SIZE, stdin);
			buffer_index = 0;
			if (buffer_length <= 0)// Reading past the end of file returns 0 bytes read, marks that everything has been read from the file.
				return 0; //return 0 to indicate end of file
			continue;
		}
		else
		if (file_buffer[buffer_index] == '\n' )
		{
			line_buffer[line_index] = 0; // The next buffer character will be read as the first character in the line_buffer as this will be the new line in the csv file
			line_index = 0;
			buffer_index++;//Advance the buffer index to read the next character in the buffer
			
			return 1;
		}
		else
		if (file_buffer[buffer_index] == '\r')
		{
			line_buffer[line_index--] = 0; //Ignore the \r character but add a end of string just incase there is no \n character
		}
		else
			line_buffer[line_index] = file_buffer[buffer_index]; //Ignore the \r character but add a end of string just incase there is no \n character
		
		line_index++;
		buffer_index++;
	}
	

}

//this function uses gets function to read a line form stdin and stores it in the line_buffer
//this was the first one I created to complete the assignment quickly. After that I created other
//functions to optamize the outputs and file reading 
//details are in the documentation
/*
inline int get_next_line_gets()
{
	if (fgets(line_buffer, 100, stdin) == NULL)
		return 0;
	else
		return 1;

}
*/

inline int parse_line()
{

	long long timestamp = std::strtoll(std::strtok(line_buffer, ","), nullptr,10);
	char *symbol = std::strtok(NULL, ",");
	long long volume = std::strtol(std::strtok(NULL, ","), nullptr, 10);
	long long price = std::strtol(std::strtok(NULL, ","), nullptr, 10);

	std::vector<struct stock > ::iterator it;  // declare an iterator to a vector of strings
	
	// now start at from the beginning
	// and keep iterating over the element till you find
	// elemnt with the symbol or reach the end of vector.
	for (it = stock_list.begin(); it < stock_list.end(); it++) {
		// found nth element..print and break.
		if (strcmp(symbol, it->symbol) == 0) {
			it->volume += volume;
			int time_gap = timestamp - it->timestamp;
			if (time_gap > it->time_gap)
				it->time_gap = time_gap;
			if (price > it->max_price)
				it->max_price = price;
			it->total_price += (price*volume);
			it->timestamp= timestamp;
			break;

		}
	}
	if (it == stock_list.end()) //reached the end of the vector so the symbol was not found add the new symbol in the vector
	{
		struct stock stockobject;
		std::strcpy(stockobject.symbol, symbol);
		stockobject.time_gap = 0;
		stockobject.volume = volume;
		stockobject.timestamp = timestamp;
		stockobject.max_price = price;
		stockobject.total_price = price*volume;

		stock_list.push_back(stockobject);
	}
return 0;
}

int main(int argc, char* argv[])
{
	
	file_buffer = new char[CACHE_SIZE];
	line_buffer = new char[MAX_LINE_SIZE];
	int returncode;

	
	while (1)
	{
		returncode=get_next_line();
		//returncode = get_next_line_gets();  //This was the my first attempt. This is not the most optimal solution.
		 if (returncode == 1)// 1 means that a complete line was read thereofre parse that line into required format
			 parse_line();
		 if (returncode == 0) //0 means that the end of file was reached.
			 break;

	}
	
	std::sort(stock_list.begin(), stock_list.end(), compare_stock);//sort the vector, compre_stock is the comparasion function used by the sort algorithm
	std::vector<struct stock > ::iterator it;  // declare an iterator to a vector of strings
	for (it = stock_list.begin(); it < stock_list.end(); it++) {
		std::cout << it->symbol << "," << it->time_gap << "," << it->volume << "," << it->total_price / it->volume << "," << it->max_price << std::endl;

	}
	
	delete(file_buffer);
	delete(line_buffer);
	
	return 0;

}

