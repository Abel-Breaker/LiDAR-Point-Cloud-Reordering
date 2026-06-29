extern "C" {
#include "points_writer.h"
}
#include "../../third_party/LAStools/LASlib/inc/laswriter.hpp" // For writing points
#include <cstdlib>

void write_las_points(const char *filename, const Points_sorted *pts)
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

	// Add order radius to header
	double radius = get_args()->radius_reorder;
	U8 *radius_vlr_data = new U8[sizeof(double)];
	memcpy(radius_vlr_data, &radius, sizeof(double));
	lasHeader.add_vlr("ordered", 1, sizeof(double), radius_vlr_data, FALSE, "radius");
	
	// index
	LASattribute attr_index(4, "index", NULL);
	attr_index.set_scale(1);
	attr_index.set_offset(0);
	I32 index_attr = lasHeader.add_attribute(attr_index);

	// bw_left
	LASattribute attr_bw_left(4, "bw_left", NULL);
	attr_bw_left.set_scale(1);
	attr_bw_left.set_offset(0);
	I32 bw_left_attr = lasHeader.add_attribute(attr_bw_left);

	// bw_right
	LASattribute attr_bw_right(4, "bw_right", NULL);
	attr_bw_right.set_scale(1);
	attr_bw_right.set_offset(0);
	I32 bw_right_attr = lasHeader.add_attribute(attr_bw_right);

	lasHeader.update_extra_bytes_vlr();
	lasHeader.point_data_record_length += lasHeader.get_attributes_size();

	// Offsets de cada atributo dentro del punto
	I32 att_start_index = lasHeader.get_attribute_start(index_attr);
	I32 att_start_bw_left = lasHeader.get_attribute_start(bw_left_attr);
	I32 att_start_bw_right = lasHeader.get_attribute_start(bw_right_attr);

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
	for (index_t i = 0; i < pts->points->num_points; ++i) {
		lasPoint.set_X(pts->points->x[i] * 100);
		lasPoint.set_Y(pts->points->y[i] * 100);
		lasPoint.set_Z(pts->points->z[i] * 100);

		// Only to ease cloud compare management for TFG, this index it is not necessary since points are
		// ordered
		lasPoint.set_attribute(att_start_index, U32_QUANTIZE(i));

		lasPoint.set_attribute(att_start_bw_left,
				       U32_QUANTIZE(pts->bandwith_left[get_block_index(i, pts->points->num_points)]));
		lasPoint.set_attribute(att_start_bw_right,
				       U32_QUANTIZE(pts->bandwith_right[get_block_index(i, pts->points->num_points)]));

		lasWriter->write_point(&lasPoint);
		lasWriter->update_inventory(&lasPoint);
	}

	// Update header
	lasWriter->update_header(&lasHeader, true);

	lasWriter->close();
	delete lasWriter;
}