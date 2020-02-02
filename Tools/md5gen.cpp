/*
 * MD5 generator
 */

#include <iostream>
using namespace std;

constexpr auto MD5_RANGE = 32 + 1;

constexpr auto BINARY_RANGE = 16;

constexpr auto NUMBER = 50;

constexpr auto RADIX = 4;

void md5Rand(unsigned char[]);

void md5RadixSort(unsigned char *[], unsigned char *[]);

int binarySearch(unsigned char *[], unsigned char *);

unsigned int hex2Dec(const unsigned char c);

int main(int argc, unsigned char *argv[])
{
	unsigned char *md5Str[NUMBER];
	unsigned char *md5Sorted[NUMBER];

	/* random md5 generating */
	for (int i = 0; i < NUMBER; i++)
	{
		unsigned char *str = new unsigned char[MD5_RANGE];
		md5Rand(str);
		md5Str[i] = str;
	}

	/* Radix Sort */
	md5RadixSort(md5Str, md5Sorted);

	for (int i = 0; i < NUMBER; i++)
	{
		cout << i << "  " << md5Sorted[i] << endl;
	}

	/* bi-search */
	int position;

	if ((position = binarySearch(md5Sorted, argv[1])) < 0)
	{
		cout << endl << "No match!" << endl;
	}
	else
	{
		cout << endl << "Position: " << position + 1 << endl;
	}

	system("pause");
	return 0;
}

void md5Rand(unsigned char md5Str[])
{
	for (int i = 0; i < MD5_RANGE - 1; i++)
	{
		char c = rand() % 16;
		md5Str[i] = (c >= 10) ? 'a' + c - 10 : '0' + c;
	}
	md5Str[MD5_RANGE - 1] = '\0';
}

void md5RadixSort(unsigned char *md5Input[], unsigned char *output[])
{
	/* hex string partition */
	// if necessary

	unsigned int count[BINARY_RANGE];

	for (int i = MD5_RANGE - 2; i >= 0; i--)
	{
		/* hex to dec */
		unsigned int array[NUMBER];
		memset(array, 0, sizeof(unsigned int) * NUMBER);

		for (int j = 0; j < NUMBER; j++)
		{
			array[j] = hex2Dec(*(*(md5Input + j) + i));
		}

		/* counting sort */
		memset(count, 0, sizeof(unsigned int) * BINARY_RANGE);

		for (int j = 0; j < NUMBER; j++)
		{
			count[array[j]] = count[array[j]] + 1;
		}

		for (int j = 1; j < BINARY_RANGE; j++)
		{
			count[j] = count[j] + count[j - 1];
		}

		//algorithm : array_list_b[c[array_list_a[j]] - 1] = array_list_a[j];
		for (int j = NUMBER - 1; j >= 0; j--)
		{
			unsigned char *str = new unsigned char[MD5_RANGE];
			str = *(md5Input + j);
			output[--count[array[j]]] = str;	//correct: mius beforehand
			//delete[] str;DO NOT DELETE
		}

		for (int j = 0; j < NUMBER; j++)
		{
			md5Input[j] = output[j];
		}
	}
}

unsigned int hex2Dec(const unsigned char c)
{
	return (c > '9') ? (c - 'a' + 10) : (c - '0');
}

int binarySearch(unsigned char *md5Input[], unsigned char *key)
{
	int left = 0;
	int right = NUMBER - 1;
	int mid = 0;
	
	while (left <= right)
	{
		mid = (left + right) / 2;
		if (md5Input[mid] < key)
		{
			left = mid + 1;
		}
		else if (md5Input[mid] > key)
		{
			right = mid - 1;
		}
		else
		{
			return mid;
		}
	}
	return -1;
}
