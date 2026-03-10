extern "C" {
#include "parse_lidar_points.h"
}
#include "../../third_party/LAStools/LASlib/inc/lasreader.hpp" // Para leer puntos
#include <cstdlib>

bool read_las_points(const char *filename, Points *pts)
{
	if (!filename || !pts)
		return false;

	// Crear el lector de LAS
	LASreadOpener lasReadOpener;
	lasReadOpener.set_file_name(filename);
	LASreader *lasReader = lasReadOpener.open();
	if (!lasReader)
		return false; // Error al abrir el archivo

	// Obtener el número de puntos desde el header
	size_t point_count = static_cast<size_t>(lasReader->header.number_of_point_records);

	// Reiniciar el lector
	lasReader->close();
	delete lasReader;

	// Reabrir para leer y copiar los puntos
	lasReadOpener.set_file_name(filename);
	lasReader = lasReadOpener.open();
	if (!lasReader)
		return false;

	// Reservar memoria para los puntos
	if (!reserve_memory_points(pts, point_count)) {
		lasReader->close();
		delete lasReader;
		return false;
	}

	// Leer y copiar puntos
	size_t idx = 0;
	while (lasReader->read_point()) {
		add_point(pts, idx, lasReader->point.get_x(), lasReader->point.get_y(), lasReader->point.get_z());
		idx++;
	}

	lasReader->close();
	delete lasReader;

	return true; // Éxito
}
