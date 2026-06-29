extern "C" {
#include "points_reader.h"
#include "../../parse_args.h"
#include "../../types.h"
}
#include "../../third_party/LAStools/LASlib/inc/lasreader.hpp"
#include <cstdlib>

bool read_las_points(const char *filename, Points *pts)
{
	if (!filename || !pts)
		return false;

	LASreadOpener lasReadOpener;
	lasReadOpener.set_file_name(filename);
	LASreader *lasReader = lasReadOpener.open();
	if (!lasReader)
		return false;

	index_t point_count =
		static_cast<index_t>(lasReader->header.number_of_point_records);

	if (get_args()->max_num_of_points != 0 &&
		get_args()->max_num_of_points < point_count)
	{
		point_count = get_args()->max_num_of_points;
	}

	// compute bounding box center (local origin)
	const double origin_x =
		(lasReader->header.min_x + lasReader->header.max_x) * 0.5;

	const double origin_y =
		(lasReader->header.min_y + lasReader->header.max_y) * 0.5;

	const double origin_z =
		(lasReader->header.min_z + lasReader->header.max_z) * 0.5;

	lasReader->close();
	delete lasReader;

	// reopen for reading
	lasReadOpener.set_file_name(filename);
	lasReader = lasReadOpener.open();
	if (!lasReader)
		return false;

	if (!reserve_memory_points(pts, point_count)) {
		lasReader->close();
		delete lasReader;
		return false;
	}

	for (index_t i = 0; i < point_count; ++i)
	{
		lasReader->read_point();

		const double x = lasReader->point.get_x();
		const double y = lasReader->point.get_y();
		const double z = lasReader->point.get_z();

		const data_t local_x = static_cast<data_t>(x - origin_x);
		const data_t local_y = static_cast<data_t>(y - origin_y);
		const data_t local_z = static_cast<data_t>(z - origin_z);

		add_point(pts, i, local_x, local_y, local_z);
	}

	lasReader->close();
	delete lasReader;

	return true;
}
