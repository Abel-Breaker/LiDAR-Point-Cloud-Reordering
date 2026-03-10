#include "parse_lidar_points.h"
#include "../../third_party/LAStools/LASlib/inc/lasreader.hpp" // Para leer puntos
#include <cstdlib>

extern "C" {

int read_las_points(const char *filename, Points *pts)
{
	if (!filename || !pts)
		return -1;

	// Crear el lector de LAS
	LASreadOpener lasReadOpener;
	lasReadOpener.set_file_name(filename);
	LASreader *lasReader = lasReadOpener.open();
	if (!lasReader)
		return -2; // Error al abrir el archivo

	// Obtener el número de puntos desde el header
	size_t point_count = static_cast<size_t>(lasReader->header.number_of_point_records);

	// Reiniciar el lector
	lasReader->close();
	delete lasReader;

	// Reabrir para leer y copiar los puntos
	lasReadOpener.set_file_name(filename);
	lasReader = lasReadOpener.open();
	if (!lasReader)
		return -3;

	// Reservar memoria para los puntos
	pts->num_points = point_count;
	pts->x = static_cast<double *>(malloc(point_count * sizeof(*(pts->x))));
	pts->y = static_cast<double *>(malloc(point_count * sizeof(*(pts->y))));
	pts->z = static_cast<double *>(malloc(point_count * sizeof(*(pts->z))));
	if (!pts->x || !pts->y || !pts->z) {
		lasReader->close();
		delete lasReader;
		return -4; // Error de memoria
	}

	// Leer y copiar puntos
	size_t idx = 0;
	while (lasReader->read_point()) {
		pts->x[idx] = lasReader->point.get_x();
		pts->y[idx] = lasReader->point.get_y();
		pts->z[idx] = lasReader->point.get_z();
		idx++;
	}

	lasReader->close();
	delete lasReader;

	return 0; // Éxito
}

} // extern "C"