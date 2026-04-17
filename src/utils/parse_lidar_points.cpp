extern "C" {
#include "parse_lidar_points.h"
#include "parse_args.h"
}
#include "../../third_party/LAStools/LASlib/inc/lasreader.hpp" // For reading points

#include <cstdlib>

bool read_las_points(const char *filename, Points *pts)
{
	if (!filename || !pts)
		return false;

	// Create the LAS reader
	LASreadOpener lasReadOpener;
	lasReadOpener.set_file_name(filename);
	LASreader *lasReader = lasReadOpener.open();
	if (!lasReader)
		return false;

	// Get the number of points from the header
	size_t point_count = static_cast<size_t>(lasReader->header.number_of_point_records);

	if(get_args()->max_num_of_points != 0){
		if(get_args()->max_num_of_points < point_count){
			point_count = get_args()->max_num_of_points;
		}
	}

	// Restart the reader
	lasReader->close();
	delete lasReader;

	// Reopen to read and copy the points
	lasReadOpener.set_file_name(filename);
	lasReader = lasReadOpener.open();
	if (!lasReader)
		return false;

	// Reserve memory for the points
	if (!reserve_memory_points(pts, point_count)) {
		lasReader->close();
		delete lasReader;
		return false;
	}

	// Read and copy points
	for(size_t i=0; i<point_count; ++i){
		lasReader->read_point();
		add_point(pts, i, lasReader->point.get_x(), lasReader->point.get_y(), lasReader->point.get_z());
	}

	lasReader->close();
	delete lasReader;

	return true;
}
