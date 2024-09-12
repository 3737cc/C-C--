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

//DDA思想旋转90度
void rotateBlock90DDA(int startX, int endX, int startY, int endY, const std::vector<uint8_t>& pixels,
	std::vector<uint8_t>& rotatedPixels,
	int height, int width) {
	for (int x = startX; x < endX; ++x) {
		int destY = height - 1 - startY;
		float error = 0.0f;

		for (int y = startY; y < endY; ++y) {
			int srcIndex = y * width + x;
			int destIndex = x * height + destY;

			rotatedPixels[destIndex] = pixels[srcIndex];

			error += 1.0f;
			if (error >= 1.0f) {
				destY--;
				error -= 1.0f;
			}
		}
	}
}

//旋转90度, 双层遍历
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

////多线程进行处理
//void parallelProcess(int numThreads, const std::vector<uint8_t>& pixels,
//	std::vector<uint8_t>& rotatedPixels, int height, int width) {
//	int blockSize = height / numThreads;
//	std::vector<std::thread> threads;
//
//	for (int t = 0; t < numThreads; ++t) {
//		int startRow = t * blockSize;
//		int endRow = (t == numThreads - 1) ? height : startRow + blockSize;
//		threads.emplace_back(processBlock, startRow, endRow, 0, width,
//			std::ref(pixels), std::ref(rotatedPixels), height, width);
//	}
//
//	for (auto& thread : threads) {
//		if (thread.joinable()) {
//			thread.join();
//		}
//	}
//}

////旋转180度并且镜像，但是图像会损失什么原因？？
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
//void processblock(int startrow, int endrow, int startcol, int endcol,
//	const std::vector<std::vector<uint8_t>>& pixels,
//	std::vector<std::vector<uint8_t>>& rotatedpixels,
//	int height, int width) {
//	for (int i = startrow; i < endrow; ++i) {
//		for (int j = startcol; j < endcol; ++j) {
//			int x = j;
//			int y = height - 1 - i;
//			rotatedpixels[j][i] = pixels[y][x];
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
//		int i = startRow + index / (endCol - startCol);
//		int j = startCol + index % (endCol - startCol);
//
//		int x = j;
//		int y = height - 1 - i;
//		rotatedPixels[j][i] = pixels[y][x];
//	}
//}

//异步任务进行处理
void asyncProcess(int numTasks, const std::vector<uint8_t>& pixels,
	std::vector<uint8_t>& rotatedPixels, int height, int width) {
	int blockSize = height / numTasks;
	std::vector<std::future<void>> futures;

	for (int t = 0; t < numTasks; ++t) {
		int startRow = t * blockSize;
		int endRow = (t == numTasks - 1) ? height : startRow + blockSize;
		futures.push_back(std::async(std::launch::async, processBlock, startRow, endRow, 0, width,
			std::cref(pixels), std::ref(rotatedPixels), height, width));
	}

	for (auto& future : futures) {
		future.get(); // Wait for all tasks to complete
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

	// RGB读取
	std::vector<RGBQuad> palette(paletteSize);
	inFile.read(reinterpret_cast<char*>(palette.data()), paletteSize * sizeof(RGBQuad));

	//// 读取像素
	//std::vector<std::vector<uint8_t>> pixels(height, std::vector<uint8_t>(width));
	//for (int i = 0; i < height; ++i) {
	//	inFile.read(reinterpret_cast<char*>(pixels[i].data()), width);

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

	// 旋转图像
	//std::vector<std::vector<uint8_t>> rotatedPixels(width, std::vector<uint8_t>(height));
	std::vector<uint8_t> rotatedPixels(height * width);
	auto start_rotate = std::chrono::high_resolution_clock::now();

	//asyncProcess(4, pixels, rotatedPixels, height, width);
	asyncProcess(4, pixels, rotatedPixels, height, width);

	//processBlock(0, height, 0, width, pixels, rotatedPixels, height, width);
	//rotateBlock90DDA(0, height, 0, width, pixels, rotatedPixels, height, width);
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

	// 写入文件头
	std::swap(infoHeader.width, infoHeader.height);

	std::ofstream outFile(outputFile, std::ios::binary);
	if (!outFile) {
		std::cerr << "Could not open output file." << std::endl;
		return;
	}

	auto start_write = std::chrono::high_resolution_clock::now();

	outFile.write(reinterpret_cast<const char*>(&header), sizeof(header));
	outFile.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));
	outFile.write(reinterpret_cast<const char*>(palette.data()), paletteSize * sizeof(RGBQuad));

	//// Write pixel data
	//for (int i = 0; i < infoHeader.height; ++i) {
	//	outFile.write(reinterpret_cast<const char*>(rotatedPixels[i].data()), infoHeader.width);

	//	// Add padding
	//	uint8_t padding[3] = { 0, 0, 0 };
	//	outFile.write(reinterpret_cast<const char*>(padding), (4 - (infoHeader.width) % 4) % 4);
	//}
	// 写入数据
	for (int i = 0; i < infoHeader.height; ++i) {
		int offset = i * infoHeader.width;

		outFile.write(reinterpret_cast<const char*>(&rotatedPixels[offset]), infoHeader.width);

		uint8_t padding[3] = { 0, 0, 0 };
		outFile.write(reinterpret_cast<const char*>(padding), (4 - (infoHeader.width) % 4) % 4);
	}

	outFile.close();

	auto end_write = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration_write = end_write - start_write;

	std::cout << "Write time:" << duration_write.count() << "ms" << std::endl << std::endl;
}

int main() {
	auto start = std::chrono::high_resolution_clock::now();

	rotateBMP8bit("2_53.bmp", "output_2_53.bmp");
	rotateBMP8bit("5_06.bmp", "output_5_06.bmp");
	rotateBMP8bit("11_7.bmp", "output_11_7.bmp");
	rotateBMP8bit("20.bmp", "output_20.bmp");

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> duration = end - start;

	std::cout << "Running time:" << duration.count() << "ms" << std::endl;

	return 0;
}
