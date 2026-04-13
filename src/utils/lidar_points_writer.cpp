extern "C" {
	#include "lidar_points_writer.h"
}
#include "../../third_party/LAStools/LASlib/inc/laswriter.hpp" // For writing points
#include <cstdlib>

void write_las_points(const char *filename, const Points *pts)
{
	LASwriteOpener lasWriteOpener;
	lasWriteOpener.set_file_name(filename);

	// Init header
	LASheader lasHeader;
	lasHeader.x_scale_factor = 0.01;
	lasHeader.y_scale_factor = 0.01;
	lasHeader.z_scale_factor = 0.01;
	lasHeader.x_offset = 0.0;
	lasHeader.y_offset = 0.0;
	lasHeader.z_offset = 0.0;
	lasHeader.point_data_format = 2;
	lasHeader.point_data_record_length = 26;

    // Add attribute
    LASattribute attribute(4, "index", NULL);   // unsigned long (4 bytes), as per LAS spec
    attribute.set_scale(1);
    attribute.set_offset(0);
    I32 index = lasHeader.add_attribute(attribute);
    lasHeader.update_extra_bytes_vlr();
    lasHeader.point_data_record_length += lasHeader.get_attributes_size();
    I32 att_start = lasHeader.get_attribute_start(index);

	// Init point
	LASpoint lasPoint;
	lasPoint.init(&lasHeader, lasHeader.point_data_format, lasHeader.point_data_record_length, 0);

	// Open writer
	LASwriter *lasWriter = lasWriteOpener.open(&lasHeader);
	if (!lasWriter) {
		printf("ERROR: could not open LASwriter\n");
		exit(-2);
	}

	// Write points
	for (size_t i = 0; i < pts->num_points; ++i) {
		lasPoint.set_X(pts->x[i] * 100);
		lasPoint.set_Y(pts->y[i] * 100);
		lasPoint.set_Z(pts->z[i] * 100);

        lasPoint.set_attribute(att_start, U32_QUANTIZE(i));

		lasWriter->write_point(&lasPoint);
		lasWriter->update_inventory(&lasPoint);
	}

	// Update header
	lasWriter->update_header(&lasHeader, true);

	lasWriter->close();
    delete lasWriter;
}