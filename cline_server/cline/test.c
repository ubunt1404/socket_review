#include <stdio.h>
#include "temperature.h"
int main()
{
	float 			temp=0;
	if(get_temperature(&temp)<0)
	{
		printf("get temperature error!\n");
		return -1;
	}
         printf("get ds18b20 temperature is:%f\n",temp);
	return 0;
}
