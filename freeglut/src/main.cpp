#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <GL/glew.h>
#include <GL/freeglut.h>

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

GLuint textureID;
int windowWidth, windowHeight;
std::vector<RGBQuad> palette;
BMPHeader header;
BMPInfoHeader infoHeader;

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D ourTexture;
    void main() {
        vec2 rotatedTexCoord = vec2(1.0 - TexCoord.y, TexCoord.x);
        FragColor = texture(ourTexture, rotatedTexCoord);
    }
)";

GLuint shaderProgram;

void compileShaders() {
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void loadBMP(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		std::cerr << "Could not open file: " << filename << std::endl;
		return;
	}

	file.read(reinterpret_cast<char*>(&header), sizeof(header));
	file.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

	if (infoHeader.bitCount != 8) {
		std::cerr << "Only 8-bit BMP files are supported." << std::endl;
		return;
	}

	windowWidth = infoHeader.width;
	windowHeight = infoHeader.height;

	int paletteSize = (header.offsetData - sizeof(header) - sizeof(infoHeader)) / sizeof(RGBQuad);
	palette.resize(paletteSize);
	file.read(reinterpret_cast<char*>(palette.data()), paletteSize * sizeof(RGBQuad));

	std::vector<uint8_t> pixels(infoHeader.width * infoHeader.height);
	for (int i = 0; i < infoHeader.height; ++i) {
		file.read(reinterpret_cast<char*>(&pixels[i * infoHeader.width]), infoHeader.width);
		file.seekg((4 - (infoHeader.width) % 4) % 4, std::ios::cur);
	}

	std::vector<uint8_t> rgbPixels(infoHeader.width * infoHeader.height * 3);
	for (int i = 0; i < infoHeader.width * infoHeader.height; ++i) {
		rgbPixels[i * 3] = palette[pixels[i]].red;
		rgbPixels[i * 3 + 1] = palette[pixels[i]].green;
		rgbPixels[i * 3 + 2] = palette[pixels[i]].blue;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, infoHeader.width, infoHeader.height, 0, GL_RGB, GL_UNSIGNED_BYTE, rgbPixels.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void display() {
	// 开始计时
	auto start = std::chrono::steady_clock::now();

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shaderProgram);
	glBindTexture(GL_TEXTURE_2D, textureID);

	float vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glutSwapBuffers();

	// 结束计时并计算持续时间
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli>duration = end - start;
	std::cout << "Time taken to rotate the image: " << duration.count() << " ms" << std::endl;
}

void reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void saveRotatedBMP(const char* filename) {
	// 确保正确的渲染上下文已经完成，并且绑定了正确的窗口大小
	std::vector<uint8_t> pixels(windowHeight * windowWidth * 3);  // 读取RGB格式

	// 读取当前屏幕的像素
	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

	std::ofstream outFile(filename, std::ios::binary);
	if (!outFile) {
		std::cerr << "Could not open output file." << std::endl;
		return;
	}

	BMPHeader newHeader = header;
	BMPInfoHeader newInfoHeader = infoHeader;
	std::swap(newInfoHeader.width, newInfoHeader.height);  // 交换宽高

	int paddedRowSize = (newInfoHeader.width * 3 + 3) & (~3);  // 计算行的对齐
	int imageSize = paddedRowSize * newInfoHeader.height;
	newHeader.fileSize = sizeof(BMPHeader) + sizeof(BMPInfoHeader) + palette.size() * sizeof(RGBQuad) + imageSize;
	newInfoHeader.sizeImage = imageSize;

	outFile.write(reinterpret_cast<const char*>(&newHeader), sizeof(newHeader));
	outFile.write(reinterpret_cast<const char*>(&newInfoHeader), sizeof(newInfoHeader));
	outFile.write(reinterpret_cast<const char*>(palette.data()), palette.size() * sizeof(RGBQuad));

	std::vector<uint8_t> paddedRow(paddedRowSize);
	for (int y = 0; y < newInfoHeader.height; ++y) {
		// BMP文件从上到下保存像素，但glReadPixels从左下到右上读取，因此需要翻转
		int srcY = newInfoHeader.height - 1 - y;  // 翻转行

		for (int x = 0; x < newInfoHeader.width; ++x) {
			int srcIndex = (srcY * newInfoHeader.width + x) * 3;
			paddedRow[x * 3] = pixels[srcIndex + 2];  // 蓝色
			paddedRow[x * 3 + 1] = pixels[srcIndex + 1];  // 绿色
			paddedRow[x * 3 + 2] = pixels[srcIndex];  // 红色
		}

		outFile.write(reinterpret_cast<const char*>(paddedRow.data()), paddedRowSize);  // 写入每一行
	}

	outFile.close();
	std::cout << "Rotated image saved as " << filename << std::endl;
}


void keyboard(unsigned char key, int x, int y) {
	if (key == 's' || key == 'S') {
		saveRotatedBMP("rotated_output.bmp");
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 600);
	glutCreateWindow("GPU-accelerated BMP Rotation");

	glewInit();

	compileShaders();
	loadBMP("5_06.bmp");

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}