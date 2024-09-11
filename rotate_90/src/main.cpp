#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>

#pragma pack(push, 1)
struct BMPHeader {
	uint16_t fileType;
	uint32_t fileSize;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offsetData;
};

struct BMPInfoHeader {
	uint32_t size;
	int32_t width;
	int32_t height;
	uint16_t planes;
	uint16_t bitCount;
	uint32_t compression;
	uint32_t sizeImage;
	int32_t xPixelsPerMeter;
	int32_t yPixelsPerMeter;
	uint32_t colorsUsed;
	uint32_t colorsImportant;
};

struct RGBQuad {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
	uint8_t reserved;
};
#pragma pack(pop)

const int NUM_THREADS = 8;

void processBlock(int startRow, int endRow, int startCol, int endCol,
	const std::vector<std::vector<uint8_t>>& pixels,
	std::vector<std::vector<uint8_t>>& rotatedPixels,
	int height, int width) {
	for (int i = startRow; i < endRow; ++i) {
		for (int j = startCol; j < endCol; ++j) {
			int x = j;
			int y = height - 1 - i;
			rotatedPixels[j][i] = pixels[y][x];
		}
	}
}

void rotateBMP8bit(const char* inputFile, const char* outputFile) {
	std::ifstream inFile(inputFile, std::ios::binary);
	if (!inFile) {
		std::cerr << "Could not open input file." << std::endl;
		return;
	}

	BMPHeader header;
	BMPInfoHeader infoHeader;

	auto start_read = std::chrono::high_resolution_clock::now();

	inFile.read(reinterpret_cast<char*>(&header), sizeof(header));
	inFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	if (infoHeader.bitCount != 8) {
		std::cerr << "Only 8-bit BMP files are supported." << std::endl;
		return;
	}

	int width = infoHeader.width;
	int height = infoHeader.height;
	int paletteSize = (header.offsetData - sizeof(header) - sizeof(infoHeader)) / sizeof(RGBQuad);

	// Read the color palette
	std::vector<RGBQuad> palette(paletteSize);
	inFile.read(reinterpret_cast<char*>(palette.data()), paletteSize * sizeof(RGBQuad));

	// Read pixel data
	std::vector<std::vector<uint8_t>> pixels(height, std::vector<uint8_t>(width));
	for (int i = 0; i < height; ++i) {
		inFile.read(reinterpret_cast<char*>(pixels[i].data()), width);

		// Skip padding
		inFile.seekg((4 - (width) % 4) % 4, std::ios::cur);
	}

	inFile.close();

	auto end_read = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_read = end_read - start_read;

	std::cout << "Read time:" << duration_read.count() << "ms" << std::endl;

	// Rotate the image
	auto start_rotate = std::chrono::high_resolution_clock::now();

	std::vector<std::vector<uint8_t>> rotatedPixels(width, std::vector<uint8_t>(height));
	//for (int i = 0; i < height; ++i) {
	//	for (int j = 0; j < width; ++j) {
	//		rotatedPixels[j][height - 1 - i] = pixels[i][j];
	//	}
	//}

	//Bilinear interpolation
	//for (int i = 0; i < height; ++i) {
	//	for (int j = 0; j < width; ++j) {
	//		float x = j;
	//		float y = height - 1 - i;
	//		int x0 = static_cast<int>(floor(x));
	//		int y0 = static_cast<int>(floor(y));
	//		int x1 = x0 + 1;
	//		int y1 = y0 + 1;
	//		float u = x - x0;
	//		float v = y - y0;

	//		if (x0 < 0) x0 = 0;
	//		if (y0 < 0) y0 = 0;
	//		if (x1 >= width) x1 = width - 1;
	//		if (y1 >= height) y1 = height - 1;

	//		rotatedPixels[j][i] =
	//			(1 - u) * (1 - v) * pixels[y0][x0] +
	//			u * (1 - v) * pixels[y0][x1] +
	//			(1 - u) * v * pixels[y1][x0] +
	//			u * v * pixels[y1][x1];
	//	}
	//}

	//Nearest neighbor interpolation
	//for (int i = 0; i < height; ++i) {
	//	for (int j = 0; j < width; ++j) {
	//		int x = j;
	//		int y = height - 1 - i;

	//		if (x < 0 || x >= width || y < 0 || y >= height) {
	//			rotatedPixels[j][i] = 0; 
	//		}
	//		else {
	//			rotatedPixels[j][i] = pixels[y][x];
	//		}
	//	}
	//}

	//Nearest neighbor interpolation 2

	/*int minX = 0, maxX = width - 1;
	int minY = 0, maxY = height - 1;

	for (int i = 0; i < height; ++i) {
		int y = height - 1 - i;
		if (y < minY) minY = y;
		if (y > maxY) maxY = y;
	}

	for (int j = 0; j < width; ++j) {
		if (j < minX) minX = j;
		if (j > maxX) maxX = j;
	}

	for (int i = minY; i <= maxY; ++i) {
		for (int j = minX; j <= maxX; ++j) {
			int x = j;
			int y = height - 1 - i;
			rotatedPixels[j][i] = pixels[y][x];
		}
	}*/

	// Nearest neighbor interpolation multi-threaded block processing
	int height1 = infoHeader.height;
	int width1 = infoHeader.width;
	std::vector<std::vector<uint8_t>> rotatedPixels1(width1, std::vector<uint8_t>(height1));

	int blockRows = height / NUM_THREADS;
	int blockCols = width;

	std::vector<std::thread> threads;
	for (int i = 0; i < NUM_THREADS; ++i) {
		int startRow = i * blockRows;
		int endRow = (i == NUM_THREADS - 1) ? height : startRow + blockRows;
		threads.emplace_back(processBlock, startRow, endRow, 0, blockCols, std::ref(pixels), std::ref(rotatedPixels), height, width);
	}

	for (auto& thread : threads) {
		thread.join();
	}

	auto end_rotate = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_rotate = end_rotate - start_rotate;

	std::cout << "Rotate time:" << duration_rotate.count() << "ms" << std::endl;

	// Update BMP info header for the rotated image
	std::swap(infoHeader.width, infoHeader.height);

	std::ofstream outFile(outputFile, std::ios::binary);
	if (!outFile) {
		std::cerr << "Could not open output file." << std::endl;
		return;
	}

	auto start_write = std::chrono::high_resolution_clock::now();

	outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
	outFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

	// Write the color palette
	outFile.write(reinterpret_cast<const char*>(palette.data()), paletteSize * sizeof(RGBQuad));

	// Write pixel data
	for (int i = 0; i < infoHeader.height; ++i) {
		outFile.write(reinterpret_cast<const char*>(rotatedPixels[i].data()), infoHeader.width);

		// Add padding
		uint8_t padding[3] = { 0, 0, 0 };
		outFile.write(reinterpret_cast<const char*>(padding), (4 - (infoHeader.width) % 4) % 4);
	}

	outFile.close();

	auto end_write = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_write = end_write - start_write;

	std::cout << "Write time:" << duration_write.count() << "ms" << std::endl;
}

int main() {
	auto start = std::chrono::high_resolution_clock::now();

	rotateBMP8bit("20.bmp", "output.bmp");

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "Running time:" << duration.count() << "ms" << std::endl;

	return 0;
}