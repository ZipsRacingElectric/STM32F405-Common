#ifndef SORT_H
#define SORT_H

// C Standard Library
#include <stdbool.h>

#define sortValues(data, dataCount, sort, sortIndices, sortCount, comparison, extrema)										\
	for (typeof (sortIndices [0]) sortIndicesIndex = 0; sortIndicesIndex < sortCount; ++sortIndicesIndex)					\
	{																														\
		sort [sortIndicesIndex] = extrema;																					\
		sortIndices [sortIndicesIndex] = -1;																				\
	}																														\
																															\
	for (typeof (sortIndices [0]) sortIndicesIndex = 0; sortIndicesIndex < sortCount; ++sortIndicesIndex)					\
	{																														\
		for (typeof (sortIndices [0]) dataIndex = 0; dataIndex < dataCount; ++dataIndex)									\
		{																													\
			bool skip = false;																								\
			for (typeof (dataCount) subSortIndicesIndex = 0; subSortIndicesIndex < sortIndicesIndex; ++subSortIndicesIndex)	\
				if (sortIndices [subSortIndicesIndex] == dataIndex)															\
					skip = true;																							\
			if (skip)																										\
				continue;																									\
																															\
			if (data [dataIndex] comparison sort [sortIndicesIndex])														\
			{																												\
				sort [sortIndicesIndex] = data [dataIndex];																	\
				sortIndices [sortIndicesIndex] = dataIndex;																	\
			}																												\
		}																													\
	}

#endif // SORT_H