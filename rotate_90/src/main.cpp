#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <future>
#include <algorithm>

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
void processBlock(int startRow, int endRow, int startCol, int endCol,
	const std::vector<uint8_t>& pixels,
	std::vector<uint8_t>& rotatedPixels,
	int height, int width) {
	int rotatedWidth = height;
	int rotatedHeight = width;

	for (int i = startRow; i < endRow; ++i) {
		for (int j = startCol; j < endCol; ++j) {
			int srcIndex = i * rotatedHeight + j;
			int destIndex = (rotatedHeight - 1 - j) * rotatedWidth + i;
			rotatedPixels[destIndex] = pixels[srcIndex];
		}
	}
}



//void processBlock(int startRow, int endRow, int startCol, int endCol,
//	const std::vector<uint8_t>& pixels,
//	std::vector<uint8_t>& rotatedPixels,
//	int height, int width) {
//	int widthBytes = width * sizeof(uint8_t);
//	for (int i = startRow; i < endRow; ++i) {
//		int srcRow = i;
//		int srcRowStart = srcRow * widthBytes;
//
//		for (int j = startCol; j < endCol; ++j) {
//			int srcCol = j;
//			int srcIndex = srcRowStart + srcCol;
//
//			// 计算旋转后的目标位置
//			int destRow = srcCol;
//			int destCol = width - 1 - srcRow;
//			int destRowStart = destRow * height;
//
//			int destIndex = destRowStart + destCol;
//			rotatedPixels[destIndex] = pixels[srcIndex];
//		}
//	}
//}

////分块两层嵌套
//void processBlock(int startRow, int endRow, int startCol, int endCol,
//	const std::vector<uint8_t>& pixels,
//	std::vector<uint8_t>& rotatedPixels,
//	int height, int width) {
//
//	for (int i = startRow; i < endRow; ++i) {
//		for (int j = startCol; j < endCol; ++j) {
//			int srcIndex = i * width + j;
//			int destCol = i;
//			int destRow = width - 1 - j;
//			int destIndex = destRow * height + destCol;
//
//			rotatedPixels[destIndex] = pixels[srcIndex];
//		}
//	}
//}


//单层循环，一维数组
//void processBlock(int startRow, int endRow, int startCol, int endCol,
//	const std::vector<uint8_t>& pixels,
//	std::vector<uint8_t>& rotatedPixels,
//	int height, int width) {
//
//	int pixelOffset = startRow * width + startCol;
//	int rotatedOffset = startCol * height + startRow;
//
//	int totalPixels = (endRow - startRow) * (endCol - startCol);
//	for (int idx = 0; idx < totalPixels; ++idx) {
//		int i = (idx / width) + startRow;
//		int j = (idx % width) + startCol;
//
//		int x = j;
//		int y = height - 1 - i;
//
//		rotatedPixels[x * height + i] = pixels[y * width + x];
//	}
//}

//二维数组，嵌套循环
//void processBlock(int startRow, int endRow, int startCol, int endCol,
//	const std::vector<std::vector<uint8_t>>& pixels,
//	std::vector<std::vector<uint8_t>>& rotatedPixels,
//	int height, int width) {
//	for (int i = startRow; i < endRow; ++i) {
//		for (int j = startCol; j < endCol; ++j) {
//			int x = j;
//			int y = height - 1 - i;
//			rotatedPixels[j][i] = pixels[y][x];
//		}
//	}
//}

//一维数组
//void processBlock(int startRow, int endRow, int startCol, int endCol,
//	const std::vector<std::vector<uint8_t>>& pixels,
//	std::vector<std::vector<uint8_t>>& rotatedPixels,
//	int height, int width) {
//
//	int totalPixels = (endRow - startRow) * (endCol - startCol);
//
//	for (int index = 0; index < totalPixels; ++index) {
//		int i = startRow + index / (endCol - startCol);  // 当前行
//		int j = startCol + index % (endCol - startCol);  // 当前列
//
//		int x = j;
//		int y = height - 1 - i;
//		rotatedPixels[j][i] = pixels[y][x];
//	}
//}


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

	//// Read pixel data
	//std::vector<std::vector<uint8_t>> pixels(height, std::vector<uint8_t>(width));
	//for (int i = 0; i < height; ++i) {
	//	inFile.read(reinterpret_cast<char*>(pixels[i].data()), width);

	//	// Skip padding
	//	inFile.seekg((4 - (width) % 4) % 4, std::ios::cur);
	//}

	std::vector<uint8_t> pixels(height * width);

	for (int i = 0; i < height; ++i) {
		int offset = i * width;

		inFile.read(reinterpret_cast<char*>(&pixels[offset]), width);

		inFile.seekg((4 - (width) % 4) % 4, std::ios::cur);
	}

	inFile.close();

	auto end_read = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_read = end_read - start_read;

	std::cout << "Read time:" << duration_read.count() << "ms" << std::endl;

	// Rotate the image
	//std::vector<std::vector<uint8_t>> rotatedPixels(width, std::vector<uint8_t>(height));
	std::vector<uint8_t> rotatedPixels(height * width);

	// Single-threaded processing of the entire image
	auto start_rotate = std::chrono::high_resolution_clock::now();
	processBlock(0, height, 0, width, pixels, rotatedPixels, height, width);
	//int blockSize = 64;
	//int numBlocksRow = (height + blockSize - 1) / blockSize;
	//int numBlocksCol = (width + blockSize - 1) / blockSize;

	//for (int blockIndex = 0; blockIndex < numBlocksRow * numBlocksCol; ++blockIndex) {
	//	int startRow = (blockIndex / numBlocksCol) * blockSize;
	//	int startCol = (blockIndex % numBlocksCol) * blockSize;
	//	int endRow = std::min(startRow + blockSize, height);
	//	int endCol = std::min(startCol + blockSize, width);

	//	processBlock(startRow, endRow, startCol, endCol, pixels, rotatedPixels, height, width);
	//}

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

	//// Write pixel data
	//for (int i = 0; i < infoHeader.height; ++i) {
	//	outFile.write(reinterpret_cast<const char*>(rotatedPixels[i].data()), infoHeader.width);

	//	// Add padding
	//	uint8_t padding[3] = { 0, 0, 0 };
	//	outFile.write(reinterpret_cast<const char*>(padding), (4 - (infoHeader.width) % 4) % 4);
	//}
	// Write pixel data
	for (int i = 0; i < infoHeader.height; ++i) {
		int offset = i * infoHeader.width;

		outFile.write(reinterpret_cast<const char*>(&rotatedPixels[offset]), infoHeader.width);

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

	rotateBMP8bit("5_06.bmp", "output.bmp");

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "Running time:" << duration.count() << "ms" << std::endl;

	return 0;
}
