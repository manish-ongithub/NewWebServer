#pragma once

#include <malloc.h>
#include <stdio.h>



class PerformanceData
{
public:
	static PerformanceData& get()
	{
		static PerformanceData instance;
		return instance;
	}
	int GetProcessCount();
	double GetCpuUsage();
	double GetUserTime();
	double GetProcessorTime();
	double GetMemoryUsage();
private:
	PerformanceData() {}
	PerformanceData(const PerformanceData&);
};

