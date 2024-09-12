#include <QImage>
#include <QTransform>
#include <QDebug>
#include <chrono>

int main()
{
	// Load the image
	QImage image("2_53.bmp");

	if (image.isNull()) {
		qDebug() << "Failed to load image";
		return -1;
	}

	// Create a QTransform object
	QTransform transform;

	// Rotate 90 degrees (clockwise)
	auto start = std::chrono::high_resolution_clock::now();
	transform.rotate(90);

	// Apply the transformation to the image
	QImage rotatedImage = image.transformed(transform);

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;
	qDebug() << "Rotation time:" << duration.count() << "ms";

	// Save the rotated image
	if (rotatedImage.save("output.bmp")) {
		qDebug() << "Image successfully rotated and saved";
	}
	else {
		qDebug() << "Error saving image";
	}

	return 0;
}
